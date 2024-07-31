# String

The string is defined by its material properties and the number of strands being used.
Its length is determined automatically from the brace height that was set under _Dimensions_.

<figure>
  <img src="images/screenshots/editor/string.png" style="width:200px">
  <figcaption><b>Figure:</b> String properties</figcaption>
</figure>

The following three properties are required:

- **Strand density:** Linear density of a single strand (mass per unit length)

- **Strand stiffness:** Stiffness of a single strand (force per unit strain)

- **Number of strands:** Total number of strands in the string

> **Note:** Mass and stiffness of the string can be important for dynamic analysis.
The effect on the static results however is small, provided that the stiffness is high enough to prevent significant elongation.

The linear density of a string material can be easily determined with an accurate scale (weight divided by length), the stiffness however is more difficult to obtain.
Manufacturers usually don't publish this number.
The table below shows the results of tensile tests for three common bow string materials.
They were done by the German Institutes for Textile and Fiber Research [^1] in July 2018.

| Material        | Density [kg/m] | Breaking strength [N] | Elongation at break [%] | Stiffness [N/100%] |
|:----------------|:---------------|:----------------------|:------------------------|:-------------------|
| Dacron B50      | 370e-6         | 180                   | 8.5                     | 2118               |
| Fastflight Plus | 176e-6         | 318                   | 2.9                     | 10966              |
| BCY 452X        | 192e-6         | 309                   | 2.5                     | 12360              |

**Table:** Material properties for common string materials. The stiffness values are a linear estimation from breaking strength and elongation.

[^1]: [https://www.ditf.de/en/](https://www.ditf.de/en/)