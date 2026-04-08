# <img src="images/icons/model-settings.png" style="height:24; vertical-align:center"> Settings

The Settings allow you to fine-tune how the simulation is carried out.
In most cases, the default values provide a good balance and will work just fine, so if you are reading this manual for the first time, you may choose to skip this section and revisit it later if needed.

Because the default settings are chosen as a reliable general choice, they prioritize accuracy and stability over raw simulation speed.
For certain tasks, however - such as running a large number of scripted simulations - it may be worthwile to adjust the settings for better performance.
Conversely, some bow designs may occasionally fail with default settings, in which case alternative settings can help the simulation complete successfully.

The available options are divided into general settings that affect both simulation modes, as well as mode‑specific settings for the static and dynamic simulations.

## General

**Limb elements:** Number of finite elements used to approximate the limb geometry.
Increasing this value improves accuracy at the cost of longer computation times.

**Limb eval points:** Number of sampling points along the limb where stresses, strains and other quantities are evaluated.
Higher values produce result plots with a finer resolution.
The added computational cost of increasing this value is small, but beyond a certain point the increased resolution offers no practical advantage anymore.

## Statics

**Min. draw resolution:** Defines the minimum number of steps the static simulation takes as it progresses from brace height to full draw.
A higher value increases the resolution of the static results.
You can usually reduce this value to speed up the simulation, especially if you are primarily interested in the dynamic results.

**Max. draw resolution:** Defines the maximum number of steps the static simulation takes as it progresses from brace height to full draw.
This setting is currently unused, but future versions of VirtualBow might use it for adaptive step size control.

**Iteration tolerance:** Stopping tolerance for the static equilibrium iterations. This value is a tradeoff between accuracy and simulation time.
A tolerance that is too large may produce inaccurate results, while one that is too small wastes computation on unnecessary precision.
Both an overly low and an overly high tolerance can eventually lead to simulation failure.

## Dynamics

**Arrow clamp force:** The force the arrow must overcome to separate from the string.
This value is set fairly low by default.
Increasing it can help resolve issues that may occur when simulating very light arrows.

**String compression factor:** Determines the compressive stiffness of the string as a fraction of its tensile stiffness.
An idealized string carries load only in tension, not in compression.
For numerical reasons, however, the compressive stiffness cannot be set to zero, which is why this small positive factor is required.

**Timespan factor:** This factor controls the duration of the simulated time interval.
A value of 100% corresponds to the moment when the arrow passes brace height.
The default value is higher to capture events that may occur after the arrow has left the bow, such as peak forces and stresses on the limbs and string.

**Timeout factor:** Factor controlling the timeout of the dynamic simulation.
The simulation is aborted if arrow separation has not occurred by the time the simulated duration exceeds this factor multiplied by a reference time determined for the bow.

**Min. timestep:** Lower bound for the time step of the dynamic simulation. The solver adjusts its step size automatically, but this setting ensures that the steps stay large enough to prevent the simulation from slowing to a crawl.

**Max. timestep:** Upper bound for the time step of the dynamic simulation. The solver adjusts its step size automatically, but this setting ensures that the steps stay small enough to maintain sufficient detail in the results.

**Steps per period:** Number of steps the dynamic solver tries to take per current characteristic time period of the system.
This serves as the target the solver uses when adjusting its time step.
Increasing it improves accuracy and stability, at the cost of additional computation time.

**Iteration tolerance:** Stopping tolerance for the dynamic equilibrium iterations. This value is a tradeoff between accuracy and simulation time.
A tolerance that is too large may produce inaccurate results, while one that is too small wastes computation on unnecessary precision.
Both an overly low and an overly high tolerance can eventually lead to simulation failure.
