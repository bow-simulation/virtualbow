use std::collections::HashMap;
use itertools::Itertools;
use nalgebra::{DMatrix, DVector, matrix, SMatrix};
use serde::{Deserialize, Serialize};
use crate::bow::errors::ModelError;
use crate::bow::input::{Layer, Material, Width};
use crate::fem::elements::beam::geometry::CrossSection;
use crate::numerics::cubic_spline::{BoundaryCondition, CubicSpline, Extrapolation};
use crate::numerics::intervals::{Bound, Interval};

// Defines, how the layers are aligned with the profile curve
// There are two categories:
// - Section: The back side, belly side, or geometrical center of the combined section is aligned with the profile curve
// - Layer: The back side, belly side, or geometrical center of the layer with the given name is aligned with the profile curve.
#[derive(Serialize, Deserialize, Clone, PartialEq, Debug)]
pub enum LayerAlignment {
    #[serde(rename = "section-back")]
    SectionBack,

    #[serde(rename = "section-belly")]
    SectionBelly,

    #[serde(rename = "section-center")]
    SectionCenter,

    #[serde(rename = "layer-back")]
    LayerBack(String),

    #[serde(rename = "layer-belly")]
    LayerBelly(String),

    #[serde(rename = "layer-center")]
    LayerCenter(String)
}

#[derive(Debug)]
pub struct LayerGeometry {
    pub height: CubicSpline,
    pub material: Material,
}

#[derive(Debug)]
pub struct LayeredCrossSection {
    length: f64,
    width: CubicSpline,
    layers: Vec<LayerGeometry>,
    stacking: DMatrix<f64>
}

impl LayeredCrossSection {
    pub fn new(length: f64, width: &Width, layers: &Vec<Layer>, materials: &Vec<Material>, alignment: &LayerAlignment) -> Result<Self, ModelError> {
        // Check inputs for validity and return error on failure.
        // Also builds two hashmaps (material name) -> (material) and (layer name) -> (index) in the process.
        let (material_map, layer_map) = Self::validate(length, width, layers, materials, alignment)?;

        // Construct width spline
        let width = CubicSpline::from_points(&width.points, true, BoundaryCondition::SecondDerivative(0.0), BoundaryCondition::SecondDerivative(0.0));

        // Construct layer geometries
        let layers = layers.iter().map(|layer| {
            let height = CubicSpline::from_points(&layer.height, true, BoundaryCondition::SecondDerivative(0.0), BoundaryCondition::SecondDerivative(0.0));
            LayerGeometry {
                height,
                material: material_map.get(&layer.material).unwrap().clone()    // Unwrap because validity has been checked previously
            }
        }).collect_vec();

        // Some useful constants
        // A stack of n layers produces k = n + 1 layer boundaries
        let n = layers.len();
        let k = n + 1;

        // Initially we align the first layer boundary y[0], which is the belly of the section, at coordinate zero.
        // The other layer boundaries follow by partially summing up the heights up to that layer:
        //
        // y[0] = 0
        // y[1] = h[0]
        // y[2] = h[0] + h[1]
        // ...
        // y[k-1] = h[0] + h[1] + ... + h[n-1]
        //
        // We can write this as the matrix multiplication y = A*h,
        // where A is a k x n matrix with a lower triangular part filled with 1.
        // We call A the stacking matrix, because it contains the logic how the layers are stacked together.
        let mut stacking = DMatrix::<f64>::zeros(k, n);
        stacking.fill_lower_triangle(1.0, 1);

        // The actual y positions are shifted by -y*, where y* is the position where the profile curve is aligned with the section.
        // Depending on the kind of alignment, the matrix is modified in order to include this shift.
        match alignment {
            // The back of the section is located at y* = y[k-1] = h[0] + h[1] + ... + h[n-1].
            // Subtracting y* is therefore equivalent to subtracting 1 from every matrix element.
            LayerAlignment::SectionBack => {
                stacking.add_scalar_mut(-1.0)
            },

            // The belly of the section is located at y* = y[0] = 0.
            // Therefore nothing has to be subtracted and the matrix is not modified
            LayerAlignment::SectionBelly => {
                // Do nothing
            },

            // The section center is located at y* = 0.5*(y[0] + y[k-1]) = 0.5*h[0] + 0.5*h[1] + ... + 0.5*h[n-1].
            // Subtracting y* is therefore equivalent to subtracting 0.5 from every matrix element.
            LayerAlignment::SectionCenter => {
                stacking.add_scalar_mut(-0.5)
            },

            // The back of layer i is located at y* = y[i+1] = h[0] + h[1] + ... + h[i]
            // Subtracting y* is equivalent to subtracting 1 from columns 0 to i+1.
            LayerAlignment::LayerBack(name) => {
                let i = *layer_map.get(name).unwrap();    // Unwrap because validity has been checked previously
                stacking.view_mut((0, 0), (k, i+1)).add_scalar_mut(-1.0);
            }

            // The belly of layer i is located at y* = y[i] = h[0] + h[1] + ... + h[i-1]
            // Subtracting y* is equivalent to subtracting 1 from columns 0 to i.
            LayerAlignment::LayerBelly(name) => {
                let i = *layer_map.get(name).unwrap();    // Unwrap because validity has been checked previously
                stacking.view_mut((0, 0), (k, i)).add_scalar_mut(-1.0);
            }

            // The center of layer i is located at y* = 0.5*(y[i] + y[i+1]) = h[0] + h[1] + ... + 0.5*h[i]
            // Subtracting y* is equivalent to subtracting 1 from columns 0 to i and 0.5 from column i+1.
            LayerAlignment::LayerCenter(name) => {
                let i = *layer_map.get(name).unwrap();    // Unwrap because validity has been checked previously
                stacking.view_mut((0, 0), (k, i)).add_scalar_mut(-1.0);
                stacking.view_mut((0, i), (k, 1)).add_scalar_mut(-0.5);
            }
        }

        Ok(Self {
            length,
            width,
            layers,
            stacking
        })
    }

