// The Dof enum represents a single degree of freedom of the FEM system.
// It can either be active (free to move) or locked (fixed in place) and contains the index of the corresponding system state.

#[derive(Copy, Clone, Debug, PartialEq)]
pub enum DofDimension {
    Position,
    Rotation
}

#[derive(Copy, Clone, Debug, PartialEq)]
pub enum DofType {
    Active,
    Locked
}

impl DofType {
    pub fn active_if(condition: bool) -> DofType {
        if condition {
            DofType::Active
        } else {
            DofType::Locked
        }
    }

    pub fn locked_if(condition: bool) -> DofType {
        DofType::active_if(!condition)
    }
}

// TODO: Can we block instantiation of Dofs outside the system implementation?
#[derive(Copy, Clone, Debug, PartialEq)]
pub struct Dof {
    pub kind: DofType,
    pub index: usize
}

#[allow(dead_code)]
impl Dof {
    pub fn is_active(&self) -> bool {
        self.kind == DofType::Active
    }

    pub fn is_locked(&self) -> bool {
        self.kind == DofType::Locked
    }
}
