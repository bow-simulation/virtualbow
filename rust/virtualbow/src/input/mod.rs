mod versions;
mod v1;
mod v2;
mod v3;
mod v4;

use std::fs::File;
use std::path::Path;
use itertools::Itertools;
use crate::errors::ModelError;
use crate::utils::validation::{FloatValidation, IntegerValidation, StringValidation};
use versions::BowModelVersions;

pub use v4::*;  // Export latest version to the outside and implement below

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

    // Create a simple but valid example bow
    pub fn example() -> Self {
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

        n_limb_elements.validate_positive().map_err(ModelError::SettingsInvalidLimbElements)?;
        n_limb_eval_points.validate_at_least(2).map_err(ModelError::SettingsInvalidLimbEvalPoints)?;
        min_draw_resolution.validate_positive().map_err(ModelError::SettingsInvalidMinDrawResolution)?;
        max_draw_resolution.validate_positive().map_err(ModelError::SettingsInvalidMaxDrawResolution)?;

        arrow_clamp_force.validate_nonneg().map_err(ModelError::SettingsInvalidArrowClampForce)?;
        string_compression_factor.validate_positive().map_err(ModelError::SettingsInvalidStringCompressionFactor)?;
        timespan_factor.validate_at_least(1.0).map_err(ModelError::SettingsInvalidTimeSpanFactor)?;
        timeout_factor.validate_at_least(1.0).map_err(ModelError::SettingsInvalidTimeOutFactor)?;

        min_timestep.validate_positive().map_err(ModelError::SettingsInvalidMinTimeStep)?;
        max_timestep.validate_positive().map_err(ModelError::SettingsInvalidMaxTimeStep)?;
        steps_per_period.validate_positive().map_err(ModelError::SettingsInvalidStepsPerPeriod)?;

        Ok(())
    }
}

impl Dimensions {
    pub fn validate(&self) -> Result<(), ModelError> {
        let &Self { brace_height, draw_length, handle_length, handle_setback, handle_angle} = self;

        brace_height.validate_positive().map_err(ModelError::DimensionsInvalidBraceHeight)?;
        draw_length.validate_larger_than(brace_height).map_err(ModelError::DimensionsInvalidDrawLength)?;

        handle_length.validate_nonneg().map_err(ModelError::DimensionsInvalidHandleLength)?;
        handle_setback.validate_finite().map_err(ModelError::DimensionsInvalidHandleSetback)?;
        handle_angle.validate_finite().map_err(ModelError::DimensionsInvalidHandleAngle)?;

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

        name.validate_name().map_err(ModelError::MaterialInvalidName)?;
        color.validate_hex_color().map_err(ModelError::MaterialInvalidColor)?;

        rho.validate_positive().map_err(ModelError::MaterialInvalidDensity)?;
        E.validate_positive().map_err(ModelError::MaterialInvalidDensity)?;
        G.validate_positive().map_err(ModelError::MaterialInvalidDensity)?;

        Ok(())
    }
}

impl Layer {
    pub fn new(name: &str, material: &str, height: Height) -> Self {
        Self {
            name: name.to_string(),
            material: material.to_string(),
            height
        }
    }

    pub fn validate(&self) -> Result<(), ModelError> {
        let Self { name, material, height } = self;

        name.validate_name().map_err(ModelError::LayerInvalidName)?;
        material.validate_name().map_err(ModelError::LayerInvalidName)?;
        height.validate()?;

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
                name.validate_name().map_err(ModelError::ProfileAnlignemtInvalidLayerName)?;
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

        // At least two control points are required
        points.len().validate_at_least(2).map_err(ModelError::WidthControlPointsTooFew)?;

        // The control points must be sorted by strictly increasing position
        if let Some((a, b)) = points.iter().tuple_windows().find(|(a, b)| b[0] <= a[0]) {
            return Err(ModelError::WidthControlPointsNotSorted(a[0], b[0]));
        }

        let first = points.first().unwrap();
        let last = points.last().unwrap();

        // The control points must cover the range 0 to 1 exactly
        first[0].validate_equals(0.0).map_err(|_| ModelError::WidthControlPointsInvalidRange(first[0], last[0]))?;
        last[0].validate_equals(1.0).map_err(|_| ModelError::WidthControlPointsInvalidRange(first[0], last[0]))?;

        // The widths contained in the control points must be strictly positive
        if let Some(a) = points.iter().find(|a| !a[0].is_finite() || !a[1].is_finite() || a[1] <= 0.0) {
            return Err(ModelError::WidthControlPointsInvalidValue(a[0], a[1]));
        }

        Ok(())
    }
}

