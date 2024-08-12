use std::collections::hash_map::{DefaultHasher, Entry};
use std::collections::HashMap;
use std::hash::{Hash, Hasher};
use itertools::Itertools;
use serde_json::{json, Value};
use crate::bow::errors::ModelError;
use crate::bow::model::BowModel;

// Converts the json definition of a bow file to the currently required format, if necessary and possible.
// Each time the input format changes, the version counter must be increased and a new conversion function must be added.
// Increasing the version without adding a conversion leads to a compile-time error, see https://stackoverflow.com/q/77456313/10489834
pub fn convert_to_current_format(value: &mut Value) -> Result<(), ModelError> {
    let version = get_file_version(value)?;

    if version > BowModel::FILE_VERSION {
        return Err(ModelError::InputVersionTooNew(version));
    }

    for current in version..BowModel::FILE_VERSION {
        match current {
            0 => convert_v0_to_v1(value)?,
            1 => convert_v1_to_v2(value)?,
            2 => convert_v2_to_v3(value)?,
            BowModel::FILE_VERSION => (),
            BowModel::FILE_VERSION.. => unreachable!(),
        }
    }

    Ok(())
}

// Returns the version of the bow file, if it can be determined.
// Since version 0.10 of VirtualBow the bow files are versioned independently from the application by a top-level integer field "version".
// Until version 0.9 however, that version field was instead equal to the application's version string.
//   => If such an older version string is found, it is mapped to a retroactively assigned integer version number.
//   => Version 0.7 was decided as the cutoff for support, so it is mapped to integer version 0.
fn get_file_version(json: &Value) -> Result<u64, ModelError> {
    // Try to find the version entry, return error if none is present
    let entry = json.get("version").ok_or(ModelError::InputVersionNotFound)?;

    // Entry can be a number according to the current versioning scheme, a string according
    // to the old versioning scheme or something else and therefore invalid
    match entry {
        Value::Number(n) => n.as_u64().ok_or(ModelError::InputVersionInvalid(n.to_string())),
        Value::String(s) => match s.as_str() {
            "0.9" | "0.9.0" | "0.9.1" => Ok(2),
            "0.8" | "0.8.0"           => Ok(1),
            "0.7" | "0.7.0" | "0.7.1" => Ok(0),
            "0.6" | "0.6.0" | "0.6.1" => Err(ModelError::InputVersionTooOld(s.clone())),
            "0.5" | "0.5.0"           => Err(ModelError::InputVersionTooOld(s.clone())),
            "0.4" | "0.4.0"           => Err(ModelError::InputVersionTooOld(s.clone())),
            "0.3" | "0.3.0"           => Err(ModelError::InputVersionTooOld(s.clone())),
            "0.2" | "0.2.0"           => Err(ModelError::InputVersionTooOld(s.clone())),
            "0.1" | "0.1.0"           => Err(ModelError::InputVersionTooOld(s.clone())),
            _ => Err(ModelError::InputVersionInvalid(s.clone()))
        },
        _ => Err(ModelError::InputVersionInvalid(entry.to_string()))
    }
}

// Mapping from file version to the virtualbow version in which it was first introduced
fn get_application_version<'a>(version: u64) -> Option<&'a str> {
    match version {
        0 => Some("0.7.0"),
        1 => Some("0.8.0"),
        2 => Some("0.9.0"),
        BowModel::FILE_VERSION => Some(env!("CARGO_PKG_VERSION")),
        BowModel::FILE_VERSION.. => unreachable!(),
    }
}

fn convert_v2_to_v3(value: &mut Value) -> Result<(), ModelError> {
    value["version"] = json!(3);

    let segments = value["profile"].as_array().ok_or(ModelError::InputConversionError(2, 3, "Entry 'profile' not found or invalid".to_string()))?;
    value["profile"] = json!({
        "alignment": "section-back",
        "segments":  segments.iter().map(|segment| {
            match segment["type"].as_str() {
                Some("line") => {
                    json!({
                        "type": "line",
                        "length": segment["parameters"]["length"]
                    })
                }
                Some("arc") => {
                    json!({
                        "type": "arc",
                        "length": segment["parameters"]["length"],
                        "radius": segment["parameters"]["radius"]
                    })
                }
                Some("spiral") => {
                    json!({
                        "type": "spiral",
                        "length": segment["parameters"]["length"],
                        "radius0": segment["parameters"]["r_start"],
                        "radius1": segment["parameters"]["r_end"]
                    })
                }
                Some("spline") => {
                    json!({
                        "type": "spline",
                        "points": segment["parameters"]["points"]
                    })
                }
                _ => json!({})
            }
        }).collect_vec()
    });

    // Older versions did not have the shear modulus as a material property, therefore calculate a default
    // value based on the elastic modulus and a poisson ratio of 0.4
    let materials = value["materials"].as_array_mut().ok_or(ModelError::InputConversionError(2, 3, "Entry 'materials' not found or invalid".to_string()))?;
    materials.iter_mut()
        .for_each(|material| {
            let material = material.as_object_mut().expect("Material must be an object");
            let E = material["E"].as_f64().expect("Invalid elastic modulus");
            let G = E/(2.0*(1.0 + 0.4));

            material.insert("G".into(), json!(G));
        });

    // New settings entries for number of evaluation points
    value["settings"]["n_limb_eval_points"] = json!(100);
    value["settings"]["n_layer_eval_points"] = json!(100);

    // Move width to width/points
    value["width"] = json!({
        "points": value["width"]
    });

    Ok(())
}

