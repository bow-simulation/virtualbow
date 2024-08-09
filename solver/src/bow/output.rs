use std::fs::File;
use std::io::{BufReader, Write};
use std::path::Path;
use nalgebra::{SMatrix, SVector};
use serde::{Deserialize, Serialize};
use soa_derive::StructOfArray;
use rmpv::Value;
use crate::bow::errors::ModelError;
use crate::bow::versioning::{VersionedWrapper, VersionedWrapperRef};

#[derive(Serialize, Deserialize, PartialEq, Debug)]
pub struct Output {
    pub setup: Setup,
    pub statics: Option<Statics>,
    pub dynamics: Option<Dynamics>,
}

impl Output {
    // Current version of the output format, increase when incompatible changes are made.
    pub const FILE_VERSION: u64 = 3;

    // Loads output from a msgpack file, including a version check.
    // Since output files make no attempt at backwards compatibility, the version is simply checked for equality and rejected on mismatch.
    pub fn load<P: AsRef<Path>>(path: P) -> Result<Output, ModelError> {
        let file = File::open(&path).map_err(|e| ModelError::OutputLoadFileError(path.as_ref().to_owned(), e))?;
        let mut reader = BufReader::new(file);

        let value: Value = rmpv::decode::value::read_value(&mut reader).map_err(|e| ModelError::OutputDecodeMsgPackError(e))?;
        let version = Self::get_file_version(&value)?;

        if version != Self::FILE_VERSION {
            return Err(ModelError::OutputVersionMismatch(version));
        }

        // Parse wrapper (data + version number) from msgpack value. Discard version number and only return the data.
        let wrapper: VersionedWrapper<Self> = rmpv::ext::from_value(value).map_err(|e| ModelError::OutputInterpretMsgPackError(e))?;
        Ok(wrapper.data)
    }

    // Save the output to a msgpack file
    pub fn save<P: AsRef<Path>>(&self, path: P) -> Result<(), ModelError> {
        let mut file = File::create(&path).map_err(|e| ModelError::OutputSaveFileError(path.as_ref().to_owned(), e))?;

        // Create wrapper object that adds the current version number
        let wrapper = VersionedWrapperRef::new(Self::FILE_VERSION, self);

        // Save wrapper object to file
        let bytes = rmp_serde::to_vec_named(&wrapper).map_err(|e| ModelError::OutputEncodeMsgPackError(e))?;
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

impl Default for Output {
    fn default() -> Self {
        Self {
            setup: Default::default(),
            statics: None,
            dynamics: None,
        }
    }
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug)]
pub struct Setup {
    pub limb: LimbSetup,
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug)]
pub struct Statics {
    pub states: StateVec,

    pub final_draw_force: f64,
    pub drawing_work: f64,
    pub storage_factor: f64,

    pub max_string_force: (f64, usize),    // (value, state)
    pub max_grip_force: (f64, usize),      // (value, state)
    pub max_draw_force: (f64, usize),      // (value, state)

    pub min_layer_stresses: Vec<(f64, usize, usize)>,    // (value, state, node) for each layer
    pub max_layer_stresses: Vec<(f64, usize, usize)>,    // (value, state, node) for each layer
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug)]
pub struct Dynamics {
    pub states: StateVec,

    pub final_arrow_pos: f64,
    pub final_arrow_vel: f64,

    pub final_e_kin_arrow: f64,
    pub final_e_pot_limbs: f64,
    pub final_e_kin_limbs: f64,
    pub final_e_pot_string: f64,
    pub final_e_kin_string: f64,
    pub energy_efficiency: f64,

    pub max_string_force: (f64, usize),    // (value, state)
    pub max_grip_force: (f64, usize),      // (value, state)
    pub max_draw_force: (f64, usize),      // (value, state)

    pub min_layer_stresses: Vec<(f64, usize, usize)>,    // (value, state, node) for each layer
    pub max_layer_stresses: Vec<(f64, usize, usize)>,    // (value, state, node) for each layer
}

