use std::fmt::{Display, Formatter};

use std::path::PathBuf;
use virtualbow_num::fem::solvers::dynamics::DynamicSolverError;
use virtualbow_num::fem::solvers::eigen::EigenSolverError;
use virtualbow_num::fem::solvers::statics::StaticSolverError;

#[derive(Debug)]
pub enum ModelError {
    InputLoadFileError(PathBuf, std::io::Error),
    InputSaveFileError(PathBuf, std::io::Error),
    InputSerializeJsonError(serde_json::error::Error),
    InputDeserializeJsonError(serde_json::error::Error),
    InputVersionUnsupported,  // TODO: Create issue for getting out the actual version information
    InputVersionUnrecognized,  // TODO: Create issue for getting out the actual version information
    InputEncodeMsgPackError(rmp_serde::encode::Error),
    InputDecodeMsgPackError(rmp_serde::decode::Error),

    OutputLoadFileError(PathBuf, std::io::Error),
    OutputSaveFileError(PathBuf, std::io::Error),
    OutputEncodeMsgPackError(rmp_serde::encode::Error),
    OutputDecodeMsgPackError(rmp_serde::decode::Error),
    OutputVersionUnsupported,  // TODO: Create issue for getting out the actual version information
    OutputVersionUnrecognized,  // TODO: Create issue for getting out the actual version information

    SettingsInvalidLimbElements(usize),
    SettingsInvalidLimbEvalPoints(usize),
    SettingsInvalidMinDrawResolution(usize),
    SettingsInvalidMaxDrawResolution(usize),
    SettingsInvalidStaticTolerance(f64),
    SettingsInvalidArrowClampForce(f64),
    SettingsInvalidStringCompressionFactor(f64),
    SettingsInvalidTimeSpanFactor(f64),
    SettingsInvalidTimeOutFactor(f64),
    SettingsInvalidMinTimeStep(f64),
    SettingsInvalidMaxTimeStep(f64),
    SettingsInvalidStepsPerPeriod(usize),
    SettingsInvalidDynamicTolerance(f64),

    DimensionsInvalidBraceHeight(f64),
    DimensionsInvalidDrawLength(f64),

    HandleInvalidLength(f64),
    HandleInvalidAngle(f64),
    HandleInvalidPivot(f64),

    MaterialInvalidName(String),
    MaterialInvalidColor(String, String),
    MaterialInvalidDensity(String, f64),
    MaterialInvalidYoungsModulus(String, f64),
    MaterialInvalidShearModulus(String, f64),
    MaterialInvalidTensileStrength(String, f64),
    MaterialInvalidCompressiveStrength(String, f64),
    MaterialInvalidSafetyMargin(String, f64),

    WidthControlPointsTooFew(usize),
    WidthControlPointsNotSorted(f64, f64),
    WidthControlPointsInvalidRange(f64, f64),
    WidthControlPointsInvalidValue(f64, f64),

    LayerInvalidName(String),
    LayerInvalidMaterial(String, String),
    LayerHeightControlPointsTooFew(String, usize),
    LayerHeightControlPointsNotSorted(String, f64, f64),
    LayerHeightControlPointsInvalidRange(String, f64, f64),
    LayerHeightControlPointsInvalidBoundaryValue(String, f64, f64),
    LayerHeightControlPointsInvalidInteriorValue(String, f64, f64),
    LayerHeightControlPointsDiscontinuousBoundary(String, f64, f64),

    StringInvalidNumberOfStrands(usize),
    StringInvalidStrandDensity(f64),
    StringInvalidStrandStiffness(f64),

    MassesInvalidArrowMass(f64),
    MassesInvalidArrowMassPerForce(f64),
    MassesInvalidArrowMassPerEnergy(f64),
    MassesInvalidLimbTipMass(f64),
    MassesInvalidStringCenterMass(f64),
    MassesInvalidStringTipMass(f64),

    DampingInvalidLimbDampingRatio(f64),
    DampingInvalidStringDampingRatio(f64),