impl Height {
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

    pub fn validate(&self) -> Result<(), ModelError> {
        let Self( points ) = self;

        // At least two control points are required
        points.len().validate_at_least(2).map_err(ModelError::LayerHeightControlPointsTooFew)?;

        // Control points must be sorted by strictly increasing position
        if let Some((a, b)) = points.iter().tuple_windows().find(|(a, b)| b[0] <= a[0]) {
            return Err(ModelError::LayerHeightControlPointsNotSorted(a[0], b[0]));
        }

        let first = points.first().unwrap();
        let last = points.last().unwrap();

        // First control point must lie within the range 0 to 1 and its height must be positive or zero
        first[0].validate_range_inclusive(0.0, 1.0).map_err(|_| ModelError::LayerHeightControlPointsInvalidRange(first[0], last[0]))?;
        first[1].validate_nonneg().map_err(|_| ModelError::LayerHeightControlPointsInvalidBoundaryValue(first[0], first[1]))?;

        // Last control point must lie within the range 0 to 1 and its height must be positive or zero
        last[0].validate_range_inclusive(0.0, 1.0).map_err(|_| ModelError::LayerHeightControlPointsInvalidRange(first[0], last[0]))?;
        last[1].validate_nonneg().map_err(|_| ModelError::LayerHeightControlPointsInvalidBoundaryValue(last[0], last[1]))?;

        // Other non-boundary points must have s positive height
        for point in points.iter().skip(1).take(points.len() - 2) {
            point[1].validate_positive().map_err(|_| ModelError::LayerHeightControlPointsInvalidInteriorValue(point[0], point[1]))?;
        }

        // If the first control point is not at position zero, it must have a height of zero for continuity reasons
        if first[0] > 0.0 && first[1] != 0.0 {
            return Err(ModelError::LayerHeightControlPointsDiscontinuousBoundary(first[0], first[1]));
        }

        // If the last control point is not at position 1, it must have a height of zero for continuity reasons
        if last[0] < 1.0 && last[1] != 0.0 {
            return Err(ModelError::LayerHeightControlPointsDiscontinuousBoundary(last[0], last[1]));
        }

        Ok(())
    }
}

impl BowString {
    pub fn validate(&self) -> Result<(), ModelError> {
        let &Self { n_strands, strand_density, strand_stiffness } = self;
        n_strands.validate_positive().map_err(ModelError::StringInvalidNumberOfStrands)?;
        strand_density.validate_positive().map_err(ModelError::StringInvalidStrandDensity)?;
        strand_stiffness.validate_positive().map_err(ModelError::StringInvalidStrandStiffness)?;

        Ok(())
    }
}

impl Masses {
    pub fn validate(&self) -> Result<(), ModelError> {
        let &Self { arrow, limb_tip, string_center, string_tip } = self;
        arrow.validate_positive().map_err(ModelError::MassesInvalidArrowMass)?;
        limb_tip.validate_nonneg().map_err(ModelError::MassesInvalidLimbTipMass)?;
        string_center.validate_nonneg().map_err(ModelError::MassesInvalidStringCenterMass)?;
        string_tip.validate_nonneg().map_err(ModelError::MassesInvalidStringTipMass)?;

        Ok(())
    }
}

impl Damping {
    pub fn validate(&self) -> Result<(), ModelError> {
        let &Self { damping_ratio_limbs, damping_ratio_string } = self;
        damping_ratio_limbs.validate_range_inclusive(0.0, 1.0).map_err(ModelError::DampingInvalidLimbDampingRatio)?;
        damping_ratio_string.validate_range_inclusive(0.0, 1.0).map_err(ModelError::DampingInvalidStringDampingRatio)?;

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
        length.validate_positive().map_err(|_| ModelError::LineSegmentInvalidLength(index, length))?;

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
        length.validate_positive().map_err(|_| ModelError::ArcSegmentInvalidLength(index, length))?;
        radius.validate_finite().map_err(|_| ModelError::ArcSegmentInvalidRadius(index, length))?;

        Ok(())
    }
}

impl Spiral {
    pub fn new(length: f64, radius0: f64, radius1: f64) -> Self {
        Self {
            length,
            radius_start: radius0,
            radius_end: radius1
        }
    }

