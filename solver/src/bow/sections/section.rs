use nalgebra::{DVector, matrix, SMatrix, SVector, vector};
use serde::{Deserialize, Serialize};
use crate::fem::elements::beam::CrossSection;
use crate::bow::errors::ModelError;
use crate::bow::input::{Layer, Material, Width};
use crate::numerics::cubic_spline::{BoundaryCondition, CubicSpline, Extrapolation};
use crate::numerics::intervals::{Bound, Interval};

// Defines, how the layers are aligned with the profile curve
// There are two categories:
// - Section: The back side, belly side, or geometrical center of the combined section is aligned with the profile curve
// - Layer: The back side, belly side, or geometrical center of the layer with given index is aligned with the profile curve.
#[derive(Serialize, Deserialize, Copy, Clone, PartialEq, Debug)]
pub enum LayerAlignment {
    #[serde(rename = "section-back")]
    SectionBack,

    #[serde(rename = "section-belly")]
    SectionBelly,

    #[serde(rename = "section-center")]
    SectionCenter,

    #[serde(rename = "layer-back")]
    LayerBack(usize),

    #[serde(rename = "layer-belly")]
    LayerBelly(usize),

    #[serde(rename = "layer-center")]
    LayerCenter(usize)
}

#[derive(Debug)]
pub struct LayerGeometry {
    height: CubicSpline,
    material: Material,
}

#[derive(Debug)]
pub struct LayeredCrossSection {
    length: f64,
    width: CubicSpline,
    layers: Vec<LayerGeometry>,
    alignment: LayerAlignment
}

// Contains data for recovering the stresses of a specific layer at a specific length
pub struct StressEval {
    factors_btm: SVector<f64, 3>,    // Factors that relate the stress at the bottom of the layer to the strains
    factors_top: SVector<f64, 3>,    // Factors that relate the stress at the top of the layer to the strains
}

impl StressEval {
    // strains: epsilon, kappa, gamma
    // output: normal stress at bottom and top of layer
    fn eval(&self, strains: &SVector<f64, 3>) -> (f64, f64) {
        (self.factors_btm.dot(strains), self.factors_top.dot(strains))
    }
}

impl LayeredCrossSection {
    pub fn new(length: f64, width: &Width, layers: &Vec<Layer>, materials: &Vec<Material>, alignment: LayerAlignment) -> Result<Self, ModelError> {
        // Check inputs for validity and return error on failure
        Self::validate(length, width, layers, materials, alignment)?;

        // Construct width spline
        let width = CubicSpline::from_points(&width.points, false, BoundaryCondition::SecondDerivative(0.0), BoundaryCondition::SecondDerivative(0.0));

        // Construct layer geometries
        let layers = layers.iter().map(|layer| {
            let height = CubicSpline::from_points(&layer.height, true, BoundaryCondition::SecondDerivative(0.0), BoundaryCondition::SecondDerivative(0.0));
            LayerGeometry {
                height,
                material: materials[layer.material].clone()
            }
        }).collect();

        Ok(Self {
            length,
            width,
            layers,
            alignment
        })
    }

    fn validate(length: f64, width: &Width, layers: &Vec<Layer>, materials: &Vec<Material>, alignment: LayerAlignment) -> Result<(), ModelError> {
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

        // Validate material model
        for material in materials {
            material.validate()?;
        }

        // Validate layer model
        for layer in layers {
            // Validate layers themselves and check if material index is valid
            layer.validate()?;
            if layer.material >= materials.len() {
                return Err(ModelError::CrossSectionInvalidMaterialIndex(layer.name.clone(), layer.material));
            }
        }

        // Check validity of the alignment specification
        match alignment {
            LayerAlignment::LayerBack(index) | LayerAlignment::LayerBelly(index) | LayerAlignment::LayerCenter(index) => {
                if index >= layers.len() {
                    return Err(ModelError::CrossSectionInvalidAlignmentIndex(index));
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
            let to_bound = |point: &[f64; 2]| {
                if point[1] == 0.0 {
                    Bound::Exclusive(point[0])
                } else {
                    Bound::Inclusive(point[0])
                }
            };

            Interval {
                lower: to_bound(&layer.height.first().unwrap()),
                upper: to_bound(&layer.height.last().unwrap()),
            }
        }).collect();

        let union = Interval::left_union(intervals);
        if union != Interval::inclusive(0.0, 1.0) {
            return Err(ModelError::CrossSectionZeroCombinedHeight(union.upper.value()));
        }

        Ok(())
    }

    pub fn stress(&self, s: f64, i: usize) -> StressEval {
        let p = s/self.length;
        let h: Vec<f64> = self.layers.iter().map(|layer| layer.height.value(p, Extrapolation::Constant)).collect();
        let y = compute_layer_bounds(&h, self.alignment);

        let layer = &self.layers[i];
        let y_btm = y[i];
        let y_top = y[i+1];

        StressEval {
            factors_btm: vector![layer.material.E, -layer.material.E*y_btm, 0.0],
            factors_top: vector![layer.material.E, -layer.material.E*y_top, 0.0],
        }
    }
}

impl CrossSection for LayeredCrossSection {
    fn rhoA(&self, s: f64) -> f64 {
        let p = s/self.length;
        let w = self.width.value(p, Extrapolation::Constant);
        self.layers.iter().map(|layer| {
            let h = layer.height.value(p, Extrapolation::Constant);
            layer.material.rho*w*h
        }).sum()
    }

