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

mod fem;
mod numerics;
mod bow;

#[cfg(test)]
mod tests;

use std::path::PathBuf;
use std::process::ExitCode;
use clap::Parser;
use crate::bow::errors::ModelError;
use crate::bow::model::BowModel;
use crate::bow::simulation::{Simulation, SimulationMode};

#[derive(Parser, Debug)]
#[command(version, about, long_about = None, disable_version_flag = true)]
struct Args {
    /// Mode of simulation
    #[arg(value_name = "MODE")]
    mode: SimulationMode,

    /// Input model file (*.bow)
    #[arg(value_name = "INPUT")]
    input: PathBuf,

    /// Output result file (*.res)
    #[arg(value_name = "OUTPUT")]
    output: PathBuf,

    /// Print simulation progress
    #[arg(short='p', long="progress", default_value_t = false)]
    progress: bool,

    /// Print version
    #[arg(short='v', long="version", action = clap::builder::ArgAction::Version)]
    version: (),
}

impl Args {
    // Performs the desired simulation according to the command line arguments
    // and save the results to the specified output path
    fn execute(&self) -> Result<(), ModelError> {
        let model = BowModel::load(&self.input)?;

        let output = Simulation::simulate(&model, self.mode, |stage, progress| {
            if self.progress {
                println!("stage: {stage}, progress: {progress:.1}%");
            }
            true
        })?;

        output.save(&self.output)?;
        Ok(())
    }
}

fn main() -> ExitCode {
    // Attempt to parse command line arguments
    let args = Args::parse();

    // Attempt to perform simulation and print any errors that occur
    if let Err(e) = args.execute() {
        eprintln!("ERROR: {}", e);
        return ExitCode::FAILURE;
    }

    ExitCode::SUCCESS
}