package hk.trackplotter;

public class Vector2D {

	double x, y;
	
	public Vector2D() {
		x = y = 0;
	}
	
	public Vector2D(double x, double y) {
		this.x = x;
		this.y = y;
	}

	public double mag() {
		return Math.sqrt((x * x) + (y* y));
	}
	
	public void normalize() {
		x /= mag();
		y /= mag();
	}
	
	public Vector2D sub(Vector2D vec) {
		return new Vector2D(x-vec.x, y-vec.y);
	}
}