    fn rhoI(&self, s: f64) -> f64 {
        let p = s/self.length;
        let w = self.width.value(p, Extrapolation::Constant);

        let h: Vec<f64> = self.layers.iter().map(|layer| layer.height.value(p, Extrapolation::Constant)).collect();
        let y = compute_layer_bounds(&h, self.alignment);

        self.layers.iter().enumerate().map(|(i, layer)| {
            let A = w*h[i];
            let d = (y[i] + y[i+1])/2.0;
            let I = A*(h[i].powi(2)/12.0 + d.powi(2));
            layer.material.rho*I
        }).sum()
    }

    fn C(&self, s: f64) -> SMatrix<f64, 3, 3> {
        let p = s/self.length;
        let w = self.width.value(p, Extrapolation::Constant);
        let h: Vec<f64> = self.layers.iter().map(|layer| layer.height.value(p, Extrapolation::Constant)).collect();
        let y = compute_layer_bounds(&h, self.alignment);

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

    fn height(&self, s: f64) -> f64 {
        let p = s/self.length;
        self.layers.iter().map(|layer| layer.height.value(p, Extrapolation::Constant)).sum()
    }
}

// Calculates y-positions of the layer bounds from a list of layer heights, with the reference point
// determined by the given alignment option.
fn compute_layer_bounds(heights: &[f64], alignment: LayerAlignment) -> DVector<f64> {
    assert!(heights.len() >= 1);

    // Calculate positions with belly as reference point.
    // First position is zero, then each new position is the previous one plus the respective layer height.
    let mut y_pos = DVector::<f64>::zeros(heights.len() + 1);
    for (i, h) in heights.iter().enumerate() {
        y_pos[i+1] = y_pos[i] + h;
    }

    // Reference position depending on alignment
    let y_ref: f64 = match alignment {
        LayerAlignment::SectionBelly   => y_pos[0],
        LayerAlignment::SectionBack    => y_pos[y_pos.len() - 1],
        LayerAlignment::SectionCenter  => (y_pos[0] + y_pos[y_pos.len() - 1])/2.0,
        LayerAlignment::LayerBelly(i)  => y_pos[i],
        LayerAlignment::LayerBack(i)   => y_pos[i+1],
        LayerAlignment::LayerCenter(i) => (y_pos[i] + y_pos[i+1])/2.0
    };

    // Apply offet to original positions
    y_pos.add_scalar_mut(-y_ref);
    return y_pos;
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_error_conditions() {
        let materials = vec![Material::new("", "", 7000.0, 200e9, 80.9)];
        let width = Width::new(vec![[0.0, 0.05], [1.0, 0.05]]);
        let layers = vec![Layer::new("", 0, vec![[0.0, 0.02], [1.0, 0.02]])];

        // 1. Valid single layer model
        assert!(LayeredCrossSection::new(1.5, &width, &layers, &materials, LayerAlignment::SectionCenter).is_ok());

        // 2. Invalid length
        assert_matches!(LayeredCrossSection::new(0.0, &width, &layers, &materials, LayerAlignment::SectionCenter), Err(ModelError::CrossSectionInvalidLength(0.0)));
        assert_matches!(LayeredCrossSection::new(-1.0, &width, &layers, &materials, LayerAlignment::SectionCenter), Err(ModelError::CrossSectionInvalidLength(-1.0)));
        assert_matches!(LayeredCrossSection::new(f64::INFINITY, &width, &layers, &materials, LayerAlignment::SectionCenter), Err(ModelError::CrossSectionInvalidLength(f64::INFINITY)));

        // 3. Invalid width
        assert_matches!(LayeredCrossSection::new(1.5, &Width::new(vec![]), &layers, &materials, LayerAlignment::SectionCenter), Err(ModelError::WidthControlPointsTooFew(0)));
        assert_matches!(LayeredCrossSection::new(1.5, &Width::new(vec![[0.0, 0.05]]), &layers, &materials, LayerAlignment::SectionCenter), Err(ModelError::WidthControlPointsTooFew(1)));
        assert_matches!(LayeredCrossSection::new(1.5, &Width::new(vec![[1.0, 0.05], [0.0, 0.05]]), &layers, &materials, LayerAlignment::SectionCenter), Err(ModelError::WidthControlPointsNotSorted(1.0, 0.0)));
        assert_matches!(LayeredCrossSection::new(1.5, &Width::new(vec![[0.1, 0.05], [1.0, 0.05]]), &layers, &materials, LayerAlignment::SectionCenter), Err(ModelError::WidthControlPointsInvalidRange(0.1, 1.0)));
        assert_matches!(LayeredCrossSection::new(1.5, &Width::new(vec![[0.0, 0.05], [0.9, 0.05]]), &layers, &materials, LayerAlignment::SectionCenter), Err(ModelError::WidthControlPointsInvalidRange(0.0, 0.9)));
        assert_matches!(LayeredCrossSection::new(1.5, &Width::new(vec![[0.0, 0.05], [1.0, 0.0]]), &layers, &materials, LayerAlignment::SectionCenter), Err(ModelError::WidthControlPointsInvalidValue(1.0, 0.0)));

        // 4. Invalid layers
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("", 0, vec![])], &materials, LayerAlignment::SectionCenter), Err(ModelError::HeightControlPointsTooFew(0)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("", 0, vec![[0.0, 0.02]])], &materials, LayerAlignment::SectionCenter), Err(ModelError::HeightControlPointsTooFew(1)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("", 0, vec![[1.0, 0.02], [0.0, 0.02]])], &materials, LayerAlignment::SectionCenter), Err(ModelError::HeightControlPointsNotSorted(1.0, 0.0)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("", 0, vec![[-0.1, 0.02], [1.0, 0.02]])], &materials, LayerAlignment::SectionCenter), Err(ModelError::HeightControlPointsInvalidRange(-0.1, 1.0)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("", 0, vec![[0.0, 0.02], [1.1, 0.02]])], &materials, LayerAlignment::SectionCenter), Err(ModelError::HeightControlPointsInvalidRange(0.0, 1.1)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("", 0, vec![[0.0, 0.0], [0.5, 0.0], [1.0, 0.0]])], &materials, LayerAlignment::SectionCenter), Err(ModelError::HeightControlPointsInvalidInteriorValue(0.5, 0.0)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("", 0, vec![[0.0, -0.1], [0.5, 0.02], [1.0, 0.0]])], &materials, LayerAlignment::SectionCenter), Err(ModelError::HeightControlPointsInvalidBoundaryValue(0.0, -0.1)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("", 0, vec![[0.1, 0.02], [1.0, 0.02]])], &materials, LayerAlignment::SectionCenter), Err(ModelError::HeightControlPointsDiscontinuousBoundary(0.1, 0.02)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("", 0, vec![[0.0, 0.02], [0.9, 0.02]])], &materials, LayerAlignment::SectionCenter), Err(ModelError::HeightControlPointsDiscontinuousBoundary(0.9, 0.02)));

        // 5. Invalid cross section
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![], &materials, LayerAlignment::SectionCenter), Err(ModelError::CrossSectionNoLayers));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &layers, &vec![], LayerAlignment::SectionCenter), Err(ModelError::CrossSectionNoMaterials));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("abc", 1, vec![[0.0, 0.02], [1.0, 0.02]])], &materials, LayerAlignment::SectionCenter), Err(ModelError::CrossSectionInvalidMaterialIndex(_, 1)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &layers, &materials, LayerAlignment::LayerBack(1)), Err(ModelError::CrossSectionInvalidAlignmentIndex(1)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &layers, &materials, LayerAlignment::LayerBelly(1)), Err(ModelError::CrossSectionInvalidAlignmentIndex(1)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &layers, &materials, LayerAlignment::LayerCenter(1)), Err(ModelError::CrossSectionInvalidAlignmentIndex(1)));
        assert_matches!(LayeredCrossSection::new(1.5, &width, &vec![Layer::new("", 0, vec![[0.0, 0.02], [1.0, 0.0]])], &materials, LayerAlignment::SectionCenter), Err(ModelError::CrossSectionZeroCombinedHeight(1.0)));
    }

    #[test]
    fn test_compute_layer_bounds() {
        // Checks the computation of the positions of the layer bounds from section heights and an alignment option

        // Single layer
        assert_abs_diff_eq!(compute_layer_bounds(&[0.4], LayerAlignment::SectionBelly), DVector::from_row_slice(&[0.0, 0.4]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.4], LayerAlignment::SectionBack), DVector::from_row_slice(&[-0.4, 0.0]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.4], LayerAlignment::SectionCenter), DVector::from_row_slice(&[-0.2, 0.2]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.4], LayerAlignment::LayerBelly(0)), DVector::from_row_slice(&[0.0, 0.4]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.4], LayerAlignment::LayerBack(0)), DVector::from_row_slice(&[-0.4, 0.0]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.4], LayerAlignment::LayerCenter(0)), DVector::from_row_slice(&[-0.2, 0.2]));

        // Multiple layers
        assert_abs_diff_eq!(compute_layer_bounds(&[0.1, 0.2, 0.1], LayerAlignment::SectionBelly), DVector::from_row_slice(&[0.0, 0.1, 0.3, 0.4]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.1, 0.2, 0.1], LayerAlignment::SectionBack), DVector::from_row_slice(&[-0.4, -0.3, -0.1, 0.0]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.1, 0.2, 0.1], LayerAlignment::SectionCenter), DVector::from_row_slice(&[-0.2, -0.1, 0.1, 0.2]));

        assert_abs_diff_eq!(compute_layer_bounds(&[0.1, 0.2, 0.1], LayerAlignment::LayerBelly(0)), DVector::from_row_slice(&[0.0, 0.1, 0.3, 0.4]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.1, 0.2, 0.1], LayerAlignment::LayerBack(0)), DVector::from_row_slice(&[-0.1, 0.0, 0.2, 0.3]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.1, 0.2, 0.1], LayerAlignment::LayerCenter(0)), DVector::from_row_slice(&[-0.05, 0.05, 0.25, 0.35]));

        assert_abs_diff_eq!(compute_layer_bounds(&[0.1, 0.2, 0.1], LayerAlignment::LayerBelly(1)), DVector::from_row_slice(&[-0.1, 0.0, 0.2, 0.3]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.1, 0.2, 0.1], LayerAlignment::LayerBack(1)), DVector::from_row_slice(&[-0.3, -0.2, 0.0, 0.1]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.1, 0.2, 0.1], LayerAlignment::LayerCenter(1)), DVector::from_row_slice(&[-0.2, -0.1, 0.1, 0.2]));

        assert_abs_diff_eq!(compute_layer_bounds(&[0.1, 0.2, 0.1], LayerAlignment::LayerBelly(2)), DVector::from_row_slice(&[-0.3, -0.2, 0.0, 0.1]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.1, 0.2, 0.1], LayerAlignment::LayerBack(2)), DVector::from_row_slice(&[-0.4, -0.3, -0.1, 0.0]));
        assert_abs_diff_eq!(compute_layer_bounds(&[0.1, 0.2, 0.1], LayerAlignment::LayerCenter(2)), DVector::from_row_slice(&[-0.35, -0.25, -0.05, 0.05]));
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

        let width = Width::new(vec![[0.0, w], [1.0, w]]);
        let materials = vec![Material::new("", "", rho, E, G)];
        let layers = vec![Layer::new("", 0, vec![[0.0, h], [1.0, h]])];

        // Reference point is section center
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, LayerAlignment::SectionCenter).unwrap();
        let C_ref = matrix![
            E*w*h, 0.0, 0.0;
            0.0, E*w*h.powi(3)/12.0, 0.0;
            0.0, 0.0, w*h*G
        ];

        assert_abs_diff_eq!(section.width(s), w, epsilon=1e-9);
        assert_abs_diff_eq!(section.height(s), h, epsilon=1e-9);
        assert_abs_diff_eq!(section.rhoA(s), rho*w*h, epsilon=1e-9);
        assert_abs_diff_eq!(section.C(s), C_ref, epsilon=1e-9);

        // Reference point is section back
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, LayerAlignment::SectionBack).unwrap();
        let C_ref = matrix![
            E*w*h, E*(w*h*0.5*h), 0.0;
            E*(w*h*0.5*h), E*(w*h.powi(3)/12.0 + (0.5*h).powi(2)*(w*h)), 0.0;
            0.0, 0.0, w*h*G
        ];

        assert_abs_diff_eq!(section.width(s), w, epsilon=1e-9);
        assert_abs_diff_eq!(section.height(s), h, epsilon=1e-9);
        assert_abs_diff_eq!(section.rhoA(s), rho*w*h, epsilon=1e-9);
        assert_abs_diff_eq!(section.C(s), C_ref, epsilon=1e-9);

        // Reference point is section belly
        let section = LayeredCrossSection::new(l, &width, &layers, &materials, LayerAlignment::SectionBelly).unwrap();
        let C_ref = matrix![
            E*w*h, -E*(w*h*0.5*h), 0.0;
            -E*(w*h*0.5*h), E*(w*h.powi(3)/12.0 + (0.5*h).powi(2)*(w*h)), 0.0;
            0.0, 0.0, w*h*G
        ];

        assert_abs_diff_eq!(section.width(s), w, epsilon=1e-9);
        assert_abs_diff_eq!(section.height(s), h, epsilon=1e-9);
        assert_abs_diff_eq!(section.rhoA(s), rho*w*h, epsilon=1e-9);
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

        let material1 = Material { name: "".to_string(), color: "".to_string(), rho: rho1, E: E1, G: G1 };
        let material2 = Material { name: "".to_string(), color: "".to_string(), rho: rho2, E: E2, G: G2 };
        let material3 = Material { name: "".to_string(), color: "".to_string(), rho: rho3, E: E3, G: G3 };

        let width = Width::new(vec![[0.0, w], [1.0, w]]);
        let layer1 = Layer::new("", 0, vec![[0.0, h1], [1.0, h1]]);
        let layer2 = Layer::new("", 1, vec![[0.0, h2], [1.0, h2]]);
        let layer3 = Layer::new("", 2, vec![[0.0, h3], [1.0, h3]]);

        let section = LayeredCrossSection::new(l, &width, &vec![layer1, layer2, layer3], &vec![material1, material2, material3], LayerAlignment::SectionCenter).unwrap();
        let C_ref = matrix![
            E1*A1 + E2*A2 + E3*A3, -E1*A1*y1 - E2*A2*y2 - E3*A3*y3, 0.0;
            -E1*A1*y1 - E2*A2*y2 - E3*A3*y3, E1*I1 + E2*I2 + E3*I3, 0.0;
            0.0, 0.0, G1*A1 + G2*A2 + G3*A3
        ];

        assert_abs_diff_eq!(section.width(s), w, epsilon=1e-12);
        assert_abs_diff_eq!(section.height(s), h1 + h2 + h3, epsilon=1e-12);
        assert_abs_diff_eq!(section.rhoA(s), rho1*A1 + rho2*A2 + rho3*A3, epsilon=1e-12);
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
        let sigma_ref = &[(2.38e6, -0.14e6), (-0.07e6, -1.32e6), (-2.64e6, -5.16e6), (-2.58e6, -3.83e6), (-7.66e6, -10.18e6)];

        let material_c = Material { name: "".to_string(), color: "".to_string(), rho: rho, E: Ec, G: 0.5*Ec };
        let material_s = Material { name: "".to_string(), color: "".to_string(), rho: rho, E: Es, G: 0.5*Es };

        let width = Width::new(vec![[0.0, w], [1.0, w]]);
        let layer1 = Layer::new("", 0, vec![[0.0, h], [1.0, h]]);
        let layer2 = Layer::new("", 1, vec![[0.0, h], [1.0, h]]);
        let layer3 = Layer::new("", 0, vec![[0.0, h], [1.0, h]]);
        let layer4 = Layer::new("", 1, vec![[0.0, h], [1.0, h]]);
        let layer5 = Layer::new("", 0, vec![[0.0, h], [1.0, h]]);

        let section = LayeredCrossSection::new(l, &width, &vec![layer1, layer2, layer3, layer4, layer5], &vec![material_c, material_s], LayerAlignment::SectionCenter).unwrap();

        let C = section.C(s);
        //let H = section.stress(s);

        // Determine strains from normal force and torque by inverting/solving the stiffness relation
        let strains = C.qr().solve(&vector![N, M, 0.0]).unwrap();
        let epsilon = strains[0];
        let kappa = strains[1];
        let _gamma = strains[2];

        // Check strains
        assert_relative_eq!(section.strain(epsilon, kappa, 2.5*h), epsilon_o, max_relative=1e-2);
        assert_relative_eq!(section.strain(epsilon, kappa, -2.5*h), epsilon_u, max_relative=1e-2);

        // Check stresses
        for i in 0..section.layers.len() {
            let stress = section.stress(s, i);
            let (sigma_u, sigma_o) = stress.eval(&strains);

            assert_relative_eq!(sigma_u, sigma_ref[i].0, max_relative=1e-2);
            assert_relative_eq!(sigma_o, sigma_ref[i].1, max_relative=1e-2);
        }
    }
}