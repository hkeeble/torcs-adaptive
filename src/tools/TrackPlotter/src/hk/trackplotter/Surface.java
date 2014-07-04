package hk.trackplotter;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.geom.AffineTransform;
import java.awt.geom.Path2D;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.util.ArrayList;

import javax.swing.JPanel;
import javax.swing.event.MouseInputListener;

public class Surface implements MouseMotionListener, MouseInputListener, MouseWheelListener {

	private ArrayList<Path2D> plots;
	private ArrayList<Integer> pointCount;
	private ArrayList<Color> plotColors;
	
	private ArrayList<TextElement> textElements;
	
	private class TextElement {
		String text;
		Point2D position;
		Color color;
		
		public TextElement(Point2D pos, String text, Color color){
			this.position = pos;
			this.color = color;
			this.text = text;
		}
		
		public Point2D getPos() {
			return position;
		}
		
		public Color getColor() {
			return color;
		}
		
		public String getText() {
			return text;
		}
	}
	
	private class DrawSurface extends JPanel {
		
		public void paintComponent(Graphics g) {
			super.paintComponent(g);
			draw(g);
		}
		
		private void draw(Graphics g) {
			Graphics2D g2d = (Graphics2D)g;
			g2d.setColor(Color.white);
			g.fillRect(0,  0, drawSurface.getWidth(), drawSurface.getHeight());
			g2d.setColor(Color.black);
			
			g2d.setTransform(plotTransform);
			g2d.scale(1.0, -1.0);
			g2d.translate(0, -this.getHeight());
			
			for(int i = 0; i < plots.size(); i++) {
				g2d.setColor(plotColors.get(i));
				g2d.draw(plots.get(i));
			}

			// Draw mouse position
			g2d.setTransform(identity);
			g2d.drawString(String.valueOf(mousePos.x) + ", " + String.valueOf(mousePos.y), drawSurface.getWidth() - 60, drawSurface.getHeight() - 10);
			
			// Draw Plot Points
			for(int i = 0; i < plots.size(); i++) {
				Path2D path = new Path2D.Double();
				g2d.setColor(plotColors.get(i));
				path.moveTo(10, 30*i+10);
				path.lineTo(20, 30*i+10);
				g2d.drawString(String.valueOf(pointCount.get(i)), 25, 30*i+15);
				g2d.draw(path);
			}
		}
	}
	
	private DrawSurface drawSurface;
	
	private AffineTransform plotTransform;
	private AffineTransform identity;
	
	private Point mouseClickPos;
	private Point mousePos;
	
	private final double MAX_SCROLL_SENSE = 15.0;
	private final double MIN_SCROLL_SENSE = 5.0;
	private final double SCALE_SENSITIVITY = 0.1;
	
	private double scrollSensitivity = MAX_SCROLL_SENSE;
	
	public Surface() {
		plots = new ArrayList<Path2D>();
		plotColors = new ArrayList<Color>();
		pointCount = new ArrayList<Integer>();
		
		// Transformation matrix
		plotTransform = new AffineTransform();
		plotTransform.setToIdentity();
		
		// Identity matrix
		identity = new AffineTransform();
		identity.setToIdentity();
		
		// Current mouse position initialised
		mousePos = new Point();
		
		// Main panel
		drawSurface = new DrawSurface();
		
		// Default surface size
		drawSurface.setPreferredSize(new Dimension(500, 500));
		
		// Add event listeners
		drawSurface.addMouseListener(this);
		drawSurface.addMouseMotionListener(this);
		drawSurface.addMouseWheelListener(this);
		
		// Initialize text elements
		textElements = new ArrayList<TextElement>();
	}
	
	public void clearPlots() {
		plots.clear();
		plotColors.clear();
		pointCount.clear();
		textElements.clear();
	}
	
	public void addPlotData(ArrayList<Point2D> data, Color color) {
		
		// Form path
		Path2D path = new Path2D.Double();
		path.moveTo(data.get(0).getX(), data.get(0).getY());
		for(int i = 0; i < data.size(); i++) {
			path.lineTo(data.get(i).getX(), data.get(i).getY());
		}
		
		// Add all neccesary data to the surface
		plots.add(path);
		pointCount.add(data.size());
		plotColors.add(color);
	}

	public void addTextElement(Point2D position, String text, Color color) {
		textElements.add(new TextElement(position, text, color));
	}
	
	@Override
	public void mouseClicked(MouseEvent e) {
		if(e.getButton() == MouseEvent.BUTTON2) {
			plotTransform.setToIdentity();
			scrollSensitivity = MAX_SCROLL_SENSE;
			drawSurface.repaint();
		}
	}

	@Override
	public void mouseEntered(MouseEvent e) {

	}

	@Override
	public void mouseExited(MouseEvent e) {
	
	}

	@Override
	public void mousePressed(MouseEvent e) {
		mouseClickPos = new Point(e.getX(), e.getY());
		
	}

	@Override
	public void mouseReleased(MouseEvent e) {

	}

	@Override
	public void mouseDragged(MouseEvent e) {
		Point p = e.getPoint();
		Point2D dir = new Point2D.Double(p.x - mouseClickPos.x, p.y - mouseClickPos.y);
		
		double mag = Math.abs(Math.sqrt((dir.getX()*dir.getX()) + (dir.getY()*dir.getY())));
		Point2D nDir = new Point2D.Double(dir.getX()/mag, dir.getY()/mag);
			
		plotTransform.translate(nDir.getX() * scrollSensitivity, nDir.getY() * scrollSensitivity);
			
		drawSurface.repaint();
			
		mouseClickPos = p;
	}

	@Override
	public void mouseMoved(MouseEvent arg0) {
		mousePos = arg0.getPoint();
		drawSurface.repaint();
	}

	@Override
	public void mouseWheelMoved(MouseWheelEvent arg0) {
		double dir = arg0.getPreciseWheelRotation();

		if(dir < 0) {
			plotTransform.scale(1.5, 1.5);
			scrollSensitivity -= 1.5;
		}
		else {
			plotTransform.scale(0.5, 0.5);
			scrollSensitivity += 1.5;
		}
		
		scrollSensitivity = Math.max(MIN_SCROLL_SENSE, Math.min(MAX_SCROLL_SENSE, scrollSensitivity));
		
		System.out.println(scrollSensitivity);
		
		drawSurface.repaint();
	}
	
	public JPanel get() {
		return drawSurface;
	}
	
	public void repaint() {
		drawSurface.repaint();
	}
	
	private int largestPlot() {
		int largest = 0;
		Rectangle2D rect = plots.get(0).getBounds2D();
		double previousSize = rect.getWidth() * rect.getHeight();
		
		for(int i = 1; i < plots.size(); i++) {
			Rectangle2D currentRect = plots.get(i).getBounds2D();
			double currentSize = currentRect.getWidth() * currentRect.getHeight();
			if(currentSize > previousSize)
				largest = i;
		}
		
		return largest;
	}
}
