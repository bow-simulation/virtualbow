# Settings

The settings can be used for tweaking the simulation.
Usually the default values should be fine though, so if you're reading this manual for the first time you might want to skip this section.

<figure>
  <img src="images/screenshots/editor/settings.png" style="width:200px">
  <figcaption><b>Figure:</b> Settings</figcaption>
</figure>

Since the default settings are meant to be a good general choice, they favor accuracy and reliability over simulation speed.
So for specific use cases it could make sense to find more efficient settings.
Think about running a large number of scripted simulations, for example.
On the other hand, even the default settings might sometimes fail with certain bow designs such that different settings have to be used.

The settings are divided into general settings that affect both simulation modes as well as specific settings for the static and dynamic simulations.

**General**

- **Limb elements:** Number of finite elements that are used to approximate the limb. More elements increase the accuracy but also the computing time.

- **String elements:** Number of finite elements that are used to approximate the string. This number can usually be reduced if the bow has no recurve. In the case of a static analysis with no recurve it can even be set to one without losing any accuracy.

**Statics**

- **Draw steps:** Number of steps that are performed by the static simulation from brace height to full draw. This determines the resolution of the static results. You can usually decrease this value to speed up the simulation, especially if you're only interested in the dynamic results.

**Dynamics**

- **Arrow clamp force:** The force that the arrow has to overcome in order to separate from the string.
This value is chosen fairly small by default and can improve the simulation results for very light arrows.

- **Time span factor:** This factor controls the time period that is being simulated. A value of 1 corresponds to the time at which the arrow passes the brace height. The default value is larger than that in order to capture some of the things that tend to happen after the arrow left the bow (e.g. the maximum forces/stresses on limb and string).

- **Time step factor:** When simulating the dynamics of the bow, the program will repeatedly use the current state of the bow at time stem:[t] to calculate the next state at time stem:[t + \Delta t] where stem:[\Delta t] is some small timestep. We want this timestep to be as large as possible to keep the required number of steps low. But it still has to be small enough to get an accurate and stable solution. The program will try to estimate this optimal timestep, but to be on the safe side the estimation is multiplied with a factor between 0 and 1 that you can choose here.

- **Sampling rate:** The sampling rate limits the time resolution of the dynamic results. This is done because the dynamic simulation usually produces much finer grained data than is actually useful. Not including all of that in the final output reduces the size of the result files and the simulation time.