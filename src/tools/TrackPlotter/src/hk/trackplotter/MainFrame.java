package hk.trackplotter;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.LayoutManager;
import java.awt.geom.Point2D;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JSplitPane;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartFrame;
import org.jfree.chart.JFreeChart;
import org.jfree.data.xy.DefaultXYDataset;

public class MainFrame extends Observer {

	// Main frame
	private JFrame frame;
	
	// Main frame panels
	private Surface drawSurface;
	private OutputPanel outputPanel;
	private UserPanel userPanel;
	
	// The program menu
	private Menu menu;
	
	// Main layouyt manager
	private LayoutManager layout;
	
	// Strings for file reading
	private String CURRENT_DATA_DIR;
	private final String TRACK_FILE = "track.dat";
	private final String OPTIMAL_FILE = "optimal.dat";
	private final String ACTUAL_FILE = "actual.dat";
	private final String OPTIMAL_SPD_FILE = "optSpeed.dat";
	private final String ACTUAL_SPD_FILE = "actSpeed.dat";
	
	// String to represent infinite values
	private final String FLT_ERR = "1.#INF";
	
	// Plots and value sequences
	private PathPlot track, optimalLine, actualLine;
	private ValueSequence optimalSpeeds, actualSpeeds;
	
	// Processed data
	private double[][] distancePlot, speedDifferencePlot;
	
	// Split pane between surface and input panel
	private JSplitPane splitPane;
	
	// The current type of speed plot
	private boolean renderSpeed;
	private SpeedPlotType speedPlotType;
	
	// The previous directory used for loading data
	private String prevDir;
	
	// Represents whether or not some data has been successfully loaded.
	private boolean dataLoaded;
	
	public MainFrame() {
		
		// Initialize the previous directory loader
		prevDir = System.getProperty("user.dir");
		CURRENT_DATA_DIR = prevDir;
		
		dataLoaded = false;
		
		initUI();
		
		outputPanel.send("Left-click and move the mouse to drag the plot around. Mouse wheel to zoom. Press mouse wheel to reset view.");
	}
	
	private void initUI() {
		frame = new JFrame();
		frame.setTitle("TORCS Adaptive Track Plotter - V1.0");

		layout = new BorderLayout();
		frame.setLayout(layout);
		
		menu = new Menu();
		frame.setJMenuBar(menu.getBar());
		
		drawSurface = new Surface();
		outputPanel = new OutputPanel();
		userPanel = new UserPanel();

		userPanel.register(this); // Register the frame to observe the user panel, to update when we receive input
		menu.register(this);
		
		splitPane = new JSplitPane();
		splitPane.add(drawSurface.get(), JSplitPane.RIGHT);
		splitPane.add(userPanel.get(), JSplitPane.LEFT);
		
		frame.getContentPane().add(splitPane, BorderLayout.CENTER);
		frame.getContentPane().add(outputPanel, BorderLayout.PAGE_END);
		
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setLocationRelativeTo(null);
		frame.setExtendedState(frame.getExtendedState() | JFrame.MAXIMIZED_BOTH);
		
		frame.pack();
		
		this.setVisible(true);
	}
	
	private boolean initData() {
		// Clear all existing data;
		clearData();
		drawSurface.clearAll();
		
		if(readPlots() && processData()) {
			plotTrack();
			plotData();
			return true;
		}
		else {
			return false;
		}
	}
	
	private boolean readPlots() {
		boolean success = true;
		
		try {
			readTrack();
		} catch (Exception e) {
			outputPanel.send("Error reading track plot: " + e.getMessage());
			success = false;
		}
		
		try {
			readPaths();
		} catch (Exception e) {
			outputPanel.send("Error reading paths: " + e.getMessage());	
			success = false;
		}
		
		try {
			readSpeeds();	
		} catch (Exception e) {
			outputPanel.send("Error reading speeds: " + e.getMessage());
			success = false;
		}
		
		if(success)
			outputPanel.send("Successfully read all data.");
		else
			outputPanel.send("Errors reading data, some data may be incorrectly displayed.");
		
		return success;
	}
	
