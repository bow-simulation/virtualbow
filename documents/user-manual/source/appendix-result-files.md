# Result Files

> **Note:** The following specification is subject to change.
> Scripts that work directly with `.res` files should therefore expect incompatibilities with new releases of VirtualBow.

> **Note:** For space efficiency reasons, the stress values for each layer aren't stored directly in the result files.
> Instead, each layer has two constant matrices \\(H_{e\mathrm{\ (back/belly)}}\\) that relate the longitudinal strains \\(\varepsilon\\) to the resulting stresses at the back or belly side of the layer as well as two matrices \\(H_{k\mathrm{\ (back/belly)}}\\) that do the same for the bending curvature \\(\kappa\\).
> The total stress is the sum of the stresses due to longitudinal strain and bending curvature and can be calculated as
> \\[\sigma = H_e \cdot \varepsilon + H_k \cdot \kappa\\]
> where the dot (\\(\cdot\\)) represents a matrix-vector multiplication.
> The result is a vector of stresses corresponding to the nodes of the layer.

**P:** Number of limb nodes

**Q:** Number of string nodes

**R:** Number of layer nodes

| Field                                                                                                        | Type         | Unit    | Description                                                   |
|:-------------------------------------------------------------------------------------------------------------|:-------------|:--------|:--------------------------------------------------------------|
| setup {                                                                                                      |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; string_length                                                                       | Double       | m       | Length of the string                                          |
| &nbsp;&nbsp;&nbsp;&nbsp; string_mass                                                                         | Double       | kg      | Mass of the string, including additional masses               |
| &nbsp;&nbsp;&nbsp;&nbsp; limb_mass                                                                           | Double       | kg      | Mass of the limb, including additional masses                 |
| &nbsp;&nbsp;&nbsp;&nbsp; limb_properties {                                                                   |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; length                                                     | Double[P]    | m       | Arc lengths of the limb nodes (unbraced)                      |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; angle                                                      | Double[P]    | rad     | Rotation angles of the limb nodes (unbraced)                  |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; x_pos                                                      | Double[P]    | m       | X coordinates of the limb nodes (unbraced)                    |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; y_pos                                                      | Double[P]    | m       | Y coordinates of the limb nodes (unbraced)                    |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; width                                                      | Double[P]    | m       | Cross section width                                           |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; height                                                     | Double[P]    | m       | Cross section height (all layers)                             |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; rhoA                                                       | Double[P]    | kg/m    | Linear density of the sections                                |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; Cee                                                        | Double[P]    | N       | Longitudinal stiffness of the sections                        |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; Ckk                                                        | Double[P]    | Nm²     | Bending stiffness of the sections                             |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; Cek                                                        | Double[P]    | Nm      | Coupling between bending and elongation                       |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; layers [                                                   |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; {                                 |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; length   | Double[R]    | m       | Arc lengths of the layer nodes                                |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; He_back  | Double[R][P] | N/m²    | Stress evaluation matrix (back)                               |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; Hk_back  | Double[R][P] | N/m     | Stress evaluation matrix (back)                               |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; He_belly | Double[R][P] | N/m²    | Stress evaluation matrix (belly)                              |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; Hk_belly | Double[R][P] | N/m     | Stress evaluation matrix (belly)                              |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; }                                 |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; {                                 |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; ...      |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; }                                 |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; ]                                                          |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; }                                                                                   |              |         |                                                               |
| }                                                                                                            |              |         |                                                               |
| statics {                                                                                                    |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; final_draw_force                                                                    | Double       | N       | Final draw force                                              |
| &nbsp;&nbsp;&nbsp;&nbsp; drawing_work                                                                        | Double       | J       | Drawing work                                                  |
| &nbsp;&nbsp;&nbsp;&nbsp; energy storage factor                                                               | Double       | --      | Energy storage factor                                         |
| &nbsp;&nbsp;&nbsp;&nbsp; max_string_force_index                                                              | Integer      | --      | Simulation state with maximum absolute string force           |
| &nbsp;&nbsp;&nbsp;&nbsp; max_grip_force_index                                                                | Integer      | --      | Simulation state with maximum absolute grip force             |
| &nbsp;&nbsp;&nbsp;&nbsp; max_draw_force_index                                                                | Integer      | --      | Simulation state with maximum absolute draw force             |
| &nbsp;&nbsp;&nbsp;&nbsp; min_stress_value                                                                    | Double[]     | Pa      | Minimum stress for each layer                                 |
| &nbsp;&nbsp;&nbsp;&nbsp; min_stress_index                                                                    | Integer[][]  | --      | Simulation state and layer node for each minimum stress value |
| &nbsp;&nbsp;&nbsp;&nbsp; max_stress_value                                                                    | Double[]     | Pa      | Maximum stress for each layer                                 |
| &nbsp;&nbsp;&nbsp;&nbsp; max_stress_index                                                                    | Integer[][]  | --      | Simulation state and layer node for each maximum stress value |
| &nbsp;&nbsp;&nbsp;&nbsp; states {                                                                            |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; ...                                                        |              |         | Sequence of bow states (see table below)                      |
| &nbsp;&nbsp;&nbsp;&nbsp; }                                                                                   |              |         |                                                               |
| }                                                                                                            |              |         |                                                               |
| dynamics {                                                                                                   |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; final_pos_arrow                                                                     | Double       | m       | Position of the arrow at departure                            |
| &nbsp;&nbsp;&nbsp;&nbsp; final_vel_arrow                                                                     | Double       | m/s     | Velocity of the arrow at departure                            |
| &nbsp;&nbsp;&nbsp;&nbsp; final_e_kin_arrow                                                                   | Double       | J       | Kinetic energy of the arrow at departure                      |
| &nbsp;&nbsp;&nbsp;&nbsp; final_e_pot_limbs                                                                   | Double       | J       | Potential energy of the limbs at arrow departure              |
| &nbsp;&nbsp;&nbsp;&nbsp; final_e_kin_limbs                                                                   | Double       | J       | Kinetic energy of the limbs at arrow departure                |
| &nbsp;&nbsp;&nbsp;&nbsp; final_e_pot_string                                                                  | Double       | J       | Potential energy of the string at arrow departure             |
| &nbsp;&nbsp;&nbsp;&nbsp; final_e_kin_string                                                                  | Double       | J       | Kinetic energy of the string at arrow departure               |
| &nbsp;&nbsp;&nbsp;&nbsp; efficiency                                                                          | Double       | --      | Degree of efficiency                                          |
| &nbsp;&nbsp;&nbsp;&nbsp; max_string_force_index                                                              | Integer      | --      | Simulation state with maximum absolute string force           |
| &nbsp;&nbsp;&nbsp;&nbsp; max_grip_force_index                                                                | Integer      | --      | Simulation state with maximum absolute grip force             |
| &nbsp;&nbsp;&nbsp;&nbsp; arrow_departure_index                                                               | Integer      | --      | Simulation state at which the arrow departs with the string   |
| &nbsp;&nbsp;&nbsp;&nbsp; min_stress_value                                                                    | Double[]     | Pa      | Minimum stress for each layer                                 |
| &nbsp;&nbsp;&nbsp;&nbsp; min_stress_index                                                                    | Integer[][]  | --      | Simulation state and layer node for each stress value         |
| &nbsp;&nbsp;&nbsp;&nbsp; max_stress_value                                                                    | Double[]     | Pa      | Maximum stress for each layer                                 |
| &nbsp;&nbsp;&nbsp;&nbsp; max_stress_index                                                                    | Integer[][]  | --      | Simulation state and layer node for each stress value         |
| &nbsp;&nbsp;&nbsp;&nbsp; states {                                                                            |              |         |                                                               |
| &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; ...                                                        |              |         | Sequence of bow states (see table below)                      |
| &nbsp;&nbsp;&nbsp;&nbsp; }                                                                                   |              |         |                                                               |
| }                                                                                                            |              |         |                                                               |

