use std::collections::HashMap;
use itertools::Itertools;
use nalgebra::{DMatrix, DVector, matrix, SMatrix, SVector, vector};
use crate::errors::ModelError;
use crate::input::{Material, LayerAlignment, Section};
use virtualbow_fem::elements::beam::geometry::CrossSection;
use virtualbow_num::spline::{BoundaryCondition, CubicSpline, Extrapolation};
use virtualbow_num::intervals::{Bound, Interval};

#[derive(Debug)]
pub struct LayerGeometry {
    pub name: String,
    pub material: Material,
    pub height: CubicSpline,
}

#[derive(Debug)]
pub struct LayeredCrossSection {
    width: CubicSpline,
    layers: Vec<LayerGeometry>,
    stacking: DMatrix<f64>
}

impl LayeredCrossSection {
    pub fn new(section: &Section) -> Result<Self, ModelError> {
        // Check inputs for validity and return error on failure.
        // Also builds two hashmaps (material name) -> (material) and (layer name) -> (index) in the process.
        let (material_map, layer_map) = Self::validate(section)?;

        // Construct width spline
        let width = CubicSpline::from_points(&section.width.0, true, BoundaryCondition::SecondDerivative(0.0), BoundaryCondition::SecondDerivative(0.0));

        // Construct layer geometries
        let layers = section.layers.iter().map(|layer| {
            let points = &layer.height.0;
            let boundary_left = if points.first().unwrap()[0] == 0.0 { BoundaryCondition::SecondDerivative(0.0) } else { BoundaryCondition::FirstDerivative(0.0) };
            let boundary_right = if points.last().unwrap()[0] == 1.0 { BoundaryCondition::SecondDerivative(0.0) } else { BoundaryCondition::FirstDerivative(0.0) };

            let height = CubicSpline::from_points(points, true, boundary_left, boundary_right);
            LayerGeometry {
                name: layer.name.clone(),
                height,
                material: material_map.get(&layer.material).unwrap().clone()    // Unwrap okay because validity has been checked previously
            }
        }).collect_vec();

        // Some useful constants
        // A stack of n layers produces k = n + 1 layer boundaries
        let n = layers.len();
        let k = n + 1;

        // The list of layers is ordered from back to belly.
        // (From belly to back would make the following calculations easier, but then the order in the user interface would be reversed from the 3d view of the bow.)
        // For any point along the limb we want to find the relationship between the layer heights h[0]...h[n-1] and the layer bounds y[0]...y[k-1] in cross-section coordinates.
        // Initially we align the first layer boundary y[0], which is the back of the first layer and the section, at coordinate zero.
        // The other layer boundaries follow by partially summing up the heights up to that layer in negative direction:
        //
        // y[0] = 0
        // y[1] = -(h[0])
        // y[2] = -(h[0] + h[1])
        // ...
        // y[k-1] = -(h[0] + h[1] + ... + h[n-1])
        //
        // We can write this as a matrix multiplication y = A*h, where A is a (k x n) matrix with a lower triangular part filled with -1.
        // We call A the stacking matrix, because it contains the logic of how the layers are stacked together.
        let mut stacking = DMatrix::<f64>::zeros(k, n);
        stacking.fill_lower_triangle(-1.0, 1);

        // The actual y positions are shifted by -y*, where y* is the position where the profile curve is aligned with the section.
        // Depending on the kind of alignment, the matrix is modified in order to include this shift.
        match &section.alignment {
            // The belly of the section is located at y* = y[0] = 0.
            // Therefore nothing has to be subtracted and the matrix is not modified
            LayerAlignment::SectionBack => {
                // Do nothing
            },

            // The belly of the section is located at y* = y[k-1] = -(h[0] + h[1] + ... + h[n-1]).
            // Subtracting y* is therefore equivalent to adding 1 to every matrix element.
            LayerAlignment::SectionBelly => {
                stacking.add_scalar_mut(1.0)
            },

            // The section center is located at y* = 0.5*(y[0] + y[k-1]) = 0.5*h[0] + 0.5*h[1] + ... + 0.5*h[n-1].
            // Subtracting y* is therefore equivalent to adding 0.5 to every matrix element.
            LayerAlignment::SectionCenter => {
                stacking.add_scalar_mut(0.5)
            },

            // The back of layer i is located at y* = y[i] = -(h[0] + h[1] + ... + h[i-1])
            // Subtracting y* is therefore equivalent to adding 1 to columns 0 to i.
            LayerAlignment::LayerBack(name) => {
                let i = *layer_map.get(name).unwrap();    // Unwrap okay because validity has been checked previously
                stacking.view_mut((0, 0), (k, i)).add_scalar_mut(1.0);
            }

            // The belly of layer i is located at y* = y[i+1] = -(h[0] + h[1] + ... + h[i])
            // Subtracting y* is therefore equivalent to adding 1 to columns 0 to i+1.
            LayerAlignment::LayerBelly(name) => {
                let i = *layer_map.get(name).unwrap();    // Unwrap okay because validity has been checked previously
                stacking.view_mut((0, 0), (k, i+1)).add_scalar_mut(1.0);
            }

            // The center of layer i is located at y* = 0.5*(y[i] + y[i+1]) = -(h[0] + h[1] + ... + 0.5*h[i])
            // Subtracting y* is therefore equivalent to adding 1 to columns 0 to i and 0.5 to column i+1.
            LayerAlignment::LayerCenter(name) => {
                let i = *layer_map.get(name).unwrap();    // Unwrap okay because validity has been checked previously
                stacking.view_mut((0, 0), (k, i)).add_scalar_mut(1.0);
                stacking.view_mut((0, i), (k, 1)).add_scalar_mut(0.5);
            }
        }

        Ok(Self {
            width,
            layers,
            stacking
        })
    }

