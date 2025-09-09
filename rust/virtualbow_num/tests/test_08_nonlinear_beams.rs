/*
// Various nonlinear beams are simulated (static/dynamic/eigen) and compared with
// reference results obtained with GXBeam (https://github.com/byuflowlab/GXBeam.jl)

use std::fs::File;
use serde::Deserialize;

#[derive(Deserialize)]
struct Output {
    statics: Vec<OutputState>,
    dynamics: Vec<OutputState>,
    eigen: Vec<f64>
}

#[derive(Deserialize)]
struct OutputState {
    x: Vec<f64>,
    y: Vec<f64>
}

impl Output {
    pub fn load(path: &str) -> Self {
        let file = File::open(&path).expect("Failed to open file");
        serde_json::from_reader(file).expect("Failed to deserialize")
    }
}

#[test]
fn example_beam() {
    let output = Output::load("data/gxbeam/output.json");
    for i in 0..output.statics[10].x.len() {
        println!("{}, {}", output.statics[10].x[i], output.statics[10].y[i]);
    }
}
*/