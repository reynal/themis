package controller.component;

import javafx.geometry.Orientation;
import javafx.scene.Group;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;

/**
 * a rotary encoder for enum types
 * @author sydxrey
 *
 * @param <T>
 */
public class EnumRotaryEncoder<T extends Enum<T>> extends AbstractRotaryEncoder {

	
	public EnumRotaryEncoder(String label) {
		super(label);
	}
	
	@Override
	public Node createJavaFXView() {
		Group g = new Group();
		Slider slider = new Slider(0, 1, 0.5);
		slider.setOrientation(Orientation.VERTICAL);
		slider.setShowTickMarks(true); // TODO : add enum labels!
		slider.setShowTickLabels(true);
		slider.setMajorTickUnit(0.25f);
		slider.setBlockIncrement(0.1f);		
		g.getChildren().addAll(slider);
		g.getChildren().addAll(new Label(label));		
		return g;
	} 
}
