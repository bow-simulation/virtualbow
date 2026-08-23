# <img src="images/icons/model-string.png" style="height:24px; vertical-align:baseline"> String

The string is defined by the material properties of a single strand and the number of strands it consists of.
Its final length is calculated automatically based on the brace height specified in the [Draw](model-editor-draw.md) settings.
The following three properties are required:

**Strand density:** The linear density of a single strand (mass per unit length).
This determines the overall weight of the string, which is an important factor in a bow's efficiency.
Additional masses on the string — such as servings or nocking points — can be accounted for separately in the [Masses](model-editor-masses.md) settings.

**Strand stiffness:** The stiffness of a single strand (force per unit strain).
This value determines how much the string stretches elastically under load.

**Number of strands:** The total number of strands in the string.
The string is treated as an endless-loop string in which all strands run virtually parallel, allowing their properties to be added together.
Strings with extreme amounts of twist might break this assumption.

> [!NOTE]
> The linear density of a string material can be measured easily using an accurate scale (mass divided by length). The stiffness, however, is more difficult to obtain.
> Manufacturers generally do not publish this value.
> The table below lists the results of tensile tests for three common bowstring materials, done by the *German Institutes for Textile and Fiber Research* in July 2018.
>
> | Material        | Linear density [kg/m] | Breaking strength [N] | Elongation at break [%] | Stiffness [N/100%] |
> |:----------------|:---------------|:----------------------|:------------------------|:-------------------|
> | Dacron B50      | 370e-6         | 180                   | 8.5                     | 2118               |
> | Fastflight Plus | 176e-6         | 318                   | 2.9                     | 10966              |
> | BCY 452X        | 192e-6         | 309                   | 2.5                     | 12360              |
>
> **Table:** Material properties for common string materials. The stiffness values are a linear estimation from breaking strength and elongation.
