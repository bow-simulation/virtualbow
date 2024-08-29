use std::fs::File;
use std::io::{BufReader, BufWriter, Write};
use std::path::Path;
use itertools::Itertools;
use serde::{Deserialize, Serialize};
use serde_json::Value;
use crate::bow::compatibility::convert_to_current_format;
use crate::bow::sections::section::LayerAlignment;
use crate::bow::errors::ModelError;
use crate::bow::profile::input::SegmentInput;
use crate::bow::profile::segments::clothoid::LineInput;
use crate::bow::versioning::{VersionedWrapper, VersionedWrapperRef};

#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct BowInput {
    pub comment: String,
    pub settings: Settings,
    pub dimensions: Dimensions,
    pub materials: Vec<Material>,
    pub layers: Vec<Layer>,
    pub profile: Profile,
    pub width: Width,
    pub string: BowString,
    pub masses: Masses,
    pub damping: Damping,
}

impl BowInput {
    // Current version of the bow model format, increase when incompatible changes are made.
    pub const FILE_VERSION: u64 = 3;

    // Loads a bow bow from a json file, including necessary conversions from older file formats for compatibility.
    pub fn load<P: AsRef<Path>>(path: P) -> Result<BowInput, ModelError> {
        let file = File::open(&path).map_err(|e| ModelError::InputLoadFileError(path.as_ref().to_owned(), e))?;
        let mut reader = BufReader::new(file);

        // Parse the file as a json value and convert that json value to the current version if necessary
        let mut value: Value = serde_json::from_reader(&mut reader).map_err(|e| ModelError::InputDeserializeJsonError(e))?;
        convert_to_current_format(&mut value)?;

        // Parse wrapper (data + version number) from converted json value. Discard version number and only return the data.
        let wrapper: VersionedWrapper<Self> = serde_json::from_value(value).map_err(|e| ModelError::InputInterpretJsonError(e))?;
        Ok(wrapper.data)
    }

    // Saves the bow to a file in the current json format
    pub fn save<P: AsRef<Path>>(&self, path: P) -> Result<(), ModelError> {
        let file = File::create(&path).map_err(|e| ModelError::InputSaveFileError(path.as_ref().to_owned(), e))?;
        let mut writer = BufWriter::new(file);

        // Create wrapper object that adds the current version number
        let wrapper = VersionedWrapperRef::new(Self::FILE_VERSION, self);

        // Save wrapper object to file
        serde_json::to_writer_pretty(&mut writer, &wrapper).map_err(|e| ModelError::InputSerializeJsonError(e))?;
        writer.flush().map_err(|e| ModelError::InputSaveFileError(path.as_ref().to_owned(), e))
    }

    pub fn validate(&self) -> Result<(), ModelError> {
        self.settings.validate()?;
        self.dimensions.validate()?;
        for material in &self.materials {
            material.validate()?;
        }
        for layer in &self.layers {
            layer.validate()?;
        }
        self.profile.validate()?;
        self.width.validate()?;
        self.string.validate()?;
        self.masses.validate()?;
        self.damping.validate()?;

        Ok(())
    }
}

impl Default for BowInput {
    // Valid default values for a new bow bow
    fn default() -> Self {
        Self {
            comment: "".into(),
            settings: Settings {
                n_limb_elements: 30,
                n_limb_eval_points: 100,  // TODO
                n_layer_eval_points: 100,  // TODO
                n_string_elements: 1,    // TODO
                n_draw_steps: 100,
                arrow_clamp_force: 0.5,
                time_span_factor: 1.5,
                time_step_factor: 0.2,
                sampling_rate: 10000.0
            },
            dimensions: Dimensions {
                brace_height: 0.2,
                draw_length: 0.7,
                handle_length: 0.0,
                handle_setback: 0.0,
                handle_angle: 0.0
            },
            materials: vec![Material {
                name: "".to_string(),
                color: "".to_string(),
                rho: 0.0,
                E: 0.0,
                G: 0.0
            }],
            layers: vec![Layer {
                name: "Unnamed".to_string(),
                material: 0,
                height: vec![[0.0, 0.001], [1.0, 0.001]],
            }],
            profile: Profile {
                alignment: LayerAlignment::SectionBack,
                segments: vec![SegmentInput::Line(LineInput::new(1.0))]
            },
            width: Width {
                points: vec![[0.0, 0.05], [0.5, 0.04], [1.0, 0.01]]
            },
            string: BowString {
                n_strands: 1,
                strand_density: 1.0,
                strand_stiffness: 1.0,
            },
            masses: Masses {
                arrow: 1.0,
                limb_tip: 0.0,
                string_center: 0.0,
                string_tip: 0.0,
            },
            damping: Damping {
                damping_ratio_limbs: 0.0,
                damping_ratio_string: 0.0,
            },
        }
    }
}

