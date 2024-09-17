use std::fmt::{Display, Formatter};

use std::path::PathBuf;
use crate::fem::solvers::dynamics::DynamicSolverError;
use crate::fem::solvers::eigen::EigenSolverError;
use crate::fem::solvers::statics::StaticSolverError;

#[derive(Debug)]
pub enum ModelError {
    InputLoadFileError(PathBuf, std::io::Error),
    InputSaveFileError(PathBuf, std::io::Error),
    InputSerializeJsonError(serde_json::error::Error),
    InputDeserializeJsonError(serde_json::error::Error),
    InputInterpretJsonError(serde_json::error::Error),
    InputVersionNotFound,
    InputVersionInvalid(String),
    InputVersionTooOld(String),
    InputVersionTooNew(u64),
    InputConversionError(u64, u64, String),

    OutputLoadFileError(PathBuf, std::io::Error),
    OutputSaveFileError(PathBuf, std::io::Error),
    OutputEncodeMsgPackError(rmp_serde::encode::Error),
    OutputDecodeMsgPackError(rmpv::decode::Error),
    OutputInterpretMsgPackError(rmpv::ext::Error),
    OutputVersionNotFound,
    OutputVersionInvalid(String),
    OutputVersionMismatch(u64),

    SettingsInvalidLimbElements(usize),
    SettingsInvalidLimbEvalPoints(usize),
    SettingsInvalidLayerEvalPoints(usize),
    SettingsInvalidStringElements(usize),
    SettingsInvalidDrawSteps(usize),
    SettingsInvalidArrowClampForce(f64),
    SettingsInvalidTimeSpanFactor(f64),
    SettingsInvalidTimeStepFactor(f64),
    SettingsInvalidTimeOutFactor(f64),
    SettingsInvalidSamplingRate(f64),

    DimensionsInvalidBraceHeight(f64),
    DimensionsInvalidDrawLength(f64),
    DimensionsInvalidHandleLength(f64),
    DimensionsInvalidHandleSetback(f64),
    DimensionsInvalidHandleAngle(f64),

    MaterialInvalidDensity(f64),
    MaterialInvalidYoungsModulus(f64),
    MaterialInvalidShearModulus(f64),

    WidthControlPointsTooFew(usize),
    WidthControlPointsNotSorted(f64, f64),
    WidthControlPointsInvalidRange(f64, f64),
    WidthControlPointsInvalidValue(f64, f64),

    HeightControlPointsTooFew(usize),
    HeightControlPointsNotSorted(f64, f64),
    HeightControlPointsInvalidRange(f64, f64),
    HeightControlPointsInvalidBoundaryValue(f64, f64),
    HeightControlPointsInvalidInteriorValue(f64, f64),
    HeightControlPointsDiscontinuousBoundary(f64, f64),

    StringInvalidNumberOfStrands(usize),
    StringInvalidStrandDensity(f64),
    StringInvalidStrandStiffness(f64),

    MassesInvalidArrowMass(f64),
    MassesInvalidLimbTipMass(f64),
    MassesInvalidStringCenterMass(f64),
    MassesInvalidStringTipMass(f64),

    DampingInvalidLimbDampingRatio(f64),
    DampingInvalidStringDampingRatio(f64),

    CrossSectionInvalidLength(f64),
    CrossSectionNoLayers,
    CrossSectionNoMaterials,
    CrossSectionInvalidMaterialIndex(String, usize),
    CrossSectionInvalidAlignmentIndex(usize),
    CrossSectionZeroCombinedHeight(f64),

    ProfileNoSegments,
    LineSegmentInvalidLength(usize, f64),
    ArcSegmentInvalidLength(usize, f64),
    ArcSegmentInvalidRadius(usize, f64),
    SpiralSegmentInvalidLength(usize, f64),
    SpiralSegmentInvalidRadius1(usize, f64),
    SpiralSegmentInvalidRadius2(usize, f64),
    SplineSegmentTooFewPoints(usize, usize),
    SplineSegmentInvalidPoint(usize, [f64;2]),

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
            ModelError::InputLoadFileError(path, error)     => write!(f, "Failed to open file {path:?}: {error}")?,
            ModelError::InputSaveFileError(path, error)     => write!(f, "Failed to write file {path:?}: {error}")?,
            ModelError::InputSerializeJsonError(error)      => write!(f, "Failed to convert bow model to json: {error}")?,
            ModelError::InputDeserializeJsonError(error)    => write!(f, "Failed to parse bow model from json: {error}")?,
            ModelError::InputInterpretJsonError(error)      => write!(f, "Failed to parse bow model from json: {error}")?,
            ModelError::InputVersionNotFound                => write!(f, "Failed to load bow model, file version number was not found")?,
            ModelError::InputVersionInvalid(version)        => write!(f, "Failed to load bow model, {version} is not a valid file version number.")?,
            ModelError::InputVersionTooOld(version)         => write!(f, "Failed to load bow model, file version {version} is no longer supported.")?,
            ModelError::InputVersionTooNew(version)         => write!(f, "Failed to load bow model, file version {version} is too new to be recognized. Please consider updating to a recent version of VirtualBow.")?,
            ModelError::InputConversionError(a, b, message) => write!(f, "Failed to update bow model from file version {a} to {b}: {message}")?,

