#![allow(non_snake_case)]
#![allow(mixed_script_confusables)]
#![allow(dead_code)]

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

#[cfg(test)]
mod tests;