    pub fn validate(&self, index: usize) -> Result<(), ModelError> {
        let &Self { length, radius_start: radius0, radius_end: radius1 } = self;
        length.validate_positive().map_err(|_| ModelError::SpiralSegmentInvalidLength(index, length))?;
        radius0.validate_finite().map_err(|_| ModelError::SpiralSegmentInvalidRadius0(index, length))?;
        radius1.validate_finite().map_err(|_| ModelError::SpiralSegmentInvalidRadius1(index, length))?;

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

        points.len().validate_at_least(2).map_err(|_| ModelError::SplineSegmentTooFewPoints(index, points.len()))?;

        for point in points {
            point[0].validate_finite().map_err(|_| ModelError::SplineSegmentInvalidPoint(index, *point))?;
            point[1].validate_finite().map_err(|_| ModelError::SplineSegmentInvalidPoint(index, *point))?;
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
    use assert_matches::assert_matches;
    use serde::Serialize;
    use serde_json::{json, Value};

    #[test]
    fn test_model_conversion() {
        // This test loads bow files that are supposed to be equivalent but have been saved in different file input.
        // It checks whether they can be loaded/saved successfully and if the resulting model data is equivalent.
        // See also the readme file in the referenced folder.
        for entry in std::fs::read_dir("data/versions").unwrap() {
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
        assert_matches!(BowModel::load("data/input/nonexistent.bow"), Err(ModelError::InputLoadFileError(_, _)));

        // Deserialization error due to the file containing invalid json
        assert_matches!(BowModel::load("data/input/invalid_json.bow"), Err(ModelError::InputDeserializeJsonError(_)));

        // Deserialization error due to the file containing valid json but no version entry
        assert_matches!(BowModel::load("data/input/version_missing.bow"), Err(ModelError::InputDeserializeJsonError(_)));

        // Deserialization error due to the file containing valid json but an invalid version entry (wrong type)
        assert_matches!(BowModel::load("data/input/version_invalid.bow"), Err(ModelError::InputDeserializeJsonError(_)));

        // Error when loading a bow file with a version that is unsupported (too old)
        assert_matches!(BowModel::load("data/input/version_unsupported.bow"), Err(ModelError::InputVersionUnsupported));

        // Error when loading a bow file with a version that is not recognized
        assert_matches!(BowModel::load("data/input/version_unrecognized.bow"), Err(ModelError::InputVersionUnrecognized));

        // Deserialization error due to invalid file contents (valid json and version but invalid structure)
        assert_matches!(BowModel::load("data/input/invalid_content.bow"), Err(ModelError::InputDeserializeJsonError(_)));

        // No error when loading a valid bow model
        assert_matches!(BowModel::load("data/input/valid_model.bow"), Ok(_));
    }

    #[test]
    fn test_save_model() {
        let model = BowModel::example();

        // IO error from saving to an invalid path
        assert_matches!(model.save("data/input/nonexistent/valid.bow"), Err(ModelError::InputSaveFileError(_, _)));

        // The only remaining error case is a serialization error, but that one is difficult to trigger.
        // Saving without error is already covered by the generation of the test files above.
    }

    fn generate_test_files() {
        // File that contains invalid json content, in this case just an empty fie
        File::create("data/input/invalid_json.bow").unwrap();

        // File with valid json but no version entry
        let mut file = File::create("data/input/version_missing.bow").unwrap();
        let data = NoVersion { x: 1.0, y: 2.0, z: 3.0, };
        serde_json::to_writer_pretty(&mut file, &data).unwrap();

        // File with valid json but invalid version entry (wrong type)
        let mut file = File::create("data/input/version_invalid.bow").unwrap();
        let data = VersionUsize { version: 7, x: 1.0, y: 2.0, z: 3.0, };
        serde_json::to_writer_pretty(&mut file, &data).unwrap();

        // File with valid json and valid version entry but the version is unsupported (too old)
        let mut file = File::create("data/input/version_unsupported.bow").unwrap();
        let data = VersionString { version: "0.3".to_string(), x: 1.0, y: 2.0, z: 3.0, };
        serde_json::to_writer_pretty(&mut file, &data).unwrap();

        // File with valid json and valid version entry but the version is unknown
        let mut file = File::create("data/input/version_unrecognized.bow").unwrap();
        let data = VersionString { version: "xyz".to_string(), x: 1.0, y: 2.0, z: 3.0, };
        serde_json::to_writer_pretty(&mut file, &data).unwrap();

        // File that contains valid json with matching version but not a valid bow model
        let mut file = File::create("data/input/invalid_content.bow").unwrap();
        let data = VersionString { version: env!("CARGO_PKG_VERSION").to_string(), x: 1.0, y: 2.0, z: 3.0, };
        serde_json::to_writer_pretty(&mut file, &data).unwrap();

        // File that contains valid bow model data the correct version
        let model = BowModel::example();
        model.save("data/input/valid_model.bow").unwrap();
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