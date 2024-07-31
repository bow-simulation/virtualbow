# Materials

This is a list of materials that can later be assigned to the layers of the bow.

<figure>
  <img src="images/screenshots/editor/materials.png" style="width:200px">
  <figcaption><b>Figure:</b> Materials in the model tree</figcaption>
</figure>

If the _Materials_ category in the model tree is selected, the buttons (<img src="images/icons/list-add.svg" style="width:20; vertical-align:middle">, <img src="images/icons/list-remove.svg" style="width:20; vertical-align:middle">, <img src="images/icons/list-move-up.svg" style="width:20; vertical-align:middle">, <img src="images/icons/list-move-down.svg" style="width:20; vertical-align:middle">) can be used to add, remove and reorder materials.
Materials can be renamed by double-clicking and entering a new name.

<figure>
  <img src="images/screenshots/editor/material.png" style="width:200px">
  <figcaption><b>Figure:</b> Material properties</figcaption>
</figure>

For each material, the following properties are needed:

- **Color:** Color of the material (only used for display)

- **Rho:** Density (mass per unit volume)

- **E:** Elastic modulus (measure of stiffness)

For synthetic materials like e.g. fiber-reinforced composites you can often find the mechanical properties in a datasheet provided by the manufacturer.
Natural materials like wood are more difficult, because their properties can vary quite a bit.
Average numbers can be found at [http://www.wood-database.com](http://www.wood-database.com) and other websites, which should be a good starting point.
The alternative is to determine the material properties by experiment with a bending test.
For more information on that see [Appendix C](appendix-bending-test.md).