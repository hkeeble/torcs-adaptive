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
	private final String OPTIMAL_SPD_FILE = "optSpeed.dat";
	
	private final String FLT_ERR = "1.#INF";
	
	private final int SPEED_RESOLUTION = 50;
	
	private ArrayList<Point2D> distAxes;
	private PathPlot track, optimalLine, actualLine, distanceData;
	private ValueSequence optimalSpeeds;
	
	private double maxDist;
	
	JSplitPane splitPane;
	
	public MainFrame() {
		initUI();
		
		try {
			readData();
		} catch (Exception e) {
			outputPanel.send("Error reading data: " + e.getMessage());
		}
		
		try {
			processData();
		} catch(Exception e) {
			outputPanel.send("Error processing data: " + e.getMessage());
		}
		
		plotTrack();
		plotData();
		
		outputPanel.send("Left-click and move the mouse to drag the plot around. Mouse wheel to zoom. Press mouse wheel to reset view.");
		
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
		
		track = new PathPlot(trackPlot.toArray(new Point2D[trackPlot.size()]), Color.black); // Create the plot object
		
		// Read actual plot
		fr = new FileReader(ACTUAL_FILE);
		textReader = new BufferedReader(fr);
		ArrayList<Point2D> actualPlot = new ArrayList<Point2D>();
		outputPanel.send("Reading actual path...");
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			actualPlot.add(new Point2D.Double(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
		}
		
		actualLine = new PathPlot(actualPlot.toArray(new Point2D[actualPlot.size()]), Color.red); // Create the plot object
		
		// Read Optimal plot
		fr = new FileReader(OPTIMAL_FILE);
		textReader = new BufferedReader(fr);
		ArrayList<Point2D> optimalPlot = new ArrayList<Point2D>();
		outputPanel.send("Reading optimal path...");
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			optimalPlot.add(new Point2D.Double(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
		}
		
		optimalLine = new PathPlot(optimalPlot.toArray(new Point2D[optimalPlot.size()]), Color.orange); // Create the plot object
		
		// Read Optimal Speed
		fr = new FileReader(OPTIMAL_SPD_FILE);
		textReader = new BufferedReader(fr);
		ArrayList<TextObject> optSpeedPlot = new ArrayList<TextObject>();
		float highest = 0.0f;
		int count = 0;
		while((line = textReader.readLine()) != null) {
			optSpeedPlot.add(new TextObject(line, optimalPlot.get(count), Color.black));
			count++;
			
			if(!line.equals(FLT_ERR)) {
				float current = Float.parseFloat(line);
				if(current > highest) {
					highest = current;
				}
			}
		}
		
		// Set color values based on interpolation
		for(TextObject t : optSpeedPlot) {
			if(!t.getText().equals(FLT_ERR)) {
				float val = Float.parseFloat(t.getText());
				float weight = val/highest;
				t.setColor(interpolate(Color.green, Color.red, weight));
			}
		}
		
		// Convert into a value sequence object
		optimalSpeeds = new ValueSequence(optSpeedPlot.toArray(new TextObject[optSpeedPlot.size()]), optSpeedPlot.size()/20, 1);
		
		textReader.close();
		fr.close();
		
		outputPanel.send("Successfully read all data.");
	}
	
	public void processData() throws Exception {
		outputPanel.send("Processing data from plots.");
		
		maxDist = 0;
		
		// Compute distance plot
		outputPanel.send("Computing distance plot...");
		ArrayList<Point2D> distancePlot = new ArrayList<Point2D>();
		
		for(int i = 0; i < actualLine.getPointCount(); i++) {
			distancePlot.add(new Point2D.Double(i, optimalLine.getDistance(actualLine.getPoint(i))));
		}
		
		distanceData = new PathPlot(distancePlot.toArray(new Point2D[distancePlot.size()]), Color.red);
		
		outputPanel.send("Distance plot completed.");
		
		// Create distance axes
		distAxes = new ArrayList<Point2D>();
		distAxes.add(new Point2D.Double(0, maxDist));
		distAxes.add(new Point2D.Double(0, 0));
		distAxes.add(new Point2D.Double(distancePlot.size(), 0));
		
		outputPanel.send("Successfully processed all data.");
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
		drawSurface.addPlotData(track);
		drawSurface.addPlotData(actualLine);
		drawSurface.addPlotData(optimalLine);
		drawSurface.repaint();
	}
	
	public void plotData() {
		drawSurface.addValueSequence(optimalSpeeds);
		userPanel.setSliderValues(optimalSpeeds.getMaxRes(), optimalSpeeds.getMinRes());
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
	
	public void notify(GUIMessage message, Object userValue) {
		if(message == GUIMessage.PLOT_DISTANCE_DIFF) {
			plotDistanceDifference();
		}
		if(message == GUIMessage.TOGGLE_POINT_RENDER) {
			drawSurface.togglePointRender();
		}
		if(message == GUIMessage.TOGGLE_SPEED_RENDER) {
			if(drawSurface.containsValueSequence(optimalSpeeds))
				drawSurface.removeValueSequence(optimalSpeeds);
			else
				drawSurface.addValueSequence(optimalSpeeds);
		}
		if(message == GUIMessage.CHANGE_SPEED_RES) {
			optimalSpeeds.setResolution((int) userValue);
			drawSurface.repaint();
		}
	}
	
	public Color interpolate(Color a, Color b, float weight) {
		float[] aRGB = new float[3];
		float[] bRGB = new float[3];
		
		aRGB = a.getRGBColorComponents(aRGB);
		bRGB = b.getRGBColorComponents(bRGB);
		
		return new Color(interpolate(aRGB[0], bRGB[0], weight), interpolate(aRGB[1], bRGB[1], weight), interpolate(aRGB[2], bRGB[2], weight));
	}
	
	public float interpolate(float a, float b, float weight) {
		return a + weight * (b - a);
	}
}
