use std::fs::File;
use std::path::Path;

use serde::{Serialize, Deserialize};

use crate::errors::ModelError;

use super::versions::latest;
use super::versions::version3;
use super::versions::version2;
use super::versions::version1;

// TODO: Use env!("CARGO_PKG_VERSION") for the latest bow model variant instead of hard-coding the
// version as soon as https://github.com/serde-rs/serde/issues/2485 gets solved.
// Inspired by https://stackoverflow.com/a/70380491
#[derive(Serialize, Deserialize)]
#[serde(tag = "version")]
pub enum BowModelVersion {
    #[serde(rename = "0.10.0")]
    Latest(latest::BowModel),

    #[serde(rename = "0.9.1", alias = "0.9")]
    Version3(version3::BowModel),

    #[serde(rename = "0.8")]
    Version2(version2::BowModel),

    #[serde(rename = "0.7.1", alias = "0.7")]
    Version1(version1::BowModel),

    #[serde(alias = "0.6.1", alias = "0.6", alias = "0.5", alias = "0.4", alias = "0.3", alias = "0.2", alias="0.1")]
    Unsupported,

    #[serde(other)]
    Unrecognized,
}

impl BowModelVersion {
    pub fn load<P: AsRef<Path>>(path: P) -> Result<Self, ModelError> {
        let file = File::open(&path).map_err(|e| ModelError::InputLoadFileError(path.as_ref().to_owned(), e))?;
        serde_json::from_reader(file).map_err(ModelError::InputDeserializeJsonError)
    }

    pub fn save<P: AsRef<Path>>(path: P, model: &latest::BowModel) -> Result<(), ModelError> {
        let mut file = File::create(&path).map_err(|e| ModelError::InputSaveFileError(path.as_ref().to_owned(), e))?;
        let version = BowModelVersion::Latest(model.clone());    // TODO: Unnecessary clone?
        serde_json::to_writer_pretty(&mut file, &version).map_err(ModelError::InputSerializeJsonError)?;
        
        Ok(())
    }

    pub fn is_latest(&self) -> bool {
        matches!(self, Self::Latest(_))
    }

    pub fn get_latest(self) -> Result<latest::BowModel, ModelError> {
        match self {
            Self::Latest(model) => Ok(model),
            Self::Version3(model) => Ok(latest::BowModel::from(model)),
            Self::Version2(model) => Ok(latest::BowModel::from(version3::BowModel::from(model))),
            Self::Version1(model) => Ok(latest::BowModel::from(version3::BowModel::from(version2::BowModel::from(model)))),
            Self::Unsupported => Err(ModelError::InputVersionUnsupported),
            Self::Unrecognized => Err(ModelError::InputVersionUnrecognized),
        }
    }
}