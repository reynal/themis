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
		//Slider slider = new Slider(0, 1, 0.5);
		slider.setMax(3.0);
		slider.setMin(0.0);
		slider.setOrientation(Orientation.VERTICAL);
		slider.setShowTickMarks(true); // TODO : add enum labels!
		slider.setShowTickLabels(false);
		slider.setSnapToTicks(true);
		slider.setValue(0);
		slider.setMajorTickUnit(4f);
		slider.setBlockIncrement(1f);
		//g.getChildren().addAll(new Label(label));		
		return slider;
	} 
}