            ModelError::OutputLoadFileError(path, error)   => write!(f, "Failed to open file {path:?}: {error}")?,
            ModelError::OutputSaveFileError(path, error)   => write!(f, "Failed to write file {path:?}: {error}")?,
            ModelError::OutputEncodeMsgPackError(error)    => write!(f, "Failed to convert results to msgpack: {error}")?,
            ModelError::OutputDecodeMsgPackError(error)    => write!(f, "Failed to parse results from msgpack: {error}")?,
            ModelError::OutputInterpretMsgPackError(error) => write!(f, "Failed to parse results from msgpack: {error}")?,
            ModelError::OutputVersionNotFound              => write!(f, "Failed to load results, file version number was not found")?,
            ModelError::OutputVersionInvalid(version)      => write!(f, "Failed to load results, {version} is not a valid file version number.")?,
            ModelError::OutputVersionMismatch(version)     => write!(f, "Failed to load results, file version {version} is not supported by this version of VirtualBow. Please regenerate the results or open them with a matching version of VirtualBow.")?,

            ModelError::SettingsInvalidLimbElements(value)    => write!(f, "Settings: Number of limb elements must be at least 1 but actual number is {value}.")?,
            ModelError::SettingsInvalidLimbEvalPoints(value)  => write!(f, "Settings: Number of limb evaluation points must be at least 2 but actual number is {value}.")?,
            ModelError::SettingsInvalidLayerEvalPoints(value) => write!(f, "Settings: Number of layer evaluation points must be at least 2 but actual number is {value}.")?,
            ModelError::SettingsInvalidStringElements(value)  => write!(f, "Settings: Number of string elements must be at least 1 but actual number is {value}.")?,
            ModelError::SettingsInvalidDrawSteps(value)       => write!(f, "Settings: Number of draw steps must be at least 1 but actual number is {value}.")?,
            ModelError::SettingsInvalidArrowClampForce(value) => write!(f, "Settings: Arrow clamp force must be a non-negative number but actual value is {value}.")?,
            ModelError::SettingsInvalidTimeSpanFactor(value)  => write!(f, "Settings: Timespan factor must be a non-negative number but actual value is {value}.")?,
            ModelError::SettingsInvalidTimeStepFactor(value)  => write!(f, "Settings: Timestep factor must be a non-negative number but actual value is {value}.")?,
            ModelError::SettingsInvalidTimeOutFactor(value)   => write!(f, "Settings: Timeout factor must be a non-negative number but actual value is {value}.")?,
            ModelError::SettingsInvalidSamplingRate(value)    => write!(f, "Settings: Sampling rate must be a positive number but actual value is {value}.")?,

            ModelError::DimensionsInvalidBraceHeight(value)   => write!(f, "Dimensions: Brace height must be a finite number, actual value is {value}.")?,
            ModelError::DimensionsInvalidDrawLength(value)    => write!(f, "Dimensions: Draw length must be finite and larger than the brace height but actual value is {value}.")?,
            ModelError::DimensionsInvalidHandleLength(value)  => write!(f, "Dimensions: Handle length must be a non-negative number but actual value is {value}.")?,
            ModelError::DimensionsInvalidHandleSetback(value) => write!(f, "Dimensions: Handle setback must be a finite number but actual value is {value}.")?,
            ModelError::DimensionsInvalidHandleAngle(value)   => write!(f, "Dimensions: Handle angle must be a finite number but actual value is {value}.")?,

            ModelError::MaterialInvalidDensity(value)       => write!(f, "Material: Density must be a positive number but actual value is {value}.")?,
            ModelError::MaterialInvalidYoungsModulus(value) => write!(f, "Material: Young's modulus must be a positive number but actual value is {value}.")?,
            ModelError::MaterialInvalidShearModulus(value)  => write!(f, "Material: Shear modulus must be a positive number but actual value is {value}.")?,

            ModelError::WidthControlPointsTooFew(value)      => write!(f, "Width: At least 2 control points are required but actual number is {value}.")?,
            ModelError::WidthControlPointsNotSorted(a, b)    => write!(f, "Width: Control points must be sorted by length but found actual values {a}, {b}.")?,
            ModelError::WidthControlPointsInvalidRange(a, b) => write!(f, "Width: Control points must cover the range [0, 1] but actual range is [{a}, {b}].")?,
            ModelError::WidthControlPointsInvalidValue(a, b) => write!(f, "Width: Control points must be positive and finite but actual value found is [{a}, {b}].")?,

