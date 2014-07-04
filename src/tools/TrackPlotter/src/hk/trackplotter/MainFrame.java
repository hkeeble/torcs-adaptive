package hk.trackplotter;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.LayoutManager;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;

import javax.swing.JFrame;
import javax.swing.JSplitPane;

public class MainFrame extends Observer {

	private JFrame frame;
	
	private Surface drawSurface;
	private OutputPanel outputPanel;
	private UserPanel userPanel;
	
	private LayoutManager layout;
	
	private final String TRACK_FILE = "track.dat";
	private final String OPTIMAL_FILE = "optimal.dat";
	private final String ACTUAL_FILE = "actual.dat";
	
	private ArrayList<Point2D> trackPlot, optimalPlot, actualPlot, distancePlot;
	private ArrayList<Point2D> distAxes;
	
	private double maxDist;
	
	JSplitPane splitPane;
	
	PlotType plotType;
	
	public MainFrame() {
		initUI();
		
		plotType = PlotType.NONE;
		
		try {
			readData();
		} catch (Exception e) {
			outputPanel.send("Error reading and computing data: " + e.getMessage());
		}
		
		plotTrack();
		
		userPanel.addTable(trackPlot, "Track Plot");
		userPanel.addTable(optimalPlot, "Optimal Plot");
		userPanel.addTable(actualPlot, "Actual Plot");
		userPanel.addTable(distancePlot, "Distance Plot");
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
	}
	
	private void readData() throws Exception {
		FileReader fr;
		BufferedReader textReader;

		// Read track plot
		fr = new FileReader(TRACK_FILE);
		textReader = new BufferedReader(fr);
		trackPlot = new ArrayList<Point2D>();
		outputPanel.send("Plotting track...");
		String line;
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			trackPlot.add(new Point2D.Double(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
		}
		
		// Read actual plot
		fr = new FileReader(ACTUAL_FILE);
		textReader = new BufferedReader(fr);
		actualPlot = new ArrayList<Point2D>();
		outputPanel.send("Plotting actual path...");
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			actualPlot.add(new Point2D.Double(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
			outputPanel.send(" - " + String.valueOf(trackPlot.get(trackPlot.size()-1).getX() + ", " + trackPlot.get(trackPlot.size()-1).getY()));
		}
		
		// Read Optimal plot
		fr = new FileReader(OPTIMAL_FILE);
		textReader = new BufferedReader(fr);
		optimalPlot = new ArrayList<Point2D>();
		outputPanel.send("Plotting actual path...");
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			optimalPlot.add(new Point2D.Double(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
			outputPanel.send(" - " + String.valueOf(trackPlot.get(trackPlot.size()-1).getX() + ", " + trackPlot.get(trackPlot.size()-1).getY()));
		}
		
//		for(int i = 0; i < 200; i++) {
//			optimalPlot.add(new Point2D.Double(i, i));
//			actualPlot.add(new Point2D.Double(i, 0));
//		}
		
		
		processData();
		
		outputPanel.send("Successfully read all data.");
		outputPanel.send("Left-click and move the mouse to drag the plot around. Mouse wheel to zoom. Press mouse wheel to reset view.");
	}
	
	public void processData() {
		outputPanel.send("Processing data from plots.");
		
		maxDist = 0;
		
		// Compute distance plot
		distancePlot = new ArrayList<Point2D>();
		
		for(int i = 0; i < actualPlot.size()-1; i++) {
			Point2D act = actualPlot.get(i);
			Point2D opt = optimalPlot.get(i);
			
			opt.distance(act);

			// Find distance to this line, and add to plot
			distancePlot.add(new Point2D.Double(i, opt.distance(act))); //line.ptLineDist(new Point2D.Double(act.getX(), act.getY()))));
			outputPanel.send(String.valueOf(distancePlot.get(i).getY()));
			
			if(distancePlot.get(i).getX() > maxDist)
				maxDist = distancePlot.get(i).getY();
		}
		
		// Create distance axes
		distAxes = new ArrayList<Point2D>();
		distAxes.add(new Point2D.Double(0, maxDist));
		distAxes.add(new Point2D.Double(0, 0));
		distAxes.add(new Point2D.Double(distancePlot.size(), 0));
	}
	
	/**
	 * Finds the index in the given array of the closest point to the given point.
	 * @param points The set of points to search.
	 * @param point The point to search for the closest point to.
	 */
	private int closestPoint(ArrayList<Point2D> points, Point2D point) {
		int closest = 0;
		
		Point2D opt = optimalPlot.get(0);
		double prevDist = opt.distance(point);
			
		for(int i = 1; i < optimalPlot.size(); i++) {
			opt = optimalPlot.get(i);
			double currentDist = opt.distance(point);
					
			if(currentDist > prevDist) {
				closest = i-1;
				outputPanel.send(String.valueOf(closest));
				break;
			}

			prevDist = currentDist;
		}
		
		return closest;
	}
	
	public void setVisible(boolean vis) {
		frame.setVisible(vis);
	}
	
	public static void main(String[] args) {
		MainFrame mainFrame = new MainFrame();
		mainFrame.setVisible(true);
	}
	
	public void plotTrack() {
		if(plotType != PlotType.TRACK) {
			outputPanel.send("Plotting track.");
			plotType = PlotType.TRACK;
			
			drawSurface.clearPlots();
			drawSurface.addPlotData(trackPlot, Color.black);
			drawSurface.addPlotData(actualPlot, Color.red);
			drawSurface.addPlotData(optimalPlot, Color.orange);
			drawSurface.repaint();
		} else {
			outputPanel.send("Already plotted this!");
		}
	}
	
	public void plotDistanceDifference() {
		if(plotType != PlotType.DIST_DIFF) {
			outputPanel.send("Plotting distance difference.");
			plotType = PlotType.DIST_DIFF;
			
			drawSurface.clearPlots();
			drawSurface.addPlotData(distancePlot, Color.red);
			drawSurface.addPlotData(distAxes, Color.black);
			
			drawSurface.addTextElement(new Point2D.Double(-10, distAxes.get(1).getX() + (maxDist/2)), "Distance", Color.black);
			drawSurface.addTextElement(new Point2D.Double(distAxes.get(1).getX() + (distancePlot.size()/2), -10), "Time", Color.black);
			
			drawSurface.repaint();
		}
		else {
			outputPanel.send("Already plotted this!");
		}
	}
	
	public void notify(GUIMessage message) {
		if(message == GUIMessage.PLOT_DISTANCE_DIFF) {
			plotDistanceDifference();
		}
		else if(message == GUIMessage.PLOT_TRACK) {
			plotTrack();
		}
	}
}
