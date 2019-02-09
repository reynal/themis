package controller.component;

import java.awt.Color;
import java.awt.Component;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;

import javax.swing.*;

import controller.event.*;
import javafx.geometry.Orientation;
import javafx.scene.Node;
import javafx.scene.control.Slider;

/**
 * A physical quadratic encoder that can fire UP or DOWN change events upon rotation.
 * 
 * @author SR
 * 
 */
public class RotaryEncoder extends Control {

	// labels for UI buttons
	static final String CW_LBL = "->";
	static final String CCW_LBL = "<-";
	
	
	public RotaryEncoder(String label) {
		super(label);
	}

	/**
	 * Adds the specified listener to receive action events from this encoder.
	 * @param l the listener
	 */
	public void addRotaryEncoderChangeListener(RotaryEncoderChangeListener l) {
	     listenerList.add(RotaryEncoderChangeListener.class, l);
	 }

	/**
	 * Removes the specified listener so that it no longer receives action events from this encoder.
	 * @param l the listener that was previously added
	 */
	 public void removeRotaryEncoderChangeListener(RotaryEncoderChangeListener l) {
	     listenerList.remove(RotaryEncoderChangeListener.class, l);
	 }
	 
	/**
	 * Notify all listeners that have registered interest for
	 * notification on this event type.  The event instance
	 * is lazily created using the parameters passed into
	 * the fire method.
	 */
	 protected void fireRotaryEncoderEvent(RotaryEncoderDirection dir) {
		 
	     // Guaranteed to return a non-null array
	     Object[] listeners = listenerList.getListenerList();
	     
	     // Process the listeners last to first, notifying
	     // those that are interested in this event
	     RotaryEncoderEvent e = null;
	     for (int i = listeners.length-2; i>=0; i-=2) {
	         if (listeners[i]==RotaryEncoderChangeListener.class) {
	             // Lazily create the event:
	             if (e == null) e = new RotaryEncoderEvent(this, dir);
	             ((RotaryEncoderChangeListener)listeners[i+1]).encoderRotated(e); // TODO (reynal) fire changes on EDT!
	         }
	     }
	 }

	 /**
	  * Listens to change event coming from the simulator UI ; this is just an event forwarder
	  * to RotaryEncoderChangeListener's.
	  * @author sydxrey
	  */
	protected class VirtualEncoderChangeListener implements java.awt.event.ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			if (e.getActionCommand().equals(CW_LBL))
				fireRotaryEncoderEvent(RotaryEncoderDirection.UP);
			else if (e.getActionCommand().equals(CCW_LBL))
				fireRotaryEncoderEvent(RotaryEncoderDirection.DOWN);
			
		}
	}	
	
	@Override
	public Node createJavaFXView() {
		Slider slider = new Slider(0,127,1);
		//slider.valueProperty().addListener(new VirtualEncoderChangeListener());
		slider.setOrientation(Orientation.VERTICAL);
		slider.setShowTickMarks(true);
		slider.setShowTickLabels(true);
		slider.setSnapToTicks(true);
		slider.setMajorTickUnit(1f);
		slider.setBlockIncrement(1f);
		return slider;
	}

	
	
	@Override
	public JComponent createSwingView() {
		JPanel p = new JPanel();
		p.setBackground(Color.black);
		p.setLayout(new GridLayout(1,2));
		JButton butMinus = new JButton(CCW_LBL);
		p.add(butMinus);
		JButton butPlus = new JButton(CW_LBL);
		p.add(butPlus);
		butPlus.addActionListener(new VirtualEncoderChangeListener());
		butMinus.addActionListener(new VirtualEncoderChangeListener());
		return p;
	}

	
	// TODO : registerComponent(MCP23017 mcpDevice, int pintA, int pinB){...}
	
	
}
