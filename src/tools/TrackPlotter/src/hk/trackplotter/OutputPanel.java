package hk.trackplotter;

import java.awt.BorderLayout;
import java.awt.Dimension;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.ScrollPaneConstants;

public class OutputPanel extends JPanel {

	JTextArea textArea;
	JScrollPane scrollPane;
	
	public OutputPanel() {
		textArea = new JTextArea();
		textArea.setEditable(false);
		
		scrollPane = new JScrollPane(textArea);
		scrollPane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
		scrollPane.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
		
		scrollPane.setPreferredSize(new Dimension(400, 150));
		
		setLayout(new BorderLayout());
		add(scrollPane, BorderLayout.CENTER);
	}
	
	public void send(String text) {
		textArea.append(text + "\n");
		textArea.setCaretPosition(textArea.getDocument().getLength());
	}
}
