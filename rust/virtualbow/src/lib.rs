#![forbid(unsafe_code)]

#![allow(non_snake_case)]
#![allow(mixed_script_confusables)]
#![allow(confusable_idents)]

#![allow(clippy::needless_return)]        // Sometimes an explicit return is more readable, in my opinion
#![allow(clippy::toplevel_ref_arg)]       // Triggers on nalgebra stack! macros otherwise, reenable when possible
#![allow(clippy::needless_range_loop)]    // Sometimes it is just the most clear way when accessing multiple collections
#![allow(clippy::too_many_arguments)]     // Sometimes needed, particularly in numerical functions/algorithms
#![allow(clippy::identity_op)]            // Judicious use of + 0 can make index operations more clear
#![allow(clippy::module_inception)]       // Don't feel like reorganizing everything, maybe later...
#![allow(clippy::manual_range_contains)]  // The manual version is more versatile readable to me
#![allow(clippy::collapsible_else_if)]    // Sometimes nested ifs can be more clear

#[cfg(test)]
#[macro_use]
extern crate approx;

#[cfg(test)]
#[macro_use]
extern crate assert_matches;

#[macro_use]
extern crate downcast_rs;

pub mod bow;
mod fem;
mod numerics;
mod utils;

#[cfg(test)]
mod tests;