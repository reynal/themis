package view.frontpane;

public class BarGraph {

	public static final int MIN_VALUE = 0;
	public static final int MAX_VALUE = 10;
	
	private int value; // must be b/w 0 (no LED active) and MAX_VALUE;
	
	public void setValue(int v) {
		if (v > MAX_VALUE)
			throw new IllegalArgumentException(v + " greater than " + MAX_VALUE);
		else if (v < MIN_VALUE)
			throw new IllegalArgumentException(v + " lower than " + MIN_VALUE);
		value = v;
	}
}
