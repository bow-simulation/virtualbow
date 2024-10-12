use std::collections::HashMap;
use plotters::backend::BitMapBackend;
use plotters::chart::ChartBuilder;
use plotters::drawing::IntoDrawingArea;
use plotters::element::PathElement;
use plotters::series::LineSeries;
use plotters::style::WHITE;
use plotters::style::BLACK;
use plotters::style::BLUE;
use plotters::style::RED;
use itertools::chain;

// Utility for creating simple comparison plots for tests without having to aggregate the data manually.
// Create an instance and add points for various plots as needed. When the instance goes out of scope,
// the plot images are written to a directory named after the currently running test.
// Inspired by https://github.com/fabianboesiger/debug-plotter

pub struct Plotter {
    plots: HashMap<PlotInfo, PlotData>
}

impl Plotter {
    pub fn new() -> Self {
        Self {
            plots: HashMap::new()
        }
    }

    pub fn add_point(&mut self, point: (f64, f64), point_ref: (f64, f64), name: &str, x_label: &str, y_label: &str) {
        self.data_mut(name, x_label, y_label).add_point(point, point_ref);
    }

    pub fn add_points<I1, I2>(&mut self, points: I1, points_ref: I2, name: &str, x_label: &str, y_label: &str)
        where I1: IntoIterator<Item=(f64, f64)>, I2: IntoIterator<Item=(f64, f64)>
    {
        self.data_mut(name, x_label, y_label).add_points(points, points_ref);
    }

    fn data_mut(&mut self, name: &str, x_label: &str, y_label: &str) -> &mut PlotData {
        let info = PlotInfo {
            name: name.into(),
            x_label: x_label.into(),
            y_label: y_label.into()
        };

        self.plots.entry(info).or_insert_with(|| PlotData::default())
    }
}

impl Drop for Plotter {
    fn drop(&mut self) {
        /*
        // Determine output path from thread name, which corresponds to the test being run
        // https://users.rust-lang.org/t/reliably-getting-name-of-currently-running-test-with-rust-test-threads-1/65138
        let thread_name = std::thread::current().name()
            .expect("Failed to determine thread name")
            .to_string();
        let output_path = thread_name.replace("::", "/");

        // Create output directory, if it doesn't yet exist and remove any previous files
        std::fs::create_dir_all(&output_path).expect("Failed to create output directory");
        for entry in std::fs::read_dir(&output_path).unwrap() {
            if let Ok(entry) = entry {
                std::fs::remove_file(entry.path()).unwrap();
            }
        }

        self.plots.iter().for_each(|(info, data)| {
            create_plot(&output_path, &info, &data);
        });
        */
    }
}

#[derive(Hash, Eq, PartialEq)]
struct PlotInfo {
    name: String,
    x_label: String,
    y_label: String
}

#[derive(Default)]
struct PlotData {
    points: Vec<(f64, f64)>,
    points_ref: Vec<(f64, f64)>,
}

impl PlotData {
    fn add_point(&mut self, point: (f64, f64), point_ref: (f64, f64)) {
        self.points.push(point);
        self.points_ref.push(point_ref);
    }

    fn add_points<I1, I2>(&mut self, points: I1, points_ref: I2)
        where I1: IntoIterator<Item=(f64, f64)>, I2: IntoIterator<Item=(f64, f64)>
    {
        self.points.extend(points);
        self.points_ref.extend(points_ref);
    }

    fn x_min(&self) -> f64 {
        chain!(&self.points, &self.points_ref).map(|pt| pt.0).fold(f64::NAN, f64::min)
    }

    fn x_max(&self) -> f64 {
        chain!(&self.points, &self.points_ref).map(|pt| pt.0).fold(f64::NAN, f64::max)
    }

    fn y_min(&self) -> f64 {
        chain!(&self.points, &self.points_ref).map(|pt| pt.1).fold(f64::NAN, f64::min)
    }

    fn y_max(&self) -> f64 {
        chain!(&self.points, &self.points_ref).map(|pt| pt.1).fold(f64::NAN, f64::max)
    }
}

// Actually creates the plot file from the given info and data
// The output directory is determined from the name of the current thread, which is named after the test method
fn create_plot(output_path: &str, info: &PlotInfo, data: &PlotData) {
    let file_path = format!("{}/{}.png", output_path, info.name.to_lowercase().replace(" ", "_"));
    let root_area = BitMapBackend::new(&file_path, (1200, 800)).into_drawing_area();
    root_area.fill(&WHITE).unwrap();

    let mut ctx = ChartBuilder::on(&root_area)
        .margin(30)
        .x_label_area_size(30)
        .y_label_area_size(60)
        .caption(&info.name, 20)
        .build_cartesian_2d(data.x_min()..data.x_max(), data.y_min()..data.y_max())
        .unwrap();

    ctx.configure_mesh()
        .x_desc(&info.x_label)
        .y_desc(&info.y_label)
        .draw()
        .unwrap();

    ctx.draw_series(LineSeries::new(data.points.iter().copied(), &BLUE)).unwrap()
        .label("Actual")
        .legend(|(x, y)| PathElement::new(vec![(x, y), (x + 20, y)], &BLUE));

    ctx.draw_series(LineSeries::new(data.points_ref.iter().copied(), &RED)).unwrap()
        .label("Reference")
        .legend(|(x, y)| PathElement::new(vec![(x, y), (x + 20, y)], &RED));

    ctx.configure_series_labels()
        .border_style(&BLACK)
        .background_style(&WHITE)
        .draw()
        .unwrap();
}