    // TODO: Redundancy with Section.validate()?
    fn validate(section: &Section) -> Result<(HashMap<String, Material>, HashMap<String, usize>), ModelError> {
        section.validate()?;

        // Check if there is at least one layer
        if section.layers.is_empty() {
            return Err(ModelError::CrossSectionNoLayers);
        }

        // Check if there is at least one material
        if section.materials.is_empty() {
            return Err(ModelError::CrossSectionNoMaterials);
        }

        // Validate materials individually
        for material in &section.materials {
            material.validate()?;
        }

        // Build map from material name to material, error if names are duplicated
        let mut material_map = HashMap::with_capacity(section.materials.len());
        for material in &section.materials {
            if material_map.insert(material.name.clone(), material.clone()).is_some() {
                return Err(ModelError::CrossSectionDuplicateMaterialName(material.name.clone()));
            }
        }

        // Build map from layer name to index, error if names are duplicated
        let mut layer_map = HashMap::with_capacity(section.layers.len());
        for (index, layer) in section.layers.iter().enumerate() {
            if layer_map.insert(layer.name.clone(), index).is_some() {
                return Err(ModelError::CrossSectionDuplicateLayerName(layer.name.clone()));
            }
        }

        // Validate layer model
        for layer in &section.layers {
            // Validate layers themselves and check if material reference is valid
            layer.validate()?;
            if !material_map.contains_key(&layer.material) {
                return Err(ModelError::CrossSectionInvalidMaterialName(layer.name.clone(), layer.material.clone()));
            }
        }

        // Check validity of the alignment specification
        match &section.alignment {
            LayerAlignment::LayerBack(name) | LayerAlignment::LayerBelly(name) | LayerAlignment::LayerCenter(name) => {
                if !layer_map.contains_key(name) {
                    return Err(ModelError::CrossSectionInvalidLayerName(name.clone()));
                }
            }
            _ => {
                // Other alignment options don't need validation
            }
        }

        // Check whether the total height is zero somewhere along the limb
        // Since we know that the layer control points are positive within the layer and only allowed to be zero at the bounds,
        // we can view each layer as an interval of positive height with either inclusive (height != 0) or exclusive (height == 0) bounds.
        // The union of those intervals should cover the whole limb with exclusive bounds, i.e. the result should be [0, 1].
        let intervals = section.layers.iter().map(|layer| {
            let to_bound = |point: &[f64; 2]| {
                if point[1] == 0.0 {
                    Bound::Exclusive(point[0])
                } else {
                    Bound::Inclusive(point[0])
                }
            };

            Interval {
                lower: to_bound(layer.height.0.first().unwrap()),    // Unwrap okay because number of layer heights has been validated
                upper: to_bound(layer.height.0.last().unwrap()),    // Unwrap okay because number of layer heights has been validated
            }
        }).collect();

        let union = Interval::left_union(intervals);
        if union != Interval::inclusive(0.0, 1.0) {
            return Err(ModelError::CrossSectionZeroCombinedHeight(union.upper.value()));
        }

        Ok((material_map, layer_map))
    }

    pub fn layers(&self) -> &Vec<LayerGeometry> {
        &self.layers
    }

    // Computes the layer boundaries at normalized length n in [0, 1].
    // Also returns the layer heights as a byproduct.
    pub fn layer_bounds(&self, n: f64) -> (DVector<f64>, DVector<f64>) {
        let h = self.layer_heights(n);
        (&self.stacking*&h, h)
    }