#[derive(StructOfArray, Serialize, Deserialize, PartialEq, Debug)]
#[soa_derive(Serialize, Deserialize, PartialEq, Debug)]
pub struct State {
    pub time: f64,
    pub draw_length: f64,

    pub limb_pos: Vec<SVector<f64, 3>>,    // x, y, φ
    pub limb_vel: Vec<SVector<f64, 3>>,    // x, y, φ
    pub limb_acc: Vec<SVector<f64, 3>>,    // x, y, φ

    pub string_pos: Vec<SVector<f64, 2>>,    // x, y
    pub string_vel: Vec<SVector<f64, 2>>,    // x, y
    pub string_acc: Vec<SVector<f64, 2>>,    // x, y

    pub limb_strain: Vec<SVector<f64, 3>>,    // epsilon, kappa, gamma
    pub limb_force: Vec<SVector<f64, 3>>,     // N, M, Q

    pub layer_strain: Vec<Vec<(f64, f64)>>,    // layer, point, back/belly
    pub layer_stress: Vec<Vec<(f64, f64)>>,    // layer, point, back/belly

    pub arrow_pos: f64,
    pub arrow_vel: f64,
    pub arrow_acc: f64,

    pub e_pot_limbs: f64,
    pub e_kin_limbs: f64,
    pub e_pot_string: f64,
    pub e_kin_string: f64,
    pub e_kin_arrow: f64,

    pub draw_force: f64,
    pub grip_force: f64,
    pub string_force: f64,
    pub strand_force: f64,
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug)]
pub struct LimbSetup {
    // Layer properties
    pub layers: Vec<LayerSetup>,

    // Nodes
    pub length: Vec<f64>,
    pub position: Vec<SVector<f64, 3>>,    // x, y, φ

    // Section profile
    pub width: Vec<f64>,
    pub height: Vec<f64>,

    // Section properties
    pub density: Vec<f64>,
    pub stiffness: Vec<SMatrix<f64, 3, 3>>,        // epsilon, kappa, gamma
}

#[derive(Serialize, Deserialize, PartialEq, Debug)]
pub struct LayerSetup {
    length: Vec<f64>,    // Arc lengths at which the layer is evaluated
}

#[cfg(test)]
mod tests {
    use crate::bow::output::Output;
    use crate::bow::errors::ModelError;

    /*
    #[test]
    fn test_load_output() {
        // IO error when loading from an invalid path
        assert_matches!(Output::load("bows/tests/nonexistent.res"), Err(ModelError::OutputLoadFileError(_, _)));

        // Decoding error due to invalid file contents (invalid messagepack)
        assert_matches!(Output::load("bows/tests/invalid_msgpack_1.res"), Err(ModelError::OutputDecodeMsgPackError(_)));

        // Decoding error due to invalid file contents (valid messagepack but invalid structure)
        assert_matches!(Output::load("bows/tests/invalid_msgpack_2.res"), Err(ModelError::OutputInterpretMsgPackError(_)));

        // Error when loading results without version entry
        assert_matches!(Output::load("bows/tests/no_version.res"), Err(ModelError::OutputVersionNotFound));

        // Error when loading results with an invalid version entry (wrong type)
        assert_matches!(Output::load("bows/tests/invalid_version.res"), Err(ModelError::OutputVersionInvalid(_)));

        // Error when loading results with a different version
        assert_matches!(Output::load("bows/tests/new_version.res"), Err(ModelError::OutputVersionMismatch(_)));

        // No error when loading a valid output file
        assert_matches!(Output::load("bows/tests/valid.res"), Ok(_));
    }
    */

    #[test]
    fn test_save_output() {
        let output = Output::default();

        // IO error from saving to an invalid path
        assert_matches!(output.save("bows/tests/nonexistent/output.res"), Err(ModelError::OutputSaveFileError(_, _)));

        // The only remaining error case is an encoding error, but that one is difficult to trigger.
        // Therefore we just save the output once verify that the serialization works.
        assert_matches!(output.save("bows/tests/saved.res"), Ok(_));
    }
}