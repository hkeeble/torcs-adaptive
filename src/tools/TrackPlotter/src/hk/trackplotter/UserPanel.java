package hk.trackplotter;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.Point2D;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.table.JTableHeader;
import javax.swing.table.TableColumn;
import javax.swing.table.TableColumnModel;

public class UserPanel extends Subject {
	
	// The main panel for this class
	JPanel mainPanel;
	
	// Data panel
	JPanel data;
	
	// Input Panels
	JPanel inputs;
	JPanel renderPanel, speedPanel, plotPanel, miscPanel;
	
	// Plot panel buttons
	JButton plotDistGraph, plotSpeedDiffGraph, plotSpeedComparisonGraph, plotSkillLevelGraph;
	
	// Render Panel Buttons
	JButton toggleSpeedRender, togglePointRender, toggleSkillRender;

	// Speed panel buttons
	JButton showOptimalSpeed, showActualSpeed;
	JSlider speedResSlider;
	
	// Summary Button
	JButton showRaceSummary;
	
	public UserPanel() {
		super();
		mainPanel = new JPanel();
		mainPanel.setPreferredSize(new Dimension(300, 100));
		mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
		
		initInputPanel(); // Initialize the input panel
		
		mainPanel.add(inputs);
	}

	private void initInputPanel() {
		inputs = new JPanel();
		renderPanel = new JPanel();
		speedPanel = new JPanel();
		plotPanel = new JPanel();
		miscPanel = new JPanel();
		
		inputs.setLayout(new GridLayout(0, 1));
		inputs.setBorder(BorderFactory.createTitledBorder("Inputs"));
		
		// Render Panel
		renderPanel.setLayout(new GridLayout(0, 1));
		renderPanel.setBorder(BorderFactory.createTitledBorder("Rendering Options"));
		togglePointRender = new JButton("Toggle Point/Line Render");
		toggleSpeedRender = new JButton("Toggle Speed Render");
		toggleSkillRender = new JButton("Toggle Skill Level Render");
		togglePointRender.addActionListener(new TogglePointRender());
		toggleSpeedRender.addActionListener(new ToggleSpeedRender());
		toggleSkillRender.addActionListener(new ToggleSkillRender());
		renderPanel.add(toggleSpeedRender);
		renderPanel.add(togglePointRender);
		renderPanel.add(toggleSkillRender);
		
		// Speed Panel
		speedPanel.setLayout(new GridLayout(0, 1));
		speedPanel.setBorder(BorderFactory.createTitledBorder("Speed Display Options"));
		JLabel label = new JLabel("Speed Display Resolution:");
		speedResSlider = new JSlider(JSlider.HORIZONTAL, 0, 10, 5);
		speedResSlider.addChangeListener(new SpeedResSlider());
		speedResSlider.setPaintLabels(true);
		showOptimalSpeed = new JButton("Show Optimal Speed");
		showActualSpeed = new JButton("Show Actual Speed");
		showOptimalSpeed.addActionListener(new ShowOptimalSpeed());
		showActualSpeed.addActionListener(new ShowActualSpeed());
		
		speedPanel.add(showActualSpeed);
		speedPanel.add(showOptimalSpeed);
		speedPanel.add(label);
		speedPanel.add(speedResSlider);
		
		// Plot Panel
		plotPanel.setLayout(new GridLayout(0, 1));
		plotPanel.setBorder(BorderFactory.createTitledBorder("Graph Plotting"));
		plotDistGraph = new JButton("Plot Distance Graph");
		plotSpeedDiffGraph = new JButton("Plot Speed Difference Graph");
		plotSpeedComparisonGraph = new JButton("Plot Actual Speed against Optimal Speed");
		plotSkillLevelGraph = new JButton("Plot Skill Level Graph");
		
		plotDistGraph.addActionListener(new PlotDistanceGraphButton());
		plotSpeedDiffGraph.addActionListener(new PlotSpeedDiffGraph());
		plotSpeedComparisonGraph.addActionListener(new PlotSpeedComparisonGraph());
		plotSkillLevelGraph.addActionListener(new PlotSkillLevelGraph());
		
		plotPanel.add(plotDistGraph);
		plotPanel.add(plotSpeedDiffGraph);
		plotPanel.add(plotSpeedComparisonGraph);
		plotPanel.add(plotSkillLevelGraph);
		
		miscPanel.setLayout(new GridLayout(0, 1));
		miscPanel.setBorder(BorderFactory.createTitledBorder("Other Options"));
		showRaceSummary = new JButton("Show Race Summary");
		showRaceSummary.addActionListener(new ShowRaceSummary());
		miscPanel.add(showRaceSummary);
		
		// Add to input panel
		inputs.add(renderPanel);
		inputs.add(speedPanel);
		inputs.add(plotPanel);
		inputs.add(miscPanel);
		
		setActive(false);
	}
	
