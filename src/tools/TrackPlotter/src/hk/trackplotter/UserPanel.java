package hk.trackplotter;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.Point2D;
import java.util.ArrayList;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.table.JTableHeader;
import javax.swing.table.TableColumn;
import javax.swing.table.TableColumnModel;

public class UserPanel extends Subject {
	
	JPanel mainPanel;
	
	JPanel data;
	JPanel inputs;
	
	JLabel optimalVerts, actualVerts, trackVerts;
	
	JTabbedPane tablePane;
	
	JButton plotDistGraph;
	
	public UserPanel() {
		super();
		mainPanel = new JPanel();
		mainPanel.setPreferredSize(new Dimension(300, 100));
		mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
		
		// INITIALIZE TABLE PANELS
		tablePane = new JTabbedPane();

		// INITIALIZE USER INPUT PANEL
		inputs = new JPanel();
		inputs.setLayout(new GridLayout(3, 3));
		plotDistGraph = new JButton("Plot Distance Graph");
		
		plotDistGraph.addActionListener(new PlotDistanceGraphButton());
		
		inputs.add(plotDistGraph);
		
		mainPanel.add(tablePane);
		mainPanel.add(inputs);
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
			sendMessage(GUIMessage.PLOT_DISTANCE_DIFF);
		}
	}
	
	private void sendMessage(GUIMessage message) {
		notifyObservers(message);
	}
	
	public JPanel get() {
		return mainPanel;
	}
}
