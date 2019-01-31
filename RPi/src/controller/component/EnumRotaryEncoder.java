package controller.component;

import javafx.geometry.Orientation;
import javafx.scene.Group;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import controller.component.Control;

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
		Slider slider = new Slider(0,127,32);
		slider.setOrientation(Orientation.VERTICAL);
		slider.setShowTickMarks(true); // TODO : add enum labels!
		slider.setShowTickLabels(false);
		slider.setSnapToTicks(true);
		slider.setValue(0);
		slider.setMajorTickUnit(127f);
		slider.setBlockIncrement(127f);
		//g.getChildren().addAll(new Label(label));		
		return slider;
	} 
}
