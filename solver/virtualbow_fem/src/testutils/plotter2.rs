use indexmap::IndexMap;

// Utility for creating simple comparison plots for tests without having to aggregate the data manually.
// Create an instance and add points for various plots as needed. When the instance goes out of scope,
// the plot images are written to the target directory, named after the currently running test.
// Inspired by https://github.com/fabianboesiger/debug-plotter

// TODO: Replace plotter module with plotter2

pub struct Plotter {
    plots: IndexMap<String, PlotData>    // IndexMap preserves order of insertion
}

impl Plotter {
    pub fn new() -> Self {
        Self {
            plots: Default::default()
        }
    }

    pub fn add_point(&mut self, title: &str, xlabel: &str, ylabel: &str, series: &str, color: usize, point: (f64, f64)) {
        self.data_mut(title, xlabel, ylabel, series, color).add_point(point);
    }

    pub fn add_points<I>(&mut self, title: &str, xlabel: &str, ylabel: &str, series: &str, color: usize, points: I)
        where I: IntoIterator<Item=(f64, f64)>
    {
        self.data_mut(title, xlabel, ylabel, series, color).add_points(points);
    }

    fn data_mut(&mut self, title: &str, xlabel: &str, ylabel: &str, series: &str, color: usize) -> &mut SeriesData {
        let plot = self.plots.entry(title.into()).or_insert(PlotData {
            series: Default::default(),
            xlabel: xlabel.into(),
            ylabel: ylabel.into(),
        });

        plot.series.entry(series.into()).or_insert(SeriesData {
            points: Vec::new(),
            color,
        })
    }
}

impl Drop for Plotter {
    #[cfg(feature = "plotters")]
    fn drop(&mut self) {
        // Determine output path from thread name, which corresponds to the test being run
        // https://users.rust-lang.org/t/reliably-getting-name-of-currently-running-test-with-rust-test-threads-1/65138
        // https://github.com/rust-lang/cargo/issues/9661
        let thread_name = std::thread::current().name()
            .expect("Failed to determine thread name")
            .to_string();
        let output_path = "target/".to_string() + &thread_name.replace("::", "/");

        // Create output directory, if it doesn't yet exist and remove any previous files
        std::fs::create_dir_all(&output_path).expect("Failed to create output directory");
        for entry in std::fs::read_dir(&output_path).unwrap().flatten() {
            std::fs::remove_file(entry.path()).unwrap();
        }

        self.plots.iter().for_each(|(info, data)| {
            create_plot(&output_path, info, data);
        });
    }

    #[cfg(not(feature = "plotters"))]
    fn drop(&mut self) {
        // Do nothing if the plotters dependency is not enabled
    }
}

#[allow(dead_code)]
struct PlotData {
    series: IndexMap<String, SeriesData>,    // IndexMap preserves order of insertion
    xlabel: String,
    ylabel: String,
}

#[allow(dead_code)]
struct SeriesData {
    points: Vec<(f64, f64)>,
    color: usize
}

impl SeriesData {
    fn add_point(&mut self, point: (f64, f64)) {
        self.points.push(point);
    }

    fn add_points<I>(&mut self, points: I)
        where I: IntoIterator<Item=(f64, f64)>
    {
        self.points.extend(points);
    }

    fn x_min(&self) -> f64 {
        self.points.iter().map(|p| p.0).fold(f64::NAN, f64::min)
    }

    fn x_max(&self) -> f64 {
        self.points.iter().map(|p| p.0).fold(f64::NAN, f64::max)
    }

    fn y_min(&self) -> f64 {
        self.points.iter().map(|p| p.1).fold(f64::NAN, f64::min)
    }

    fn y_max(&self) -> f64 {
        self.points.iter().map(|p| p.1).fold(f64::NAN, f64::max)
    }
}

#[allow(dead_code)]
impl PlotData {
    fn x_min(&self) -> f64 {
        self.series.values().map(|s| s.x_min()).fold(f64::NAN, f64::min)
    }

    fn x_max(&self) -> f64 {
        self.series.values().map(|s| s.x_max()).fold(f64::NAN, f64::max)
    }

    fn y_min(&self) -> f64 {
        self.series.values().map(|s| s.y_min()).fold(f64::NAN, f64::min)
    }

    fn y_max(&self) -> f64 {
        self.series.values().map(|s| s.y_max()).fold(f64::NAN, f64::max)
    }
}

// Actually creates the plot file from the given info and data (only if the optional "plotters" dependency is enabled)
// The output directory is determined from the name of the current thread, which is named after the test method
#[cfg(feature = "plotters")]
fn create_plot(output_path: &str, title: &str, data: &PlotData) {
    use plotters::backend::BitMapBackend;
    use plotters::chart::ChartBuilder;
    use plotters::drawing::IntoDrawingArea;
    use plotters::series::LineSeries;
    use plotters::element::PathElement;
    use plotters::style::{BLACK, WHITE, BLUE, RED};

    let file_path = format!("{}/{}.png", output_path, title.to_lowercase().replace(" ", "_"));
    let root_area = BitMapBackend::new(&file_path, (1200, 800)).into_drawing_area();
    root_area.fill(&WHITE).unwrap();

    let color_from_index = |index| {
        match index {
            0 => BLUE,
            1 => RED,
            _ => panic!("Color does not exist")
        }
    };

    let mut ctx = ChartBuilder::on(&root_area)
        .margin(30)
        .x_label_area_size(30)
        .y_label_area_size(60)
        .caption(&title, 20)
        .build_cartesian_2d(data.x_min()..data.x_max(), data.y_min()..data.y_max())
        .unwrap();

    ctx.configure_mesh()
        .x_desc(&data.xlabel)
        .y_desc(&data.ylabel)
        .draw()
        .unwrap();

    for (name, series) in &data.series {
        let color = color_from_index(series.color);
        ctx.draw_series(LineSeries::new(series.points.iter().copied(), color)).unwrap()
            .label(name)
            .legend(move |(x, y)| PathElement::new(vec![(x, y), (x + 20, y)], color));
    }

    ctx.configure_series_labels()
        .border_style(BLACK)
        .background_style(WHITE)
        .draw()
        .unwrap();
}