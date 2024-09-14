#[derive(Copy, Clone, Debug, PartialEq)]
pub enum Dof {
    Free{ index: usize, offset: f64 },
    Fixed{ offset: f64 },
}

impl Dof {
    pub fn new_free(index: usize, offset: f64) -> Self {
        Self::Free { index, offset }
    }

    pub fn new_fixed(offset: f64) -> Self {
        Self::Fixed { offset }
    }

    pub fn is_fixed(&self) -> bool {
        matches!(self, Dof::Fixed{..})
    }

    pub fn is_free(&self) -> bool {
        matches!(self, Dof::Free{..})
    }
}