    #[allow(clippy::type_complexity)]  // TODO: Maybe improve later?
    fn validate(length: f64, width: &Width, layers: &Vec<Layer>, materials: &Vec<Material>, alignment: &LayerAlignment) -> Result<(HashMap<String, Material>, HashMap<String, usize>), ModelError> {
        // Length: Must be positive and finite
        if !length.is_finite() || length <= 0.0 {
            return Err(ModelError::CrossSectionInvalidLength(length));
        }

        // Validate width model
        width.validate()?;

        // Check if there is at least one layer
        if layers.is_empty() {
            return Err(ModelError::CrossSectionNoLayers);
        }

        // Check if there is at least one material
        if materials.is_empty() {
            return Err(ModelError::CrossSectionNoMaterials);
        }

        // Validate materials individually
        for material in materials {
            material.validate()?;
        }

        // Build map from material name to material, error if names are duplicated
        let mut material_map = HashMap::with_capacity(materials.len());
        for material in materials {
            if material_map.insert(material.name.clone(), material.clone()).is_some() {
                return Err(ModelError::CrossSectionDuplicateMaterialName(material.name.clone()));
            }
        }

        // Build map from layer name to index, error if names are duplicated
        let mut layer_map = HashMap::with_capacity(layers.len());
        for (index, layer) in layers.iter().enumerate() {
            if layer_map.insert(layer.name.clone(), index).is_some() {
                return Err(ModelError::CrossSectionDuplicateLayerName(layer.name.clone()));
            }
        }

        // Validate layer model
        for layer in layers {
            // Validate layers themselves and check if material reference is valid
            layer.validate()?;
            if !material_map.contains_key(&layer.material) {
                return Err(ModelError::CrossSectionInvalidMaterialName(layer.name.clone(), layer.material.clone()));
            }
        }

        // Check validity of the alignment specification
        match alignment {
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
        let intervals = layers.iter().map(|layer| {
            let to_bound = |point: &(f64, f64)| {
                if point.1 == 0.0 {
                    Bound::Exclusive(point.0)
                } else {
                    Bound::Inclusive(point.0)
                }
            };

            Interval {
                lower: to_bound(layer.height.first().unwrap()),
                upper: to_bound(layer.height.last().unwrap()),
            }
        }).collect();

        let union = Interval::left_union(intervals);
        if union != Interval::inclusive(0.0, 1.0) {
            return Err(ModelError::CrossSectionZeroCombinedHeight(union.upper.value()));
        }

        Ok((material_map, layer_map))
    }

    pub fn length(&self) -> f64 {
        self.length
    }

    pub fn layers(&self) -> &Vec<LayerGeometry> {
        &self.layers
    }

    // Computes the layer boundaries at arc length s. Also returns the heights as a by product.
    pub fn layer_bounds(&self, s: f64) -> (DVector<f64>, DVector<f64>) {
        let h = self.layer_heights(s);
        (&self.stacking*&h, h)
    }

    // Evaluates the heights of the individual layers at arc length s and returns them as a vector
    pub fn layer_heights(&self, s: f64) -> DVector<f64> {
        let p = s/self.length;
        DVector::<f64>::from_fn(self.layers.len(), |i, _| {
            self.layers[i].height.value(p, Extrapolation::Constant)
        })
    }
}

impl CrossSection for LayeredCrossSection {
    fn ρA(&self, s: f64) -> f64 {
        let p = s/self.length;
        let w = self.width.value(p, Extrapolation::Constant);
        self.layers.iter().map(|layer| {
            let h = layer.height.value(p, Extrapolation::Constant);
            layer.material.rho*w*h
        }).sum()
    }

