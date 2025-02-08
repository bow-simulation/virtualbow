mod versions;
mod v1;
mod v2;
mod v3;
mod v4;

use std::fs::File;
use std::path::Path;
use itertools::Itertools;
use crate::bow::errors::ModelError;
use crate::utils::validation::is_hex_color;

pub use v4::*;
use versions::BowModelVersions;

impl BowModel {
    pub fn load<P: AsRef<Path>>(path: P) -> Result<BowModel, ModelError> {
        let file = File::open(&path).map_err(|e| ModelError::InputLoadFileError(path.as_ref().to_owned(), e))?;
        let model: BowModelVersions = serde_json::from_reader(file).map_err(ModelError::InputDeserializeJsonError)?;
        model.get_latest()
    }

    pub fn save<P: AsRef<Path>>(&self, path: P) -> Result<(), ModelError> {
        let mut file = File::create(&path).map_err(|e| ModelError::InputSaveFileError(path.as_ref().to_owned(), e))?;
        let model = BowModelVersions::V4(self.clone());    // TODO: Unnecessary clone?
        serde_json::to_writer_pretty(&mut file, &model).map_err(ModelError::InputSerializeJsonError)?;
        Ok(())
    }

    pub fn validate(&self) -> Result<(), ModelError> {
        let Self { comment: _, settings, dimensions, materials, layers, profile, width, string, masses, damping } = self;

        settings.validate()?;
        dimensions.validate()?;
        profile.validate()?;
        width.validate()?;
        string.validate()?;
        masses.validate()?;
        damping.validate()?;

        for material in materials {
            material.validate()?;
        }
        for layer in layers {
            layer.validate()?;
        }

        Ok(())
    }
}

