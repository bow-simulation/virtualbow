#[derive(Copy, Clone, Debug, PartialEq)]
pub enum Dof {
    Fixed{ offset: f64 },
    Free{ index: usize, offset: f64, scale: f64 }
}

impl Dof {
    // Creates a new dof whose local displacement is the scaled value of the displacement of this dof
    // Free case: factor * ( scale*u[i] + offset ) = factor*scale*u[i] + factor*offset
    // Fixed case: factor * offset
    pub fn scaled(&self, factor: f64) -> Self {
        match *self {
            Dof::Fixed{offset} => {
                Self::Fixed {
                    offset: factor*offset
                }
            }
            Dof::Free{index, offset, scale} => {
                Self::Free {
                    index: index,
                    offset: factor*offset,
                    scale: factor*scale
                }
            }
        }
    }

    pub fn is_fixed(&self) -> bool {
        matches!(self, Dof::Fixed{..})
    }

    pub fn is_free(&self) -> bool {
        matches!(self, Dof::Free{..})
    }
}