	public void setSliderValues(int max, int min) {
		speedResSlider.setMaximum(max);
		speedResSlider.setMinimum(min);
		speedResSlider.setMajorTickSpacing(max/5);
		speedResSlider.setMinorTickSpacing(max/25);
		speedResSlider.setValue(max/2);
	}
	
	private JTable createTable(Point2D[] data) {
		JTable table = new JTable(data.length, 2);
		
		for(int i = 0; i < data.length; i++) {
			table.setValueAt(data[i].getX(), i, 0);
			table.setValueAt(data[i].getY(), i, 1);
		}
		
		JTableHeader th = table.getTableHeader();
		TableColumnModel tcm = th.getColumnModel();
		TableColumn x = tcm.getColumn(0);
		TableColumn y = tcm.getColumn(1);
		
		x.setHeaderValue("X");
		y.setHeaderValue("Y");
		
		mainPanel.setPreferredSize(new Dimension(200, 900));
		
		return table;
	}
	
	private class PlotDistanceGraphButton implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent arg0) {
			sendMessage(GUIMessage.PLOT_DISTANCE_DIFF, null);
		}
	}
	
	private class PlotSpeedDiffGraph implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent arg0) {
			sendMessage(GUIMessage.PLOT_SPEED_DIFF, null);
		}
	}
	
	private class PlotSpeedComparisonGraph implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent arg0) {
			sendMessage(GUIMessage.PLOT_SPEED_COMPARISON, null);
		}
	}
	
	private class PlotSkillLevelGraph implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			sendMessage(GUIMessage.PLOT_SKILL_LEVEL, null);
		}
		
	}
	
	private class TogglePointRender implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent arg0) {
			sendMessage(GUIMessage.TOGGLE_POINT_RENDER, null);
		}
	}
	
	private class ToggleSpeedRender implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent arg0) {
			sendMessage(GUIMessage.TOGGLE_SPEED_RENDER, null);
			
			boolean isEnabled = !speedPanel.isEnabled();
			Component[] components = speedPanel.getComponents();
			for(int i = 0; i < components.length; i++) {
				components[i].setEnabled(isEnabled);
			}
			speedPanel.setEnabled(isEnabled);
		}
	}
	
	private class ToggleSkillRender implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent arg0) {
			sendMessage(GUIMessage.TOGGLE_SKILL_RENDER, null);
		}
		
	}
	
	private class SpeedResSlider implements ChangeListener {
		@Override
		public void stateChanged(ChangeEvent e) {
			JSlider source = (JSlider)e.getSource();
			if(source.getValueIsAdjusting()) {
				sendMessage(GUIMessage.CHANGE_SPEED_RES, source.getValue());
			}
		}
	}
	
	private class ShowOptimalSpeed implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent e) {
			sendMessage(GUIMessage.CHANGE_SPD_RENDER, SpeedPlotType.OPTIMAL);
		}
	}
	
	private class ShowActualSpeed implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent e) {
			sendMessage(GUIMessage.CHANGE_SPD_RENDER, SpeedPlotType.ACTUAL);
		}
	}
	
	private class ShowRaceSummary implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent e) {
			sendMessage(GUIMessage.SHOW_RACE_SUMMARY, null);
		}
	}
	
	private void sendMessage(GUIMessage message, Object userValue) {
		notifyObservers(message, userValue);
	}
	
	public JPanel get() {
		return mainPanel;
	}
	
	public void setActive(boolean active) {
		setPanelActive(inputs, active);
	}
	
	public void setPanelActive(JPanel panel, boolean active) {
		Component[] components = panel.getComponents();
		for(int i = 0; i < components.length; i++) {
			if(components[i] instanceof JPanel) {
				setPanelActive((JPanel)components[i], active);
			}
			components[i].setEnabled(active);
		}
	}
}
