use std::ffi::{c_char, c_void, CStr, CString};
use std::ptr;

use virtualbow::simulation::SimulationMode;

mod api;

// TODO
// - Generated size field should be size_t (setting?)
// - Wrap into namespace? virtualbow::? vb::? ffi::?

#[repr(C)]
pub struct Response {
    error: *mut c_char,
    data: *mut u8,
    size: usize,
    capacity: usize
}

impl Response {
    // Empty response with neither data nor error present
    fn empty() -> Self {
        Self {
            error: ptr::null_mut(),
            data: ptr::null_mut(),
            size: 0,
            capacity: 0
        }
    }

    // Creates a response with an error string and empty  data (nullptr + zero size)
    fn error(message: String) -> Self {
        let message = CString::new(message).expect("Failed to convert error message into C string");

        Self {
            error: message.into_raw(),
            data: ptr::null_mut(),
            size: 0,
            capacity: 0
        }
    }

    // Creates a response with data and no error string (nullptr)
    // Construct buffer by deconstructing existing vector (https://users.rust-lang.org/t/pass-a-vec-from-rust-to-c/59184/3)
    // - Get pointer to vector's data and length of the data
    // - Forget vector so that the memory isn't dropped yet
    fn data(mut bytes: Vec<u8>) -> Self {
        let data = bytes.as_mut_ptr();
        let size = bytes.len();
        let capacity = bytes.capacity();

        std::mem::forget(bytes);

        Self {
            error: ptr::null_mut(),
            data,
            size,
            capacity
        }
    }

    // Free memory by reconstructing the corresponding Rust objects and letting them go out of scope
    unsafe fn free(self) {
        if !self.error.is_null() {
            let _string = CString::from_raw(self.error);
        }
        if !self.data.is_null() {
            let _vector = Vec::from_raw_parts(self.data, self.size, self.capacity);
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn load_model(path: *const c_char, converted: &mut bool) -> Response {
    let path = CStr::from_ptr(path);
    let path = path.to_str().expect("Failed to convert path to UTF-8");  // TODO: Encapsulate

    match api::load_model(path, converted) {
        Ok(vec) => Response::data(vec),
        Err(msg) => Response::error(msg),
    }
}

#[no_mangle]
pub unsafe extern "C" fn save_model(data: *const u8, size: usize, path: *const c_char, backup: bool) -> Response {
    let path = CStr::from_ptr(path);
    let path = path.to_str().expect("Failed to convert path to UTF-8");  // TODO: Encapsulate
    let data = std::slice::from_raw_parts(data, size);

    match api::save_model(data, path, backup) {
        Ok(()) => Response::empty(),
        Err(msg) => Response::error(msg),
    }
}

#[no_mangle]
pub unsafe extern "C" fn load_result(path: *const c_char) -> Response {    
    let path = CStr::from_ptr(path);
    let path = path.to_str().expect("Failed to convert path to UTF-8");  // TODO: Encapsulate

    match api::load_result(path) {
        Ok(vec) => Response::data(vec),
        Err(msg) => Response::error(msg),
    }
}

#[no_mangle]
pub unsafe extern "C" fn save_result(data: *const u8, size: usize, path: *const c_char) -> Response {    
    let path = CStr::from_ptr(path);
    let path = path.to_str().expect("Failed to convert path to UTF-8");  // TODO: Encapsulate
    let data = std::slice::from_raw_parts(data, size);

    match api::save_result(data, path) {
        Ok(()) => Response::empty(),
        Err(msg) => Response::error(msg),
    }
}

#[no_mangle]
pub unsafe extern "C" fn compute_geometry(data: *const u8, size: usize) -> Response {
    let data = std::slice::from_raw_parts(data, size);
    let result = api::compute_geometry(data);

    match result {
        Ok(res) => Response::data(res),
        Err(msg) => Response::error(msg),
    }
}

#[repr(C)]
pub enum Mode {
    Static,
    Dynamic
}

impl From<SimulationMode> for Mode {
    fn from(value: SimulationMode) -> Self {
        match value {
            SimulationMode::Static => Mode::Static,
            SimulationMode::Dynamic => Mode::Dynamic,
        }
    }
}

impl From<Mode> for SimulationMode {
    fn from(value: Mode) -> Self {
        match value {
            Mode::Static => SimulationMode::Static,
            Mode::Dynamic => SimulationMode::Dynamic,
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn simulate_model(data: *const u8, size: usize, mode: Mode, callback: unsafe extern "C" fn(Mode, f64, *mut c_void) -> bool, userdata: *mut c_void) -> Response {
    let data = std::slice::from_raw_parts(data, size);
    let result = api::simulate_model(data, mode.into(), |mode, progress| {
        callback(mode.into(), progress, userdata)
    });

    match result {
        Ok(res) => Response::data(res),
        Err(msg) => Response::error(msg),
    }
}

#[no_mangle]
pub unsafe extern "C" fn free_response(response: Response) {
    response.free();
}