#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct Settings {
    pub n_limb_elements: usize,
    pub n_limb_eval_points: usize,
    pub n_layer_eval_points: usize,
    pub n_string_elements: usize,
    pub n_draw_steps: usize,
    pub arrow_clamp_force: f64,
    pub time_span_factor: f64,
    pub time_step_factor: f64,
    pub sampling_rate: f64
}

impl Settings {
    pub fn validate(&self) -> Result<(), ModelError> {
        if self.n_limb_elements < 1 {
            return Err(ModelError::SettingsInvalidLimbElements(self.n_limb_elements));
        }
        if self.n_limb_eval_points < 2 {
            return Err(ModelError::SettingsInvalidLimbEvalPoints(self.n_limb_eval_points));
        }
        if self.n_layer_eval_points < 2 {
            return Err(ModelError::SettingsInvalidLayerEvalPoints(self.n_layer_eval_points));
        }
        if self.n_string_elements < 1 {
            return Err(ModelError::SettingsInvalidStringElements(self.n_string_elements));
        }
        if self.n_draw_steps < 1 {
            return Err(ModelError::SettingsInvalidDrawSteps(self.n_draw_steps));
        }
        if !self.arrow_clamp_force.is_finite() || self.arrow_clamp_force < 0.0 {
            return Err(ModelError::SettingsInvalidArrowClampForce(self.arrow_clamp_force));
        }
        if !self.time_span_factor.is_finite() || self.time_span_factor < 0.0 {
            return Err(ModelError::SettingsInvalidTimeSpanFactor(self.time_span_factor));
        }
        if !self.time_step_factor.is_finite() || self.time_step_factor < 0.0 {
            return Err(ModelError::SettingsInvalidTimeStepFactor(self.time_step_factor));
        }
        if !self.sampling_rate.is_finite() || self.sampling_rate <= 0.0 {
            return Err(ModelError::SettingsInvalidSamplingRate(self.sampling_rate));
        }

        Ok(())
    }
}

#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct Dimensions {
    pub brace_height: f64,
    pub draw_length: f64,
    pub handle_length: f64,
    pub handle_setback: f64,
    pub handle_angle: f64
}

impl Dimensions {
    pub fn validate(&self) -> Result<(), ModelError> {
        if !self.brace_height.is_finite() {
            return Err(ModelError::DimensionsInvalidBraceHeight(self.brace_height));
        }
        if !self.draw_length.is_finite() || self.draw_length <= self.brace_height {
            return Err(ModelError::DimensionsInvalidDrawLength(self.draw_length));
        }
        if !self.handle_length.is_finite() || self.handle_length < 0.0 {
            return Err(ModelError::DimensionsInvalidHandleLength(self.handle_length));
        }
        if !self.handle_setback.is_finite() {
            return Err(ModelError::DimensionsInvalidHandleSetback(self.handle_setback));
        }
        if !self.handle_angle.is_finite() {
            return Err(ModelError::DimensionsInvalidHandleAngle(self.handle_angle));
        }

        Ok(())
    }
}


#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct Material {
    pub name: String,
    pub color: String,
    pub rho: f64,
    pub E: f64,
    pub G: f64
}

impl Material {
    pub fn new(name: &str, color: &str, rho: f64, E: f64, G: f64) -> Self {
        Self {
            name: name.to_string(),
            color: color.to_string(),
            rho,
            E,
            G
        }
    }

    pub fn validate(&self) -> Result<(), ModelError> {
        if !self.rho.is_finite() || self.rho <= 0.0 {
            return Err(ModelError::MaterialInvalidDensity(self.rho));
        }
        if !self.E.is_finite() || self.E <= 0.0 {
            return Err(ModelError::MaterialInvalidYoungsModulus(self.E));
        }
        if !self.G.is_finite() || self.G <= 0.0 {
            return Err(ModelError::MaterialInvalidShearModulus(self.G));
        }

        Ok(())
    }
}

