use serde::{Deserialize, Serialize};
use crate::errors::ModelError;
use super::BowOutput;

// TODO: Use env!("CARGO_PKG_VERSION") for the latest bow model variant instead of hard-coding the
// version as soon as https://github.com/serde-rs/serde/issues/2485 gets solved.
// Inspired by https://stackoverflow.com/a/70380491

#[derive(Serialize, Deserialize)]
#[serde(tag = "version")]
#[allow(clippy::large_enum_variant)]
pub enum BowOutputVersions {
    #[serde(rename = "0.10.0")]
    Latest(BowOutput),

    #[serde(alias = "0.9.1", alias = "0.9", alias = "0.8", alias = "0.7.1", alias = "0.7", alias = "0.6.1", alias = "0.6", alias = "0.5", alias = "0.4", alias = "0.3", alias = "0.2", alias="0.1")]
    Unsupported,

    #[serde(other)]
    Unrecognized,
}

impl BowOutputVersions {
    pub fn get_latest(self) -> Result<BowOutput, ModelError> {
        match self {
            BowOutputVersions::Latest(output) => Ok(output),
            BowOutputVersions::Unsupported => Err(ModelError::OutputVersionUnsupported),
            BowOutputVersions::Unrecognized => Err(ModelError::OutputVersionUnrecognized)
        }
    }
}