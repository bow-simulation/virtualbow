use nalgebra::{dvector, vector};
use virtualbow_num::fem::elements::mass::MassElement;
use virtualbow_num::fem::system::system::System;
use virtualbow_num::fem::elements::beam::beam::BeamElement;
use virtualbow_num::fem::elements::beam::linear::LinearBeamSegment;
use virtualbow_num::fem::elements::string::StringElement;
use virtualbow_num::testutils::syschecks::assert_system_invariants;
use virtualbow_num::fem::elements::beam::geometry::{ArcCurve, RectangularSection};
use virtualbow_num::fem::system::dof::DofType;

// These tests perform basic consistency checks on the various elements
// See the testutils::checks::check_system_invariants function for the details

#[test]
fn mass_element() {
    let m = 1.5;
    let J = 0.5;

    let mut system = System::new();
    let node = system.create_node(&vector![0.0, 0.0, 0.0], &[DofType::Active; 3]);
    system.add_element(&[node], MassElement::new(m, J));

    assert_system_invariants(&mut system);
}

#[test]
fn string_element() {
    let l0 = 1.5;
    let EA = 2100.0;
    let ηA = 400.0;
    let cf = 1.0;

    // Two nodes
    {
        let mut system = System::new();
        let node0 = system.create_node(&vector![0.0, 0.0, 1.5], &[DofType::Active; 3]);
        let node1 = system.create_node(&vector![0.0, 1.0, 1.5], &[DofType::Active; 3]);

        let offsets = vec![-0.1, -0.1];
        system.add_element(&[node0, node1], StringElement::new(EA, ηA, l0, cf, offsets));

        assert_system_invariants(&mut system);
    }

    // Three nodes, middle node has contact
    {
        let mut system = System::new();
        let node0 = system.create_node(&vector![0.0, 0.0, 1.5], &[DofType::Active; 3]);
        let node1 = system.create_node(&vector![0.1, 1.0, 1.5], &[DofType::Active; 3]);
        let node2 = system.create_node(&vector![0.0, 2.0, 1.5], &[DofType::Active; 3]);

        let offsets = vec![-0.1, -0.1, -0.1];
        system.add_element(&[node0, node1, node2], StringElement::new(EA, ηA, l0, cf, offsets));

        assert_system_invariants(&mut system);
    }

    // Three nodes, middle node has no contact
    {
        let mut system = System::new();
        let node0 = system.create_node(&vector![0.0, 0.0, 1.5], &[DofType::Active; 3]);
        let node1 = system.create_node(&vector![-0.1, 1.0, 1.5], &[DofType::Active; 3]);
        let node2 = system.create_node(&vector![0.0, 2.0, 1.5], &[DofType::Active; 3]);
    
        let offsets = vec![-0.1, -0.1, -0.1];
        system.add_element(&[node0, node1, node2], StringElement::new(EA, ηA, l0, cf, offsets));
    
        assert_system_invariants(&mut system);
    }

    // Four nodes, middle nodes have contact
    {
        let mut system = System::new();
        let node0 = system.create_node(&vector![0.0, 0.0, 1.5], &[DofType::Active; 3]);
        let node1 = system.create_node(&vector![0.1, 1.0, 1.5], &[DofType::Active; 3]);
        let node2 = system.create_node(&vector![0.1, 2.0, 1.5], &[DofType::Active; 3]);
        let node3 = system.create_node(&vector![0.0, 3.0, 1.5], &[DofType::Active; 3]);
        
        let offsets = vec![-0.1, -0.1, -0.1, -0.1];
        system.add_element(&[node0, node1, node2, node3], StringElement::new(EA, ηA, l0, cf, offsets));
        
        assert_system_invariants(&mut system);
    }
}

#[test]
fn beam_element() {
    let l = 0.6;
    let r = 0.4;

    let curve = ArcCurve { x: 0.0, y: 0.0, φ: 0.0, l, r, };
    let section = RectangularSection { w0: 0.01, h0: 0.01, w1: 0.005, h1: 0.005, ρ: 740.0, E: 11670e6, G: 8000e6 };
    let segment = LinearBeamSegment::new(&curve, &section, 0.0, l, &[]);

    let mut element = BeamElement::new(&segment);
    element.set_damping(0.1);

    let mut system = System::new();
    let node0 = system.create_node(&segment.p0, &[DofType::Active; 3]);
    let node1 = system.create_node(&segment.p1, &[DofType::Active; 3]);
    system.add_element(&[node0, node1], element);

    let u0 = system.get_displacements().clone();
    let v0 = system.get_displacements().clone();

    // Check at some different system states

    system.set_displacements(&(&u0 + dvector![0.5, 0.5, 0.5, 0.5, 0.5, 0.5]));
    system.set_velocities(&(&v0 + dvector![0.5, 0.5, 0.5, 0.5, 0.5, 0.5]));
    assert_system_invariants(&mut system);

    system.set_displacements(&(&u0 + dvector![0.5, 0.5, 0.5, -0.5, -0.5, -0.5]));
    system.set_velocities(&(&v0 + dvector![0.5, 0.5, 0.5, -0.5, -0.5, -0.5]));
    assert_system_invariants(&mut system);

    system.set_displacements(&(&u0 + dvector![0.5, -0.5, 0.5, -0.5, 0.5, -0.5]));
    system.set_velocities(&(&v0 + dvector![0.5, -0.5, 0.5, -0.5, 0.5, -0.5]));
    assert_system_invariants(&mut system);

    system.set_displacements(&(&u0 + dvector![-5.0, -5.0, -5.0, 5.0, 5.0, 5.0]));
    system.set_velocities(&(&v0 + dvector![5.0, 5.0, 5.0, -5.0, -5.0, -5.0]));
    assert_system_invariants(&mut system);
}