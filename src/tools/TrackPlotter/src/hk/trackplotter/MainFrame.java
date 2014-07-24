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
import java.text.DecimalFormat;
import java.util.ArrayList;

import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JSplitPane;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartFrame;
import org.jfree.chart.JFreeChart;
import org.jfree.data.Range;
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
	private final String PERF_DATA_FILE = "perfData.dat";
	private final String TRACK_CURVATURE_FILE = "curvature.dat";
	private final String SPEED_RATING_FILE = "spdRatings.dat";
	private final String TRAJECTORY_RATING_FILE = "trajRatings.dat";
	
	// String to represent infinite values
	private final String FLT_ERR = "1.#INF";
	
	// Main surface plots and value sequences
	private PathPlot track, optimalLine, actualLine;
	private ValueSequence optimalSpeeds, actualSpeeds, skillLevels;
	
	// Processed plot data
	private double[][] distancePlot, speedDifferencePlot, optimalSpeedPlot, actualSpeedPlot, skillLevelPlot,
							curvaturePlot, speedRatingPlot, trajectoryRatingPlot;
	
	// Averaged data
	double averageDistance, averageSpeed, optimalAverageSpeed, averageSkillLevel, averageCurvature, averageSpeedRating, averageTrajectoryRating;
	
	// Split pane between surface and input panel
	private JSplitPane splitPane;
	
	// The current type of speed plot
	private boolean renderSpeed, renderSkill;
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
		
		try {
			readSkillLevels();
		} catch(Exception e) {
			outputPanel.send("Error reading performance measurement data: " + e.getMessage());
			success = false;
		}
		
		try {
			readCurvature();
		} catch(Exception e) {
			outputPanel.send("Error reading curvature data: " + e.getMessage());
			success = false;
		}
		
		try {
			readRatings();
		} catch(Exception e) {
			outputPanel.send("Error reading rating data: " + e.getMessage());
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
	
	private void readSkillLevels() throws IOException, FileNotFoundException {
		FileReader fr = new FileReader(CURRENT_DATA_DIR + "/" + PERF_DATA_FILE);
		BufferedReader textReader = new BufferedReader(fr);
		ArrayList<TextObject> skillPlot = new ArrayList<TextObject>();
		String line;
		while((line = textReader.readLine()) != null) {
			skillPlot.add(new TextObject(line, new Point2D.Double(0, 0), Color.black));
		}

		double div = Math.floor(actualLine.getPointCount()/skillPlot.size());
		double total = 0;
		for(int i = 0, j = 0; i < skillPlot.size(); i++, j+=div) {
			skillPlot.get(i).setPosition(actualLine.getPoint(j));
			total += Double.parseDouble(skillPlot.get(i).getText());
		}
		averageSkillLevel = total/skillPlot.size();
		
		skillLevels = new ValueSequence(skillPlot.toArray(new TextObject[skillPlot.size()]));
		
		fr.close();
		textReader.close();
	}
	
	private void readCurvature() throws IOException, FileNotFoundException {
		FileReader fr = new FileReader(CURRENT_DATA_DIR + "/" + TRACK_CURVATURE_FILE);
		BufferedReader textReader = new BufferedReader(fr);
		
		ArrayList<Double> points = new ArrayList<Double>();
		String line;
		double total = 0;
		while((line = textReader.readLine()) != null) {
			points.add(Math.abs(Double.parseDouble(line)));
			total += points.get(points.size()-1);
		}
		
		averageCurvature = total/points.size();
		curvaturePlot = toPlotArray(points);
		
		fr.close();
		textReader.close();
	}
	
	private void readRatings() throws IOException, FileNotFoundException {
		FileReader fr = new FileReader(CURRENT_DATA_DIR + "/" + SPEED_RATING_FILE);
		BufferedReader textReader = new BufferedReader(fr);
		
		ArrayList<Double> ratings = new ArrayList<Double>();
		String line;
		double total = 0;
		while((line = textReader.readLine()) != null) {
			ratings.add(Double.parseDouble(line));
			total += ratings.get(ratings.size()-1);
		}
		
		averageSpeedRating = total/ratings.size();
		speedRatingPlot = toPlotArray(ratings);
		
		fr = new FileReader(CURRENT_DATA_DIR + "/" + TRAJECTORY_RATING_FILE);
		textReader = new BufferedReader(fr);
		ratings.clear();
		total = 0;
		while((line = textReader.readLine()) != null) {
			ratings.add(Double.parseDouble(line));
			total += ratings.get(ratings.size()-1);
		}
		
		averageTrajectoryRating = total/ratings.size();
		trajectoryRatingPlot = toPlotArray(ratings);
		
		fr.close();
		textReader.close();
	}
	
	private double[][] toPlotArray(ArrayList<Double> data) {
		double[][] plot = new double[2][data.size()];
		for(int i = 0; i < data.size(); i++) {
			plot[0][i] = i;
			plot[1][i] = data.get(i);
		}
		return plot;
	}
	
	private boolean processData() {
		outputPanel.send("Processing data from plots.");
		
		boolean success = true;
		
		// Compute distance plot
		double total = 0; // Used to calculate the averages
		try {
			outputPanel.send("Computing distance plot...");
			distancePlot = new double[2][actualLine.getPointCount()];
			for(int i = 0; i < actualLine.getPointCount(); i++) {
				distancePlot[0][i] = i;
				distancePlot[1][i] = optimalLine.getDistance(actualLine.getPoint(i));
				total += optimalLine.getDistance(actualLine.getPoint(i));
			}
			outputPanel.send("Distance plot completed.");
			averageDistance = total/actualLine.getPointCount();
		} catch (Exception e) {
			outputPanel.send("Error computing distance plot: " + e.getMessage());
			success = false;
		}
		
		// Compute speed difference plot
		try {
			outputPanel.send("Computing speed difference plot...");
			speedDifferencePlot = new double[2][actualLine.getPointCount()];
			total = 0;
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
		
		try {
			outputPanel.send("Formatting optimal/actual speeds for graph plotting.");

			// Form native arrays for optimal/actual speed plots
			actualSpeedPlot = new double[2][actualSpeeds.size()];
			optimalSpeedPlot = new double[2][optimalSpeeds.size()];

			double totalSpeed = 0, totalOptSpeed = 0;
			for(int i = 0; i < actualSpeeds.size(); i++) {
				actualSpeedPlot[0][i] = i;
				actualSpeedPlot[1][i] = actualSpeeds.getValueAt(i);
				totalSpeed += actualSpeedPlot[1][i];
				totalOptSpeed += Double.parseDouble(optimalSpeeds.getClosest(actualSpeeds.getPositionOf(i)).getText());
			}
			averageSpeed = totalSpeed/actualSpeeds.size();
			optimalAverageSpeed = totalOptSpeed/actualSpeeds.size();

			for(int i = 0; i < optimalSpeeds.size(); i++) {
				optimalSpeedPlot[0][i] = i;
				optimalSpeedPlot[1][i] = optimalSpeeds.getValueAt(i);
			}

			outputPanel.send("Optimal/actual speeds formatted correctly.");
		} catch (Exception e) {
			outputPanel.send("Error formatting optimal/actual speeds: " + e.getMessage());
			success = false;
		}
		
		try {
			skillLevelPlot = new double[2][skillLevels.size()];	
			for(int i = 0; i < skillLevels.size(); i++) {
				skillLevelPlot[0][i] = i;
				skillLevelPlot[1][i] = skillLevels.getValueAt(i);
			}
		} catch (Exception e) {
			outputPanel.send("Error formatting skill data for plotting: " + e.getMessage());
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
	}
	
	private void plotData() {
		drawSurface.addValueSequence(optimalSpeeds);
		
		renderSpeed = true;
		speedPlotType = SpeedPlotType.OPTIMAL;
		
		userPanel.setSliderValues(optimalSpeeds.getMaxRes(), optimalSpeeds.getMinRes());
	}
	
	private ChartFrame plotXYGraph(double[][] data, String title, String xTag, String yTag) {
		DefaultXYDataset dSet = new DefaultXYDataset();
		dSet.addSeries(title, data);
		JFreeChart chart = ChartFactory.createXYLineChart(title, xTag, yTag, dSet);
		ChartFrame frame = new ChartFrame(title, chart);
		frame.pack();
		return frame;
	}
	
	private ChartFrame plotXYGraph(double[][] dataA, double[][] dataB, String seriesAName, String seriesBName, String title, String xTag, String yTag) {
		DefaultXYDataset dSet = new DefaultXYDataset();
		dSet.addSeries(seriesAName, dataA);
		dSet.addSeries(seriesBName, dataB);
		JFreeChart chart = ChartFactory.createXYLineChart(title, xTag, yTag, dSet);
		ChartFrame frame = new ChartFrame(title, chart);
		frame.pack();
		return frame;
	}
	
	public void notify(GUIMessage message, Object userValue) {
		
		// Plot a distance difference graph
		if(message == GUIMessage.PLOT_DISTANCE_DIFF) {
			plotXYGraph(distancePlot, "Distance to Optimal Path", "Distance Travelled", "Distance to Optimal Path").setVisible(true);
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
		
		// Handle toggle of skill level rendering
		if(message == GUIMessage.TOGGLE_SKILL_RENDER) {
			renderSkill = !renderSkill;
			
			if(renderSkill) {
				drawSurface.addValueSequence(skillLevels);
			}
			else {
				drawSurface.removeValueSequence(skillLevels);
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
			plotXYGraph(speedDifferencePlot, "Difference between Actual and Optimal Speeds", "Distance Travelled", "Speed Difference").setVisible(true);
		}
		
		if(message == GUIMessage.PLOT_SPEED_COMPARISON) {
			plotXYGraph(actualSpeedPlot, optimalSpeedPlot,"Actual Speed", "Optimal Speed", "Optimal and Actual Speed", "Distance Travelled", "Speed").setVisible(true);
		}
		
		if(message == GUIMessage.PLOT_SKILL_LEVEL) {
			ChartFrame chart = plotXYGraph(skillLevelPlot, "Skill Level Plot", "Evaluation Index", "Performance Value");
			chart.getChartPanel().getChart().getXYPlot().getRangeAxis().setRange(new Range(0, 1));
			chart.setVisible(true);
		}
		
		if(message == GUIMessage.PLOT_TRACK_CURVATURE) {
			ChartFrame chart = plotXYGraph(curvaturePlot, "Track Curvature", "Distance", "Curvature");
			chart.setVisible(true);
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
		
		if(message == GUIMessage.SHOW_RACE_SUMMARY) {
			DecimalFormat d1 = new DecimalFormat("#.##");
			DecimalFormat d2 = new DecimalFormat("#.####");
			
			JOptionPane.showMessageDialog(frame, "Average Speed: " + 				String.valueOf(d1.format(averageSpeed)) + "\n" +
												  "Optimal Average Speed: " +	    String.valueOf(d1.format(optimalAverageSpeed)) + "\n" +
												  "Average Distance from path: " +  String.valueOf(d1.format(averageDistance)) + "\n" +
												  "Average Skill Level: " +	 		String.valueOf(d1.format(averageSkillLevel)) + "\n" +
												  "Average Track Curvature: " + 	String.valueOf(d2.format(averageCurvature)), "Race Summary", JOptionPane.INFORMATION_MESSAGE);
		}
		
		if(message == GUIMessage.PLOT_SPEED_RATINGS) {
			plotXYGraph(speedRatingPlot, "Speed Ratings", "Distance Travelled", "Speed Rating").setVisible(true);
		}
		
		if(message == GUIMessage.PLOT_TRAJECTORY_RATINGS) {
			plotXYGraph(trajectoryRatingPlot, "Trajectory Ratings", "Distance Travelled", "Trajectory Rating").setVisible(true);
		}

		if(message == GUIMessage.PLOT_RATING_COMPARISON) {
			plotXYGraph(speedRatingPlot, trajectoryRatingPlot, "Speed Rating", "Trajectory Rating", "Rating Comparison", "Distance Travelled", "Rating").setVisible(true);
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
