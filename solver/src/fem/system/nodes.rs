use crate::fem::system::dof::Dof;

pub trait Node {
    fn dofs(&self) -> &[Dof];
}

#[derive(Copy, Clone)]
pub struct PointNode {
    dofs: [Dof; 2]
}

impl Node for PointNode {
    fn dofs(&self) -> &[Dof] {
        &self.dofs
    }
}

impl PointNode {
    pub fn new(x: Dof, y: Dof) -> Self {
        Self {
            dofs: [x, y]
        }
    }

    pub fn oriented(&self) -> OrientedNode {
        OrientedNode::new(self.dofs[0], self.dofs[1], Dof::Fixed(0.0))
    }

    pub fn x(&self) -> Dof {
        self.dofs[0]
    }

    pub fn y(&self) -> Dof {
        self.dofs[1]
    }
}

#[derive(Copy, Clone, Debug)]
pub struct OrientedNode {
    dofs: [Dof; 3]
}

impl Node for OrientedNode {
    fn dofs(&self) -> &[Dof] {
        &self.dofs
    }
}

impl OrientedNode {
    pub fn new(x: Dof, y: Dof, φ: Dof) -> Self {
        Self {
            dofs: [x, y, φ]
        }
    }

    pub fn point(&self) -> PointNode {
        PointNode::new(self.dofs[0], self.dofs[1])
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

    pub fn all_free() -> Self {
        Self {
            x_pos_fixed: false,
            y_pos_fixed: false,
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

    pub fn all_fixed() -> Self {
        Self {
            x_pos_fixed: true,
            y_pos_fixed: true,
            z_rot_fixed: true
        }
    }

    pub fn pos_fixed() -> Self {
        Self {
            x_pos_fixed: true,
            y_pos_fixed: true,
            z_rot_fixed: false
        }
    }

    pub fn rot_fixed() -> Self {
        Self {
            x_pos_fixed: false,
            y_pos_fixed: false,
            z_rot_fixed: true
        }
    }
}