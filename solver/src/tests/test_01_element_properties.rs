use nalgebra::vector;
use crate::fem::elements::mass::MassElement;
use crate::fem::system::node::Constraints;
use crate::fem::system::system::System;
use crate::bow::sections::section::{LayerAlignment, LayeredCrossSection};
use crate::bow::input::{Layer, Material, Width};
use crate::bow::profile::profile::CurvePoint;
use crate::bow::profile::segments::clothoid::{ArcInput, ClothoidSegment};
use crate::fem::elements::beam::beam::BeamElement;
use crate::fem::elements::beam::linear::LinearBeamSegment;
use crate::fem::elements::string::StringElement;
use crate::tests::utils;

use std::f64::consts::FRAC_PI_2;

// These tests perform basic consistency checks on the various elements
// See the utils::checks::check_system_invariants function for the details

#[test]
fn mass_element() {
    let m = 1.5;

    let mut system = System::new();
    let node = system.create_node(&vector![0.0, 0.0, 0.0], Constraints::all_free());
    system.add_element(&[node], MassElement::new(m));

    utils::checks::check_system_invariants(&mut system);
}

#[test]
fn string_element() {
    let L = 1.5;
    let EA = 2100.0;
    let ηA = 400.0;

    let mut system = System::new();
    let node0 = system.create_node(&vector![0.0, 0.0, FRAC_PI_2 + 0.1], Constraints::all_free());
    let node1 = system.create_node(&vector![-1.0, 1.0, FRAC_PI_2 + 0.1], Constraints::all_free());
    let node2 = system.create_node(&vector![0.0, 2.0, FRAC_PI_2 + 0.1], Constraints::all_free());
    let node3 = system.create_node(&vector![0.0, 2.5, FRAC_PI_2 + 0.1], Constraints::all_free());
    let node4 = system.create_node(&vector![0.0, 3.0, FRAC_PI_2 + 0.1], Constraints::all_free());

    let offsets = vec![0.1, -0.1, -0.15, -0.15, -0.1];

    system.add_element(&[node0, node1, node2, node3, node4], StringElement::new(EA, ηA, L, offsets));

    utils::checks::check_system_invariants(&mut system);
}

#[test]
fn beam_element() {
    let l = 0.6;
    let r = 0.4;

    let material = Material::new("", "", 740.0, 11670e6, 8000e6);
    let width = Width::new(vec![[0.0, 0.01], [1.0, 0.01]]);
    let layer = Layer::new("", 0, vec![[0.0, 0.01], [1.0, 0.01]]);
    let curve = ClothoidSegment::arc(&CurvePoint::zero(), &ArcInput{length: l, radius: r});
    let section = LayeredCrossSection::new(l, &width, &vec![layer], &vec![material], LayerAlignment::SectionCenter).unwrap();
    let segment = LinearBeamSegment::new(&curve, &section, 0.0, l, &[]);

    let mut element = BeamElement::new(&segment);
    element.set_damping(0.1);

    let mut system = System::new();
    let node0 = system.create_node(&segment.p0, Constraints::all_free());
    let node1 = system.create_node(&segment.p1, Constraints::all_free());
    system.add_element(&[node0, node1], element);

    utils::checks::check_system_invariants(&mut system);
}