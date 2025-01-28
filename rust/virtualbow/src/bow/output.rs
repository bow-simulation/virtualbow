use std::fs::File;
use std::io::{BufReader, Write};
use std::path::Path;
use nalgebra::{DVector, SVector};
use serde::{Deserialize, Serialize};
use soa_derive::StructOfArray;
use rmpv::Value;
use crate::bow::errors::ModelError;
use crate::bow::versioning::{VersionedWrapper, VersionedWrapperRef};

#[derive(Serialize, Deserialize, Default, PartialEq, Debug)]
pub struct BowOutput {
    pub common: Common,
    pub statics: Option<Statics>,
    pub dynamics: Option<Dynamics>,
}

impl BowOutput {
    // Current version of the output format, increase when incompatible changes are made.
    pub const FILE_VERSION: u64 = 3;

    // Loads output from a msgpack file, including a version check.
    // Since output files make no attempt at backwards compatibility, the version is simply checked for equality and rejected on mismatch.
    pub fn load<P: AsRef<Path>>(path: P) -> Result<BowOutput, ModelError> {
        let file = File::open(&path).map_err(|e| ModelError::OutputLoadFileError(path.as_ref().to_owned(), e))?;
        let mut reader = BufReader::new(file);

        let value: Value = rmpv::decode::value::read_value(&mut reader).map_err(ModelError::OutputDecodeMsgPackError)?;
        let version = Self::get_file_version(&value)?;

        if version != Self::FILE_VERSION {
            return Err(ModelError::OutputVersionMismatch(version));
        }

        // Parse wrapper (data + version number) from msgpack value. Discard version number and only return the data.
        let wrapper: VersionedWrapper<Self> = rmpv::ext::from_value(value).map_err(ModelError::OutputInterpretMsgPackError)?;
        Ok(wrapper.data)
    }

    // Save the output to a msgpack file
    pub fn save<P: AsRef<Path>>(&self, path: P) -> Result<(), ModelError> {
        let mut file = File::create(&path).map_err(|e| ModelError::OutputSaveFileError(path.as_ref().to_owned(), e))?;

        // Create wrapper object that adds the current version number
        let wrapper = VersionedWrapperRef::new(Self::FILE_VERSION, self);

        // Save wrapper object to file
        let bytes = rmp_serde::to_vec_named(&wrapper).map_err(ModelError::OutputEncodeMsgPackError)?;
        file.write_all(&bytes).map_err(|e| ModelError::OutputSaveFileError(path.as_ref().to_owned(), e))
    }

    // Returns the version of the result file, if it can be determined.
    fn get_file_version(value: &Value) -> Result<u64, ModelError> {
        let map = value.as_map().ok_or(ModelError::OutputVersionNotFound)?;
        let entry = map.iter().find(|&(key, _value)| {
            match key {
                Value::String(s) => s.as_str() == Some("version"),
                _ => false
            }
        }).ok_or(ModelError::OutputVersionNotFound)?;

        match entry.1 {
            Value::Integer(n) => n.as_u64().ok_or(ModelError::OutputVersionInvalid(n.to_string())),
            _ => Err(ModelError::OutputVersionInvalid(entry.1.to_string()))
        }
    }
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug)]
pub struct Common {
    pub limb: LimbInfo,
    pub layers: Vec<LayerInfo>,

    pub string_length: f64,
    pub string_mass: f64,
    pub limb_mass: f64,
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug)]
pub struct Statics {
    pub states: StateVec,

    pub final_draw_force: f64,
    pub final_drawing_work: f64,
    pub storage_factor: f64,

    pub max_string_force: (f64, usize),    // (value, state)
    pub max_strand_force: (f64, usize),    // (value, state)
    pub max_draw_force: (f64, usize),      // (value, state)
    pub min_grip_force: (f64, usize),      // (value, state)
    pub max_grip_force: (f64, usize),      // (value, state)

    pub min_layer_stresses: Vec<(f64, [usize; 3])>,    // (value, [state, length, belly/back]) for each layer
    pub max_layer_stresses: Vec<(f64, [usize; 3])>,    // (value, [state, length, belly/back]) for each layer
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug)]
pub struct Dynamics {
    pub states: StateVec,

    pub arrow_departure: Option<ArrowDeparture>,

    pub max_string_force: (f64, usize),    // (value, state)
    pub max_strand_force: (f64, usize),    // (value, state)
    pub max_draw_force: (f64, usize),      // (value, state)
    pub min_grip_force: (f64, usize),      // (value, state)
    pub max_grip_force: (f64, usize),      // (value, state)

    pub min_layer_stresses: Vec<(f64, [usize; 3])>,    // (value, [state, length, belly/back]) for each layer
    pub max_layer_stresses: Vec<(f64, [usize; 3])>,    // (value, [state, length, belly/back]) for each layer
}

