package controller.component;

import javax.swing.event.EventListenerList;

import controller.event.*;
import javafx.geometry.Orientation;
import javafx.scene.Node;
import javafx.scene.control.*;

/**
 * 
 * @author sydxrey
 *
 */
public abstract class AbstractRotaryEncoder extends Control {

	public AbstractRotaryEncoder(String label) {
		super();
	}

	private EventListenerList listenerList;
	    
	public void addRotaryEncoderChangeListener(RotaryEncoderChangeListener l) {
	     listenerList.add(RotaryEncoderChangeListener.class, l);
	 }

	 public void removeRotaryEncoderChangeListener(RotaryEncoderChangeListener l) {
	     listenerList.remove(RotaryEncoderChangeListener.class, l);
	 }
	 
	/**
	 * Notify all listeners that have registered interest for
	 * notification on this event type.  The event instance
	 * is lazily created using the parameters passed into
	 * the fire method.
	 *
	 * To be called from Pi4J code
	 */
	 protected void fireRotaryEncoderEvent() {
		 
	     // Guaranteed to return a non-null array
	     Object[] listeners = listenerList.getListenerList();
	     
	     // Process the listeners last to first, notifying
	     // those that are interested in this event
	     RotaryEncoderEvent e = null;
	     for (int i = listeners.length-2; i>=0; i-=2) {
	         if (listeners[i]==RotaryEncoderChangeListener.class) {
	             // Lazily create the event:
	             if (e == null) e = new RotaryEncoderEvent(this);
	             ((RotaryEncoderChangeListener)listeners[i+1]).encoderRotated(e); // TODO (reynal) fire changes on EDT!
	         }
	     }
}

	@Override
	public Node getJavaFXView() {
		Slider slider = new Slider(0, 1, 0.5);
		slider.setOrientation(Orientation.VERTICAL);
		slider.setShowTickMarks(true);
		slider.setShowTickLabels(true);
		slider.setMajorTickUnit(0.25f);
		slider.setBlockIncrement(0.1f);		
		return slider;
	} 
	
	
}
