package hk.trackplotter;

import java.awt.Color;
import java.awt.geom.Point2D;

/**
 * Represents an individually plotted text element on the surface.
 * @author Henri
 *
 */
public class TextObject {
	
	private String text;
	private Point2D position;
	private Color color;
	
	public TextObject(String text, Point2D position, Color color) {
		this.text = text;
		this.position = position;
		this.color = color;
	}
	
	public String getText() {
		return text;
	}
	
	public Color getColor() {
		return color;
	}
	
	public Point2D getPosition() {
		return position;
	}
	
	public void setColor(Color color) {
		this.color = color;
	}
	
	public void setText(String text) {
		this.text = text;
	}
	
	public void setPosition(Point2D position) {
		this.position = position;
	}
}