#pragma once
#include <QString>

// Constants for all texts shown throughout the application
// TODO: Actually pull all texts into here, not only tooltips
// TODO: Put actual strings into a *.cpp file to reduce compile times

namespace Tooltips {
    inline const QString EnergiesStacked = QStringLiteral("If selected, the energies are plotted as areas stacked on top of each other.\nOtherwise each energy is drawn as an individual line.");
    inline const QString EnergiesByPart = QStringLiteral("Group energies by the component they belong to (limbs, string, arrow)");
    inline const QString EnergiesByType = QStringLiteral("Group enegies by the type they belong to (potential, kinetic, damping)");

    inline const QString HandleTypeDefinition = QStringLiteral("Selects the type of handle for this bow");
    inline const QString HandleTypeFlexible = QStringLiteral("The handle is flexible and modelled as part of the bow limbs");
    inline const QString HandleTypeRigid = QStringLiteral("The handle is modelled as a separate rigid section between the bow limbs");
    inline const QString HandleLength = QStringLiteral("Length of the handle section");
    inline const QString HandleAngle = QStringLiteral("Attachment angle of the limbs (positive = reflex, negative = deflex)");
    inline const QString HandlePivot = QStringLiteral("Position of the pivot point (positive = reflex, negative = deflex)");

    inline const QString BraceHeight = QStringLiteral("Brace height of the bow, i.e. distance between string and the handle's pivot point in the braced state");
    inline const QString DrawLength = QStringLiteral("Draw length of the bow, i.e. distance between string and the handle's pivot point in the fully drawn state");

    inline const QString DrawForceShowLine = QStringLiteral("Show a hypothetical line connecting the start- and endpoint of the force-draw curve");
    inline const QString DrawForceShowStiffness = QStringLiteral("Show the stiffness of the force-draw curve, i.e. force increase per length increase");

    inline const QString ArrowMassDefinition = QStringLiteral("Selects how the arrow mass is specified");
    inline const QString ArrowMassAbsolute = QStringLiteral("Arrow mass as an absolute value");
    inline const QString ArrowMassPerForce = QStringLiteral("Arrow mass relative to the final draw force of the bow");
    inline const QString ArrowMassPerEnergy = QStringLiteral("Arrow mass relative to the stored energy of the bow");

    inline const QString MassStringCenter = QStringLiteral("Additional mass(es) at the string center (e.g. serving, nocking point)");
    inline const QString MassStringTip = QStringLiteral("Additional mass(es) at the string tip (e.g. serving)");
    inline const QString MassLimbTip = QStringLiteral("Additional mass(es) at the limb tip (e.g. tip overlay)");

    inline const QString DrawLengthDefinition = QStringLiteral("Selects the draw length definition to be used");
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
    inline const QString ViewReset = QStringLiteral("Reset zoom");
    inline const QString ViewSymmetric = QStringLiteral("Show both limbs");

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

    inline const QString ResetSIDefaults = QStringLiteral("Reset selection to SI default units");
    inline const QString ResetUSDefaults = QStringLiteral("Reset selection to US default units");