    // Computes the section boundaries (belly, back) at normalized length n.
    // Equivalent to first and last layer bound.
    pub fn section_bounds(&self, n: f64) -> (f64, f64) {
        let (y, _) = self.layer_bounds(n);
        (y[0], y[y.len() - 1])
    }

    // Evaluates the heights of the individual layers at normalized length n and returns them as a vector
    pub fn layer_heights(&self, n: f64) -> DVector<f64> {
        DVector::<f64>::from_fn(self.layers.len(), |i, _| {
            self.layers[i].height.value(n, Extrapolation::Constant)
        })
    }
}

impl CrossSection for LayeredCrossSection {
    fn stiffness(&self, n: f64) -> SMatrix<f64, 3, 3> {
        let w = self.width(n);
        let (y, h) = self.layer_bounds(n);

        let Cee = self.layers.iter().map(|layer| {
            let h = layer.height.value(n, Extrapolation::Constant);
            layer.material.youngs_modulus*w*h
        }).sum();

        let Ckk = self.layers.iter().enumerate().map(|(i, layer)| {
            let A = w*h[i];
            let d = (y[i] + y[i+1])/2.0;
            let I = A*(h[i].powi(2)/12.0 + d.powi(2));
            layer.material.youngs_modulus*I
        }).sum();

        let Cek = self.layers.iter().enumerate().map(|(i, layer)| {
            let A = w*h[i];
            let d = (y[i] + y[i+1])/2.0;
            -layer.material.youngs_modulus*A*d
        }).sum();

        let Cγγ = self.layers.iter().map(|layer| {
            let h = layer.height.value(n, Extrapolation::Constant);
            layer.material.shear_modulus*w*h
        }).sum();

        matrix![
            Cee, 0.0, Cek;
            0.0, Cγγ, 0.0;
            Cek, 0.0, Ckk;
        ]
    }

    fn mass(&self, n: f64) -> SMatrix<f64, 3, 3> {
        let w = self.width(n);
        let (y, h) = self.layer_bounds(n);

        let ρA = self.layers.iter().map(|layer| {
            let h = layer.height.value(n, Extrapolation::Constant);
            layer.material.density*w*h
        }).sum();

        let ρI = self.layers.iter().enumerate().map(|(i, layer)| {
            let A = w*h[i];
            let d = (y[i] + y[i+1])/2.0;
            let I = A*(h[i].powi(2)/12.0 + d.powi(2));
            layer.material.density*I
        }).sum();

        // TODO: Even though the beam element ignores them, there should generally be off-diagonal elements that couple translation and rotation
        // Maybe implement and document them for completeness
        matrix![
            ρA, 0.0, 0.0;
            0.0, ρA, 0.0;
            0.0, 0.0, ρI;
        ]
    }

    fn width(&self, n: f64) -> f64 {
        self.width.value(n, Extrapolation::Constant)
    }

    // Total height as the sum of all layers
    fn height(&self, n: f64) -> f64 {
        self.layers.iter().map(|layer| layer.height.value(n, Extrapolation::Constant)).sum()
    }

    // Strain evaluation matrices, two for each layer (back, belly)
    fn strain_recovery(&self, n: f64) -> Vec<SVector<f64, 3>> {
        // Layer bounds are the points of interest
        let (y, _) = self.layer_bounds(n);

        let mut results = Vec::with_capacity(2*self.layers.len());
        for i in 0..self.layers.len() {
            // Check if n lies within the layer, return zero strain otherwise
            if n >= self.layers[i].height.arg_min() && n <= self.layers[i].height.arg_max() {
                results.push(vector![1.0, 0.0, -y[i]]);
                results.push(vector![1.0, 0.0, -y[i+1]]);
            }
            else {
                results.push(SVector::zeros());
                results.push(SVector::zeros());
            }
        }

        results
    }

    // Strain evaluation matrices, two for each layer (back, belly)
    fn stress_recovery(&self, n: f64) -> Vec<SVector<f64, 3>> {
        // Layer bounds are the points of interest
        let (y, _) = self.layer_bounds(n);

        let mut results = Vec::with_capacity(2*self.layers.len());
        for i in 0..self.layers.len() {
            // Check if n lies within the layer, return zero stress otherwise
            if n >= self.layers[i].height.arg_min() && n <= self.layers[i].height.arg_max() {
                results.push(self.layers[i].material.youngs_modulus * vector![1.0, 0.0, -y[i]]);
                results.push(self.layers[i].material.youngs_modulus * vector![1.0, 0.0, -y[i+1]]);
            }
            else {
                results.push(SVector::zeros());
                results.push(SVector::zeros());
            }
        }

        results
    }
}