    CrossSectionNoLayers,
    CrossSectionNoMaterials,
    CrossSectionDuplicateMaterialName(String),
    CrossSectionInvalidMaterialName(String, String),
    CrossSectionDuplicateLayerName(String),
    CrossSectionInvalidLayerName(String),
    CrossSectionZeroCombinedHeight(f64),

    ProfileNoSegments,
    ProfileAnlignemtInvalidLayerName(String),
    LineSegmentInvalidLength(usize, f64),
    ArcSegmentInvalidLength(usize, f64),
    ArcSegmentInvalidRadius(usize, f64),
    SpiralSegmentInvalidLength(usize, f64),
    SpiralSegmentInvalidRadius0(usize, f64),
    SpiralSegmentInvalidRadius1(usize, f64),
    SplineSegmentTooFewPoints(usize, usize),
    SplineSegmentInvalidPoint(usize, [f64; 2]),

    GeometrySelfIntersectionBack(f64),
    GeometrySelfIntersectionBelly(f64),

    SimulationBraceHeightTooLow(f64),
    SimulationBracingNoSignChange,
    SimulationBracingNoConvergence,
    SimulationEigenSolutionFailed(EigenSolverError),
    SimulationStaticSolutionFailed(StaticSolverError),
    SimulationDynamicSolutionFailed(DynamicSolverError),
}

