package hk.trackplotter;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.geom.Point2D;

/**
 * Represents a sequence of values that can be plotted onto a surface.
 * @author Henri
 *
 */
public class ValueSequence {

	private int maxResolution, minResolution, resolution;
	TextObject[] objects;
	
	public ValueSequence(TextObject[] objects) {
		this.objects = objects;
		maxResolution = objects.length;
		minResolution = 1;
		
		setResolution(maxResolution/2);
	}
	
	public ValueSequence(TextObject[] objects, int maxResolution, int minResolution) {
		this(objects);
		
		if(maxResolution <= objects.length) {
			this.maxResolution = maxResolution;
		}
		
		this.minResolution = minResolution;
		
		setResolution(maxResolution/2);
	}
	
	public void draw(Graphics2D g2d) {
		float divs = objects.length/resolution;
		for(int i = 0; i < objects.length; i+=divs) {
			g2d.setColor(objects[i].getColor());
			g2d.drawString(objects[i].getText(), (float)objects[i].getPosition().getX(), -(float)objects[i].getPosition().getY());
		}
		g2d.setColor(Color.black);
	}
	
	public int getResolution() {
		return resolution;
	}
	
	public void setResolution(int resolution) {
		this.resolution = Math.max(minResolution, Math.min(maxResolution, resolution));
	}
	
	public int size() {
		return objects.length;
	}
	
	public int getMaxRes() {
		return maxResolution;
	}
	
	public int getMinRes() {
		return minResolution;
	}
	
	public double getValueAt(int index) {
		double d = Double.parseDouble(objects[index].getText());
		System.out.println("Returning:");
		System.out.println(d);
		return d;
	}
	
	public String getTextAt(int index) {
		return objects[index].getText();
	}
	
	public TextObject getClosest(Point2D point) {
		TextObject closest = objects[0];
		double currentDist = closest.getPosition().distance(point);
		for(int i = 0; i < objects.length; i++) {
			double dist = objects[i].getPosition().distance(point);
			if(dist < currentDist) {
				currentDist = dist;
				closest = objects[i];
			}
		}
		
		return closest;
	}
}
