mod versions;
mod output;

use std::fs::File;
use std::io::{BufReader, Write};
use std::path::Path;
use crate::errors::ModelError;

pub use output::*;
use crate::output::versions::BowOutputVersions;

impl BowOutput {
    // Loads output from a msgpack file, including a version check.
    // Since output files make no attempt at backwards input, the version is simply checked for equality and rejected on mismatch.
    pub fn load<P: AsRef<Path>>(path: P) -> Result<BowOutput, ModelError> {
        let file = File::open(&path).map_err(|e| ModelError::OutputLoadFileError(path.as_ref().to_owned(), e))?;
        let mut reader = BufReader::new(file);  // TODO: Find out if buffering is of any advantage here.

        let output: BowOutputVersions = rmp_serde::from_read(&mut reader).map_err(ModelError::OutputDecodeMsgPackError)?;
        output.get_latest()
    }

    // Save the output to a msgpack file
    pub fn save<P: AsRef<Path>>(&self, path: P) -> Result<(), ModelError> {
        let output = BowOutputVersions::Latest(self.clone());    // TODO: Unnecessary clone?

        let mut file = File::create(&path).map_err(|e| ModelError::OutputSaveFileError(path.as_ref().to_owned(), e))?;
        let bytes = rmp_serde::to_vec_named(&output).map_err(ModelError::OutputEncodeMsgPackError)?;
        file.write_all(&bytes).map_err(|e| ModelError::OutputSaveFileError(path.as_ref().to_owned(), e))
    }
}

#[cfg(test)]
mod tests {
    use std::fs::File;
    use std::io::Write;
    use serde::Serialize;
    use crate::output::BowOutput;
    use crate::errors::ModelError;
    use assert_matches::assert_matches;

    #[test]
    fn test_load_output() {
        generate_test_files();

        // IO error when loading from a nonexistent file path
        assert_matches!(BowOutput::load("data/output/nonexistent.res"), Err(ModelError::OutputLoadFileError(_, _)));

        // Deserialization error due to the file containing invalid messagepack
        assert_matches!(BowOutput::load("data/output/invalid_msgpack.res"), Err(ModelError::OutputDecodeMsgPackError(_)));

        // Deserialization error due to the file containing valid messagepack but no version entry
        assert_matches!(BowOutput::load("data/output/version_missing.res"), Err(ModelError::OutputDecodeMsgPackError(_)));

        // Error when loading a result file with a version that is not supported
        assert_matches!(BowOutput::load("data/output/version_unsupported.res"), Err(ModelError::OutputVersionUnsupported));

        // Error when loading a result file with a version that is not recognized
        assert_matches!(BowOutput::load("data/output/version_unrecognized.res"), Err(ModelError::OutputVersionUnrecognized));

        // Error due to the file containing valid messagepack but an invalid version entry (wrong type)
        // => Different behaviour than the bow model (for json the deserialization into the enum fails on type mismatch of the tag)
        assert_matches!(BowOutput::load("data/output/version_invalid.res"), Err(ModelError::OutputVersionUnrecognized));

        // Deserialization error due to invalid file contents (valid messagepack and version but invalid structure)
        assert_matches!(BowOutput::load("data/output/invalid_content.res"), Err(ModelError::OutputDecodeMsgPackError(_)));

        // No error when loading a valid output file
        assert_matches!(BowOutput::load("data/output/valid_results.res"), Ok(_));

    }

    #[test]
    fn test_save_output() {
        let output = BowOutput::default();

        // IO error from saving to an invalid path
        assert_matches!(output.save("data/output/nonexistent/output.res"), Err(ModelError::OutputSaveFileError(_, _)));

        // The only remaining error case is an encoding error, but that one is difficult to trigger.
        // Saving without error is already covered by the generation of the test files above.
    }

    fn generate_test_files() {
        // File that contains invalid msgpack content, in this case just an empty fie
        File::create("data/output/invalid_msgpack.res").unwrap();

        // File with valid msgpack but no version entry
        let mut file = File::create("data/output/version_missing.res").unwrap();
        let data = NoVersion { x: 1.0, y: 2.0, z: 3.0, };
        let bytes = rmp_serde::to_vec_named(&data).unwrap();
        file.write_all(&bytes).unwrap();

        // File with valid msgpack but invalid version entry (wrong type)
        let mut file = File::create("data/output/version_invalid.res").unwrap();
        let data = VersionUsize { version: 7, x: 1.0, y: 2.0, z: 3.0, };
        let bytes = rmp_serde::to_vec_named(&data).unwrap();
        file.write_all(&bytes).unwrap();

        // File with valid messagepack and valid version entry but the version is unsupported
        let mut file = File::create("data/output/version_unsupported.res").unwrap();
        let data = VersionString { version: "0.3".to_string(), x: 1.0, y: 2.0, z: 3.0, };
        let bytes = rmp_serde::to_vec_named(&data).unwrap();
        file.write_all(&bytes).unwrap();

        // File with valid messagepack and valid version entry but the version is unknown
        let mut file = File::create("data/output/version_unrecognized.res").unwrap();
        let data = VersionString { version: "xyz".to_string(), x: 1.0, y: 2.0, z: 3.0, };
        let bytes = rmp_serde::to_vec_named(&data).unwrap();
        file.write_all(&bytes).unwrap();

        // File that contains valid msgpack with matching version but not a valid result
        let mut file = File::create("data/output/invalid_content.res").unwrap();
        let data = VersionString { version: env!("CARGO_PKG_VERSION").to_string(), x: 1.0, y: 2.0, z: 3.0, };
        let bytes = rmp_serde::to_vec_named(&data).unwrap();
        file.write_all(&bytes).unwrap();

        // File that contains valid result data in the correct version
        let output = BowOutput::default();
        output.save("data/output/valid_results.res").unwrap();
    }

    #[derive(Serialize)]
    struct NoVersion {
        x: f64,
        y: f64,
        z: f64
    }

    #[derive(Serialize)]
    struct VersionUsize {
        version: usize,
        x: f64,
        y: f64,
        z: f64
    }

    #[derive(Serialize)]
    struct VersionString {
        version: String,
        x: f64,
        y: f64,
        z: f64
    }
}