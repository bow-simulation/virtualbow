use std::path::Path;
use virtualbow::errors::ModelError;
use virtualbow::input::BowModel;
use virtualbow::input::BowModelVersion;
use virtualbow::output::BowResult;
use virtualbow::simulation::{SimulationMode, Simulation};

// Safe API to be wrapped and exposed by the unsafe FFI

pub fn new_model() -> Result<Vec<u8>, String> {
    let model = BowModel::example();
    let data = model.try_into().map_err(|e: ModelError| e.to_string())?;

    Ok(data)
}

pub fn load_model<P>(path: P, _convert: bool) -> Result<Vec<u8>, String>
    where P: AsRef<Path>
{
    let version = BowModelVersion::load(path).map_err(|e| e.to_string())?;
    let model = version.get_latest().map_err(|e| e.to_string())?;
    let data = model.try_into().map_err(|e: ModelError| e.to_string())?;

    Ok(data)
}

pub fn save_model<P>(data: &[u8], path: P) -> Result<(), String>
    where P: AsRef<Path>
{
    let model = BowModel::try_from(data).map_err(|e| e.to_string())?;
    model.save(path).map_err(|e| e.to_string())
}

pub fn load_result<P>(path: P) -> Result<Vec<u8>, String>
    where P: AsRef<Path>
{
    let result = BowResult::load(path).map_err(|e| e.to_string())?;
    let data = result.try_into().map_err(|e: ModelError| e.to_string())?;

    Ok(data)
}

pub fn save_result<P>(data: &[u8], path: P) -> Result<(), String>
    where P: AsRef<Path>
{
    let result = BowResult::try_from(data).map_err(|e| e.to_string())?;
    result.save(path).map_err(|e| e.to_string())
}

pub fn simulate_model<F>(data: &[u8], mode: SimulationMode, callback: F) -> Result<Vec<u8>, String>
    where F: Fn(SimulationMode, f64) -> bool
{
    let model = BowModel::try_from(data).map_err(|e| e.to_string())?;
    let result = Simulation::simulate(&model, mode, callback).map_err(|e| e.to_string())?;
    let data = result.try_into().map_err(|e: ModelError| e.to_string())?;

    Ok(data)
}