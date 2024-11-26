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
use crate::utils::validation::is_hex_color;

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
        let Self { comment: _, settings, dimensions, materials, layers, profile, width, string, masses, damping } = self;

        settings.validate()?;
        dimensions.validate()?;
        for material in materials {
            material.validate()?;
        }
        for layer in layers {
            layer.validate()?;
        }
        profile.validate()?;
        width.validate()?;
        string.validate()?;
        masses.validate()?;
        damping.validate()?;

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
                n_limb_eval_points: 100,
                min_draw_resolution: 100,
                max_draw_resolution: 100,
                arrow_clamp_force: 0.5,
                string_compression_factor: 1e-6,
                timespan_factor: 1.5,
                timeout_factor: 10.0,
                min_timestep: 1e-6,
                max_timestep: 1e-4,
                steps_per_period: 250,
            },
            dimensions: Dimensions {
                brace_height: 0.2,
                draw_length: 0.7,
                handle_length: 0.0,
                handle_setback: 0.0,
                handle_angle: 0.0
            },
            materials: vec![
                Material::new("Default", "#000000", 1.0, 1.0, 1.0)
            ],
            layers: vec![
                Layer::new("Unnamed", "Default", vec![(0.0, 0.001), (1.0, 0.001)])
            ],
            profile: Profile::new(LayerAlignment::SectionBack, vec![
                SegmentInput::Line(LineInput::new(1.0))
            ]),
            width: Width::new(vec![
                (0.0, 0.05),
                (0.5, 0.04),
                (1.0, 0.01)
            ]),
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

    pub min_draw_resolution: usize,
    pub max_draw_resolution: usize,

    pub arrow_clamp_force: f64,
    pub string_compression_factor: f64,
    pub timespan_factor: f64,
    pub timeout_factor: f64,
    pub min_timestep: f64,
    pub max_timestep: f64,
    pub steps_per_period: usize
}

impl Settings {
    pub fn validate(&self) -> Result<(), ModelError> {
        let &Self { n_limb_elements, n_limb_eval_points, min_draw_resolution, max_draw_resolution, arrow_clamp_force, string_compression_factor, timespan_factor, timeout_factor, min_timestep, max_timestep, steps_per_period } = self;

        if n_limb_elements < 1 {
            return Err(ModelError::SettingsInvalidLimbElements(n_limb_elements));
        }
        if n_limb_eval_points < 2 {
            return Err(ModelError::SettingsInvalidLimbEvalPoints(n_limb_eval_points));
        }
        if min_draw_resolution < 1 {
            return Err(ModelError::SettingsInvalidMinDrawResolution(min_draw_resolution));
        }
        if max_draw_resolution < 1 {
            return Err(ModelError::SettingsInvalidMaxDrawResolution(max_draw_resolution));
        }
        if !arrow_clamp_force.is_finite() || arrow_clamp_force < 0.0 {
            return Err(ModelError::SettingsInvalidArrowClampForce(arrow_clamp_force));
        }
        if !string_compression_factor.is_finite() || string_compression_factor <= 0.0 {
            return Err(ModelError::SettingsInvalidStringCompressionFactor(string_compression_factor));
        }
        if !timespan_factor.is_finite() || timespan_factor < 1.0 {
            return Err(ModelError::SettingsInvalidTimeSpanFactor(timespan_factor));
        }
        if !timeout_factor.is_finite() || timeout_factor < 1.0 {
            return Err(ModelError::SettingsInvalidTimeOutFactor(timeout_factor));
        }
        if !min_timestep.is_finite() || min_timestep <= 0.0 {
            return Err(ModelError::SettingsInvalidMinTimeStep(min_timestep));
        }
        if !max_timestep.is_finite() || max_timestep <= 0.0 {
            return Err(ModelError::SettingsInvalidMaxTimeStep(max_timestep));
        }
        if steps_per_period < 1 {
            return Err(ModelError::SettingsInvalidStepsPerPeriod(steps_per_period));
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
        let &Self { brace_height, draw_length, handle_length, handle_setback, handle_angle} = self;

        if !brace_height.is_finite() {
            return Err(ModelError::DimensionsInvalidBraceHeight(brace_height));
        }
        if !draw_length.is_finite() || draw_length <= brace_height {
            return Err(ModelError::DimensionsInvalidDrawLength(draw_length));
        }
        if !handle_length.is_finite() || handle_length < 0.0 {
            return Err(ModelError::DimensionsInvalidHandleLength(handle_length));
        }
        if !handle_setback.is_finite() {
            return Err(ModelError::DimensionsInvalidHandleSetback(handle_setback));
        }
        if !handle_angle.is_finite() {
            return Err(ModelError::DimensionsInvalidHandleAngle(handle_angle));
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
        let Self { name, color, rho, E, G } = self;

        if name.is_empty() {
            return Err(ModelError::MaterialInvalidName(name.clone()));
        }
        if !is_hex_color(&color) {
            return Err(ModelError::MaterialInvalidColor(color.clone()));
        }
        if !rho.is_finite() || *rho <= 0.0 {
            return Err(ModelError::MaterialInvalidDensity(*rho));
        }
        if !E.is_finite() || *E <= 0.0 {
            return Err(ModelError::MaterialInvalidYoungsModulus(*E));
        }
        if !G.is_finite() || *G <= 0.0 {
            return Err(ModelError::MaterialInvalidShearModulus(*G));
        }

        Ok(())
    }
}

#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct Layer {
    pub name: String,
    pub material: String,
    pub height: Vec<(f64, f64)>,
}

impl Layer {
    const REL_LENGTH_TOL: f64 = 1e-9;    // Tolerance used for validating inputs that are relative lengths

