package hk.trackplotter;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.RenderingHints;
import java.awt.RenderingHints.Key;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.geom.AffineTransform;
import java.awt.geom.Path2D;
import java.awt.geom.Point2D;
import java.util.ArrayList;

import javax.swing.JPanel;
import javax.swing.event.MouseInputListener;

public class Surface implements MouseMotionListener, MouseInputListener, MouseWheelListener {

	private ArrayList<Path2D> plots;
	private ArrayList<Integer> pointCount;
	private ArrayList<Color> plotColors;
	
	private class DrawSurface extends JPanel {
		
		public void paintComponent(Graphics g) {
			super.paintComponent(g);
			draw(g);
		}
		
		private void draw(Graphics g) {
			Graphics2D g2d = (Graphics2D)g;
			RenderingHints rh = new RenderingHints(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
			rh.put(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY);
			g2d.setRenderingHints(rh);
			
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
	}
	
	public void clearPlots() {
		plots.clear();
		plotColors.clear();
		pointCount.clear();
	}
	
	public void addPlotData(Path2D path, int nPoints, Color color) {
		
		// Add all neccesary data to the surface
		plots.add(path);
		pointCount.add(nPoints);
		plotColors.add(color);
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
		
		drawSurface.repaint();
	}
	
	public JPanel get() {
		return drawSurface;
	}
	
	public void repaint() {
		drawSurface.repaint();
	}
}
