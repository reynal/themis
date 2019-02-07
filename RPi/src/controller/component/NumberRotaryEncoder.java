package controller.component;

import javafx.geometry.Orientation;
import javafx.scene.Group;
import javafx.scene.Node;
import javafx.scene.control.*;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;

import java.awt.Component;

import javax.swing.JPanel;
import javax.swing.JSlider;

import controller.component.*;

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
		Slider slider = new Slider(0,127,1);
		slider.setOrientation(Orientation.VERTICAL);
		slider.setShowTickMarks(true);
		slider.setShowTickLabels(true);
		slider.setSnapToTicks(true);
		slider.setMajorTickUnit(32f);
		slider.setBlockIncrement(1f);	

		//g.getChildren().addAll(new Label(label));		
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
