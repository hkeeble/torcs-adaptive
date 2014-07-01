package hk.trackplotter;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.LayoutManager;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSplitPane;
import javax.swing.JTable;

public class MainFrame extends Observer {

	private JFrame frame;
	
	private Surface drawSurface;
	private OutputPanel outputPanel;
	private UserPanel userPanel;
	
	private LayoutManager layout;
	
	private final String TRACK_FILE = "track.dat";
	private final String OPTIMAL_FILE = "optimal.dat";
	private final String ACTUAL_FILE = "actual.dat";
	
	private ArrayList<Vector2D> trackPlot, optimalPlot, actualPlot, distancePlot;
	private ArrayList<Vector2D> distAxes;
	
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
		trackPlot = new ArrayList<Vector2D>();
		outputPanel.send("Plotting track...");
		String line;
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			trackPlot.add(new Vector2D(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
			outputPanel.send(" - " + String.valueOf(trackPlot.get(trackPlot.size()-1).x + ", " + trackPlot.get(trackPlot.size()-1).y));
		}
		
		// Read actual plot
		fr = new FileReader(ACTUAL_FILE);
		textReader = new BufferedReader(fr);
		actualPlot = new ArrayList<Vector2D>();
		outputPanel.send("Plotting actual path...");
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			actualPlot.add(new Vector2D(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
			outputPanel.send(" - " + String.valueOf(trackPlot.get(trackPlot.size()-1).x + ", " + trackPlot.get(trackPlot.size()-1).y));
		}
		
		// Read Optimal plot
		fr = new FileReader(OPTIMAL_FILE);
		textReader = new BufferedReader(fr);
		optimalPlot = new ArrayList<Vector2D>();
		outputPanel.send("Plotting actual path...");
		while((line = textReader.readLine()) != null) {
			String coords[] = line.split(",");
			optimalPlot.add(new Vector2D(Double.parseDouble(coords[0]), Double.parseDouble(coords[1])));
			outputPanel.send(" - " + String.valueOf(trackPlot.get(trackPlot.size()-1).x + ", " + trackPlot.get(trackPlot.size()-1).y));
		}
		
		processData();
		
		outputPanel.send("Successfully read all data.");
		outputPanel.send("Left-click and move the mouse to drag the plot around. Mouse wheel to zoom. Press mouse wheel to reset view.");
	}
	
	public void processData() {
		outputPanel.send("Processing data from plots.");
		
		maxDist = 0;
		
		// Compute distance plot
		distancePlot = new ArrayList<Vector2D>();
		for(int i = 0; i < actualPlot.size(); i++) {
			Vector2D opt = optimalPlot.get(i);
			Vector2D act = actualPlot.get(i);
			
			distancePlot.add(new Vector2D(i, dist(opt.x, opt.y, act.x, act.y)));
					
			if(distancePlot.get(i).x > maxDist)
				maxDist = distancePlot.get(i).y;
			
			outputPanel.send(" - " + String.valueOf(distancePlot.get(i).x + ", " + String.valueOf(distancePlot.get(i).y)));
		}
		
		// Create distance axes
		distAxes = new ArrayList<Vector2D>();
		distAxes.add(new Vector2D(0, maxDist));
		distAxes.add(new Vector2D(0, 0));
		distAxes.add(new Vector2D(distancePlot.size(), 0));
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
			
			drawSurface.addTextElement(new Vector2D(-10, distAxes.get(1).y + (maxDist/2)), "Distance", Color.black);
			drawSurface.addTextElement(new Vector2D(distAxes.get(1).x + (distancePlot.size()/2), -10), "Time", Color.black);
			
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
	
	public double dist(double x1, double y1, double x2, double y2) {
		return Math.sqrt(((x2*x2)-(x1*x1)) + ((y2*y2)-(y1*y1)));
	}
}