    pub fn new(name: &str, material: &str, height: Vec<(f64, f64)>) -> Self {
        Self {
            name: name.to_string(),
            material: material.to_string(),
            height
        }
    }

    pub fn validate(&self) -> Result<(), ModelError> {
        let Self { name, material: _, height } = self;

        if name.is_empty() {
            return Err(ModelError::LayerInvalidName(name.clone()));
        }

        // Material index is validated by the cross sections

        // TODO: Height should be validated by the cross section in order to give an error message with the index of the layer
        // Or maybe implement validation on Vec<Layer>?

        if height.len() < 2 {
            return Err(ModelError::LayerHeightControlPointsTooFew(height.len()));
        }

        if let Some((a, b)) = height.iter().tuple_windows().find(|(a, b)| b.0 <= a.0) {
            return Err(ModelError::LayerHeightControlPointsNotSorted(a.0, b.0));
        }

        let first = height.first().unwrap();
        let last = height.last().unwrap();

        if (first.0 < 0.0 - Width::REL_LENGTH_TOL) || (last.0 > 1.0 + Width::REL_LENGTH_TOL) {
            return Err(ModelError::LayerHeightControlPointsInvalidRange(first.0, last.0));
        }

        for (i, a) in height.iter().enumerate() {
            if i == 0 || i == height.len()-1 {
                if !a.0.is_finite() || !a.1.is_finite() || a.0 < 0.0 || a.1 < 0.0 {
                    return Err(ModelError::LayerHeightControlPointsInvalidBoundaryValue(a.0, a.1));
                }
            }
            else {
                if !a.0.is_finite() || !a.1.is_finite() || a.0 <= 0.0 || a.1 <= 0.0 {
                    return Err(ModelError::LayerHeightControlPointsInvalidInteriorValue(a.0, a.1));
                }
            }
        }

        if first.0 > 0.0 + Layer::REL_LENGTH_TOL && first.1 != 0.0 {
            return Err(ModelError::LayerHeightControlPointsDiscontinuousBoundary(first.0, first.1));
        }

        if last.0 < 1.0 - Layer::REL_LENGTH_TOL && last.1 != 0.0 {
            return Err(ModelError::LayerHeightControlPointsDiscontinuousBoundary(last.0, last.1));
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
        let Self {alignment, segments } = self;

        match alignment {
            LayerAlignment::LayerBack(name) | LayerAlignment::LayerBelly(name) | LayerAlignment::LayerCenter(name) => {
                if name.is_empty() {
                    return Err(ModelError::ProfileAnlignemtInvalidLayerName(name.clone()));
                }
            }
            _ => {
                // Other alignment options don't need validation
            }
        }

        for (index, segment) in segments.iter().enumerate() {
            segment.validate(index)?;
        }

        Ok(())
    }
}

#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct Width {
    pub points: Vec<(f64, f64)>
}

impl Width {
    const REL_LENGTH_TOL: f64 = 1e-9;    // Tolerance used for validating inputs that are relative lengths

