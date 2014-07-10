package hk.trackplotter;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.RenderingHints;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.geom.AffineTransform;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Path2D;
import java.awt.geom.Point2D;
import java.util.ArrayList;

import javax.swing.JPanel;
import javax.swing.event.MouseInputListener;

public class Surface implements MouseMotionListener, MouseInputListener, MouseWheelListener { 

	/**
	 * The data plots for this surface to draw.
	 */
	private ArrayList<PathPlot> plots;
	
	/**
	 * The sequences of text values for this surface to draw.
	 */
	private ArrayList<ValueSequence> valueSequences;
	
	/**
	 * The text objects for this surface to draw.
	 */
	private ArrayList<TextObject> textObjects;
	
	private RenderType renderType;
	
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
			
			// Render all plots
			if(renderType == RenderType.LINE) {
				for(int i = 0; i < plots.size(); i++) {
					g2d.setColor(plots.get(i).getColor());
					g2d.draw(plots.get(i).getPath());
				}
			}
			else if(renderType == RenderType.POINT) {
				for(int i = 0; i < plots.size(); i++) {
					PathPlot current = plots.get(i);
					g2d.setColor(current.getColor());
					for(int p = 0; p < current.getPointCount(); p++) {
						g2d.draw(new Ellipse2D.Double(current.getPoint(p).getX(), current.getPoint(p).getY(), 0.1, 0.1));
					}
				}
			}
			
			g2d.scale(1, -1);
			Font originalFont = g2d.getFont();
			g2d.setFont(originalFont.deriveFont(11f));
				
			
			// Render Text Elements
			for(TextObject t : textObjects) {
				g2d.setColor(t.getColor());
				g2d.drawString(t.getText(), (int)t.getPosition().getX(), -(int)t.getPosition().getY());
			}
				
			g2d.setFont(originalFont);
			g2d.setColor(Color.black);
			
			// Render value sequences
			for(ValueSequence s : valueSequences) {
				s.draw(g2d);
			}
			
			// Draw mouse position
			g2d.setTransform(identity);
			g2d.drawString(String.valueOf(mousePos.x) + ", " + String.valueOf(mousePos.y), drawSurface.getWidth() - 60, drawSurface.getHeight() - 10);
			
			// Draw Plot Points
			for(int i = 0; i < plots.size(); i++) {
				Path2D path = new Path2D.Double();
				g2d.setColor(plots.get(i).getColor());
				path.moveTo(10, 30*i+10);
				path.lineTo(20, 30*i+10);
				g2d.drawString(String.valueOf(plots.get(i).getPointCount()), 25, 30*i+15);
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
	private double scrollSensitivity = MAX_SCROLL_SENSE;
	
	public Surface() {
		plots = new ArrayList<PathPlot>();
		textObjects = new ArrayList<TextObject>();
		valueSequences = new ArrayList<ValueSequence>();
		
		// Initialize the render type
		renderType = RenderType.LINE;
		
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
	}
	
	public void clearValueSequences() {
		valueSequences.clear();
	}
	
	public void clearTextObjects() {
		textObjects.clear();
	}
	
	public void clearAll() {
		clearPlots();
		clearValueSequences();
		clearTextObjects();
	}
	
	public void addPlotData(PathPlot path) {
		plots.add(path);
	}
	
	public void addTextObject(TextObject object) {
		textObjects.add(object);
	}
	
	public void addValueSequence(ValueSequence sequence) {
		valueSequences.add(sequence);
	}
	
	public void removePlotData(PathPlot path) {
		plots.remove(path);
	}
	
	public void removeTextObject(TextObject object) {
		textObjects.remove(object);
	}
	
	public void removeValueSequence(ValueSequence sequence) {
		valueSequences.remove(sequence);
	}
	
	public boolean containsValueSequence(ValueSequence sequence) {
		return valueSequences.contains(sequence);
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
	
	public void togglePointRender() {
		if(renderType == RenderType.LINE) {
			renderType = RenderType.POINT;
		}
		else {
			renderType = RenderType.LINE;
		}
		
		repaint();
	}
}