	private void readTrack() throws IOException, FileNotFoundException {
		// Read track plot
		FileReader fr = new FileReader(CURRENT_DATA_DIR + "/" + TRACK_FILE);
		BufferedReader textReader = new BufferedReader(fr);
		ArrayList<Point2D> trackPlot = new ArrayList<Point2D>();
		outputPanel.send("Reading track...");
		String line;
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			trackPlot.add(new Point2D.Double(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
		}

		textReader.close();
		fr.close();
		
		track = new PathPlot(trackPlot.toArray(new Point2D[trackPlot.size()]), Color.black); // Create the plot object
	}
	
	private void readPaths() throws IOException, FileNotFoundException  {
		// Read actual plot
		FileReader fr = new FileReader(CURRENT_DATA_DIR + "/" + ACTUAL_FILE);
		BufferedReader textReader = new BufferedReader(fr);
		ArrayList<Point2D> actualPlot = new ArrayList<Point2D>();
		outputPanel.send("Reading actual path...");
		String line;
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			actualPlot.add(new Point2D.Double(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
		}
		
		textReader.close();
		fr.close();
		
		actualLine = new PathPlot(actualPlot.toArray(new Point2D[actualPlot.size()]), Color.red); // Create the plot object
		
		// Read Optimal plot
		fr = new FileReader(CURRENT_DATA_DIR + "/" + OPTIMAL_FILE);
		textReader = new BufferedReader(fr);
		ArrayList<Point2D> optimalPlot = new ArrayList<Point2D>();
		outputPanel.send("Reading optimal path...");
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			optimalPlot.add(new Point2D.Double(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
		}
		
		textReader.close();
		fr.close();
		
		optimalLine = new PathPlot(optimalPlot.toArray(new Point2D[optimalPlot.size()]), Color.orange); // Create the plot object
	}
	
	private void readSpeeds()  throws IOException, FileNotFoundException {
		// Read Optimal Speed
		FileReader fr = new FileReader(CURRENT_DATA_DIR + "/" + OPTIMAL_SPD_FILE);
		BufferedReader textReader = new BufferedReader(fr);
		ArrayList<TextObject> optSpeedPlot = new ArrayList<TextObject>();
		float highest = 0.0f;
		int count = 0;
		String line;
		while((line = textReader.readLine()) != null) {
			optSpeedPlot.add(new TextObject(line, optimalLine.getPoint(count), Color.black));
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

		textReader.close();
		fr.close();
		
		// Read Actual Speed
		fr = new FileReader(CURRENT_DATA_DIR + "/" + ACTUAL_SPD_FILE);
		textReader = new BufferedReader(fr);
		ArrayList<TextObject> actSpeedPlot = new ArrayList<TextObject>();
		highest = 0.0f;
		count = 0;
		while((line = textReader.readLine()) != null) {
			actSpeedPlot.add(new TextObject(line, actualLine.getPoint(count), Color.black));
			count++;
		}

		// Convert arrays into value sequence objects
		optimalSpeeds = new ValueSequence(optSpeedPlot.toArray(new TextObject[optSpeedPlot.size()]), optSpeedPlot.size()/20, 1);
		actualSpeeds = new ValueSequence(actSpeedPlot.toArray(new TextObject[actSpeedPlot.size()]), actSpeedPlot.size()/20, 1);
		
		textReader.close();
		fr.close();
	}
	
	private boolean processData() {
		outputPanel.send("Processing data from plots.");
		
		boolean success = true;
		
		// Compute distance plot
		try {
			outputPanel.send("Computing distance plot...");
			distancePlot = new double[2][actualLine.getPointCount()];
			for(int i = 0; i < actualLine.getPointCount(); i++) {
				distancePlot[0][i] = i;
				distancePlot[1][i] = optimalLine.getDistance(actualLine.getPoint(i));
			}
			outputPanel.send("Distance plot completed.");
		} catch (Exception e) {
			outputPanel.send("Error computing distance plot: " + e.getMessage());
			success = false;
		}
		
		// Compute speed difference plot
		try {
			outputPanel.send("Computing speed difference plot...");
			speedDifferencePlot = new double[2][actualLine.getPointCount()];
			for(int i = 0; i < actualLine.getPointCount(); i++) {
				speedDifferencePlot[0][i] = i;

				String closestSpd = optimalSpeeds.getClosest(actualLine.getPoint(i)).getText();

				if(closestSpd.equals(FLT_ERR)) {
					closestSpd = "100";
				}
				speedDifferencePlot[1][i] = Double.parseDouble(closestSpd);
			}
			outputPanel.send("Speed difference plot completed.");
		} catch (Exception e) {
			outputPanel.send("Error computing speed difference plot: " + e.getMessage());
			success = false;
		}
		
		outputPanel.send("Successfully processed all data.");
		
		return success;
	}
	
	private void clearData() {
		track = null; 
		optimalLine = null;
		actualLine = null;
		optimalSpeeds = null;
		actualSpeeds = null;
		distancePlot = null;
		speedDifferencePlot = null;
	}
	
	public void setVisible(boolean vis) {
		frame.setVisible(vis);
	}
	
	public static void main(String[] args) {
		MainFrame mainFrame = new MainFrame();
	}
	
	private void plotTrack() {
		outputPanel.send("Plotting track.");
		
		drawSurface.clearPlots();
		drawSurface.addPlotData(track);
		drawSurface.addPlotData(actualLine);
		drawSurface.addPlotData(optimalLine);
		drawSurface.repaint();
		
		outputPanel.send(String.valueOf(Double.POSITIVE_INFINITY));
	}
	
	private void plotData() {
		drawSurface.addValueSequence(optimalSpeeds);
		
		renderSpeed = true;
		speedPlotType = SpeedPlotType.OPTIMAL;
		
		userPanel.setSliderValues(optimalSpeeds.getMaxRes(), optimalSpeeds.getMinRes());
	}
	
	private void plotXYGraph(double[][] data, String title, String xTag, String yTag) {
		DefaultXYDataset dSet = new DefaultXYDataset();
		dSet.addSeries(title, data);
		JFreeChart chart = ChartFactory.createXYLineChart(title, xTag, yTag, dSet);
		ChartFrame frame = new ChartFrame(title, chart);
		frame.pack();
		frame.setVisible(true);
	}
	
	public void notify(GUIMessage message, Object userValue) {
		
		// Plot a distance difference graph
		if(message == GUIMessage.PLOT_DISTANCE_DIFF) {
			plotXYGraph(distancePlot, "Distance to Optimal Path", "Distance Travelled", "Distance to Optimal Path");
		}
		
		// Handle toggle point render
		if(message == GUIMessage.TOGGLE_POINT_RENDER) {
			drawSurface.togglePointRender();
		}
		
		// Handle toggling of speed render
		if(message == GUIMessage.TOGGLE_SPEED_RENDER) {
			renderSpeed = !renderSpeed;
			
			if(speedPlotType == SpeedPlotType.ACTUAL) {
				if(renderSpeed) {
					drawSurface.addValueSequence(actualSpeeds);
				} else {
					drawSurface.removeValueSequence(actualSpeeds);
				}
			}
			else if (speedPlotType == SpeedPlotType.OPTIMAL) {
				if(renderSpeed) {
					drawSurface.addValueSequence(optimalSpeeds);
				} else {
					drawSurface.removeValueSequence(optimalSpeeds);
				}
			}
			
			drawSurface.repaint();
		}
		
		// Modify the speed display resolution
		if(message == GUIMessage.CHANGE_SPEED_RES) {
			if(speedPlotType == SpeedPlotType.OPTIMAL)
				optimalSpeeds.setResolution((int) userValue);
			else
				actualSpeeds.setResolution((int) userValue);
			
			drawSurface.repaint();
		}
		
		if(message == GUIMessage.CHANGE_SPD_RENDER) {
			SpeedPlotType t = (SpeedPlotType) userValue;

			if(speedPlotType != t) {
				speedPlotType = t;
				if(speedPlotType == SpeedPlotType.ACTUAL) {
					drawSurface.removeValueSequence(optimalSpeeds);
					drawSurface.addValueSequence(actualSpeeds);
					userPanel.setSliderValues(actualSpeeds.getMaxRes(), actualSpeeds.getMinRes());
				} else if(speedPlotType == SpeedPlotType.OPTIMAL) {
					drawSurface.removeValueSequence(actualSpeeds);
					drawSurface.addValueSequence(optimalSpeeds);
					userPanel.setSliderValues(optimalSpeeds.getMaxRes(), optimalSpeeds.getMinRes());
				}
			}
			
			drawSurface.repaint();
		}
		
		if(message == GUIMessage.PLOT_SPEED_DIFF) {
			plotXYGraph(speedDifferencePlot, "Difference between Actual and Optimal Speeds", "Distance Travelled", "Speed Difference");
		}
		
		if(message == GUIMessage.EXIT) {
			frame.setVisible(false);
			frame.dispose();
		}
		
		if(message == GUIMessage.LOAD_NEW_DATA) {
			JFileChooser fileChooser = new JFileChooser();
			fileChooser.setDialogTitle("Select Folder Containing Data");
			fileChooser.setApproveButtonText("Select Folder");
			fileChooser.setCurrentDirectory(new File(prevDir));
			fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
			int response = fileChooser.showOpenDialog(frame);
			
			if(response == JFileChooser.APPROVE_OPTION) {
				CURRENT_DATA_DIR = fileChooser.getSelectedFile().toString();
				outputPanel.send("Attempting to read data from: " + CURRENT_DATA_DIR);
				if(initData()) {
					dataLoaded = true;
					userPanel.setActive(true);
				} else {
					dataLoaded = false;
					userPanel.setActive(false);
				}
			}
			
			prevDir = fileChooser.getCurrentDirectory().toString();
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