#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct Layer {
    pub name: String,
    pub material: usize,
    pub height: Vec<[f64; 2]>,
}

impl Layer {
    const REL_LENGTH_TOL: f64 = 1e-9;    // Tolerance used for validating inputs that are relative lengths

    pub fn new(name: &str, material: usize, height: Vec<[f64; 2]>) -> Self {
        Self {
            name: name.to_string(),
            material,
            height
        }
    }

    pub fn validate(&self) -> Result<(), ModelError> {
        if self.height.len() < 2 {
            return Err(ModelError::HeightControlPointsTooFew(self.height.len()));
        }

        if let Some((a, b)) = self.height.iter().tuple_windows().find(|(a, b)| b[0] <= a[0]) {
            return Err(ModelError::HeightControlPointsNotSorted(a[0], b[0]));
        }

        let first = self.height.first().unwrap();
        let last = self.height.last().unwrap();

        if (first[0] < 0.0 - Width::REL_LENGTH_TOL) || (last[0] > 1.0 + Width::REL_LENGTH_TOL) {
            return Err(ModelError::HeightControlPointsInvalidRange(first[0], last[0]));
        }

        for (i, a) in self.height.iter().enumerate() {
            if i == 0 || i == self.height.len()-1 {
                if !a[0].is_finite() || !a[1].is_finite() || a[0] < 0.0 || a[1] < 0.0 {
                    return Err(ModelError::HeightControlPointsInvalidBoundaryValue(a[0], a[1]));
                }
            }
            else {
                if !a[0].is_finite() || !a[1].is_finite() || a[0] <= 0.0 || a[1] <= 0.0 {
                    return Err(ModelError::HeightControlPointsInvalidInteriorValue(a[0], a[1]));
                }
            }
        }

        if first[0] > 0.0 + Layer::REL_LENGTH_TOL && first[1] != 0.0 {
            return Err(ModelError::HeightControlPointsDiscontinuousBoundary(first[0], first[1]));
        }

        if last[0] < 1.0 - Layer::REL_LENGTH_TOL && last[1] != 0.0 {
            return Err(ModelError::HeightControlPointsDiscontinuousBoundary(last[0], last[1]));
        }

        Ok(())
    }
}

#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct Profile {
    pub alignment: LayerAlignment,
    pub segments: Vec<SegmentInput>,
}

impl Profile {
    pub fn new(alignment: LayerAlignment, segments: Vec<SegmentInput>) -> Self {
        Self {
            alignment,
            segments
        }
    }

    pub fn validate(&self) -> Result<(), ModelError> {
        for (index, segment) in self.segments.iter().enumerate() {
            segment.validate(index)?;
        }

        Ok(())
    }
}

#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct Width {
    pub points: Vec<[f64; 2]>
}

impl Width {
    const REL_LENGTH_TOL: f64 = 1e-9;    // Tolerance used for validating inputs that are relative lengths

    pub fn new(points: Vec<[f64; 2]>) -> Self {
        Self {
            points
        }
    }

    pub fn validate(&self) -> Result<(), ModelError> {
        if self.points.len() < 2 {
            return Err(ModelError::WidthControlPointsTooFew(self.points.len()));
        }

        if let Some((a, b)) = self.points.iter().tuple_windows().find(|(a, b)| b[0] <= a[0]) {
            return Err(ModelError::WidthControlPointsNotSorted(a[0], b[0]));
        }

        let first = self.points.first().unwrap();
        let last = self.points.last().unwrap();

        if ((first[0] - 0.0).abs() > Width::REL_LENGTH_TOL) || ((last[0] - 1.0).abs() > Width::REL_LENGTH_TOL) {
            return Err(ModelError::WidthControlPointsInvalidRange(first[0], last[0]));
        }

        if let Some(a) = self.points.iter().find(|a| !a[0].is_finite() || !a[1].is_finite() || a[1] <= 0.0) {
            return Err(ModelError::WidthControlPointsInvalidValue(a[0], a[1]));
        }

        Ok(())
    }
}

#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct BowString {
    pub n_strands: usize,
    pub strand_density: f64,
    pub strand_stiffness: f64,
}