impl Default for BowModel {
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
                Layer::new("Unnamed", "Default", Height::constant(0.001))
            ],
            profile: Profile::new(ProfileAlignment::SectionBack, vec![
                ProfileSegment::Line(Line::new(1.0))
            ]),
            width: Width::new(vec![
                [0.0, 0.05],
                [0.5, 0.04],
                [1.0, 0.01]
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
        if !is_hex_color(color) {
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

impl Layer {
    const REL_LENGTH_TOL: f64 = 1e-9;    // Tolerance used for validating inputs that are relative lengths

    pub fn new(name: &str, material: &str, height: Height) -> Self {
        Self {
            name: name.to_string(),
            material: material.to_string(),
            height
        }
    }

    pub fn validate(&self) -> Result<(), ModelError> {
        let Self { name, material: _, height } = self;

        let height = &height.0;

        if name.is_empty() {
            return Err(ModelError::LayerInvalidName(name.clone()));
        }

        // Material index is validated by the cross sections

        // TODO: Height should be validated by the cross section in order to give an error message with the index of the layer
        // Or maybe implement validation on Vec<Layer>?

        if height.len() < 2 {
            return Err(ModelError::LayerHeightControlPointsTooFew(height.len()));
        }

        if let Some((a, b)) = height.iter().tuple_windows().find(|(a, b)| b[0] <= a[0]) {
            return Err(ModelError::LayerHeightControlPointsNotSorted(a[0], b[0]));
        }

        let first = height.first().unwrap();
        let last = height.last().unwrap();

        if (first[0] < 0.0 - Width::REL_LENGTH_TOL) || (last[0] > 1.0 + Width::REL_LENGTH_TOL) {
            return Err(ModelError::LayerHeightControlPointsInvalidRange(first[0], last[0]));
        }

        for (i, a) in height.iter().enumerate() {
            if i == 0 || i == height.len()-1 {
                if !a[0].is_finite() || !a[1].is_finite() || a[0] < 0.0 || a[1] < 0.0 {
                    return Err(ModelError::LayerHeightControlPointsInvalidBoundaryValue(a[0], a[1]));
                }
            }
            else {
                if !a[0].is_finite() || !a[1].is_finite() || a[0] <= 0.0 || a[1] <= 0.0 {
                    return Err(ModelError::LayerHeightControlPointsInvalidInteriorValue(a[0], a[1]));
                }
            }
        }

        if first[0] > 0.0 + Layer::REL_LENGTH_TOL && first[1] != 0.0 {
            return Err(ModelError::LayerHeightControlPointsDiscontinuousBoundary(first[0], first[1]));
        }

        if last[0] < 1.0 - Layer::REL_LENGTH_TOL && last[1] != 0.0 {
            return Err(ModelError::LayerHeightControlPointsDiscontinuousBoundary(last[0], last[1]));
        }

        Ok(())
    }
}

impl Profile {
    pub fn new(alignment: ProfileAlignment, segments: Vec<ProfileSegment>) -> Self {
        Self {
            alignment,
            segments
        }
    }

    pub fn validate(&self) -> Result<(), ModelError> {
        let Self { alignment, segments } = self;

        match alignment {
            ProfileAlignment::LayerBack(name) | ProfileAlignment::LayerBelly(name) | ProfileAlignment::LayerCenter(name) => {
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

impl Width {
    const REL_LENGTH_TOL: f64 = 1e-9;    // Tolerance used for validating inputs that are relative lengths

    pub fn new(points: Vec<[f64; 2]>) -> Self {
        Self(points)
    }

    // Convenience function for creating a constant width distribution
    pub fn constant(w: f64) -> Self {
        Self::new(vec![
            [0.0, w],
            [1.0, w]
        ])
    }

    // Convenience function for creating a linear width distribution
    pub fn linear(w0: f64, w1: f64) -> Self {
        Self::new(vec![
            [0.0, w0],
            [1.0, w1]
        ])
    }

    pub fn validate(&self) -> Result<(), ModelError> {
        let Self( points ) = self;

        if points.len() < 2 {
            return Err(ModelError::WidthControlPointsTooFew(points.len()));
        }

        if let Some((a, b)) = points.iter().tuple_windows().find(|(a, b)| b[0] <= a[0]) {
            return Err(ModelError::WidthControlPointsNotSorted(a[0], b[0]));
        }

        let first = points.first().unwrap();
        let last = points.last().unwrap();

        if ((first[0] - 0.0).abs() > Width::REL_LENGTH_TOL) || ((last[0] - 1.0).abs() > Width::REL_LENGTH_TOL) {
            return Err(ModelError::WidthControlPointsInvalidRange(first[0], last[0]));
        }

        if let Some(a) = points.iter().find(|a| !a[0].is_finite() || !a[1].is_finite() || a[1] <= 0.0) {
            return Err(ModelError::WidthControlPointsInvalidValue(a[0], a[1]));
        }

        Ok(())
    }
}

impl Height {
    const REL_LENGTH_TOL: f64 = 1e-9;    // Tolerance used for validating inputs that are relative lengths

    pub fn new(points: Vec<[f64; 2]>) -> Self {
        Self(points)
    }

    // Convenience function for creating a constant width distribution
    pub fn constant(h: f64) -> Self {
        Self::new(vec![
            [0.0, h],
            [1.0, h]
        ])
    }

    // Convenience function for creating a linear width distribution
    pub fn linear(h0: f64, h1: f64) -> Self {
        Self::new(vec![
            [0.0, h0],
            [1.0, h1]
        ])
    }

    // TODO: Use validation function within layer validation?
    // TODO: This was copied from width, does it apply to height as well? Is it even used?
    pub fn validate(&self) -> Result<(), ModelError> {
        let Self( points ) = self;

        if points.len() < 2 {
            return Err(ModelError::WidthControlPointsTooFew(points.len()));
        }

        if let Some((a, b)) = points.iter().tuple_windows().find(|(a, b)| b[0] <= a[0]) {
            return Err(ModelError::WidthControlPointsNotSorted(a[0], b[0]));
        }

        let first = points.first().unwrap();
        let last = points.last().unwrap();

        if ((first[0] - 0.0).abs() > Height::REL_LENGTH_TOL) || ((last[0] - 1.0).abs() > Height::REL_LENGTH_TOL) {
            return Err(ModelError::WidthControlPointsInvalidRange(first[0], last[0]));
        }

        if let Some(a) = points.iter().find(|a| !a[0].is_finite() || !a[1].is_finite() || a[1] <= 0.0) {
            return Err(ModelError::WidthControlPointsInvalidValue(a[0], a[1]));
        }

        Ok(())
    }
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

impl ProfileSegment {
    pub fn validate(&self, index: usize) -> Result<(), ModelError> {
        match self {
            ProfileSegment::Line(input)   => input.validate(index),
            ProfileSegment::Arc(input)    => input.validate(index),
            ProfileSegment::Spiral(input) => input.validate(index),
            ProfileSegment::Spline(input) => input.validate(index),
        }
    }
}

impl Line {
    pub fn new(length: f64) -> Self {
        Self {
            length
        }
    }

    pub fn validate(&self, index: usize) -> Result<(), ModelError> {
        let &Self { length } = self;

        if !length.is_finite() || length <= 0.0 {
            return Err(ModelError::LineSegmentInvalidLength(index, length));
        }

        Ok(())
    }
}

impl Arc {
    pub fn new(length: f64, radius: f64) -> Self {
        Self {
            length,
            radius,
        }
    }

    pub fn validate(&self, index: usize) -> Result<(), ModelError> {
        let &Self { length, radius } = self;

        if !length.is_finite() || length <= 0.0 {
            return Err(ModelError::ArcSegmentInvalidLength(index, length));
        }
        if !radius.is_finite() {
            return Err(ModelError::ArcSegmentInvalidRadius(index, radius));
        }

        Ok(())
    }
}

impl Spiral {
    pub fn new(length: f64, radius0: f64, radius1: f64) -> Self {
        Self {
            length,
            radius0,
            radius1
        }
    }

    pub fn validate(&self, index: usize) -> Result<(), ModelError> {
        let &Self { length, radius0, radius1 } = self;

        if !length.is_finite() || length <= 0.0 {
            return Err(ModelError::SpiralSegmentInvalidLength(index, length));
        }
        if !radius0.is_finite() {
            return Err(ModelError::SpiralSegmentInvalidRadius1(index, radius0));
        }
        if !radius1.is_finite() {
            return Err(ModelError::SpiralSegmentInvalidRadius2(index, radius1));
        }

        Ok(())
    }
}

impl Spline {
    pub fn new(points: Vec<[f64; 2]>) -> Self {
        Self {
            points
        }
    }

    pub fn validate(&self, index: usize) -> Result<(), ModelError> {
        let Self { points } = self;

        if points.len() < 2 {
            return Err(ModelError::SplineSegmentTooFewPoints(index, points.len()));
        }
        for point in points {
            if !point[0].is_finite() || !point[1].is_finite() {
                return Err(ModelError::SplineSegmentInvalidPoint(index, *point));
            }
        }

        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::PathBuf;
    use itertools::Itertools;
    use assert2::assert;
    use serde::Serialize;
    use serde_json::{json, Value};

    #[test]
    fn test_model_conversion() {
        // This test loads bow files that are supposed to be equivalent but have been saved in different file input.
        // It checks whether they can be loaded/saved successfully and if the resulting model data is equivalent.
        // See also the readme file in the referenced folder.
        for entry in std::fs::read_dir("bows/versions").unwrap() {
            let path = entry.unwrap().path();
            if path.is_dir() {
                println!("{:?}", path);
                check_version_folder(path);
            }
        }
    }

    // Finds and loads all .bow files in a given directory and returns the file paths and model data
    fn load_models_from_dir<P: AsRef<Path>>(path: P) -> Vec<(PathBuf, BowModel)> {
        std::fs::read_dir(&path).unwrap()
            .map(|entry| entry.unwrap().path())
            .filter(|path| path.is_file())
            .filter(|path| path.extension().map(|s| s == "bow").unwrap())
            .map(|file| {
                let model = BowModel::load(&file).unwrap_or_else(|_| panic!("Failed to load model {:?}", file));
                return (file, model);
            })
            .collect()
    }

    // Performs loading and conversion checks for the model files found in the given directory
    fn check_version_folder<P: AsRef<Path>>(path: P) {
        // Check 1: Load all models from the folder, which verifies that they can be loaded and converted
        let models = load_models_from_dir(&path);
        assert!(!models.is_empty());

        // Check 2: The resulting model data after conversion must be equal for each file
        for ((file_a, model_a), (file_b, model_b)) in models.iter().tuple_windows() {
            assert!(model_a == model_b, "Model data of {:?} and {:?} is not equal", file_a, file_b);
        }

        // Save model data in the latest version
        let (_, model) = &models[0];
        let file = path.as_ref().join("latest.bow");
        model.save(&file).unwrap_or_else(|_| panic!("Failed to save model {:?}", file));

        // Load it again and check for equality
        let loaded = BowModel::load(&file).unwrap_or_else(|_| panic!("Failed to load model {:?}", file));
        assert!(loaded == *model, "Model data of {:?} must be equal to its source", file);

        // Load it once more as a Json Value and check if the version entry matches the Cargo package version
        let mut reader = File::open(&file).unwrap_or_else(|_| panic!("Failed to load file {:?}", file));
        let value: Value = serde_json::from_reader(&mut reader).unwrap_or_else(|_| panic!("Failed to parse file {:?}", file));
        let version = value.get("version").unwrap_or_else(|| panic!("Model {:?} has no version entry", file));
        assert!(version == &json!(env!("CARGO_PKG_VERSION")), "Version of model {:?} does not match the Cargo package version", file);
    }

    #[test]
    fn test_load_model() {
        generate_test_files();

        // IO error when loading from an invalid path
        assert_matches!(BowModel::load("bows/tests/input/nonexistent.bow"), Err(ModelError::InputLoadFileError(_, _)));

        // Deserialization error due to the file containing invalid json
        assert_matches!(BowModel::load("bows/tests/input/invalid_json.bow"), Err(ModelError::InputDeserializeJsonError(_)));

        // Deserialization error due to the file containing valid json but no version entry
        assert_matches!(BowModel::load("bows/tests/input/version_missing.bow"), Err(ModelError::InputDeserializeJsonError(_)));

        // Deserialization error due to the file containing valid json but an invalid version entry (wrong type)
        assert_matches!(BowModel::load("bows/tests/input/version_invalid.bow"), Err(ModelError::InputDeserializeJsonError(_)));

        // Error when loading a bow file with a version that is unsupported (too old)
        assert_matches!(BowModel::load("bows/tests/input/version_unsupported.bow"), Err(ModelError::InputVersionUnsupported));

        // Error when loading a bow file with a version that is not recognized
        assert_matches!(BowModel::load("bows/tests/input/version_unrecognized.bow"), Err(ModelError::InputVersionUnrecognized));

        // Deserialization error due to invalid file contents (valid json and version but invalid structure)
        assert_matches!(BowModel::load("bows/tests/input/invalid_content.bow"), Err(ModelError::InputDeserializeJsonError(_)));

        // No error when loading a valid bow model
        assert_matches!(BowModel::load("bows/tests/input/valid_model.bow"), Ok(_));
    }

    #[test]
    fn test_save_model() {
        let model = BowModel::default();

        // IO error from saving to an invalid path
        assert_matches!(model.save("bows/tests/input/nonexistent/valid.bow"), Err(ModelError::InputSaveFileError(_, _)));

        // The only remaining error case is a serialization error, but that one is difficult to trigger.
        // Saving without error is already covered by the generation of the test files above.
    }

    fn generate_test_files() {
        // File that contains invalid json content, in this case just an empty fie
        File::create("bows/tests/input/invalid_json.bow").unwrap();

        // File with valid json but no version entry
        let mut file = File::create("bows/tests/input/version_missing.bow").unwrap();
        let data = NoVersion { x: 1.0, y: 2.0, z: 3.0, };
        serde_json::to_writer_pretty(&mut file, &data).unwrap();

        // File with valid json but invalid version entry (wrong type)
        let mut file = File::create("bows/tests/input/version_invalid.bow").unwrap();
        let data = VersionUsize { version: 7, x: 1.0, y: 2.0, z: 3.0, };
        serde_json::to_writer_pretty(&mut file, &data).unwrap();

        // File with valid json and valid version entry but the version is unsupported (too old)
        let mut file = File::create("bows/tests/input/version_unsupported.bow").unwrap();
        let data = VersionString { version: "0.3".to_string(), x: 1.0, y: 2.0, z: 3.0, };
        serde_json::to_writer_pretty(&mut file, &data).unwrap();

        // File with valid json and valid version entry but the version is unknown
        let mut file = File::create("bows/tests/input/version_unrecognized.bow").unwrap();
        let data = VersionString { version: "xyz".to_string(), x: 1.0, y: 2.0, z: 3.0, };
        serde_json::to_writer_pretty(&mut file, &data).unwrap();

        // File that contains valid json with matching version but not a valid bow model
        let mut file = File::create("bows/tests/input/invalid_content.bow").unwrap();
        let data = VersionString { version: env!("CARGO_PKG_VERSION").to_string(), x: 1.0, y: 2.0, z: 3.0, };
        serde_json::to_writer_pretty(&mut file, &data).unwrap();

        // File that contains valid bow model data the correct version
        let model = BowModel::default();
        model.save("bows/tests/input/valid_model.bow").unwrap();
    }

    #[derive(Serialize)]
    struct NoVersion {
        x: f64,
        y: f64,
        z: f64
    }

    #[derive(Serialize)]
    struct VersionUsize {
        version: usize,
        x: f64,
        y: f64,
        z: f64
    }

    #[derive(Serialize)]
    struct VersionString {
        version: String,
        x: f64,
        y: f64,
        z: f64
    }
}