    inline const QString OutputFinalDrawForce = QStringLiteral("Draw force of the bow at full draw");
    inline const QString OutputDrawingWork = QStringLiteral("Total work done by drawing the bow from brace height to full draw");
    inline const QString OutputEnergyStorageFactor = QStringLiteral("Quality indicator for the shape of the force-draw curve in terms of energy storage");
    inline const QString OutputLimbMass = QStringLiteral("Computed mass of a single bow limb, including the additional tip mass");
    inline const QString OutputStringMass = QStringLiteral("Computed mass of the bow string, including additional masses");
    inline const QString OutputStringLength = QStringLiteral("Length of the string as determined by the configured brace height");
    inline const QString OutputPowerStroke = QStringLiteral("Length the string travels from brace height to full draw");
    inline const QString OutputMaxDrawForce = QStringLiteral("Maximum draw force (not necessarily equal to the final draw force)");
    inline const QString OutputMaxStringForce = QStringLiteral("Maximum tensile force in the string");
    inline const QString OutputMaxStrandForce = QStringLiteral("Maximum tensile force in the string per strand");
    inline const QString OutputArrowMass = QStringLiteral("Arrow mass (either from direct input or computed from mass per force/energy)");
    inline const QString OutputFinalArrowVelocity = QStringLiteral("Final velocity of the arrow when leaving the bow");
    inline const QString OutputFinalArrowEnergy = QStringLiteral("Final kinetic energy of the arrow when leaving the bow");
    inline const QString OutputEnergyEfficiency = QStringLiteral("Degree of efficiency of the bow\nRatio of the drawing work that is being converted into kinetic energy of the arrow");
    inline const QString OutputMaxGripPushForce = QStringLiteral("Maximum push force that the grip exerts onto the bow hand");
    inline const QString OutputMaxGripPullForce = QStringLiteral("Maximum pull force that the grip exerts onto the bow hand");
    inline const QString OutputMaxTensileStress = QStringLiteral("Maximum tensile stress for layer \"%1\"");
    inline const QString OutputMaxCompressiveStress = QStringLiteral("Maximum compressive stress for layer \"%1\"");
    inline const QString OutputMaxTensileStrain = QStringLiteral("Maximum tensile strain for layer \"%1\"");
    inline const QString OutputMaxCompressiveStrain = QStringLiteral("Maximum compressive strain for layer \"%1\"");

    inline const QString OutputEnergyLossLimbsKineticAbsolute = QStringLiteral("Energy lost due to kinetic energy remaining in the limbs after arrow departure");
    inline const QString OutputEnergyLossLimbsKineticRelative = QStringLiteral("Efficiency lost due to kinetic energy remaining in the limbs after arrow departure");
    inline const QString OutputEnergyLossLimbsElasticAbsolute = QStringLiteral("Energy lost due to elastic energy remaining in the limbs after arrow departure");
    inline const QString OutputEnergyLossLimbsElasticRelative = QStringLiteral("Efficiency lost due to elastic energy remaining in the limbs after arrow departure");
    inline const QString OutputEnergyLossLimbsDampingAbsolute = QStringLiteral("Energy lost due to damping in the limbs up until arrow departure");
    inline const QString OutputEnergyLossLimbsDampingRelative = QStringLiteral("Efficiency lost due to damping in the limbs up until arrow departure");
    inline const QString OutputEnergyLossStringKineticAbsolute = QStringLiteral("Energy lost due to kinetic energy remaining in the string after arrow departure");
    inline const QString OutputEnergyLossStringKineticRelative = QStringLiteral("Efficiency lost due to kinetic energy remaining in the string after arrow departure");
    inline const QString OutputEnergyLossStringElasticAbsolute = QStringLiteral("Energy lost due to elastic energy remaining in the string after arrow departure");
    inline const QString OutputEnergyLossStringElasticRelative = QStringLiteral("Efficiency lost due to elastic energy remaining in the string after arrow departure");
    inline const QString OutputEnergyLossStringDampingAbsolute = QStringLiteral("Energy lost due to damping in the string up until arrow departure");
    inline const QString OutputEnergyLossStringDampingRelative = QStringLiteral("Efficiency lost due to damping in the string up until arrow departure");

    inline const QString SliderJumpTo = QStringLiteral("Jump to a specific point in the simulation results");
    inline const QString SliderSkipToStart = QStringLiteral("Jump to start");
    inline const QString SliderSkipToEnd = QStringLiteral("Jump to end");
    inline const QString SliderPlayPause = QStringLiteral("Play/Pause");

    inline const QString SelectPlotChannelX = QStringLiteral("Select simulation result to be plotted on the x axis");
    inline const QString SelectPlotChannelY = QStringLiteral("Select simulation result to be plotted on the y axis");

    inline const QString ButtonShowStatics = QStringLiteral("Show the static simulation results");
    inline const QString ButtonShowDynamics = QStringLiteral("Show the dynamic simulation results");
}
