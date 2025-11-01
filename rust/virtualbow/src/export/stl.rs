/*
use std::fs::File;
use std::io::{BufWriter, Write};
use std::path::Path;
use itertools::Itertools;
use nalgebra::vector;
use crate::export::{LayerExportInfo, Point};

struct StlSolid {
    name: String,
    triangles: Vec<[Point; 3]>,
}

struct StlData {
    solids: Vec<StlSolid>
}

impl StlData {
    fn save<P: AsRef<Path>>(&self, path: P) -> std::io::Result<()> {
        // Open the file in write mode (creates it if it doesn't exist)
        let file = File::create(path)?;
        let mut writer = BufWriter::new(file);

        for solid in &self.solids {
            // Begin new solid
            writeln!(writer, "solid {}", solid.name)?;

            // Write a facet for each triangle
            for [p1, p2, p3] in &solid.triangles {
                // Calculate facet normal
                let n = (p2 - p1).cross(&(p3 - p1)).normalize();

                // Write facet (points + normal)
                writeln!(writer, "  facet normal {} {} {}", n[0], n[1], n[2])?;
                writeln!(writer, "    outer loop")?;
                writeln!(writer, "      vertex {} {} {}", p1[0], p1[1], p1[2])?;
                writeln!(writer, "      vertex {} {} {}", p2[0], p2[1], p2[2])?;
                writeln!(writer, "      vertex {} {} {}", p3[0], p3[1], p3[2])?;
                writeln!(writer, "    endloop")?;
                writeln!(writer, "  endfacet")?;
            }

            // End solid
            writeln!(writer, "endsolid")?;
        }

        // File is automatically closed when `writer` goes out of scope
        Ok(())
    }
}

fn layer_to_stl(layer: &LayerExportInfo, path: &str) {
    let mut triangles = Vec::new();    // TODO: with_capacity()

    // Opening section
    let section = layer.sections.first().unwrap();
    triangles.push([section[0], section[1], section[3]]);
    triangles.push([section[0], section[3], section[2]]);

    // Between sections
    layer.sections.iter().tuple_windows().for_each(|(sec1, sec2)| {
        // Back side
        triangles.push([sec1[0], sec2[1], sec1[1]]);
        triangles.push([sec1[0], sec2[0], sec2[1]]);

        // Belly side
        triangles.push([sec1[2], sec1[3], sec2[3]]);
        triangles.push([sec1[2], sec2[3], sec2[2]]);

        // Left side
        triangles.push([sec1[0], sec2[2], sec2[0]]);
        triangles.push([sec1[0], sec1[2], sec2[2]]);

        // Right side
        triangles.push([sec1[1], sec2[1], sec2[3]]);
        triangles.push([sec1[1], sec2[3], sec1[3]]);
    });

    // Closing section
    let section = layer.sections.last().unwrap();
    triangles.push([section[0], section[3], section[1]]);
    triangles.push([section[0], section[2], section[3]]);

    let solid = StlSolid {
        name: layer.name.clone(),
        triangles
    };

    let data = StlData {
        solids: vec![solid],
    };

    data.save(path).unwrap();
}

#[cfg(test)]
mod tests {
    use crate::export::{LayerExportInfo, LimbExportInfo};
    use crate::export::stl::layer_to_stl;
    use crate::geometry::LimbGeometry;
    use crate::input::BowModel;

    #[test]
    fn test_single_layer() {
        let model = BowModel::example();
        let geometry = LimbGeometry::new(&model).unwrap();
        let export = LimbExportInfo::new(&geometry);

        layer_to_stl(&export.layers[0], "layer.stl");
    }
}
*/