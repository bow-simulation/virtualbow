use serde::{Deserialize, Serialize};
use crate::errors::ModelError;
use crate::input::{v1, v2, v3, v4};

// TODO: Use env!("CARGO_PKG_VERSION") for the latest bow model variant instead of hard-coding the
// version as soon as https://github.com/serde-rs/serde/issues/2485 gets solved.
// Inspired by https://stackoverflow.com/a/70380491
#[derive(Serialize, Deserialize)]
#[serde(tag = "version")]
pub enum BowModelVersions {
    #[serde(rename = "0.10.0")]
    V4(v4::BowModel),

    #[serde(rename = "0.9.1", alias = "0.9")]
    V3(v3::BowModel),

    #[serde(rename = "0.8")]
    V2(v2::BowModel),

    #[serde(rename = "0.7.1", alias = "0.7")]
    V1(v1::BowModel),

    #[serde(alias = "0.6.1", alias = "0.6", alias = "0.5", alias = "0.4", alias = "0.3", alias = "0.2", alias="0.1")]
    Unsupported,

    #[serde(other)]
    Unrecognized,
}

impl BowModelVersions {
    pub fn get_latest(self) -> Result<v4::BowModel, ModelError> {
        match self {
            BowModelVersions::V4(model) => Ok(model),
            BowModelVersions::V3(model) => Ok(v4::BowModel::from(model)),
            BowModelVersions::V2(model) => Ok(v4::BowModel::from(v3::BowModel::from(model))),
            BowModelVersions::V1(model) => Ok(v4::BowModel::from(v3::BowModel::from(v2::BowModel::from(model)))),
            BowModelVersions::Unsupported => Err(ModelError::InputVersionUnsupported),
            BowModelVersions::Unrecognized => Err(ModelError::InputVersionUnrecognized),
        }
    }
}