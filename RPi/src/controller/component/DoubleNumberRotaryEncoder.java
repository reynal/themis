package controller.component;

import javafx.geometry.Orientation;
import javafx.scene.Node;
import javafx.scene.control.Slider;
import model.*;

/**
 * A rotary encoder than can be linked to a DoubleParameter.
 * @author SR
 */
public class DoubleNumberRotaryEncoder extends AbstractRotaryEncoder {

	@Override
	public Node createJavaFXView() {
		Slider slider = new Slider(0, 1, 0.5);
		slider.setOrientation(Orientation.VERTICAL);
		slider.setShowTickMarks(true);
		slider.setShowTickLabels(true);
		slider.setMajorTickUnit(0.25f);
		slider.setBlockIncrement(0.1f);		
		return slider;
	} 
	
}
