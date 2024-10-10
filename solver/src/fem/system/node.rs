use crate::fem::system::dof::Dof;

#[derive(Copy, Clone, Debug)]
pub struct Node {
    dofs: [Dof; 3]
}

impl Node {
    pub(crate) fn dofs(&self) -> &[Dof] {
        &self.dofs
    }
}

impl Node {
    pub fn new(x: Dof, y: Dof, φ: Dof) -> Self {
        Self {
            dofs: [x, y, φ]
        }
    }

    pub fn x(&self) -> Dof {
        self.dofs[0]
    }

    pub fn y(&self) -> Dof {
        self.dofs[1]
    }

    pub fn φ(&self) -> Dof {
        self.dofs[2]
    }
}

#[derive(Copy, Clone)]
pub struct Constraints {
    pub x_pos_fixed: bool,
    pub y_pos_fixed: bool,
    pub z_rot_fixed: bool
}

impl Constraints {
    pub fn new(x_pos_fixed: bool, y_pos_fixed: bool, z_rot_fixed: bool) -> Self {
        Self {
            x_pos_fixed,
            y_pos_fixed,
            z_rot_fixed
        }    
    }

    pub fn all_fixed() -> Self {
        Self {
            x_pos_fixed: true,
            y_pos_fixed: true,
            z_rot_fixed: true
        }
    }

    pub fn all_free() -> Self {
        Self {
            x_pos_fixed: false,
            y_pos_fixed: false,
            z_rot_fixed: false
        }
    }

    pub fn pos_free() -> Self {
        Self {
            x_pos_fixed: false,
            y_pos_fixed: false,
            z_rot_fixed: true
        }
    }

    pub fn rot_free() -> Self {
        Self {
            x_pos_fixed: true,
            y_pos_fixed: true,
            z_rot_fixed: false
        }
    }

    pub fn x_pos_free() -> Self {
        Self {
            x_pos_fixed: false,
            y_pos_fixed: true,
            z_rot_fixed: true
        }
    }

    pub fn y_pos_free() -> Self {
        Self {
            x_pos_fixed: true,
            y_pos_fixed: false,
            z_rot_fixed: true
        }
    }

    pub fn z_rot_free() -> Self {
        Self {
            x_pos_fixed: true,
            y_pos_fixed: true,
            z_rot_fixed: false
        }
    }
}