    fn rhoI(&self, s: f64) -> f64 {
        let w = self.width(s);
        let (y, h) = self.layer_bounds(s);

        self.layers.iter().enumerate().map(|(i, layer)| {
            let A = w*h[i];
            let d = (y[i] + y[i+1])/2.0;
            let I = A*(h[i].powi(2)/12.0 + d.powi(2));
            layer.material.rho*I
        }).sum()
    }

    fn C(&self, s: f64) -> SMatrix<f64, 3, 3> {
        let p = s/self.length;
        let w = self.width(s);
        let (y, h) = self.layer_bounds(s);

        let Cee = self.layers.iter().map(|layer| {
            let h = layer.height.value(p, Extrapolation::Constant);
            layer.material.E*w*h
        }).sum();

        let Ckk = self.layers.iter().enumerate().map(|(i, layer)| {
            let A = w*h[i];
            let d = (y[i] + y[i+1])/2.0;
            let I = A*(h[i].powi(2)/12.0 + d.powi(2));
            layer.material.E*I
        }).sum();

        let Cek = self.layers.iter().enumerate().map(|(i, layer)| {
            let A = w*h[i];
            let d = (y[i] + y[i+1])/2.0;
            -layer.material.E*A*d
        }).sum();

        let Cγγ = self.layers.iter().map(|layer| {
            let h = layer.height.value(p, Extrapolation::Constant);
            layer.material.G*w*h
        }).sum();

        matrix![
            Cee, Cek, 0.0;
            Cek, Ckk, 0.0;
            0.0, 0.0, Cγγ
        ]
    }

    fn width(&self, s: f64) -> f64 {
        let p = s/self.length;
        self.width.value(p, Extrapolation::Constant)
    }

    // Total height as the sum of all layers
    fn height(&self, s: f64) -> f64 {
        let p = s/self.length;
        self.layers.iter().map(|layer| layer.height.value(p, Extrapolation::Constant)).sum()
    }

    // Strain evaluation matrix. Produces normal strains at back and belly of each layer.
    fn strain_eval(&self, s: f64) -> DMatrix<f64> {
        // Layer bounds are the points of interest
        let (y, _) = self.layer_bounds(s);

        // Normal strain is epsilon - kappa*y
        DMatrix::from_fn(2*self.layers.len(), 3, |i, j| {
            let k = (i+1)/2;  // Index of the current y position
            match j {
                0 =>   1.0,    // Factor for epsilon
                1 => -y[k],    // Factor for kappa
                _ =>   0.0     // Factor for gamma
            }
        })
    }

    // Stress evaluation matrix. Produces stresses at back and belly of each layer.
    fn stress_eval(&self, s: f64) -> DMatrix<f64> {
        // Layer bounds are the points of interest
        let (y, _) = self.layer_bounds(s);

        // Normal stress is E*(epsilon - kappa*y)
        DMatrix::from_fn(2*self.layers.len(), 3, |i, j| {
            let k = (i+1)/2;  // Index of the current y position
            let l = i/2;      // Index of the current layer

            self.layers[l].material.E * match j {
                0 =>   1.0,    // Factor for epsilon
                1 => -y[k],    // Factor for kappa
                _ =>   0.0     // Factor for gamma
            }
        })
    }
}

#[cfg(test)]
mod tests {
    use nalgebra::{dvector, vector};
    use super::*;

