use crate::fem::system::dof::Dof;

#[derive(Copy, Clone, Debug)]
pub struct Node {
    dofs: [Dof; 3]
}

impl Node {
    pub fn dofs(&self) -> &[Dof] {
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