use nalgebra::{dvector, vector};
use crate::fem::elements::mass::MassElement;
use crate::fem::system::system::System;
use crate::bow::sections::section::{LayerAlignment, LayeredCrossSection};
use crate::bow::input::{Layer, Material, Width};
use crate::bow::profile::profile::CurvePoint;
use crate::bow::profile::segments::clothoid::{ArcInput, ClothoidSegment};
use crate::fem::elements::beam::beam::BeamElement;
use crate::fem::elements::beam::linear::LinearBeamSegment;
use crate::fem::elements::string::StringElement;
use crate::tests::utils;

// These tests perform basic consistency checks on the various elements
// See the utils::checks::check_system_invariants function for the details

#[test]
fn mass_element() {
    let m = 1.5;

    let mut system = System::new();
    let node = system.create_node(&vector![0.0, 0.0, 0.0], &[true; 3]);
    system.add_element(&[node], MassElement::new(m));

    utils::checks::check_system_invariants(&mut system);
}

#[test]
fn string_element() {
    let l = 1.5;
    let EA = 2100.0;
    let ηA = 400.0;

    // Two nodes
    {
        let mut system = System::new();
        let node0 = system.create_node(&vector![0.0, 0.0, 1.5], &[true; 3]);
        let node1 = system.create_node(&vector![0.0, 1.0, 1.5], &[true; 3]);

        let offsets = vec![-0.1, -0.1];
        system.add_element(&[node0, node1], StringElement::new(EA, ηA, l, offsets));

        utils::checks::check_system_invariants(&mut system);
    }

    // Three nodes, middle node has contact
    {
        let mut system = System::new();
        let node0 = system.create_node(&vector![0.0, 0.0, 1.5], &[true; 3]);
        let node1 = system.create_node(&vector![0.1, 1.0, 1.5], &[true; 3]);
        let node2 = system.create_node(&vector![0.0, 2.0, 1.5], &[true; 3]);

        let offsets = vec![-0.1, -0.1, -0.1];
        system.add_element(&[node0, node1, node2], StringElement::new(EA, ηA, l, offsets));

        utils::checks::check_system_invariants(&mut system);
    }

    // Three nodes, middle node has no contact
    {
        let mut system = System::new();
        let node0 = system.create_node(&vector![0.0, 0.0, 1.5], &[true; 3]);
        let node1 = system.create_node(&vector![-0.1, 1.0, 1.5], &[true; 3]);
        let node2 = system.create_node(&vector![0.0, 2.0, 1.5], &[true; 3]);
    
        let offsets = vec![-0.1, -0.1, -0.1];
        system.add_element(&[node0, node1, node2], StringElement::new(EA, ηA, l, offsets));
    
        utils::checks::check_system_invariants(&mut system);
    }

    // Four nodes, middle nodes have contact
    {
        let mut system = System::new();
        let node0 = system.create_node(&vector![0.0, 0.0, 1.5], &[true; 3]);
        let node1 = system.create_node(&vector![0.1, 1.0, 1.5], &[true; 3]);
        let node2 = system.create_node(&vector![0.1, 2.0, 1.5], &[true; 3]);
        let node3 = system.create_node(&vector![0.0, 3.0, 1.5], &[true; 3]);
        
        let offsets = vec![-0.1, -0.1, -0.1, -0.1];
        system.add_element(&[node0, node1, node2, node3], StringElement::new(EA, ηA, l, offsets));
        
        utils::checks::check_system_invariants(&mut system);
    }
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
    let node0 = system.create_node(&segment.p0, &[true; 3]);
    let node1 = system.create_node(&segment.p1, &[true; 3]);
    system.add_element(&[node0, node1], element);

    let u0 = system.get_displacements().clone();
    let v0 = system.get_displacements().clone();

    // Check at some different system states

    system.set_displacements(&(&u0 + dvector![0.5, 0.5, 0.5, 0.5, 0.5, 0.5]));
    system.set_velocities(&(&v0 + dvector![0.5, 0.5, 0.5, 0.5, 0.5, 0.5]));
    utils::checks::check_system_invariants(&mut system);

    system.set_displacements(&(&u0 + dvector![0.5, 0.5, 0.5, -0.5, -0.5, -0.5]));
    system.set_velocities(&(&v0 + dvector![0.5, 0.5, 0.5, -0.5, -0.5, -0.5]));
    utils::checks::check_system_invariants(&mut system);

    system.set_displacements(&(&u0 + dvector![0.5, -0.5, 0.5, -0.5, 0.5, -0.5]));
    system.set_velocities(&(&v0 + dvector![0.5, -0.5, 0.5, -0.5, 0.5, -0.5]));
    utils::checks::check_system_invariants(&mut system);

    system.set_displacements(&(&u0 + dvector![-5.0, -5.0, -5.0, 5.0, 5.0, 5.0]));
    system.set_velocities(&(&v0 + dvector![5.0, 5.0, 5.0, -5.0, -5.0, -5.0]));
    utils::checks::check_system_invariants(&mut system);
}