            ModelError::HeightControlPointsTooFew(value)               => write!(f, "Height: At least 2 control points are required but actual number is {value}.")?,
            ModelError::HeightControlPointsNotSorted(a, b)             => write!(f, "Height: Control points must be sorted by length but found actual values {a}, {b}.")?,
            ModelError::HeightControlPointsInvalidRange(a, b)          => write!(f, "Height: Control points must be within the range [0, 1] but actual range is [{a}, {b}].")?,
            ModelError::HeightControlPointsInvalidBoundaryValue(a, b)  => write!(f, "Height: Boundary control points must be non-negative and finite but actual value found is [{a}, {b}].")?,
            ModelError::HeightControlPointsInvalidInteriorValue(a, b)  => write!(f, "Height: Intermediate control points must be positive and finite but actual value found is [{a}, {b}].")?,
            ModelError::HeightControlPointsDiscontinuousBoundary(a, b) => write!(f, "Height: Boundary control point at relative length {a} must be zero for continuity but actual value is {b}.")?,

            ModelError::StringInvalidNumberOfStrands(value) => write!(f, "String: Number of strands must be at least 1 but actual number is {value}.")?,
            ModelError::StringInvalidStrandStiffness(value) => write!(f, "String: Strand density must be a positive number but actual value is {value}.")?,
            ModelError::StringInvalidStrandDensity(value)   => write!(f, "String: Strand stiffness must be a positive number but actual value is {value}.")?,

            ModelError::MassesInvalidArrowMass(value)        => write!(f, "Masses: Arrow mass must be a positive number but actual value is {value}.")?,
            ModelError::MassesInvalidLimbTipMass(value)      => write!(f, "Masses: Limb tip mass must be a non-negative number but actual value is {value}.")?,
            ModelError::MassesInvalidStringCenterMass(value) => write!(f, "Masses: String center mass must be a non-negative number but actual value is {value}.")?,
            ModelError::MassesInvalidStringTipMass(value)    => write!(f, "Masses: Strand tip mass must be a non-negative number but actual value is {value}.")?,

            ModelError::DampingInvalidLimbDampingRatio(value)   => write!(f, "Damping: Limb damping ratio must be a non-negative number but actual value is {value}.")?,
            ModelError::DampingInvalidStringDampingRatio(value) => write!(f, "Damping: String damping ratio must be a non-negative number but actual value is {value}.")?,

            ModelError::CrossSectionInvalidLength(value)               => write!(f, "Cross section: Length must be positive and finite nut actual value is {value}.")?,
            ModelError::CrossSectionNoLayers                           => write!(f, "Cross section: At least one layer is required.")?,
            ModelError::CrossSectionNoMaterials                        => write!(f, "Cross section: At least one material is required.")?,
            ModelError::CrossSectionInvalidMaterialIndex(layer, index) => write!(f, "Cross section: Cannot assign material {index} to layer \"{layer}\", no material with this index.")?,
            ModelError::CrossSectionInvalidAlignmentIndex(index)       => write!(f, "Cross section: Cannot align profile to layer {index}, no layer with this index.")?,
            ModelError::CrossSectionZeroCombinedHeight(length)         => write!(f, "Cross section: Combined height of all layers must be positive, but was found to be zero at relative length {length}")?,

            ModelError::ProfileNoSegments                         => write!(f, "Profile curve: At least one section is required")?,
            ModelError::LineSegmentInvalidLength(index, value)    => write!(f, "Profile curve: Line segment at index {index} has an invalid length, must be a positive number but actual value is {value}.")?,
            ModelError::ArcSegmentInvalidLength(index, value)     => write!(f, "Profile curve: Arc segment at index {index} has an invalid length, must be a positive number but actual value is {value}.")?,
            ModelError::ArcSegmentInvalidRadius(index, value)     => write!(f, "Profile curve: Arc segment at index {index} has an invalid radius, must be a finite number but actual value is {value}.")?,
            ModelError::SpiralSegmentInvalidLength(index, value)  => write!(f, "Profile curve: Spiral segment at index {index} has an invalid length, must be a positive number but actual value is {value}.")?,
            ModelError::SpiralSegmentInvalidRadius1(index, value) => write!(f, "Profile curve: Spiral segment at index {index} has an invalid start radius, must be a finite number but actual value is {value}.")?,
            ModelError::SpiralSegmentInvalidRadius2(index, value) => write!(f, "Profile curve: Spiral segment at index {index} has an invalid end radius, must be a finite number but actual value is {value}.")?,
            ModelError::SplineSegmentTooFewPoints(index, value)   => write!(f, "Profile curve: Spline segment at index {index} requires at least two control points but actual number is {value}.")?,
            ModelError::SplineSegmentInvalidPoint(index, point)   => write!(f, "Profile curve: Spline segment at index {index} requires finite control points but found actual value {point:?}.")?,

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