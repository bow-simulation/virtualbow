use serde::{Deserialize, Serialize};

// Wrapper types that add an integer version field to structs for (de)serialization.
// By using the "flatten" attribute from serde, the overall structure of the data is not changed.

#[derive(Deserialize)]
pub struct VersionedWrapper<T> {
    pub version: u64,
    #[serde(flatten)]
    pub data: T,
}

impl<T> VersionedWrapper<T> {
    pub fn new(version: u64, data: T) -> Self {
        Self {
            version,
            data
        }
    }
}

#[derive(Serialize)]
pub struct VersionedWrapperRef<'a, T> {
    version: u64,
    #[serde(flatten)]
    data: &'a T,
}

impl<'a, T> VersionedWrapperRef<'a, T> {
    pub fn new(version: u64, data: &'a T) -> Self {
        Self {
            version,
            data
        }
    }
}