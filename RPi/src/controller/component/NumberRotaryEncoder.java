package controller.component;

import javafx.geometry.Orientation;
import javafx.scene.Group;
import javafx.scene.Node;
import javafx.scene.control.*;

/**
 * A rotary encoder than can be linked to a DoubleParameter.
 * @author SR
 */
public class NumberRotaryEncoder extends AbstractRotaryEncoder {

	/**
	 * construct a pushbutton with the given UI label
	 */
	public NumberRotaryEncoder(String label) {
		super(label);
	}	
	
	@Override
	public Node createJavaFXView() {
		Group g = new Group();
		Slider slider = new Slider(0, 1, 0.5);
		slider.setOrientation(Orientation.VERTICAL);
		slider.setShowTickMarks(true);
		slider.setShowTickLabels(true);
		slider.setMajorTickUnit(0.25f);
		slider.setBlockIncrement(0.1f);		
		g.getChildren().addAll(slider);
		g.getChildren().addAll(new Label(label));		
		return g;
	} 
	
}
