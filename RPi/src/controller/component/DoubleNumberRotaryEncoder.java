package controller.component;

import java.awt.Component;

import javax.swing.JPanel;
import javax.swing.JSlider;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.geometry.Orientation;
import javafx.scene.Node;
import javafx.scene.control.Slider;
import model.*;

/**
 * A rotary encoder than can be linked to a DoubleParameter.
 * @author SR
 */
public class DoubleNumberRotaryEncoder extends AbstractRotaryEncoder {

	public DoubleNumberRotaryEncoder(String label) {
		super(label);
	}

	public Node createJavaFXView() {
		Slider slider = new Slider(0,127,1);
		slider.setOrientation(Orientation.VERTICAL);
		slider.setShowTickMarks(true);
		slider.setShowTickLabels(true);
		slider.setSnapToTicks(true);
		slider.setMajorTickUnit(1f);
		slider.setBlockIncrement(1f);
		return slider;
	}
	
	public Component createJavaSwingView() {
		JSlider slider = new JSlider(JSlider.VERTICAL);
		slider.setMaximum(127);
		slider.setMinimum(0);
		slider.setMajorTickSpacing(32);
		return slider;
	}
	
}