    #[test]
    fn test_error_conditions() {
        let materials = vec![Material::new("material", "#000000", 7000.0, 200e9, 80.9)];
        let width = Width::new(vec![(0.0, 0.05), (1.0, 0.05)]);
        let layers = vec![Layer::new("layer", "material", vec![(0.0, 0.02), (1.0, 0.02)])];

        // 1. Valid single layer model
        assert!(LayeredCrossSection::new(1.5, &width, &layers, &materials, &LayerAlignment::SectionCenter).is_ok());

        // 2. Invalid length
        assert_matches!(LayeredCrossSection::new(0.0, &width, &layers, &materials, &LayerAlignment::SectionCenter), Err(ModelError::CrossSectionInvalidLength(0.0)));
        assert_matches!(LayeredCrossSection::new(-1.0, &width, &layers, &materials, &LayerAlignment::SectionCenter), Err(ModelError::CrossSectionInvalidLength(-1.0)));
        assert_matches!(LayeredCrossSection::new(f64::INFINITY, &width, &layers, &materials, &LayerAlignment::SectionCenter), Err(ModelError::CrossSectionInvalidLength(f64::INFINITY)));

        // 3. Invalid width
        assert_matches!(LayeredCrossSection::new(1.5, &Width::new(vec![]), &layers, &materials, &LayerAlignment::SectionCenter), Err(ModelError::WidthControlPointsTooFew(0)));
        assert_matches!(LayeredCrossSection::new(1.5, &Width::new(vec![(0.0, 0.05)]), &layers, &materials, &LayerAlignment::SectionCenter), Err(ModelError::WidthControlPointsTooFew(1)));
        assert_matches!(LayeredCrossSection::new(1.5, &Width::new(vec![(1.0, 0.05), (0.0, 0.05)]), &layers, &materials, &LayerAlignment::SectionCenter), Err(ModelError::WidthControlPointsNotSorted(1.0, 0.0)));
        assert_matches!(LayeredCrossSection::new(1.5, &Width::new(vec![(0.1, 0.05), (1.0, 0.05)]), &layers, &materials, &LayerAlignment::SectionCenter), Err(ModelError::WidthControlPointsInvalidRange(0.1, 1.0)));
        assert_matches!(LayeredCrossSection::new(1.5, &Width::new(vec![(0.0, 0.05), (0.9, 0.05)]), &layers, &materials, &LayerAlignment::SectionCenter), Err(ModelError::WidthControlPointsInvalidRange(0.0, 0.9)));
        assert_matches!(LayeredCrossSection::new(1.5, &Width::new(vec![(0.0, 0.05), (1.0, 0.00)]), &layers, &materials, &LayerAlignment::SectionCenter), Err(ModelError::WidthControlPointsInvalidValue(1.0, 0.0)));

        // 4. Invalid layers
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("layer", "material", vec![])], &materials, &LayerAlignment::SectionCenter), Err(ModelError::LayerHeightControlPointsTooFew(0)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("layer", "material", vec![(0.0, 0.02)])], &materials, &LayerAlignment::SectionCenter), Err(ModelError::LayerHeightControlPointsTooFew(1)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("layer", "material", vec![(1.0, 0.02), (0.0, 0.02)])], &materials, &LayerAlignment::SectionCenter), Err(ModelError::LayerHeightControlPointsNotSorted(1.0, 0.0)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("layer", "material", vec![(-0.1, 0.02), (1.0, 0.02)])], &materials, &LayerAlignment::SectionCenter), Err(ModelError::LayerHeightControlPointsInvalidRange(-0.1, 1.0)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("layer", "material", vec![(0.0, 0.02), (1.1, 0.02)])], &materials, &LayerAlignment::SectionCenter), Err(ModelError::LayerHeightControlPointsInvalidRange(0.0, 1.1)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("layer", "material", vec![(0.0, 0.0), (0.5, 0.0), (1.0, 0.0)])], &materials, &LayerAlignment::SectionCenter), Err(ModelError::LayerHeightControlPointsInvalidInteriorValue(0.5, 0.0)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("layer", "material", vec![(0.0, -0.1), (0.5, 0.02), (1.0, 0.0)])], &materials, &LayerAlignment::SectionCenter), Err(ModelError::LayerHeightControlPointsInvalidBoundaryValue(0.0, -0.1)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("layer", "material", vec![(0.1, 0.02), (1.0, 0.02)])], &materials, &LayerAlignment::SectionCenter), Err(ModelError::LayerHeightControlPointsDiscontinuousBoundary(0.1, 0.02)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("layer", "material", vec![(0.0, 0.02), (0.9, 0.02)])], &materials, &LayerAlignment::SectionCenter), Err(ModelError::LayerHeightControlPointsDiscontinuousBoundary(0.9, 0.02)));

        // 5. Invalid cross section
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![], &materials, &LayerAlignment::SectionCenter), Err(ModelError::CrossSectionNoLayers));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &layers, &vec![], &LayerAlignment::SectionCenter), Err(ModelError::CrossSectionNoMaterials));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("layer", "nonexistent", vec![(0.0, 0.02), (1.0, 0.02)])], &materials, &LayerAlignment::SectionCenter), Err(ModelError::CrossSectionInvalidMaterialName(_, _)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &layers, &materials, &LayerAlignment::LayerBack("nonexistent".to_string())), Err(ModelError::CrossSectionInvalidLayerName(_)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &layers, &materials, &LayerAlignment::LayerBelly("nonexistent".to_string())), Err(ModelError::CrossSectionInvalidLayerName(_)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &layers, &materials, &LayerAlignment::LayerCenter("nonexistent".to_string())), Err(ModelError::CrossSectionInvalidLayerName(_)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("layer", "material", vec![(0.0, 0.02), (1.0, 0.0)])], &materials, &LayerAlignment::SectionCenter), Err(ModelError::CrossSectionZeroCombinedHeight(1.0)));
    }

    #[test]
    fn test_geometry_single_layer() {
        // Checks the geometry of a single-layer cross section, i.e. width, height and layer bounds

        let l  = 1.25;             // Length
        let ss = 0.0;              // Start
        let se = l;                // End
        let sm = 0.5*(ss + se);    // Middle

        let ws = 0.1;
        let we = 0.05;
        let wm = 0.5*(ws + we);

        let hs = 0.01;
        let he = 0.005;
        let hm = 0.5*(hs + he);

        let materials = vec![Material::new("material", "#000000", 1.0, 1.0, 1.0)];
        let width = Width::new(vec![(0.0, ws), (1.0, we)]);
        let layers = vec![Layer::new("layer", "material", vec![(0.0, hs), (1.0, he)])];

        // Section with alignment SectionBelly
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::SectionBelly).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![0.0, hs]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![0.0, hm]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![0.0, he]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment SectionBack
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::SectionBack).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-hs, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-hm, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-he, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment SectionCenter
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::SectionCenter).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-0.5*hs, 0.5*hs]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-0.5*hm, 0.5*hm]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-0.5*he, 0.5*he]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment LayerBelly (same as SectionBelly since section consists of only one layer)
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::LayerBelly("layer".into())).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![0.0, hs]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![0.0, hm]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![0.0, he]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment LayerBack (same as SectionBack since section consists of only one layer)
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::LayerBack("layer".into())).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-hs, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-hm, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-he, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment LayerCenter (same as SectionCenter since section consists of only one layer)
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::LayerCenter("layer".into())).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-0.5*hs, 0.5*hs]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-0.5*hm, 0.5*hm]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-0.5*he, 0.5*he]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));
    }

    #[test]
    fn test_geometry_multi_layer() {
        // Checks the geometry of a multi-layer cross section, i.e. width, height and layer bounds

        let l  = 1.25;             // Length
        let ss = 0.0;              // Start
        let se = l;                // End
        let sm = 0.5*(ss + se);    // Middle

        // Width
        let ws = 0.1;
        let we = 0.05;
        let wm = 0.5*(ws + we);

        // Height Layer 0
        let hs0 = 0.011;
        let he0 = 0.0051;
        let hm0 = 0.5*(hs0 + he0);

        // Height Layer 1
        let hs1 = 0.012;
        let he1 = 0.0052;
        let hm1 = 0.5*(hs1 + he1);

        // Height Layer 2
        let hs2 = 0.013;
        let he2 = 0.0053;
        let hm2 = 0.5*(hs2 + he2);

        let materials = vec![Material::new("material", "#000000", 1.0, 1.0, 1.0)];
        let width = Width::new(vec![(0.0, ws), (1.0, we)]);
        let layers = vec![
            Layer::new("layer_0", "material", vec![(0.0, hs0), (1.0, he0)]),
            Layer::new("layer_1", "material", vec![(0.0, hs1), (1.0, he1)]),
            Layer::new("layer_2", "material", vec![(0.0, hs2), (1.0, he2)])
        ];

        // Section with alignment SectionBelly
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::SectionBelly).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![0.0, hs0, hs0 + hs1, hs0 + hs1 + hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![0.0, hm0, hm0 + hm1, hm0 + hm1 + hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![0.0, he0, he0 + he1, he0 + he1 + he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment SectionBack
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::SectionBack).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-hs2 - hs1 - hs0, -hs2 - hs1, -hs2, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-hm2 - hm1 - hm0, -hm2 - hm1, -hm2, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-he2 - he1 - he0, -he2 - he1, -he2, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment SectionCenter
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::SectionCenter).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-0.5*(hs0 + hs1 + hs2), hs0 - 0.5*(hs0 + hs1 + hs2), hs0 + hs1 - 0.5*(hs0 + hs1 + hs2), hs0 + hs1 + hs2 - 0.5*(hs0 + hs1 + hs2)]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-0.5*(hm0 + hm1 + hm2), hm0 - 0.5*(hm0 + hm1 + hm2), hm0 + hm1 - 0.5*(hm0 + hm1 + hm2), hm0 + hm1 + hm2 - 0.5*(hm0 + hm1 + hm2)]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-0.5*(he0 + he1 + he2), he0 - 0.5*(he0 + he1 + he2), he0 + he1 - 0.5*(he0 + he1 + he2), he0 + he1 + he2 - 0.5*(he0 + he1 + he2)]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment LayerBelly for Layer 0
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::LayerBelly("layer_0".into())).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![0.0, hs0, hs0 + hs1, hs0 + hs1 + hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![0.0, hm0, hm0 + hm1, hm0 + hm1 + hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![0.0, he0, he0 + he1, he0 + he1 + he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment LayerBack for Layer 0
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::LayerBack("layer_0".into())).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-hs0, 0.0, hs1, hs1 + hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-hm0, 0.0, hm1, hm1 + hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-he0, 0.0, he1, he1 + he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment LayerCenter for Layer 0
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::LayerCenter("layer_0".into())).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-0.5*hs0, 0.5*hs0, 0.5*hs0 + hs1, 0.5*hs0 + hs1 + hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-0.5*hm0, 0.5*hm0, 0.5*hm0 + hm1, 0.5*hm0 + hm1 + hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-0.5*he0, 0.5*he0, 0.5*he0 + he1, 0.5*he0 + he1 + he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment LayerBelly for Layer 1
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::LayerBelly("layer_1".into())).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-hs0, 0.0, hs1, hs1 + hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-hm0, 0.0, hm1, hm1 + hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-he0, 0.0, he1, he1 + he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment LayerBack for Layer 1
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::LayerBack("layer_1".into())).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-(hs0 + hs1), -hs1, 0.0, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-(hm0 + hm1), -hm1, 0.0, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-(he0 + he1), -he1, 0.0, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment LayerCenter for Layer 1
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::LayerCenter("layer_1".into())).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-0.5*hs1 - hs0, -0.5*hs1, 0.5*hs1, 0.5*hs1 + hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-0.5*hm1 - hm0, -0.5*hm1, 0.5*hm1, 0.5*hm1 + hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-0.5*he1 - he0, -0.5*he1, 0.5*he1, 0.5*he1 + he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment LayerBelly for Layer 2
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::LayerBelly("layer_2".into())).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-hs1 - hs0, -hs1, 0.0, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-hm1 - hm0, -hm1, 0.0, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-he1 - he0, -he1, 0.0, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment LayerBack for Layer 2
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::LayerBack("layer_2".into())).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-hs2 - hs1 - hs0, -hs2 - hs1, -hs2, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-hm2 - hm1 - hm0, -hm2 - hm1, -hm2, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-he2 - he1 - he0, -he2 - he1, -he2, 0.0]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));

        // Section with alignment LayerCenter for Layer 2
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::LayerCenter("layer_2".into())).unwrap();

        // Start
        assert_abs_diff_eq!(section.width(ss), ws);
        assert_abs_diff_eq!(section.height(ss), hs0 + hs1 + hs2);
        assert_abs_diff_eq!(section.layer_heights(ss), dvector![hs0, hs1, hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).0, dvector![-0.5*hs2 - hs1 - hs0, -0.5*hs2 - hs1, -0.5*hs2, 0.5*hs2]);
        assert_abs_diff_eq!(section.layer_bounds(ss).1, section.layer_heights(ss));

        // Middle
        assert_abs_diff_eq!(section.width(sm), wm);
        assert_abs_diff_eq!(section.height(sm), hm0 + hm1 + hm2);
        assert_abs_diff_eq!(section.layer_heights(sm), dvector![hm0, hm1, hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).0, dvector![-0.5*hm2 - hm1 - hm0, -0.5*hm2 - hm1, -0.5*hm2, 0.5*hm2]);
        assert_abs_diff_eq!(section.layer_bounds(sm).1, section.layer_heights(sm));

        // End
        assert_abs_diff_eq!(section.width(se), we);
        assert_abs_diff_eq!(section.height(se), he0 + he1 + he2);
        assert_abs_diff_eq!(section.layer_heights(se), dvector![he0, he1, he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).0, dvector![-0.5*he2 - he1 - he0, -0.5*he2 - he1, -0.5*he2, 0.5*he2]);
        assert_abs_diff_eq!(section.layer_bounds(se).1, section.layer_heights(se));
    }

    #[test]
    fn test_properties_single_layer() {
        // Tests whether the cross section properties of a single, rectangular section match the known analytical expressions
        // TODO: Test shear stiffness and rotary inertia too

        let rho = 7000.0;
        let E = 200e9;
        let G = 80e9;

        let l = 1.00;
        let s = 0.50;
        let w = 0.05;
        let h = 0.02;

        let width = Width::new(vec![(0.0, w), (1.0, w)]);
        let materials = vec![Material::new("material", "#000000", rho, E, G)];
        let layers = vec![Layer::new("layer", "material", vec![(0.0, h), (1.0, h)])];

        // Reference point is section center
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::SectionCenter).unwrap();
        let C_ref = matrix![
            E*w*h, 0.0, 0.0;
            0.0, E*w*h.powi(3)/12.0, 0.0;
            0.0, 0.0, w*h*G
        ];

        assert_abs_diff_eq!(section.width(s), w, epsilon=1e-9);
        assert_abs_diff_eq!(section.height(s), h, epsilon=1e-9);
        assert_abs_diff_eq!(section.ρA(s), rho*w*h, epsilon=1e-9);
        assert_abs_diff_eq!(section.C(s), C_ref, epsilon=1e-9);

        // Reference point is section back
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::SectionBack).unwrap();
        let C_ref = matrix![
            E*w*h, E*(w*h*0.5*h), 0.0;
            E*(w*h*0.5*h), E*(w*h.powi(3)/12.0 + (0.5*h).powi(2)*(w*h)), 0.0;
            0.0, 0.0, w*h*G
        ];

        assert_abs_diff_eq!(section.width(s), w, epsilon=1e-9);
        assert_abs_diff_eq!(section.height(s), h, epsilon=1e-9);
        assert_abs_diff_eq!(section.ρA(s), rho*w*h, epsilon=1e-9);
        assert_abs_diff_eq!(section.C(s), C_ref, epsilon=1e-9);

        // Reference point is section belly
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, &LayerAlignment::SectionBelly).unwrap();
        let C_ref = matrix![
            E*w*h, -E*(w*h*0.5*h), 0.0;
            -E*(w*h*0.5*h), E*(w*h.powi(3)/12.0 + (0.5*h).powi(2)*(w*h)), 0.0;
            0.0, 0.0, w*h*G
        ];

        assert_abs_diff_eq!(section.width(s), w, epsilon=1e-9);
        assert_abs_diff_eq!(section.height(s), h, epsilon=1e-9);
        assert_abs_diff_eq!(section.ρA(s), rho*w*h, epsilon=1e-9);
        assert_abs_diff_eq!(section.C(s), C_ref, epsilon=1e-9);
   }

    #[test]
    fn test_properties_multi_layer_1() {
        // Tests whether the cross section properties of multiple rectangular layers match an analytical solution
        // TODO: Test shear stiffness and rotary inertia too

        let rho1 = 7000.0;
        let E1 = 100e9;
        let G1 = 40e9;

        let rho2 = 6000.0;
        let E2 = 200e9;
        let G2 = 80e9;

        let rho3 = 5000.0;
        let E3 = 300e9;
        let G3 = 120e9;

        let s = 0.50;
        let l = 1.00;
        let w = 0.05;

        let h1: f64 = 0.01;
        let h2: f64 = 0.02;
        let h3: f64 = 0.01;

        let A1 = w*h1;
        let A2 = w*h2;
        let A3 = w*h3;

        let y1: f64 = -0.5*(h1 + h2);
        let y2: f64 = 0.0;
        let y3: f64 = 0.5*(h3 + h2);

        let I1 = A1*(h1.powi(2)/12.0 + y1.powi(2));
        let I2 = A2*(h2.powi(2)/12.0 + y2.powi(2));
        let I3 = A3*(h3.powi(2)/12.0 + y3.powi(2));

        let material1 = Material::new("A", "#000000", rho1, E1, G1);
        let material2 = Material::new("B", "#000000", rho2, E2, G2);
        let material3 = Material::new("C", "#000000", rho3, E3, G3);

        let width = Width::new(vec![(0.0, w), (1.0, w)]);
        let layer1 = Layer::new("1", "A", vec![(0.0, h1), (1.0, h1)]);
        let layer2 = Layer::new("2", "B", vec![(0.0, h2), (1.0, h2)]);
        let layer3 = Layer::new("3", "C", vec![(0.0, h3), (1.0, h3)]);

        let section = LayeredCrossSection::new(l, &width, &vec![layer1, layer2, layer3], &vec![material1, material2, material3], &LayerAlignment::SectionCenter).unwrap();
        let C_ref = matrix![
            E1*A1 + E2*A2 + E3*A3, -E1*A1*y1 - E2*A2*y2 - E3*A3*y3, 0.0;
            -E1*A1*y1 - E2*A2*y2 - E3*A3*y3, E1*I1 + E2*I2 + E3*I3, 0.0;
            0.0, 0.0, G1*A1 + G2*A2 + G3*A3
        ];

        assert_abs_diff_eq!(section.width(s), w, epsilon=1e-12);
        assert_abs_diff_eq!(section.height(s), h1 + h2 + h3, epsilon=1e-12);
        assert_abs_diff_eq!(section.ρA(s), rho1*A1 + rho2*A2 + rho3*A3, epsilon=1e-12);
        assert_abs_diff_eq!(section.C(s), C_ref, epsilon=1e-9);
    }

    #[test]
    fn test_properties_multi_layer_2() {
        // Tests whether the stresses and strains of a multi layered cross section match reference values taken from the final example
        // in the chapter about composite cross sections in [1]. Unfortunately the values in the textbook are not given to a very high precision.
        //
        // [1] Dietmar Gross, Werner Hauger, Jörg Schröder, and Wolfgang A. Wall. Technische Mechanik 2: - Elastostatik (Springer-Lehrbuch).
        // Springer, Berlin, 11 edition, 2011.

        // Parameters
        let rho = 1.0;
        let Ec = 10e9;
        let Es = 5e9;

        let s = 0.50;
        let l = 1.00;
        let w = 0.40;
        let h = 0.08;

        let N = -500e3;
        let M = 60.0e3;

        // Reference strains and stresses
        let epsilon_o = -10.18e-4;
        let epsilon_u = 2.38e-4;
        let sigma_ref = dvector![2.38e6, -0.14e6, -0.07e6, -1.32e6, -2.64e6, -5.16e6, -2.58e6, -3.83e6, -7.66e6, -10.18e6];

        let material_c = Material::new("C", "#000000", rho, Ec, 0.5*Ec);
        let material_s = Material::new("S", "#000000", rho, Es, 0.5*Es);

        let width = Width::new(vec![(0.0, w), (1.0, w)]);
        let layer1 = Layer::new("1", "C", vec![(0.0, h), (1.0, h)]);
        let layer2 = Layer::new("2", "S", vec![(0.0, h), (1.0, h)]);
        let layer3 = Layer::new("3", "C", vec![(0.0, h), (1.0, h)]);
        let layer4 = Layer::new("4", "S", vec![(0.0, h), (1.0, h)]);
        let layer5 = Layer::new("5", "C", vec![(0.0, h), (1.0, h)]);

        let section = LayeredCrossSection::new(l, &width, &vec![layer1, layer2, layer3, layer4, layer5], &vec![material_c, material_s], &LayerAlignment::SectionCenter).unwrap();

        // Determine generalized strains from normal force and torque by inverting/solving the stiffness relation
        let strains = section.C(s).qr().solve(&vector![N, M, 0.0]).unwrap();

        // Evaluate and check section normal strains
        let strain_eval = section.strain_eval(s);
        let result = &strain_eval*strains;

        assert_relative_eq!(result[0], epsilon_u, max_relative=1e-2);
        assert_relative_eq!(result[9], epsilon_o, max_relative=1e-2);

        // Evaluate and check section normal stresses
        let stress_eval = section.stress_eval(s);
        let result = &stress_eval*strains;

        assert_relative_eq!(result, sigma_ref, max_relative=1e-2);

    }
}