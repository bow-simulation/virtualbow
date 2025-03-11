use std::{ffi::CString, ptr};
use core::mem::drop;

#[repr(C)]
pub struct Buffer {
    data: *mut u8,
    size: usize
}

#[no_mangle]
pub extern "C" fn load_model(path: *const i8, convert: bool, error: *const i8) -> Buffer {

    Buffer {
        data: ptr::null_mut(),
        size: 0
    }
}

#[no_mangle]
pub unsafe extern "C" fn free_buffer(buffer: Buffer) {
    drop(Vec::from_raw_parts(buffer.data, buffer.size, buffer.size));
}