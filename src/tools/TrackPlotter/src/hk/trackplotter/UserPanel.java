package hk.trackplotter;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.Point2D;

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
	
	JPanel mainPanel;
	
	JPanel data;
	
	JPanel buttons;
	JPanel configs;
	
	JLabel optimalVerts, actualVerts, trackVerts;
	
	JTabbedPane tablePane;
	
	JButton plotDistGraph, togglePointRender, toggleSpeedRender;
	
	JSlider speedResSlider;
	
	public UserPanel() {
		super();
		mainPanel = new JPanel();
		mainPanel.setPreferredSize(new Dimension(300, 100));
		mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
		
		// INITIALIZE TABLE PANELS
		tablePane = new JTabbedPane();

		// INITIALIZE BUTTONS
		buttons = new JPanel();
		buttons.setLayout(new GridLayout(0, 1));
		plotDistGraph = new JButton("Plot Distance Graph");
		togglePointRender = new JButton("Toggle Point/Line Render");
		toggleSpeedRender = new JButton("Toggle Speed Render");
		
		plotDistGraph.addActionListener(new PlotDistanceGraphButton());
		togglePointRender.addActionListener(new TogglePointRender());
		toggleSpeedRender.addActionListener(new ToggleSpeedRender());
		
		buttons.add(plotDistGraph);
		buttons.add(togglePointRender);
		buttons.add(toggleSpeedRender);
		
		configs = new JPanel();
		configs.setLayout(new GridLayout(1, 1));
		JLabel label = new JLabel("Speed Display Resolution:");
		speedResSlider = new JSlider(JSlider.HORIZONTAL, 0, 10, 5);
		speedResSlider.addChangeListener(new SpeedResSlider());

		speedResSlider.setPaintLabels(true);
		
		configs.add(label);
		configs.add(speedResSlider);
		
		mainPanel.add(tablePane);
		mainPanel.add(buttons);
		mainPanel.add(configs);
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
	
	public void addTable(Point2D[] data, String name) {
		JPanel panel = new JPanel(new BorderLayout());
		JTable table = createTable(data);
		panel.add(new JScrollPane(table), BorderLayout.CENTER);
		tablePane.addTab(name, panel);
	}
	
	private class PlotDistanceGraphButton implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent arg0) {
			sendMessage(GUIMessage.PLOT_DISTANCE_DIFF, null);
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
	
	private void sendMessage(GUIMessage message, Object userValue) {
		notifyObservers(message, userValue);
	}
	
	public JPanel get() {
		return mainPanel;
	}
}
