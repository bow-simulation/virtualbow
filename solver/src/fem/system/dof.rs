// The Dof enum represents a single degree of freedom of the FEM system.
// It can either be free or fixed:
//  - In the free case it is defined by the index of the corresponding system state
//  - In the fixed case it is defined by a constant position/angle value

#[derive(Copy, Clone, Debug, PartialEq)]
pub enum Dof {
    Free(usize),
    Fixed(f64),
}

impl Dof {
    pub fn is_fixed(&self) -> bool {
        matches!(self, Dof::Fixed{..})
    }

    pub fn is_free(&self) -> bool {
        matches!(self, Dof::Free{..})
    }
}
