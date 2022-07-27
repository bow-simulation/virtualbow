# Model Files

> **Note:** The following specification is subject to change.
> Scripts that work directly with `.bow` files should therefore expect incompatibilities with new releases of VirtualBow.

| Field                                                               | Type       | Unit       | Description
|:--------------------------------------------------------------------|:-----------|:-----------|:-------------------------------------------|
| version                                                             | String     | --         | VirtualBow version number                  |
| comment                                                             | String     | --         | User comments                              |
| settings {                                                          |            |            |                                            |
| &nbsp;&nbsp;&nbsp;&nbsp; n_limb_elements                            | Integer    | --         | Number of limb elements                    |
| &nbsp;&nbsp;&nbsp;&nbsp; n_string_elements                          | Integer    | --         | Number of string elements                  |
| &nbsp;&nbsp;&nbsp;&nbsp; n_draw_steps                               | Integer    | --         | Number of draw steps                       |
| &nbsp;&nbsp;&nbsp;&nbsp; arrow_clamp_force                          | Double     | --         | Arrow clamp force                          |
| &nbsp;&nbsp;&nbsp;&nbsp; time_span_factor                           | Double     | --         | Time span factor                           |
| &nbsp;&nbsp;&nbsp;&nbsp; time_step_factor                           | Double     | --         | Time step factor                           |
| &nbsp;&nbsp;&nbsp;&nbsp; sampling_rate                              | Double     | --         | Sampling rate                              |
| }                                                                   |            |            |                                            |
| dimensions {                                                        |            |            |                                            |
| &nbsp;&nbsp;&nbsp;&nbsp; brace_height                               | Double     | m          | Brace height                               |
| &nbsp;&nbsp;&nbsp;&nbsp; draw_length                                | Double     | m          | Draw length                                |
| &nbsp;&nbsp;&nbsp;&nbsp; handle_length                              | Double     | m          | Handle length                              |
| &nbsp;&nbsp;&nbsp;&nbsp; handle_setback                             | Double     | m          | Handle setback                             |
| &nbsp;&nbsp;&nbsp;&nbsp; handle_angle                               | Double     | m          | Handle angle                               |
| }                                                                   |            |            |                                            |
| profile                                                             | Double[][] | m, 1/m     | Table with arc length and curvature        |
| width                                                               | Double[][] | -- , m     | Table with position and width              |
| layers [                                                            |            |            |                                            |
| &nbsp;&nbsp;&nbsp;&nbsp; {                                          |            |            |                                            |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; name              | String     |            | Layer name                                 |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; height            | Double[][] | -- , m     | Table with position and height             |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; rho               | Double     | kg/m³      | Density of the material                    |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; E                 | Double     | Pa         | Elastic modulus of the material            |
| &nbsp;&nbsp;&nbsp;&nbsp; }                                          |            |            |                                            |
| &nbsp;&nbsp;&nbsp;&nbsp; {                                          |            |            |                                            |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; ...               |            |            |                                            |
| &nbsp;&nbsp;&nbsp;&nbsp; }                                          |            |            |                                            |
| ]                                                                   |            |            |                                            |
| string {                                                            |            |            |                                            |
| &nbsp;&nbsp;&nbsp;&nbsp; strand_stiffness                           | Double     | N          | Stiffness of the string material           |
| &nbsp;&nbsp;&nbsp;&nbsp; strand_density                             | Double     | kg/m       | Density of the string material             |
| &nbsp;&nbsp;&nbsp;&nbsp; n_strands                                  | Integer    | --         | Number of strands                          |
| }                                                                   |            |            |                                            |
| masses {                                                            |            |            |                                            |
| &nbsp;&nbsp;&nbsp;&nbsp; arrow                                      | Double     | kg         | Mass of the arrow                          |
| &nbsp;&nbsp;&nbsp;&nbsp; string_center                              | Double     | kg         | Additional mass at string center           |
| &nbsp;&nbsp;&nbsp;&nbsp; string_tip                                 | Double     | kg         | Additional mass at string tips             |
| &nbsp;&nbsp;&nbsp;&nbsp; limb_tip                                   | Double     | kg         | Additional mass at limb tips               |
| }                                                                   |            |            |                                            |
| damping {                                                           |            |            |                                            |
| &nbsp;&nbsp;&nbsp;&nbsp; damping_ratio_limbs                        | Double     | --         | Damping ratio of the limbs                 |
| &nbsp;&nbsp;&nbsp;&nbsp; damping_ratio_string                       | Double     | --         | Damping ratio of the string                |
| }                                                                   |            |            |                                            |
