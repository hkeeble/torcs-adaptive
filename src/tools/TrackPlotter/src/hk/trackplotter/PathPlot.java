package hk.trackplotter;

import java.awt.geom.Line2D;
import java.awt.geom.Path2D;
import java.awt.geom.Point2D;
import java.awt.geom.Path2D.Double;

/**
 * Represents a path plot. Used to construct a path, and it's individual line segments, from the given data. Can then be
 * used to calculate distance from a given point, or for drawing to the surface.
 * @author Henri
 *
 */
public class PathPlot {

	private Point2D[] points;
	private Line2D[] segs;
	private Path2D path;
	
	/**
	 * Create a new path plot.
	 * @param points The points within the path plot.
	 */
	public PathPlot(Point2D[] points) {
		this.points = points;
		
		segs = new Line2D[points.length];
		for(int i = 0; i < points.length-1; i++) {
			segs[i] = new Line2D.Double(points[i], points[i+1]);
		}
		
		path = new Path2D.Double();
		path.moveTo(points[0].getX(), points[0].getY());
		for(int i = 1; i < points.length; i++) {
			path.lineTo(points[i].getX(), points[i].getY());
		}
	}
	
	/**
	 * Get the Path2D object for rendering.
	 */
	public Path2D getPath() {
		return path;
	}
	
	/**
	 * Get the line segment at the given index.
	 */
	public Line2D getLineSeg(int index) {
		return segs[index];
	}
	
	/**
	 * Get the number of line segments in this plot.
	 */
	public int getLineSegCount() {
		return segs.length;
	}
	
	/**
	 * Get the point on the plot at the given index.
	 */
	public Point2D getPoint(int index) {
		return points[index];
	}

	/**
	 * Get the array of points contained in this path plot.
	 */
	public Point2D[] getPoints() {
		return points;
	}
	
	/**
	 * Get the number of points in this plot. 
	 */
	public int getPointCount() {
		return points.length;
	}
	
	/**
	 * Get the distance of a point to the plot.
	 */
	public double getDistance(Point2D point) {
		int closest = closestLineSeg(point);
		return segs[closest].ptSegDist(point);
	}
	
	/**
	 * Finds the index of the closest line segment to the given point within this path plot.
	 */
	private int closestLineSeg(Point2D point) {
		int closest = 0;
		double lowestDist = segs[0].ptSegDist(point);
		for(int i = 0; i < segs.length-1; i++) {
			double dist = segs[i].ptSegDist(point);
			if(dist < lowestDist) {
				closest = i;
				lowestDist = dist;
			}
		}
		return closest;
	}
}