#[cfg(test)]
mod tests {
    use approx::{assert_abs_diff_eq, assert_relative_eq};
    use assert_matches::assert_matches;
    use nalgebra::{dvector, vector};
    use crate::input::{Height, Layer, Width};
    use super::*;

    #[test]
    fn test_error_conditions() {
        let materials = vec![Material::new("material", "#000000", 7000.0, 200e9, 80.9)];
        let width = Width::constant(0.05);
        let layers = vec![Layer::new("layer", "material", Height::constant(0.02))];
        let section = Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), layers.clone());

        // 1. Valid single layer model
        assert!(LayeredCrossSection::new(&section).is_ok());

        // 2. Invalid width
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, Width::new(vec![]), materials.clone(), layers.clone())), Err(ModelError::WidthControlPointsTooFew(0)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, Width::new(vec![[0.0, 0.05]]), materials.clone(), layers.clone())), Err(ModelError::WidthControlPointsTooFew(1)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, Width::new(vec![[1.0, 0.05], [0.0, 0.05]]), materials.clone(), layers.clone())), Err(ModelError::WidthControlPointsNotSorted(1.0, 0.0)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, Width::new(vec![[0.1, 0.05], [1.0, 0.05]]), materials.clone(), layers.clone())), Err(ModelError::WidthControlPointsInvalidRange(0.1, 1.0)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, Width::new(vec![[0.0, 0.05], [0.9, 0.05]]), materials.clone(), layers.clone())), Err(ModelError::WidthControlPointsInvalidRange(0.0, 0.9)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, Width::new(vec![[0.0, 0.05], [1.0, 0.00]]), materials.clone(), layers.clone())), Err(ModelError::WidthControlPointsInvalidValue(1.0, 0.0)));

        // 3. Invalid layers
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), vec![Layer::new("layer", "material", Height::new(vec![]))])), Err(ModelError::LayerHeightControlPointsTooFew(_, 0)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), vec![Layer::new("layer", "material", Height::new(vec![[0.0, 0.02]]))])), Err(ModelError::LayerHeightControlPointsTooFew(_, 1)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), vec![Layer::new("layer", "material", Height::new(vec![[1.0, 0.02], [0.0, 0.02]]))])), Err(ModelError::LayerHeightControlPointsNotSorted(_, 1.0, 0.0)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), vec![Layer::new("layer", "material", Height::new(vec![[-0.1, 0.02], [1.0, 0.02]]))])), Err(ModelError::LayerHeightControlPointsInvalidRange(_, -0.1, 1.0)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), vec![Layer::new("layer", "material", Height::new(vec![[0.0, 0.02], [1.1, 0.02]]))])), Err(ModelError::LayerHeightControlPointsInvalidRange(_, 0.0, 1.1)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), vec![Layer::new("layer", "material", Height::new(vec![[0.0, 0.0], [0.5, 0.0], [1.0, 0.0]]))])), Err(ModelError::LayerHeightControlPointsInvalidInteriorValue(_, 0.5, 0.0)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), vec![Layer::new("layer", "material", Height::new(vec![[0.0, -0.1], [0.5, 0.02], [1.0, 0.0]]))])), Err(ModelError::LayerHeightControlPointsInvalidBoundaryValue(_, 0.0, -0.1)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), vec![Layer::new("layer", "material", Height::new(vec![[0.1, 0.02], [1.0, 0.02]]))])), Err(ModelError::LayerHeightControlPointsDiscontinuousBoundary(_, 0.1, 0.02)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), vec![Layer::new("layer", "material", Height::new(vec![[0.0, 0.02], [0.9, 0.02]]))])), Err(ModelError::LayerHeightControlPointsDiscontinuousBoundary(_, 0.9, 0.02)));

        // 4. Invalid cross-section
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), vec![])), Err(ModelError::CrossSectionNoLayers));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), vec![], layers.clone())), Err(ModelError::CrossSectionNoMaterials));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), vec![Layer::new("layer", "nonexistent", Height::new(vec![[0.0, 0.02], [1.0, 0.02]]))])), Err(ModelError::CrossSectionInvalidMaterialName(_, _)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::LayerBack("nonexistent".to_string()), width.clone(), materials.clone(), layers.clone())), Err(ModelError::CrossSectionInvalidLayerName(_)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::LayerBelly("nonexistent".to_string()), width.clone(), materials.clone(), layers.clone())), Err(ModelError::CrossSectionInvalidLayerName(_)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::LayerCenter("nonexistent".to_string()), width.clone(), materials.clone(), layers.clone())), Err(ModelError::CrossSectionInvalidLayerName(_)));
        assert_matches!(LayeredCrossSection::new(&Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), vec![Layer::new("layer", "material", Height::new(vec![[0.0, 0.02], [1.0, 0.0]]))])), Err(ModelError::CrossSectionZeroCombinedHeight(1.0)));
    }

    #[test]
    fn test_geometry_single_layer() {
        // Checks the geometry of a single-layer cross-section, i.e. width, height and layer bounds

        let ws = 0.1;              // Width at start
        let we = 0.05;             // Width at end
        let wm = 0.5*(ws + we);    // Width in the middle

        let hs = 0.01;             // Height at start
        let he = 0.005;            // Height at end
        let hm = 0.5*(hs + he);    // Height in the middle

        let materials = vec![Material::new("material", "#000000", 1.0, 1.0, 1.0)];
        let width = Width::linear(ws, we);
        let layers = vec![Layer::new("layer", "material", Height::linear(hs, he))];

        // Section with alignment SectionBack
        let section = Section::new(LayerAlignment::SectionBack, width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![0.0, -hs]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![0.0, -hm]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![0.0, -he]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment SectionBelly
        let section = Section::new(LayerAlignment::SectionBelly, width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![hs, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![hm, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![he, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment SectionCenter
        let section = Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![0.5*hs, -0.5*hs]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![0.5*hm, -0.5*hm]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![0.5*he, -0.5*he]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment LayerBelly (same as SectionBelly since section consists of only one layer)
        let section = Section::new(LayerAlignment::LayerBelly("layer".into()), width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![hs, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![hm, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![he, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment LayerBack (same as SectionBack since section consists of only one layer)
        let section = Section::new(LayerAlignment::LayerBack("layer".into()), width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![0.0, -hs]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![0.0, -hm]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![0.0, -he]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment LayerCenter (same as SectionCenter since section consists of only one layer)
        let section = Section::new(LayerAlignment::LayerCenter("layer".into()), width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![0.5*hs, -0.5*hs]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![0.5*hm, -0.5*hm]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![0.5*he, -0.5*he]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));
    }

    #[test]
    fn test_geometry_multi_layer() {
        // Checks the geometry of a multi-layer cross-section, i.e. width, height and layer bounds

        // Width (start, end, middle)
        let ws = 0.1;
        let we = 0.05;
        let wm = 0.5*(ws + we);

        // Height Layer 0 (start, end, middle)
        let hs0 = 0.011;
        let he0 = 0.0051;
        let hm0 = 0.5*(hs0 + he0);

        // Height Layer 1 (start, end, middle)
        let hs1 = 0.012;
        let he1 = 0.0052;
        let hm1 = 0.5*(hs1 + he1);

        // Height Layer 2 (start, end, middle)
        let hs2 = 0.013;
        let he2 = 0.0053;
        let hm2 = 0.5*(hs2 + he2);

        let materials = vec![Material::new("material", "#000000", 1.0, 1.0, 1.0)];
        let width = Width::linear(ws, we);
        let layers = vec![
            Layer::new("layer_0", "material", Height::linear(hs0, he0)),
            Layer::new("layer_1", "material", Height::linear(hs1, he1)),
            Layer::new("layer_2", "material", Height::linear(hs2, he2))
        ];

        // Section with alignment SectionBack
        let section = Section::new(LayerAlignment::SectionBack, width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![0.0, -hs0, -(hs0 + hs1), -(hs0 + hs1 + hs2)]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![0.0, -hm0, -(hm0 + hm1), -(hm0 + hm1 + hm2)]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![0.0, -he0, -(he0 + he1), -(he0 + he1 + he2)]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment SectionBelly
        let section = Section::new(LayerAlignment::SectionBelly, width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![hs0 + hs1 + hs2, hs1 + hs2, hs2, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![hm0 + hm1 + hm2, hm1 + hm2, hm2, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![he0 + he1 + he2, he1 + he2, he2, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment SectionCenter
        let section = Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![0.5*(hs0 + hs1 + hs2), 0.5*(hs0 + hs1 + hs2) - hs0, 0.5*(hs0 + hs1 + hs2) - hs0 - hs1, 0.5*(hs0 + hs1 + hs2) - hs0 - hs1 - hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![0.5*(hm0 + hm1 + hm2), 0.5*(hm0 + hm1 + hm2) - hm0, 0.5*(hm0 + hm1 + hm2) - hm0 - hm1, 0.5*(hm0 + hm1 + hm2) - hm0 - hm1 - hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![0.5*(he0 + he1 + he2), 0.5*(he0 + he1 + he2) - he0, 0.5*(he0 + he1 + he2) - he0 - he1, 0.5*(he0 + he1 + he2) - he0 - he1 - he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment LayerBack for Layer 0
        let section = Section::new(LayerAlignment::LayerBack("layer_0".into()), width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![0.0, -hs0, -(hs0 + hs1), -(hs0 + hs1 + hs2)]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![0.0, -hm0, -(hm0 + hm1), -(hm0 + hm1 + hm2)]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![0.0, -he0, -(he0 + he1), -(he0 + he1 + he2)]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment LayerBelly for Layer 0
        let section = Section::new(LayerAlignment::LayerBelly("layer_0".into()), width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![hs0, 0.0, -hs1, -(hs1 + hs2)]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![hm0, 0.0, -hm1, -(hm1 + hm2)]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![he0, 0.0, -he1, -(he1 + he2)]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment LayerCenter for Layer 0
        let section = Section::new(LayerAlignment::LayerCenter("layer_0".into()), width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![0.5*hs0, -0.5*hs0, -0.5*hs0 - hs1, -0.5*hs0 - hs1 - hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![0.5*hm0, -0.5*hm0, -0.5*hm0 - hm1, -0.5*hm0 - hm1 - hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![0.5*he0, -0.5*he0, -0.5*he0 - he1, -0.5*he0 - he1 - he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment LayerBack for Layer 1
        let section = Section::new(LayerAlignment::LayerBack("layer_1".into()), width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![hs0, 0.0, -hs1, -hs1 - hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![hm0, 0.0, -hm1, -hm1 - hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![he0, 0.0, -he1, -he1 - he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment LayerBelly for Layer 1
        let section = Section::new(LayerAlignment::LayerBelly("layer_1".into()), width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![hs0 + hs1, hs1, 0.0, -hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![hm0 + hm1, hm1, 0.0, -hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![he0 + he1, he1, 0.0, -he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment LayerCenter for Layer 1
        let section = Section::new(LayerAlignment::LayerCenter("layer_1".into()), width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![0.5*hs1 + hs0, 0.5*hs1, -0.5*hs1, -0.5*hs1 - hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![0.5*hm1 + hm0, 0.5*hm1, -0.5*hm1, -0.5*hm1 - hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![0.5*he1 + he0, 0.5*he1, -0.5*he1, -0.5*he1 - he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment LayerBack for Layer 2
        let section = Section::new(LayerAlignment::LayerBack("layer_2".into()), width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![hs0 + hs1, hs1, 0.0, -hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![hm0 + hm1, hm1, 0.0, -hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![he0 + he1, he1, 0.0, -he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment LayerBelly for Layer 2
        let section = Section::new(LayerAlignment::LayerBelly("layer_2".into()), width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![hs0 + hs1 + hs2, hs1 + hs2, hs2, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![hm0 + hm1 + hm2, hm1 + hm2, hm2, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![he0 + he1 + he2, he1 + he2, he2, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));

        // Section with alignment LayerCenter for Layer 2
        let section = Section::new(LayerAlignment::LayerCenter("layer_2".into()), width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(0.0), ws);
        assert_abs_diff_eq!(section.height(0.0), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(0.0), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).0, dvector![0.5*hs2 + hs1 + hs0, 0.5*hs2 + hs1, 0.5*hs2, -0.5*hs2]);
        assert_abs_diff_eq!(section.layer_bounds(0.0).1, section.layer_heights(0.0));

        // Middle
        assert_abs_diff_eq!(section.width(0.5), wm);
        assert_abs_diff_eq!(section.height(0.5), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(0.5), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).0, dvector![0.5*hm2 + hm1 + hm0, 0.5*hm2 + hm1, 0.5*hm2, -0.5*hm2]);
        assert_abs_diff_eq!(section.layer_bounds(0.5).1, section.layer_heights(0.5));

        // End
        assert_abs_diff_eq!(section.width(1.0), we);
        assert_abs_diff_eq!(section.height(1.0), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(1.0), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).0, dvector![0.5*he2 + he1 + he0, 0.5*he2 + he1, 0.5*he2, -0.5*he2]);
        assert_abs_diff_eq!(section.layer_bounds(1.0).1, section.layer_heights(1.0));
    }

    #[test]
    fn test_properties_single_layer() {
        // Tests whether the cross-section properties of a single, rectangular section match the known analytical expressions
        // TODO: Test shear stiffness and rotary inertia too

        let rho = 7000.0;
        let E = 200e9;
        let G = 80e9;

        let w = 0.05;
        let h = 0.02;

        let width = Width::constant(w);
        let materials = vec![Material::new("material", "#000000", rho, E, G)];
        let layers = vec![Layer::new("layer", "material", Height::constant(h))];

        // Reference point is section center
        let section = Section::new(LayerAlignment::SectionCenter, width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();
        let C_ref = matrix![
            E*w*h, 0.0, 0.0;
            0.0, w*h*G, 0.0;
            0.0, 0.0, E*w*h.powi(3)/12.0;
        ];

        assert_abs_diff_eq!(section.width(0.5), w, epsilon=1e-9);
        assert_abs_diff_eq!(section.height(0.5), h, epsilon=1e-9);
        assert_abs_diff_eq!(section.mass(0.5)[(0, 0)], rho*w*h, epsilon=1e-9);
        assert_abs_diff_eq!(section.mass(0.5)[(1, 1)], rho*w*h, epsilon=1e-9);
        assert_abs_diff_eq!(section.mass(0.5)[(2, 2)], rho*w*h.powi(3)/12.0, epsilon=1e-9);
        assert_abs_diff_eq!(section.stiffness(0.5), C_ref, epsilon=1e-9);

        // Reference point is section back
        let section = Section::new(LayerAlignment::SectionBack, width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();
        let C_ref = matrix![
            E*w*h, 0.0, E*(w*h*0.5*h);
            0.0, w*h*G, 0.0;
            E*(w*h*0.5*h), 0.0, E*(w*h.powi(3)/12.0 + (0.5*h).powi(2)*(w*h));
        ];

        assert_abs_diff_eq!(section.width(0.5), w, epsilon=1e-9);
        assert_abs_diff_eq!(section.height(0.5), h, epsilon=1e-9);
        assert_abs_diff_eq!(section.mass(0.5)[(0, 0)], rho*w*h, epsilon=1e-9);
        assert_abs_diff_eq!(section.mass(0.5)[(1, 1)], rho*w*h, epsilon=1e-9);
        assert_abs_diff_eq!(section.mass(0.5)[(2, 2)], rho*(w*h.powi(3)/12.0 + (0.5*h).powi(2)*(w*h)), epsilon=1e-9);    // TODO: There should be coupling mass entries too
        assert_abs_diff_eq!(section.stiffness(0.5), C_ref, epsilon=1e-9);

        // Reference point is section belly
        let section = Section::new(LayerAlignment::SectionBelly, width.clone(), materials.clone(), layers.clone());
        let section = LayeredCrossSection::new(&section).unwrap();
        let C_ref = matrix![
            E*w*h, 0.0, -E*(w*h*0.5*h);
            0.0, w*h*G, 0.0;
            -E*(w*h*0.5*h), 0.0, E*(w*h.powi(3)/12.0 + (0.5*h).powi(2)*(w*h))
        ];

        assert_abs_diff_eq!(section.width(0.5), w, epsilon=1e-9);
        assert_abs_diff_eq!(section.height(0.5), h, epsilon=1e-9);
        assert_abs_diff_eq!(section.mass(0.5)[(0, 0)], rho*w*h, epsilon=1e-9);
        assert_abs_diff_eq!(section.mass(0.5)[(1, 1)], rho*w*h, epsilon=1e-9);
        assert_abs_diff_eq!(section.mass(0.5)[(2, 2)], rho*(w*h.powi(3)/12.0 + (0.5*h).powi(2)*(w*h)), epsilon=1e-9);    // TODO: There should be coupling mass entries too
        assert_abs_diff_eq!(section.stiffness(0.5), C_ref, epsilon=1e-9);
   }

    #[test]
    fn test_properties_multi_layer_1() {
        // Tests whether the cross-section properties of multiple rectangular layers match an analytical solution
        // TODO: Test shear stiffness and rotary inertia too

        let rho1 = 5000.0;
        let E1 = 300e9;
        let G1 = 120e9;

        let rho2 = 6000.0;
        let E2 = 200e9;
        let G2 = 80e9;

        let rho3 = 7000.0;
        let E3 = 100e9;
        let G3 = 40e9;

        let w = 0.05;
        let h1: f64 = 0.01;
        let h2: f64 = 0.02;
        let h3: f64 = 0.01;

        let A1 = w*h1;
        let A2 = w*h2;
        let A3 = w*h3;

        let y1: f64 = 0.5*(h1 + h2);
        let y2: f64 = 0.0;
        let y3: f64 = -0.5*(h3 + h2);

        let I1 = A1*(h1.powi(2)/12.0 + y1.powi(2));
        let I2 = A2*(h2.powi(2)/12.0 + y2.powi(2));
        let I3 = A3*(h3.powi(2)/12.0 + y3.powi(2));

        let material1 = Material::new("A", "#000000", rho1, E1, G1);
        let material2 = Material::new("B", "#000000", rho2, E2, G2);
        let material3 = Material::new("C", "#000000", rho3, E3, G3);

        let width = Width::constant(w);
        let layer1 = Layer::new("1", "A", Height::constant(h1));
        let layer2 = Layer::new("2", "B", Height::constant(h2));
        let layer3 = Layer::new("3", "C", Height::constant(h3));

        let section = Section::new(LayerAlignment::SectionCenter, width.clone(), vec![material1, material2, material3], vec![layer1, layer2, layer3]);
        let section = LayeredCrossSection::new(&section).unwrap();

        let C_ref = matrix![
            E1*A1 + E2*A2 + E3*A3, 0.0, -E1*A1*y1 - E2*A2*y2 - E3*A3*y3;
            0.0, G1*A1 + G2*A2 + G3*A3, 0.0;
            -E1*A1*y1 - E2*A2*y2 - E3*A3*y3, 0.0, E1*I1 + E2*I2 + E3*I3;
        ];

        assert_abs_diff_eq!(section.width(0.5), w, epsilon=1e-12);
        assert_abs_diff_eq!(section.height(0.5), h1 + h2 + h3, epsilon=1e-12);
        assert_abs_diff_eq!(section.mass(0.5)[(0, 0)], rho1*A1 + rho2*A2 + rho3*A3, epsilon=1e-9);
        assert_abs_diff_eq!(section.mass(0.5)[(1, 1)], rho1*A1 + rho2*A2 + rho3*A3, epsilon=1e-9);
        assert_abs_diff_eq!(section.mass(0.5)[(2, 2)], rho1*I1 + rho2*I2 + rho3*I3, epsilon=1e-9);    // TODO: Should be coupling mass entries too?
        assert_abs_diff_eq!(section.stiffness(0.5), C_ref, epsilon=1e-9);
    }

    #[test]
    fn test_properties_multi_layer_2() {
        // Tests whether the stresses and strains of a multi layered cross-section match reference values taken from the final example
        // in the chapter about composite cross-sections in [1]. Unfortunately the values in the textbook are not given to a very high precision.
        //
        // [1] Dietmar Gross, Werner Hauger, Jörg Schröder, and Wolfgang A. Wall. Technische Mechanik 2: - Elastostatik (Springer-Lehrbuch).
        // Springer, Berlin, 11 edition, 2011.

        // Parameters
        let rho = 1.0;
        let Ec = 10e9;
        let Es = 5e9;

        let w = 0.40;
        let h = 0.08;

        let N = -500e3;
        let M = 60.0e3;

        // Reference strains and stresses
        let epsilon_o = -10.18e-4;
        let epsilon_u = 2.38e-4;
        let sigma_ref = dvector![-10.18e6, -7.66e6, -3.83e6, -2.58e6, -5.16e6, -2.64e6, -1.32e6, -0.07e6, -0.14e6, 2.38e6];

        let material_c = Material::new("C", "#000000", rho, Ec, 0.5*Ec);
        let material_s = Material::new("S", "#000000", rho, Es, 0.5*Es);

        let width = Width::constant(w);
        let layer1 = Layer::new("1", "C", Height::constant(h));
        let layer2 = Layer::new("2", "S", Height::constant(h));
        let layer3 = Layer::new("3", "C", Height::constant(h));
        let layer4 = Layer::new("4", "S", Height::constant(h));
        let layer5 = Layer::new("5", "C", Height::constant(h));

        let section = Section::new(LayerAlignment::SectionCenter, width.clone(), vec![material_s, material_c], vec![layer1, layer2, layer3, layer4, layer5]);
        let section = LayeredCrossSection::new(&section).unwrap();

        // Determine generalized strains from normal force and torque by inverting/solving the stiffness relation
        let strains = section.stiffness(0.5).qr().solve(&vector![N, 0.0, M]).unwrap();

        // Evaluate and check section normal strains
        let recovery = section.strain_recovery(0.5);
        let results = recovery.iter().map(|r| r.dot(&strains)).collect_vec();

        assert_relative_eq!(results[0], epsilon_o, max_relative=1e-2);
        assert_relative_eq!(results[9], epsilon_u, max_relative=1e-2);

        // Evaluate and check section normal stresses
        let recovery = section.stress_recovery(0.5);
        let results = DVector::<f64>::from_iterator(recovery.len(), recovery.iter().map(|r| r.dot(&strains)));

        assert_relative_eq!(results, sigma_ref, max_relative=1e-2);
    }
}