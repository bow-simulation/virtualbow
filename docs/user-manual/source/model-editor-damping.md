# Damping

Damping allows to account for energy dissipation, for example due to internal friction/hysteresis of the materials.
It only has an effect in dynamic analysis.

<figure>
  <img src="images/screenshots/editor/damping.png" style="width:200px">
  <figcaption><b>Figure:</b> Damping properties</figcaption>
</figure>

Since modelling all the different forms of energy dissipation in a bow in an exact way would be too complex, the damping is reduced to two empirical values:
The damping ratio of the **Limbs** and the damping ratio of the **String**.

The damping ratio characterizes how quickly oscillations decay over time:

- An oscillation with a damping ratio of 0% is undamped, it doesn't dissipate any energy and just keeps going with a constant amplitude.

- The higher the damping ratio the faster the amplitudes decay over time, losing energy with each oscillation.

- Once the damping ratio reaches 100% there is no longer any oscillation (no overshoot), this is called critical damping.

See also the table below for a visualization of those three cases.

| Damping | Amplitude                                                   |
|---------|-------------------------------------------------------------|
| 0%      | <img src="images/damping-ratio-00.svg" style="width:200px"> |
| 10%     | <img src="images/damping-ratio-01.svg" style="width:200px"> |
| 100%    | <img src="images/damping-ratio-10.svg" style="width:200px"> |

> **Note:** The damping ratios of a bow's limbs and string are mostly empirical and there isn't yet much practical experience.
> Realistic values are probably in the range of 1 - 10% though.

> **Note:** Due to numerical issues, high damping ratios (> 10%) might not work very well right now.

> **Note:** In more technical terms, the limb's damping ratio applies to its first eigenmode in unbraced state and the string's damping ratio applies to its first eigenmode in longitudinal direction.