// Data that is available only if the arrow has separated from the string during the dynamic analysis
#[derive(Serialize, Deserialize, Default, PartialEq, Debug)]
pub struct ArrowDeparture {
    // Index of the state at which the arrow separated
    pub state_idx: usize,

    // Position and velocity of the arrow at separation
    pub arrow_pos: f64,
    pub arrow_vel: f64,

    // Energies of the components at separation
    pub kinetic_energy_arrow: f64,
    pub elastic_energy_limbs: f64,
    pub kinetic_energy_limbs: f64,
    pub elastic_energy_string: f64,
    pub kinetic_energy_string: f64,

    // Degree of efficiency
    pub energy_efficiency: f64,
}

#[derive(StructOfArray, Serialize, Deserialize, PartialEq, Debug)]
#[soa_derive(Serialize, Deserialize, PartialEq, Debug)]
pub struct State {
    pub time: f64,
    pub draw_length: f64,

    pub limb_pos: Vec<SVector<f64, 3>>,    // x, y, φ
    pub limb_vel: Vec<SVector<f64, 3>>,    // x, y, φ

    pub string_pos: Vec<SVector<f64, 2>>,    // x, y
    pub string_vel: Vec<SVector<f64, 2>>,    // x, y

    pub limb_strain: Vec<SVector<f64, 3>>,    // epsilon, kappa, gamma
    pub limb_force: Vec<SVector<f64, 3>>,     // N, M, Q

    pub layer_strain: Vec<Vec<[f64; 2]>>,     // layer, length, belly/back
    pub layer_stress: Vec<Vec<[f64; 2]>>,     // layer, length, belly/back

    pub arrow_pos: f64,
    pub arrow_vel: f64,
    pub arrow_acc: f64,

    pub elastic_energy_limbs: f64,
    pub elastic_energy_string: f64,

    pub kinetic_energy_limbs: f64,
    pub kinetic_energy_string: f64,
    pub kinetic_energy_arrow: f64,

    pub damping_energy_limbs: f64,
    pub damping_energy_string: f64,
    pub damping_power_limbs: f64,
    pub damping_power_string: f64,

    pub draw_force: f64,
    pub draw_stiffness: f64,
    pub grip_force: f64,
    pub string_force: f64,
    pub strand_force: f64,
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug)]
pub struct LimbInfo {
    pub length: Vec<f64>,
    pub position: Vec<SVector<f64, 3>>,    // x, y, φ
    pub width: Vec<f64>,
    pub height: Vec<f64>,
    pub bounds: Vec<DVector<f64>>    // Layer boundaries in y position
}

#[derive(Serialize, Deserialize, PartialEq, Debug)]
pub struct LayerInfo {
    pub name: String
}

#[cfg(test)]
mod tests {
    use crate::bow::output::BowOutput;
    use crate::bow::errors::ModelError;

    #[test]
    fn test_load_output() {
        // IO error when loading from an invalid path
        assert_matches!(BowOutput::load("bows/tests/nonexistent.res"), Err(ModelError::OutputLoadFileError(_, _)));

        // Decoding error due to invalid file contents (invalid messagepack)
        assert_matches!(BowOutput::load("bows/tests/invalid_msgpack_1.res"), Err(ModelError::OutputDecodeMsgPackError(_)));

        // Decoding error due to invalid file contents (valid messagepack but invalid structure)
        assert_matches!(BowOutput::load("bows/tests/invalid_msgpack_2.res"), Err(ModelError::OutputInterpretMsgPackError(_)));

        // Error when loading results without version entry
        assert_matches!(BowOutput::load("bows/tests/no_version.res"), Err(ModelError::OutputVersionNotFound));

        // Error when loading results with an invalid version entry (wrong type)
        assert_matches!(BowOutput::load("bows/tests/invalid_version.res"), Err(ModelError::OutputVersionInvalid(_)));

        // Error when loading results with a different version
        assert_matches!(BowOutput::load("bows/tests/new_version.res"), Err(ModelError::OutputVersionMismatch(_)));

        // No error when loading a valid output file
        assert_matches!(BowOutput::load("bows/tests/valid.res"), Ok(_));
    }

    #[test]
    fn test_save_output() {
        let output = BowOutput::default();

        // IO error from saving to an invalid path
        assert_matches!(output.save("bows/tests/nonexistent/output.res"), Err(ModelError::OutputSaveFileError(_, _)));

        // The only remaining error case is an encoding error, but that one is difficult to trigger.
        // Therefore we just save the output once verify that the serialization works.
        assert_matches!(output.save("bows/tests/saved.res"), Ok(_));
    }
}