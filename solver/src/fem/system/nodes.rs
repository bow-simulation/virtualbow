use crate::fem::system::dof::Dof;

pub trait Node: Copy + Clone {
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

    pub fn x(&self) -> Dof {
        self.dofs[0]
    }

    pub fn y(&self) -> Dof {
        self.dofs[1]
    }
}

#[derive(Copy, Clone, Debug)]
pub struct AncfNode {
    dofs: [Dof; 6]
}

impl Node for AncfNode {
    fn dofs(&self) -> &[Dof] {
        &self.dofs
    }
}

impl AncfNode {
    pub fn new(x: Dof, y: Dof, dxds: Dof, dyds: Dof, dxds2: Dof, dyds2: Dof) -> Self {
        Self {
            dofs: [x, y, dxds, dyds, dxds2, dyds2]
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

    pub fn dxds(&self) -> Dof {
        self.dofs[2]
    }

    pub fn dyds(&self) -> Dof {
        self.dofs[3]
    }

    pub fn dxds2(&self) -> Dof {
        self.dofs[4]
    }

    pub fn dyds2(&self) -> Dof {
        self.dofs[5]
    }
}


#[derive(Copy, Clone, Debug)]
pub struct GebfNode {
    dofs: [Dof; 3]
}

impl Node for GebfNode {
    fn dofs(&self) -> &[Dof] {
        &self.dofs
    }
}

impl GebfNode {
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