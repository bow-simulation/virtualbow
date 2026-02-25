#pragma once
#include <QString>

// Constants for all texts shown throughout the application
// TODO: Actually pull all texts into here, not only tooltips

namespace Tooltips {
    inline const QString EnergiesStacked = QStringLiteral("If selected, the energies are plotted as areas stacked on top of each other.\nOtherwise each energy is drawn as an individual line.");
    inline const QString EnergiesByPart = QStringLiteral("Group energies by the component they belong to (limbs, string, arrow)");
    inline const QString EnergiesByType = QStringLiteral("Group enegies by the type they belong to (potential, kinetic, damping)");

    inline const QString DrawForceShowLine = QStringLiteral("Show a hypothetical line connecting the start- and endpoint of the draw force curve");
    inline const QString DrawForceShowStiffness = QStringLiteral("Show the stiffness of the draw force curve, i.e. force increase per length increase");

    inline const QString ArrowMassDefinition = QStringLiteral("Select how the arrow mass is to be defined");
    inline const QString ArrowMassAbsolute = QStringLiteral("Arrow mass (absolute value)");
    inline const QString ArrowMassPerForce = QStringLiteral("Arrow mass relative to the final draw force of the bow");
    inline const QString ArrowMassPerEnergy = QStringLiteral("Arrow mass relative to the input energy of the bow");

    inline const QString MassStringCenter = QStringLiteral("Additional mass(es) at the string center (e.g. serving, nocking point)");
    inline const QString MassStringTip = QStringLiteral("Additional mass(es) at the string tip (e.g. serving)");
    inline const QString MassLimbTip = QStringLiteral("Additional mass(es) at the limb tip (e.g. tip overlay)");

    inline const QString DrawLengthDefinition = QStringLiteral("Select how the draw length is to be defined");
    inline const QString DrawLengthStandard = QStringLiteral("Draw length as measured from the pivot point of the handle");
    inline const QString DrawLengthAMO = QStringLiteral("Draw length as measured from the pivot point of the handle + 1.75\" according to the AMO definition");

    inline const QString LayerMaterialSelection = QStringLiteral("Material assigned to this layer");
    inline const QString LayerMaterialItem = QStringLiteral("Assign \"%1\" to this layer");
    inline const QString LayerHeightInput = QStringLiteral("Layer height over relative position along the limb");

    inline const QString ProfileSplineInput = QStringLiteral("Control points of the spline curve");

    inline const QString WidthInput = QStringLiteral("Limb width over relative position along the limb");

    inline const QString ViewDefault = QStringLiteral("Default view");
    inline const QString ViewProfile = QStringLiteral("Profile view");
    inline const QString ViewBack = QStringLiteral("Back view");
    inline const QString ViewReset = QStringLiteral("Reset view");
    inline const QString ViewSymmetric = QStringLiteral("Show symmetry");

    inline const QString SettingsNumLimbElements = QStringLiteral("Number of finite elements used to approximate the limb");
    inline const QString SettingsNumEvalPoints = QStringLiteral("Number of points along the limb where results are evaluated");
    inline const QString SettingsMinDrawResolution = QStringLiteral("Lower bound for the number of equilibrium points from brace height to full draw");
    inline const QString SettingsMaxDrawResolution = QStringLiteral("Upper bound for the number of equilibrium points from brace height to full draw");
    inline const QString SettingsStaticIterationTolerance = QStringLiteral("Stopping tolerance for the static equilibrium iterations");
    inline const QString SettingsArrowClampForce = QStringLiteral("Force that the arrow has to overcome before separating from the string");
    inline const QString SettingsStringCompressionFactor = QStringLiteral("Factor for the compressive stiffness of the string relative to its tensile stiffness.\nShould be very low but can't be zero for numerical reasons.");
    inline const QString SettingsTimespanFactor = QStringLiteral("Factor for controlling the end time of the dynamic simulation.\nThe value 1.0 corresponds to the time until the arrow reaches brace height.\nValues larger than 1.0 extend the simulated time beyond that.");
    inline const QString SettingsTimeoutFactor = QStringLiteral("Factor for controlling the timeout of the dynamic simulation.\nThe simulation is aborted when arrow separation didn't happen until the simulation time exceeds the timeout factor multiplied by a characteristic time of the bow.");
    inline const QString SettingsMinTimestep = QStringLiteral("Lower bound for the time step of the dynamic simulation");
    inline const QString SettingsMaxTimestep = QStringLiteral("Upper bound for the time step of the dynamic simulation");
    inline const QString SettingsStepsPerPeriod = QStringLiteral("Number of steps the dynamic solver tries to take per current characteristic time period of the system");
    inline const QString SettingsDynamicIterationTolerance = QStringLiteral("Stopping tolerance for the dynamic equilibrium iterations");

    inline const QString MaterialColor = QStringLiteral("Color of the material, only used for visualization");
    inline const QString MaterialDensity = QStringLiteral("Density of the material (mass per unit volume)");
    inline const QString MaterialYoungsModulus = QStringLiteral("Young's modulus of the material");
    inline const QString MaterialShearModulus = QStringLiteral("Shear modulus of the material");
    inline const QString MaterialTensileStrength = QStringLiteral("Tensile strength of the material");
    inline const QString MaterialCompressiveStrength = QStringLiteral("Compressive strength of the material");
    inline const QString MaterialSafetyMargin = QStringLiteral("Safety margin for tensile and compressive strength");

    inline const QString StringLinearStiffness = QStringLiteral("Linear stiffness of a single strand of the string material");
    inline const QString StringLinearDensity = QStringLiteral("Linear density of a single strand of the string material");
    inline const QString StringNumStrands = QStringLiteral("Total number of strands in the string");

    inline const QString ArcSegmentLength = QStringLiteral("Length of the arc segment");
    inline const QString ArcSegmentRadius = QStringLiteral("Radius of the arc segment");

    inline const QString SpiralSegmentLength = QStringLiteral("Length of the spiral segment");
    inline const QString SpiralSegmentRadiusStart = QStringLiteral("Start radius of the spiral segment (or zero for no radius)");
    inline const QString SpiralSegmentRadiusEnd = QStringLiteral("End radius of the spiral segment (or zero for no radius)");

    inline const QString LineSegmentLength = QStringLiteral("Length of the line segment");

    inline const QString DampingRatioLimbs = QStringLiteral("Damping ratio of the limbs");
    inline const QString DampingRatioString = QStringLiteral("Damping ratio of the string");

    inline const QString HandleReference = QStringLiteral("Reference point at the limb base from which the handle dimensions as well as the brace height and draw length are measured");
    inline const QString HandleReferenceBack = QStringLiteral("Measure from the back of the bow");
    inline const QString HandleReferenceBelly = QStringLiteral("Measure from the belly of the bow");
    inline const QString HandleReferenceProfile = QStringLiteral("Measure from the profile curve");

    inline const QString HandleLength = QStringLiteral("Length of the handle, i.e. distance between the upper and lower limb as measured by the reference points");
    inline const QString HandleOffset = QStringLiteral("Offset of the handle's pivot point in the direction of draw");
    inline const QString HandleAngle = QStringLiteral("Angle at which the limbs are attached to the handle");
    inline const QString BraceHeight = QStringLiteral("Brace height of the bow, i.e. distance between string and the handle's pivot point in the braced state");
    inline const QString DrawLength = QStringLiteral("Draw length of the bow, i.e. distance between string and the handle's pivot point in the fully drawn state");


}