**N:** Number of simulation steps

**P:** Number of limb nodes

**Q:** Number of string nodes

| Field                                  | Type         | Unit    | Description                           |
|:---------------------------------------|:-------------|:--------|:--------------------------------------|
| states {                               |              |         |                                       |
| &nbsp;&nbsp;&nbsp;&nbsp; time          | Double[N]    | s       | Time                                  |
| &nbsp;&nbsp;&nbsp;&nbsp; draw_length   | Double[N]    | m       | Draw length                           |
| &nbsp;&nbsp;&nbsp;&nbsp; draw_force    | Double[N]    | N       | Draw force                            |
| &nbsp;&nbsp;&nbsp;&nbsp; string_force  | Double[N]    | N       | String force (total)                  |
| &nbsp;&nbsp;&nbsp;&nbsp; strand_force  | Double[N]    | N       | String force (strand)                 |
| &nbsp;&nbsp;&nbsp;&nbsp; grip_force    | Double[N]    | N       | Grip force                            |
| &nbsp;&nbsp;&nbsp;&nbsp; pos_arrow     | Double[N]    | m       | Arrow position                        |
| &nbsp;&nbsp;&nbsp;&nbsp; vel_arrow     | Double[N]    | m/s     | Arrow velocity                        |
| &nbsp;&nbsp;&nbsp;&nbsp; acc_arrow     | Double[N]    | m/s²    | Arrow acceleration                    |
| &nbsp;&nbsp;&nbsp;&nbsp; x_pos_limb    | Double[N][P] | m       | X coordinates of the limb nodes       |
| &nbsp;&nbsp;&nbsp;&nbsp; y_pos_limb    | Double[N][P] | m       | Y coordinates of the limb nodes       |
| &nbsp;&nbsp;&nbsp;&nbsp; angle_limb    | Double[N][P] | rad     | Rotation angles of the limb nodes     |
| &nbsp;&nbsp;&nbsp;&nbsp; epsilon       | Double[N][P] | --      | Longitudinal strain at the limb nodes |
| &nbsp;&nbsp;&nbsp;&nbsp; kappa         | Double[N][P] | m       | Bending curvature at the limb nodes   |
| &nbsp;&nbsp;&nbsp;&nbsp; x_pos_string  | Double[N][Q] | m       | X coordinates of the string nodes     |
| &nbsp;&nbsp;&nbsp;&nbsp; y_pos_string  | Double[N][Q] | m       | Y coordinates of the string nodes     |
| &nbsp;&nbsp;&nbsp;&nbsp; e_pot_limbs   | Double[N]    | J       | Potential energy of the limbs         |
| &nbsp;&nbsp;&nbsp;&nbsp; e_kin_limbs   | Double[N]    | J       | Kinetic energy of the limbs           |
| &nbsp;&nbsp;&nbsp;&nbsp; e_pot_string  | Double[N]    | J       | Potential energy of the string        |
| &nbsp;&nbsp;&nbsp;&nbsp; e_kin_string  | Double[N]    | J       | Kinetic energy of the string          |
| &nbsp;&nbsp;&nbsp;&nbsp; e_kin_arrow   | Double[N]    | J       | Kinetic energy of the arrow           |
| }                                      |              |         |                                       |