    pub fn new(points: Vec<(f64, f64)>) -> Self {
        Self {
            points
        }
    }

    pub fn validate(&self) -> Result<(), ModelError> {
        let Self { points } = self;

        if points.len() < 2 {
            return Err(ModelError::WidthControlPointsTooFew(points.len()));
        }

        if let Some((a, b)) = points.iter().tuple_windows().find(|(a, b)| b.0 <= a.0) {
            return Err(ModelError::WidthControlPointsNotSorted(a.0, b.0));
        }

        let first = points.first().unwrap();
        let last = points.last().unwrap();

        if ((first.0 - 0.0).abs() > Width::REL_LENGTH_TOL) || ((last.0 - 1.0).abs() > Width::REL_LENGTH_TOL) {
            return Err(ModelError::WidthControlPointsInvalidRange(first.0, last.0));
        }

        if let Some(a) = points.iter().find(|a| !a.0.is_finite() || !a.1.is_finite() || a.1 <= 0.0) {
            return Err(ModelError::WidthControlPointsInvalidValue(a.0, a.1));
        }

        Ok(())
    }
}

#[derive(Serialize, Deserialize, PartialEq, Clone, Debug)]
pub struct BowString {
    pub n_strands: usize,
    pub strand_density: f64,
    pub strand_stiffness: f64
}

impl BowString {
    pub fn validate(&self) -> Result<(), ModelError> {
        let &Self { n_strands, strand_density, strand_stiffness } = self;

        if n_strands < 1 {
            return Err(ModelError::StringInvalidNumberOfStrands(n_strands));
        }
        if !strand_density.is_finite() || strand_density <= 0.0 {
            return Err(ModelError::StringInvalidStrandDensity(strand_density));
        }
        if !strand_stiffness.is_finite() || strand_stiffness <= 0.0 {
            return Err(ModelError::StringInvalidStrandStiffness(strand_stiffness));
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
        let &Self { arrow, limb_tip, string_center, string_tip } = self;

        if !arrow.is_finite() || arrow <= 0.0 {
            return Err(ModelError::MassesInvalidArrowMass(arrow));
        }
        if !limb_tip.is_finite() || limb_tip < 0.0 {
            return Err(ModelError::MassesInvalidLimbTipMass(limb_tip));
        }
        if !string_center.is_finite() || string_center < 0.0 {
            return Err(ModelError::MassesInvalidStringCenterMass(string_center));
        }
        if !string_tip.is_finite() || string_tip < 0.0 {
            return Err(ModelError::MassesInvalidStringTipMass(string_tip));
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
        let &Self { damping_ratio_limbs, damping_ratio_string } = self;

        if !damping_ratio_limbs.is_finite() || damping_ratio_limbs < 0.0  || damping_ratio_limbs > 1.0 {
            return Err(ModelError::DampingInvalidLimbDampingRatio(damping_ratio_limbs));
        }
        if !damping_ratio_string.is_finite() || damping_ratio_string < 0.0 || damping_ratio_string > 1.0 {
            return Err(ModelError::DampingInvalidStringDampingRatio(damping_ratio_string));
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