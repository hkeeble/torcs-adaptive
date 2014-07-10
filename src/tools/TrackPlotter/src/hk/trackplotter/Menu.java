package hk.trackplotter;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

public class Menu extends Subject {

	JMenuBar menuBar;
	JMenu file;
	JMenuItem exit, loadNewData;
	
	public Menu() {
		super();
		menuBar = new JMenuBar();
		file = new JMenu("File");
		
		exit = new JMenuItem("Exit");
		loadNewData = new JMenuItem("Load New Track Data");
		
		exit.addActionListener(new ExitListener());
		loadNewData.addActionListener(new LoadListener());
		
		file.add(loadNewData);
		file.add(exit);
		
		menuBar.add(file);
	}
	
	private class ExitListener implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent arg0) {
			notifyObservers(GUIMessage.EXIT, null);
		}
	}

	private class LoadListener implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent arg0) {
			notifyObservers(GUIMessage.LOAD_NEW_DATA, null);
		}
	}
	
	public JMenuBar getBar() {
		return menuBar;
	}
}