impl Display for ModelError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            ModelError::InputLoadFileError(path, error)  => write!(f, "Failed to open file {path:?}: {error}")?,
            ModelError::InputSaveFileError(path, error)  => write!(f, "Failed to write file {path:?}: {error}")?,
            ModelError::InputSerializeJsonError(error)   => write!(f, "Failed to convert bow model to json: {error}")?,
            ModelError::InputDeserializeJsonError(error) => write!(f, "Failed to parse bow model from json: {error}")?,
            ModelError::InputVersionUnsupported          => write!(f, "Failed to load bow model: Files below version 0.7 are no longer supported.")?,
            ModelError::InputVersionUnrecognized         => write!(f, "Failed to load bow model: File version is not recognized. The file might have been created with a newer version of VirtualBow, in which case updating might solve the issue.")?,
            ModelError::InputEncodeMsgPackError(error)   => write!(f, "Failed to encode bow model as msgpack: {error}")?,
            ModelError::InputDecodeMsgPackError(error)   => write!(f, "Failed to decode bow model from msgpack: {error}")?,

            ModelError::OutputLoadFileError(path, error)   => write!(f, "Failed to open file {path:?}: {error}")?,
            ModelError::OutputSaveFileError(path, error)   => write!(f, "Failed to write file {path:?}: {error}")?,
            ModelError::OutputEncodeMsgPackError(error)    => write!(f, "Failed to encode bow results to msgpack: {error}")?,
            ModelError::OutputDecodeMsgPackError(error)    => write!(f, "Failed to decode bow results from msgpack: {error}")?,
            ModelError::OutputVersionUnsupported           => write!(f, "Failed to load results, file version is not supported. VirtualBow result files can only be opened with the same version of VirtualBow that created them. Please recreate the results or open them with a matching version of VirtualBow.")?,
            ModelError::OutputVersionUnrecognized          => write!(f, "Failed to load results, file version is not recognized. VirtualBow result files can only be opened with the same version of VirtualBow that created them. Please recreate the results or open them with a matching version of VirtualBow.")?,

            ModelError::SettingsInvalidLimbElements(value)            => write!(f, "Settings: Number of limb elements must be at least 1 but actual number is {value}.")?,
            ModelError::SettingsInvalidLimbEvalPoints(value)          => write!(f, "Settings: Number of limb evaluation points must be at least 2 but actual number is {value}.")?,
            ModelError::SettingsInvalidMinDrawResolution(value)       => write!(f, "Settings: Minimum draw length resolution must be at least 1 but actual number is {value}.")?,
            ModelError::SettingsInvalidMaxDrawResolution(value)       => write!(f, "Settings: Maximum draw length resolution must be at least 1 but actual number is {value}.")?,
            ModelError::SettingsInvalidStaticTolerance(value)           => write!(f, "Settings: Static iteration tolerance must be a positive number but actual value is {value}.")?,
            ModelError::SettingsInvalidArrowClampForce(value)         => write!(f, "Settings: Arrow clamp force must be a non-negative number but actual value is {value}.")?,
            ModelError::SettingsInvalidStringCompressionFactor(value) => write!(f, "Settings: String compression factor must be a positive number but actual value is {value}.")?,
            ModelError::SettingsInvalidTimeSpanFactor(value)          => write!(f, "Settings: Timespan factor must be larger or equal to one but actual value is {value}.")?,
            ModelError::SettingsInvalidTimeOutFactor(value)           => write!(f, "Settings: Timeout factor must be larger or equal to one but actual value is {value}.")?,
            ModelError::SettingsInvalidMinTimeStep(value)             => write!(f, "Settings: Minimum timestep must be a positive number but actual value is {value}.")?,
            ModelError::SettingsInvalidMaxTimeStep(value)             => write!(f, "Settings: Maximum timestep must be a positive number but actual value is {value}.")?,
            ModelError::SettingsInvalidStepsPerPeriod(value)          => write!(f, "Settings: Number of steps per period must be at least 1 but actual number is {value}.")?,
            ModelError::SettingsInvalidDynamicTolerance(value)        => write!(f, "Settings: Dynamic iteration tolerance must be a positive number but actual value is {value}.")?,

            ModelError::DimensionsInvalidBraceHeight(value)   => write!(f, "Dimensions: Brace height must be a finite number, actual value is {value}.")?,
            ModelError::DimensionsInvalidDrawLength(value)    => write!(f, "Dimensions: Draw length wrt. handle pivot must be finite and larger than the brace height but actual value is {value}.")?,

            ModelError::HandleInvalidLength(value) => write!(f, "Handle: Handle length must be a non-negative number but actual value is {value}.")?,
            ModelError::HandleInvalidAngle(value)  => write!(f, "Handle: Handle angle must be a finite number but actual value is {value}.")?,
            ModelError::HandleInvalidPivot(value)  => write!(f, "Handle: Handle pivot must be a finite number but actual value is {value}.")?,

            ModelError::MaterialInvalidName(value)                => write!(f, "Material: Name must be a non-empty string but actual value is \"{value}\".")?,
            ModelError::MaterialInvalidColor(name, value)         => write!(f, "Material \"{name}\": Color must be a hex color string (starting with #, followed by 6 digits of 0-9, a-f) but actual value is {value}.")?,
            ModelError::MaterialInvalidDensity(name, value)       => write!(f, "Material \"{name}\": Density must be a positive number but actual value is {value}.")?,
            ModelError::MaterialInvalidYoungsModulus(name, value) => write!(f, "Material \"{name}\": Young's modulus must be a positive number but actual value is {value}.")?,
            ModelError::MaterialInvalidShearModulus(name, value)  => write!(f, "Material \"{name}\": Shear modulus must be a positive number but actual value is {value}.")?,
            ModelError::MaterialInvalidTensileStrength(name, value)      => write!(f, "Material \"{name}\": Tensile strength must be a positive number but actual value is {value}.")?,
            ModelError::MaterialInvalidCompressiveStrength(name, value)  => write!(f, "Material \"{name}\": Compressive strength must be a positive number but actual value is {value}.")?,
            ModelError::MaterialInvalidSafetyMargin(name, value)         => write!(f, "Material \"{name}\": Safety margin must be in the range [0, 1] but actual value is {value}.")?,

            ModelError::WidthControlPointsTooFew(value)      => write!(f, "Width: At least 2 control points are required but actual number is {value}.")?,
            ModelError::WidthControlPointsNotSorted(a, b)    => write!(f, "Width: Control points must be sorted by length but found actual values {a}, {b}.")?,
            ModelError::WidthControlPointsInvalidRange(a, b) => write!(f, "Width: Control points must cover the range [0, 1] but actual range is [{a}, {b}].")?,
            ModelError::WidthControlPointsInvalidValue(a, b) => write!(f, "Width: Control points must be positive and finite but actual value found is [{a}, {b}].")?,

            ModelError::LayerInvalidName(value)                             => write!(f, "Layer: Name must be a non-empty string but actual value is \"{value}\".")?,
            ModelError::LayerInvalidMaterial(name, value)                   => write!(f, "Layer \"{name}\": Material name must be a non-empty string but actual value is \"{value}\".")?,
            ModelError::LayerHeightControlPointsTooFew(name, value)         => write!(f, "Layer \"{name}\": At least 2 control points are required but actual number is {value}.")?,
            ModelError::LayerHeightControlPointsNotSorted(name, a, b)             => write!(f, "Layer \"{name}\": Control points must be sorted by length but found actual values {a}, {b}.")?,
            ModelError::LayerHeightControlPointsInvalidRange(name, a, b)          => write!(f, "Layer \"{name}\": Control points must be in the range [0, 1] but actual range is [{a}, {b}].")?,
            ModelError::LayerHeightControlPointsInvalidBoundaryValue(name, a, b)  => write!(f, "Layer \"{name}\": Boundary control points must be non-negative and finite but actual value found is [{a}, {b}].")?,
            ModelError::LayerHeightControlPointsInvalidInteriorValue(name, a, b)  => write!(f, "Layer \"{name}\": Intermediate control points must be positive and finite but actual value found is [{a}, {b}].")?,
            ModelError::LayerHeightControlPointsDiscontinuousBoundary(name, a, b) => write!(f, "Layer \"{name}\": Boundary control point at relative length {a} must be zero for continuity but actual value is {b}.")?,

            ModelError::StringInvalidNumberOfStrands(value) => write!(f, "String: Number of strands must be at least 1 but actual number is {value}.")?,
            ModelError::StringInvalidStrandStiffness(value) => write!(f, "String: Strand density must be a positive number but actual value is {value}.")?,
            ModelError::StringInvalidStrandDensity(value)   => write!(f, "String: Strand stiffness must be a positive number but actual value is {value}.")?,

            ModelError::MassesInvalidArrowMass(value)          => write!(f, "Masses: Arrow mass must be a positive number but actual value is {value}.")?,
            ModelError::MassesInvalidArrowMassPerForce(value)  => write!(f, "Masses: Arrow mass per force must be a positive number but actual value is {value}.")?,
            ModelError::MassesInvalidArrowMassPerEnergy(value) => write!(f, "Masses: Arrow mass per energy must be a positive number but actual value is {value}.")?,
            ModelError::MassesInvalidLimbTipMass(value)      => write!(f, "Masses: Limb tip mass must be a non-negative number but actual value is {value}.")?,
            ModelError::MassesInvalidStringCenterMass(value) => write!(f, "Masses: String center mass must be a non-negative number but actual value is {value}.")?,
            ModelError::MassesInvalidStringTipMass(value)    => write!(f, "Masses: Strand tip mass must be a non-negative number but actual value is {value}.")?,

            ModelError::DampingInvalidLimbDampingRatio(value)   => write!(f, "Damping: Limb damping ratio must be in the range [0, 1] but actual value is {value}.")?,
            ModelError::DampingInvalidStringDampingRatio(value) => write!(f, "Damping: String damping ratio must be in the range [0, 1] but actual value is {value}.")?,

            ModelError::CrossSectionNoLayers                             => write!(f, "Cross section: At least one layer is required.")?,
            ModelError::CrossSectionNoMaterials                          => write!(f, "Cross section: At least one material is required.")?,
            ModelError::CrossSectionDuplicateMaterialName(material)      => write!(f, "Cross section: Material with name \"{material}\" is defined multiple times.")?,
            ModelError::CrossSectionInvalidMaterialName(layer, material) => write!(f, "Cross section: Cannot assign material \"{material}\" to layer \"{layer}\", no material with this name was found.")?,
            ModelError::CrossSectionDuplicateLayerName(layer)            => write!(f, "Cross section: Layer with name \"{layer}\" is defined multiple times.")?,
            ModelError::CrossSectionInvalidLayerName(layer)              => write!(f, "Cross section: Cannot align profile to layer \"{layer}\", no layer with this name was found.")?,
            ModelError::CrossSectionZeroCombinedHeight(length)           => write!(f, "Cross section: Combined height of all layers must be positive, but was found to be zero at relative length {length}")?,

            ModelError::ProfileNoSegments                         => write!(f, "Profile curve: At least one section is required")?,
            ModelError::ProfileAnlignemtInvalidLayerName(value)   => write!(f, "Profile curve: Layer name must be a non-empty string but actual value is \"{value}\".")?,
            ModelError::LineSegmentInvalidLength(index, value)    => write!(f, "Profile curve: Line segment at index {index} has an invalid length, must be a positive number but actual value is {value}.")?,
            ModelError::ArcSegmentInvalidLength(index, value)     => write!(f, "Profile curve: Arc segment at index {index} has an invalid length, must be a positive number but actual value is {value}.")?,
            ModelError::ArcSegmentInvalidRadius(index, value)     => write!(f, "Profile curve: Arc segment at index {index} has an invalid radius, must be a finite number but actual value is {value}.")?,
            ModelError::SpiralSegmentInvalidLength(index, value)  => write!(f, "Profile curve: Spiral segment at index {index} has an invalid length, must be a positive number but actual value is {value}.")?,
            ModelError::SpiralSegmentInvalidRadius0(index, value) => write!(f, "Profile curve: Spiral segment at index {index} has an invalid start radius, must be a finite number but actual value is {value}.")?,
            ModelError::SpiralSegmentInvalidRadius1(index, value) => write!(f, "Profile curve: Spiral segment at index {index} has an invalid end radius, must be a finite number but actual value is {value}.")?,
            ModelError::SplineSegmentTooFewPoints(index, value)   => write!(f, "Profile curve: Spline segment at index {index} requires at least two control points but actual number is {value}.")?,
            ModelError::SplineSegmentInvalidPoint(index, point)   => write!(f, "Profile curve: Spline segment at index {index} requires finite control points but found actual value {point:?}.")?,

            ModelError::GeometrySelfIntersectionBack(length) => write!(f, "Limb geometry: Self-intersection at the back of the limb at arc length {length}, curvature is too high for the thickness.")?,
            ModelError::GeometrySelfIntersectionBelly(length) => write!(f, "Limb geometry: Self-intersection at the belly of the limb at arc length {length}, curvature is too high for the thickness.")?,

            ModelError::SimulationBraceHeightTooLow(value)     => write!(f, "Simulation: The specified brace height of {value} is too low for the given bow profile.")?,
            ModelError::SimulationBracingNoSignChange          => write!(f, "Simulation: Failed to find the braced equilibrium state of the bow. No sign change of the string angle was found within the allowed number of iterations.")?,
            ModelError::SimulationBracingNoConvergence         => write!(f, "Simulation: Failed to find the braced equilibrium state of the bow. Terminal root finding algorithm did not converge to the required accuracy within the allowed number of iterations.")?,
            ModelError::SimulationEigenSolutionFailed(error)   => write!(f, "Simulation: Failure during eigenvalue solution: {error}")?,
            ModelError::SimulationStaticSolutionFailed(error)  => write!(f, "Simulation: Failure during the static simulation: {error}")?,
            ModelError::SimulationDynamicSolutionFailed(error) => write!(f, "Simulation: Failure during the dynamic simulation: {error}")?,
        }

        Ok(())
    }
}

impl std::error::Error for ModelError {

}