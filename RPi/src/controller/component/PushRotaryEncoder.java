package controller.component;

import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JSlider;

import javax.swing.*;
import controller.event.*;
import javafx.geometry.Orientation;
import javafx.scene.*;
import javafx.scene.control.*;

/**
 * a quadratic encoder with push capability
 * 
 * @author SR
 *
 */
public class PushRotaryEncoder extends RotaryEncoder {

	public PushRotaryEncoder(String label) {
		super(label);
	}

	public void addPushButtonActionListener(PushButtonActionListener l) {
		listenerList.add(PushButtonActionListener.class, l);
	}

	public void removePushButtonActionListener(PushButtonActionListener l) {
		listenerList.remove(PushButtonActionListener.class, l);
	}

	/**
	 * Notify all listeners that have registered interest for notification on this
	 * event type. The event instance is lazily created using the parameters passed
	 * into the fire method.
	 *
	 */
	protected void fireEncoderPushedEvent(boolean state) {

		// Guaranteed to return a non-null array
		Object[] listeners = listenerList.getListenerList();

		// Process the listeners last to first, notifying
		// those that are interested in this event
		PushButtonActionEvent e = null;
		for (int i = listeners.length - 2; i >= 0; i -= 2) {
			if (listeners[i] == PushButtonActionListener.class) {
				// Lazily create the event:
				if (e == null)
					e = new PushButtonActionEvent(this);
				((PushButtonActionListener) listeners[i + 1]).actionPerformed(e); // TODO (reynal) fire changes on EDT!
			}
		}
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
		g.getChildren().addAll(new Button(label));		
		return g;
		
	}

	@Override
	public JComponent createSwingView() {
		JPanel panel = new JPanel();
		JButton button = new JButton();
		JSlider slider = new JSlider(JSlider.VERTICAL);
		slider.setMaximum(127);
		slider.setMinimum(0);
		slider.setMajorTickSpacing(32);	
		panel.add(slider);
		panel.add(button);	
		return panel;
	} 
	
}
