use virtualbow::output::BowOutput;
use assert_cmd::assert::OutputAssertExt;
use assert_cmd::cargo::CommandCargoExt;
use predicates::prelude::predicate;
use std::process::Command;

// Some basic functionality tests of the command line interface.

#[test]
fn command_none() {
    // Calling virtualbow without arguments should fail with an error message
    let mut cmd = Command::cargo_bin(env!("CARGO_PKG_NAME")).unwrap();
    cmd.assert()
        .failure()
        .stderr(predicate::str::contains("the following required arguments were not provided:"))
        .stdout(predicate::str::is_empty());
}

#[test]
fn command_help() {
    // Calling virtualbow with the help option should print a help message
    let mut cmd = Command::cargo_bin(env!("CARGO_PKG_NAME")).unwrap();
    cmd.arg("--help");
    cmd.assert()
        .success()
        .stderr(predicate::str::is_empty())
        .stdout(predicate::str::contains("Usage:"));
}

#[test]
fn command_version() {
    // Calling virtualbow with the version option should print a version message
    let mut cmd = Command::cargo_bin(env!("CARGO_PKG_NAME")).unwrap();
    cmd.arg("--version");
    cmd.assert()
        .success()
        .stderr(predicate::str::is_empty())
        .stdout(predicate::str::contains(env!("CARGO_PKG_VERSION")));
}

#[test]
fn command_static() {
    // Calling virtualbow with the static option should produce a result file with static results only
    let mut cmd = Command::cargo_bin(env!("CARGO_PKG_NAME")).unwrap();
    cmd.arg("static");
    cmd.arg("../../docs/examples/bows/big-paddle-ash.bow");
    cmd.arg("../../docs/examples/bows/big-paddle-ash.res");

    cmd.assert()
        .success()
        .stderr(predicate::str::is_empty())
        .stdout(predicate::str::is_empty());

    let output = BowOutput::load("../../docs/examples/bows/big-paddle-ash.res").unwrap();
    assert!(output.statics.is_some());
    assert!(output.dynamics.is_none());
}

#[test]
fn command_dynamic() {
    // Calling virtualbow with the dynamic option should produce a result file with static and dynamic results
    let mut cmd = Command::cargo_bin(env!("CARGO_PKG_NAME")).unwrap();
    cmd.arg("dynamic");
    cmd.arg("../../docs/examples/bows/big-paddle-ash.bow");
    cmd.arg("../../docs/examples/bows/big-paddle-ash.res");
    cmd.arg("--progress");

    cmd.assert()
        .success()
        .stderr(predicate::str::is_empty())
        .stdout(predicate::str::contains("progress"));

    let output = BowOutput::load("../../docs/examples/bows/big-paddle-ash.res").unwrap();
    assert!(output.statics.is_some());
    assert!(output.dynamics.is_some());
}

#[test]
fn command_error() {
    // Calling virtualbow on files that produce an error (here: input does not exist) should produce an error message
    let mut cmd = Command::cargo_bin(env!("CARGO_PKG_NAME")).unwrap();
    cmd.arg("dynamic");
    cmd.arg("nonexistent.bow");
    cmd.arg("nonexistent.res");

    cmd.assert()
        .failure()
        .stderr(predicate::str::contains("ERROR"))
        .stdout(predicate::str::is_empty());
}