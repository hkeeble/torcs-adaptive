package hk.trackplotter;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.LayoutManager;
import java.awt.geom.Point2D;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;

import javax.swing.JFrame;
import javax.swing.JSplitPane;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartFrame;
import org.jfree.chart.JFreeChart;
import org.jfree.data.xy.DefaultXYDataset;

public class MainFrame extends Observer {

	private JFrame frame;
	
	private Surface drawSurface;
	private OutputPanel outputPanel;
	private UserPanel userPanel;
	
	private LayoutManager layout;
	
	private final String TRACK_FILE = "track.dat";
	private final String OPTIMAL_FILE = "optimal.dat";
	private final String ACTUAL_FILE = "actual.dat";
	
	private ArrayList<Point2D> distAxes;
	private PathPlot track, optimalLine, actualLine, distanceData;
	
	private double maxDist;
	
	JSplitPane splitPane;
	
	public MainFrame() {
		initUI();
		
		try {
			readData();
		} catch (Exception e) {
			outputPanel.send("Error reading and computing data: " + e.getMessage());
		}
		
		plotTrack();
		
		userPanel.addTable(track.getPoints(), "Track Plot");
		userPanel.addTable(optimalLine.getPoints(), "Optimal Plot");
		userPanel.addTable(actualLine.getPoints(), "Actual Plot");
		userPanel.addTable(distanceData.getPoints(), "Distance Plot");
	}
	
	private void initUI() {
		frame = new JFrame();
		frame.setTitle("Track Plotter");

		layout = new BorderLayout();
		frame.setLayout(layout);
		
		drawSurface = new Surface();
		outputPanel = new OutputPanel();
		userPanel = new UserPanel();

		userPanel.register(this); // Register the frame to observe the user panel, to update when we receive input
		
		splitPane = new JSplitPane();
		splitPane.add(drawSurface.get(), JSplitPane.RIGHT);
		splitPane.add(userPanel.get(), JSplitPane.LEFT);
		
		frame.getContentPane().add(splitPane, BorderLayout.CENTER);
		frame.getContentPane().add(outputPanel, BorderLayout.PAGE_END);
		
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setLocationRelativeTo(null);
		
		frame.pack();
		
		this.setVisible(true);
	}
	
	private void readData() throws Exception {
		FileReader fr;
		BufferedReader textReader;

		// Read track plot
		fr = new FileReader(TRACK_FILE);
		textReader = new BufferedReader(fr);
		ArrayList<Point2D> trackPlot = new ArrayList<Point2D>();
		outputPanel.send("Reading track...");
		String line;
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			trackPlot.add(new Point2D.Double(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
		}
		
		track = new PathPlot(trackPlot.toArray(new Point2D[trackPlot.size()])); // Create the plot object
		
		// Read actual plot
		fr = new FileReader(ACTUAL_FILE);
		textReader = new BufferedReader(fr);
		ArrayList<Point2D> actualPlot = new ArrayList<Point2D>();
		outputPanel.send("Reading actual path...");
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			actualPlot.add(new Point2D.Double(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
		}
		
		actualLine = new PathPlot(actualPlot.toArray(new Point2D[actualPlot.size()])); // Create the plot object
		
		// Read Optimal plot
		fr = new FileReader(OPTIMAL_FILE);
		textReader = new BufferedReader(fr);
		ArrayList<Point2D> optimalPlot = new ArrayList<Point2D>();
		outputPanel.send("Reading optimal path...");
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			optimalPlot.add(new Point2D.Double(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
		}
		
		optimalLine = new PathPlot(optimalPlot.toArray(new Point2D[optimalPlot.size()])); // Create the plot object
		
		outputPanel.send("Successfully read all data.");
		
		processData();
		
		outputPanel.send("Left-click and move the mouse to drag the plot around. Mouse wheel to zoom. Press mouse wheel to reset view.");
	}
	
	public void processData() {
		outputPanel.send("Processing data from plots.");
		
		maxDist = 0;
		
		// Compute distance plot
		outputPanel.send("Computing distance plot...");
		ArrayList<Point2D> distancePlot = new ArrayList<Point2D>();
		
		for(int i = 0; i < actualLine.getPointCount(); i++) {
			distancePlot.add(new Point2D.Double(i, optimalLine.getDistance(actualLine.getPoint(i))));
		}
		
		distanceData = new PathPlot(distancePlot.toArray(new Point2D[distancePlot.size()]));
		
		outputPanel.send("Distance plot completed.");
		
		// Create distance axes
		distAxes = new ArrayList<Point2D>();
		distAxes.add(new Point2D.Double(0, maxDist));
		distAxes.add(new Point2D.Double(0, 0));
		distAxes.add(new Point2D.Double(distancePlot.size(), 0));
	}
	
	public void setVisible(boolean vis) {
		frame.setVisible(vis);
	}
	
	public static void main(String[] args) {
		MainFrame mainFrame = new MainFrame();
	}
	
	public void plotTrack() {
		outputPanel.send("Plotting track.");
		
		drawSurface.clearPlots();
		drawSurface.addPlotData(track.getPath(), track.getPointCount(), Color.black);
		drawSurface.addPlotData(actualLine.getPath(), actualLine.getPointCount(), Color.red);
		drawSurface.addPlotData(optimalLine.getPath(), optimalLine.getPointCount(), Color.orange);
		drawSurface.repaint();
	}
	
	public void plotDistanceDifference() {
		DefaultXYDataset dSet = new DefaultXYDataset();
		
		// Create primitive array for distance
		double[][] dist = new double[2][distanceData.getPointCount()];
		int i = 0;
		for(Point2D p : distanceData.getPoints()) {
			dist[0][i] = p.getX();
			dist[1][i] = p.getY();
			i++;
		}
		
		dSet.addSeries("Distance Plot", dist);
		
		JFreeChart chart = ChartFactory.createXYLineChart("Distance to Optimal Path", "Distance Travelled", "Distance from Optimal Path (m)", dSet);
		ChartFrame frame = new ChartFrame("Distance from Optimal Path", chart);
		
		frame.pack();
		frame.setVisible(true);
	}
	
	public void notify(GUIMessage message) {
		if(message == GUIMessage.PLOT_DISTANCE_DIFF) {
			plotDistanceDifference();
		}
	}
}
