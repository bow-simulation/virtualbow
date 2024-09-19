use nalgebra::vector;
use crate::fem::elements::bar::BarElement;
use crate::fem::elements::mass::MassElement;
use crate::fem::system::nodes::Constraints;
use crate::fem::system::system::System;
use crate::bow::sections::section::{LayerAlignment, LayeredCrossSection};
use crate::bow::input::{Layer, Material, Width};
use crate::bow::profile::profile::CurvePoint;
use crate::bow::profile::segments::clothoid::{ArcInput, ClothoidSegment};
use crate::fem::elements::beam::beam::BeamElement;
use crate::fem::elements::beam::linear::LinearBeamSegment;
use crate::tests::utils;

// These tests perform basic consistency checks on the various elements
// See the utils::checks::check_system_invariants function for the details

#[test]
fn bar_element() {
    let L = 1.5;
    let EA = 2100.0;
    let etaA = 400.0;
    let rhoA = 0.785;

    let mut system = System::new();
    let node1 = system.create_point_node(&vector![0.0, 0.0], Constraints::all_free());
    let node2 = system.create_point_node(&vector![0.0, 0.0], Constraints::all_free());
    system.add_element(&[node1, node2], BarElement::new(rhoA, etaA, EA, L));

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

    let mut system = System::new();
    let element = BeamElement::new(&segment);
    let node0 = system.create_oriented_node(&segment.p0, Constraints::all_free());
    let node1 = system.create_oriented_node(&segment.p1, Constraints::all_free());
    system.add_element(&[node0, node1], element);

    utils::checks::check_system_invariants(&mut system);
}

#[test]
fn mass_element() {
    let m = 1.5;

    let mut system = System::new();
    let node = system.create_point_node(&vector![0.0, 0.0], Constraints::all_free());
    system.add_element(&[node], MassElement::new(m));

    utils::checks::check_system_invariants(&mut system);
}