impl BowString {
    pub fn validate(&self) -> Result<(), ModelError> {
        if self.n_strands < 1 {
            return Err(ModelError::StringInvalidNumberOfStrands(self.n_strands));
        }
        if !self.strand_density.is_finite() || self.strand_density <= 0.0 {
            return Err(ModelError::StringInvalidStrandDensity(self.strand_density));
        }
        if !self.strand_stiffness.is_finite() || self.strand_stiffness <= 0.0 {
            return Err(ModelError::StringInvalidStrandStiffness(self.strand_stiffness));
        }

        Ok(())
    }
}

#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct Masses {
    pub arrow: f64,
    pub limb_tip: f64,
    pub string_center: f64,
    pub string_tip: f64,
}

impl Masses {
    pub fn validate(&self) -> Result<(), ModelError> {
        if !self.arrow.is_finite() || self.arrow <= 0.0 {
            return Err(ModelError::MassesInvalidArrowMass(self.arrow));
        }
        if !self.limb_tip.is_finite() || self.limb_tip < 0.0 {
            return Err(ModelError::MassesInvalidLimbTipMass(self.limb_tip));
        }
        if !self.string_center.is_finite() || self.string_center < 0.0 {
            return Err(ModelError::MassesInvalidStringCenterMass(self.string_center));
        }
        if !self.string_tip.is_finite() || self.string_tip < 0.0 {
            return Err(ModelError::MassesInvalidStringTipMass(self.string_tip));
        }

        Ok(())
    }
}

#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct Damping {
    pub damping_ratio_limbs: f64,
    pub damping_ratio_string: f64,
}

impl Damping {
    pub fn validate(&self) -> Result<(), ModelError> {
        if !self.damping_ratio_limbs.is_finite() || self.damping_ratio_limbs < 0.0 {
            return Err(ModelError::DampingInvalidLimbDampingRatio(self.damping_ratio_limbs));
        }
        if !self.damping_ratio_string.is_finite() || self.damping_ratio_string < 0.0 {
            return Err(ModelError::DampingInvalidStringDampingRatio(self.damping_ratio_string));
        }

        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use crate::bow::input::BowInput;
    use crate::bow::errors::ModelError;

    #[test]
    fn test_load_model() {
        // IO error when loading from an invalid path
        assert_matches!(BowInput::load("bows/tests/nonexistent.bow"), Err(ModelError::InputLoadFileError(_, _)));

        // Deserialization error due to invalid file contents (no valid json)
        assert_matches!(BowInput::load("bows/tests/invalid_json_1.bow"), Err(ModelError::InputDeserializeJsonError(_)));

        // Deserialization error due to invalid file contents (valid json but invalid structure)
        assert_matches!(BowInput::load("bows/tests/invalid_json_2.bow"), Err(ModelError::InputInterpretJsonError(_)));

        // Error when loading a bow file without version entry
        assert_matches!(BowInput::load("bows/tests/no_version.bow"), Err(ModelError::InputVersionNotFound));

        // Error when loading a bow files with an invalid version entry (wrong type)
        assert_matches!(BowInput::load("bows/tests/invalid_version_1.bow"), Err(ModelError::InputVersionInvalid(_)));

        // Error when loading a bow files with an invalid version entry (version does not exist)
        assert_matches!(BowInput::load("bows/tests/invalid_version_2.bow"), Err(ModelError::InputVersionInvalid(_)));

        // Error when loading a bow file with a version that is too old
        assert_matches!(BowInput::load("bows/tests/old_version.bow"), Err(ModelError::InputVersionTooOld(_)));

        // Error when loading a bow file with a version that is too new
        assert_matches!(BowInput::load("bows/tests/new_version.bow"), Err(ModelError::InputVersionTooNew(_)));

        // Error when loading a bow file that can not be converted to the current format
        assert_matches!(BowInput::load("bows/tests/inconvertible.bow"), Err(ModelError::InputConversionError(_, _, _)));

        // No error when loading a valid bow model
        assert_matches!(BowInput::load("bows/tests/valid.bow"), Ok(_));
    }

    #[test]
    fn test_save_model() {
        let model = BowInput::default();

        // IO error from saving to an invalid path
        assert_matches!(model.save("bows/tests/nonexistent/valid.bow"), Err(ModelError::InputSaveFileError(_, _)));

        // The only remaining error case is a serialization error, but that one is difficult to trigger.
        // Therefore we just save the model once to verify that the serialization works.
        assert_matches!(model.save("bows/tests/saved.bow"), Ok(_));
    }
}