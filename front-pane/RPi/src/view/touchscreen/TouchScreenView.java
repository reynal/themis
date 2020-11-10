package view.touchscreen;

import java.awt.*;
import java.awt.image.*;

/**
 * An interface for rendering graphics in the touch screen pane. Any class that implements
 * this interface can be plugged into TouchScreen using the setView() method.
 * 
 * @author SR
 *
 */
public interface TouchScreenView {

	/**
	 * This method gets called everytime the view need (re)painting.
	 * 
	 * The graphic coordinate system is scaled and translated before this method is called, so that
	 * the implementation of render can assume the view pane is a square of size 1.0 with the (0,0) origin
	 * sitting at the center of the view (so (0.5,0.5) is the upper-rightcorner). 
	 * Scaling factors are also provided as argument in case reverse-scaling is necessary, for
	 * example when displaying text (in which case it might be useful to scale down the font metric).
	 * 
	 * @param g2 the graphic context
	 * @param scaleX the x-scaling factor that has been applied to the coordinate system before this method was called
	 * @param scaleY the y-scaling factor that has been applied to the coordinate system before this method was called
	 * @param io the hosting JPanel; useful when rendering images loaded from a file or from the internet, where most methods require a valide ImageObserver.
	 */
	void render(Graphics2D g2, double scaleX, double scaleY, ImageObserver io);
	
	/**
	 * @return true if this view is animated ; might be used by the hosting TouchScreen to trigger a timer and call the render() method
	 * on a regular basis.
	 */
	boolean isAnimated();

}