fn convert_v1_to_v2(value: &mut Value) -> Result<(), ModelError> {
    value["version"] = json!(2);

    // In previous versions, the colors were randomly generated based on the material properties (same properties -> same color)
    // Starting with version 0.9 the colors can be chosen by users, so we have to pick some initial color here. Instead of replicating the old
    // random algorithm, they are chosen out of a fixed color palette (taken from Python's Matplotlib,  https://stackoverflow.com/a/42091037)
    const COLOR_PALETTE: &[&str; 10] =&["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf"];
    let mut used_colors = HashMap::<u64, String>::new();
    let mut next_color = 0;

    let mut get_material_color = |rho: f64, E: f64| -> String {
        // Check if the material (rho, E) was already assigned a color
        // If not, assign it the next color in the palette (with wrap-around)
        let key = {
            let mut s = DefaultHasher::new();
            rho.to_bits().hash(&mut s);
            E.to_bits().hash(&mut s);
            s.finish()
        };

        match used_colors.entry(key) {
            Entry::Occupied(entry) => entry.into_mut().clone(),
            Entry::Vacant(entry) => {
                let value = entry.insert(COLOR_PALETTE[next_color % COLOR_PALETTE.len()].to_string()).clone();
                next_color += 1;
                return value;
            },
        }
    };

    // Create new material from each layer and make layer only refer to that material
    // Also name material after layer and give layers a generic name instead
    let layers = value["layers"].as_array_mut().ok_or(ModelError::InputConversionError(1, 2, "Entry 'layers' not found or invalid".to_string()))?;
    value["materials"] = layers.iter_mut()
        .enumerate()
        .map(|(index, layer)| {
            // Create new material entry
            let material = json!({
                "name": layer["name"],
                "color": get_material_color(
                    layer["rho"].as_f64().expect("Invalid material density"),
                    layer["E"].as_f64().expect("Invalid elastic modulus")
                ),
                "rho": layer["rho"],
                "E": layer["E"]
            });

            // Update layer entry
            let layer = layer.as_object_mut().expect("Layer must be an object");
            layer.insert("name".into(), json!(format!("Layer {}", index + 1)));
            layer.insert("material".into(), json!(index));
            layer.remove("rho");
            layer.remove("E");

            material
        })
        .collect();

    // Convert old profile definition (points of length and curvature) to line, arc and spiral segments
    let mut profile = value["profile"].as_array().expect("Invalid profile definition").clone();
    profile.sort_by(|a, b| {
        let a = a[0].as_f64().expect("Length must be a float");
        let b = b[0].as_f64().expect("Length must be a float");
        a.partial_cmp(&b).expect("Failed to compare floating point values")
    });

    value["profile"] = profile.iter().tuple_windows().map(|(prev, next)| {
        // Converts a curvature to a radius, where a radius of zero encodes zero curvature
        let curvature_to_radius = |kappa: f64| {
            if kappa != 0.0 { 1.0/kappa } else { 0.0 }
        };

        let length = next[0].as_f64().expect("Length must be a float") - prev[0].as_f64().expect("Length must be a float");
        let kappa0 = prev[1].as_f64().expect("Curvature must be a float");
        let kappa1 = next[1].as_f64().expect("Curvature must be a float");

        // If both curvatures are zero, create a line segment
        // If both curvatures are nonzero and equal, create an arc segment
        // If both curvatures are different, create a spiral segment
        if kappa0 == 0.0 && kappa1 == 0.0 {
            json!({
                "type": "line",
                "parameters": {
                    "length": length,
                }
            })
        }
        else if kappa0 == kappa1 {
            json!({
                "type": "arc",
                "parameters": {
                    "length": length,
                    "radius": curvature_to_radius(kappa0)
                }
            })
        }
        else {
            json!({
                "type": "spiral",
                "parameters": {
                    "length": length,
                    "r_start": curvature_to_radius(kappa0),
                    "r_end": curvature_to_radius(kappa1)
                }
            })
        }
    }).collect();

    Ok(())
}

fn convert_v0_to_v1(value: &mut Value) -> Result<(), ModelError> {
    value["version"] = json!(1);
    value["settings"]["arrow_clamp_force"] = json!(0.0);

    Ok(())
}

#[cfg(test)]
mod tests {
    use std::io::Error;
    use itertools::Itertools;
    use crate::bow::model::BowModel;

    #[test]
    fn test_model_conversion() -> Result<(), Error> {
        // This test loads supposedly equivalent bow files that have been saved in different format versions and checks whether they
        // can be loaded/saved successfully and if the resulting model data is equivalent.
        // See also the readme file in the referenced folder.

        // Iterate over all version folders, each folder contains a series of bow files that should produce the same model
        for entry in std::fs::read_dir("bows/versions")? {
            let path = entry?.path();
            if path.is_dir() {
                println!("Folder {:?}", path);

                // Iterate over model files in the folder and load each of them
                let models = std::fs::read_dir(&path)?
                    .map(|entry| entry.unwrap().path())
                    .filter(|path| path.is_file())
                    .filter(|path| path.extension().map(|s| s == "bow").unwrap())
                    .map(|file| {
                        println!("\t- Load {:?}", file.file_name().unwrap());
                        BowModel::load(file).expect("Failed to load model")
                    })
                    .collect::<Vec<BowModel>>();

                // Compare loaded models for equality
                models.iter().tuple_windows().for_each(|(a, b)| {
                    assert_eq!(a, b, "Model data must be equal");
                });

                // Save model file in the latest version
                let model = models.last().unwrap();
                let file = &path.join(format!("v{}.bow", BowModel::FILE_VERSION));
                println!("\t- Save {:?}", file.file_name().unwrap());
                model.save(file).unwrap();

                // Load it again and check for equality
                println!("\t- Load {:?}", file.file_name().unwrap());
                let loaded = BowModel::load(file).expect("Failed to load model");
                assert_eq!(loaded, *model, "Model data must be equal");
            }
        }

        Ok(())
    }
}