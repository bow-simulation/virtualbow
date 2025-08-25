use std::fs;
use std::path::Path;
use virtualbow::errors::ModelError;
use virtualbow::geometry::LimbGeometry;
use virtualbow::input::BowModel;
use virtualbow::input::BowModelVersion;
use virtualbow::output::BowResult;
use virtualbow::simulation::{SimulationMode, Simulation};

// Safe Rust API to be wrapped and exposed by the unsafe FFI

pub fn new_model() -> Result<Vec<u8>, String> {
    let model = BowModel::example();
    let data = model.try_into().map_err(|e: ModelError| e.to_string())?;

    Ok(data)
}

pub fn load_model<P>(path: P, convert: bool) -> Result<Vec<u8>, String>
    where P: AsRef<Path>
{
    // Load model from file, keep track of whether the file contained the latest bow format
    let version = BowModelVersion::load(&path).map_err(|e| e.to_string())?;
    let latest = version.is_latest();
    let model = version.get_latest().map_err(|e| e.to_string())?;

    // If the convert option is active and the file contains an older format, the source file is to be converted
    // to the latest version while keeping a backup file with .bak extension of the original file content.
    if convert && !latest {
        // Only continue if the file is not read-only.
        // Otherwise we can't convert it, which is okay because the user can't unknowingly overwrite it and end up with no backup anyway.
        let meta = fs::metadata(&path).map_err(|e| e.to_string())?;
        if !meta.permissions().readonly() {
            // Create a path for the backup file by renaming *.bow -> *.bow.bak
            let mut backup = path.as_ref().to_path_buf();
            backup.set_extension("bow.bak");

            // If necessary, add a number at the end of the backup path until it is unique
            let mut i = 2;
            while backup.exists() {
                backup.set_extension(format!("bow.bak{}", i));
                i += 1;
            }

            // Rename the file and save the converted model under the original path
            fs::rename(&path, &backup).map_err(|e| e.to_string())?;
            model.save(&path).map_err(|e| e.to_string())?;
        }
    }

    // Convert model to msgpack and return data
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

pub fn compute_geometry(data: &[u8]) -> Result<Vec<u8>, String> {
    let model = BowModel::try_from(data).map_err(|e| e.to_string())?;
    let geometry = LimbGeometry::new(&model).map_err(|e| e.to_string())?;
    let discretized = geometry.discretize(model.settings.num_limb_eval_points, model.settings.num_limb_elements);
    let limb_info = discretized.to_limb_info();    // TODO: Get rid of this intermediate step
    let data = limb_info.try_into().map_err(|e: ModelError| e.to_string())?;

    Ok(data)
}

pub fn simulate_model<F>(data: &[u8], mode: SimulationMode, callback: F) -> Result<Vec<u8>, String>
    where F: Fn(SimulationMode, f64) -> bool
{
    let model = BowModel::try_from(data).map_err(|e| e.to_string())?;
    let result = Simulation::simulate(&model, mode, callback).map_err(|e| e.to_string())?;
    let data = result.try_into().map_err(|e: ModelError| e.to_string())?;

    Ok(data)
}