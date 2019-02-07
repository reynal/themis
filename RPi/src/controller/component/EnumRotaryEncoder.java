package controller.component;

import javafx.geometry.Orientation;
import javafx.scene.Group;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;

import java.awt.Component;
import java.util.Hashtable;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;

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
	
	public Component createJavaSwingView() {
		JSlider slider = new JSlider(JSlider.VERTICAL);
		Hashtable labelTable = new Hashtable();
		labelTable.put( new Integer(0), new JLabel("Enum") );
		labelTable.put( new Integer(127/3), new JLabel("Une") );
		labelTable.put( new Integer(127*2/3), new JLabel("Est") );
		labelTable.put( new Integer(127), new JLabel("Ceci") );
		slider.setLabelTable(labelTable);
		slider.setMaximum(127);
		slider.setMinimum(0);
		slider.setMajorTickSpacing(32);
		slider.setPaintTicks(true);
		slider.setPaintLabels(true);
		return slider;
	}
}
