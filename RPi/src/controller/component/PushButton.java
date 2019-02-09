package controller.component;

import java.awt.Component;
import javax.swing.JButton;
import java.awt.event.ActionEvent;
import javax.swing.*;
import controller.event.*;
import javafx.scene.Node;
import javafx.scene.control.*;

/**
 * a class that represents a push button on the front pane
 * 
 * @author SR
 *
 */
public class PushButton extends Control {

	/**
	 * construct a pushbutton with the given UI label
	 */
	public PushButton(String label) {
		super(label);
	}

	/**
	 * Adds the specified listener to receive action events from this button.
	 * 
	 * @param l the listener
	 */
	public void addPushButtonActionListener(PushButtonActionListener l) {
		listenerList.add(PushButtonActionListener.class, l);
	}

	/**
	 * Removes the specified listener so that it no longer receives action events
	 * from this button.
	 * 
	 * @param l
	 *            the listener that was previously added
	 */
	public void removePushButtonActionListener(PushButtonActionListener l) {
		listenerList.remove(PushButtonActionListener.class, l);
	}

	/**
	 * Notify all listeners that have registered interest for notification on this
	 * event type. The event instance is lazily created using the parameters passed
	 * into the fire method.
	 *
	 */
	protected void firePushButtonActionEvent() {

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
				((PushButtonActionListener) listeners[i + 1]).actionPerformed(e); 
			}
		}
	}

	@Override
	public Node createJavaFXView() {
		Button b = new Button();
		b.setMinSize(20.0,20.0);
       	b.setStyle("-fx-background-color : white;");
		return b;
	}

	@Override
	public JComponent createSwingView() {
		JButton b = new JButton("Push");
		b.addActionListener(new VirtualEncoderChangeListener());
		return b;
	}
	
	 /**
	  * Listens to change event coming from the simulator UI ; this is just an event forwarder
	  * to RotaryEncoderChangeListener's.
	  * @author sydxrey
	  */
	protected class VirtualEncoderChangeListener implements java.awt.event.ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			firePushButtonActionEvent();
		}
	}	
	

	public Component createJavaSwingView() {
		JButton b = new JButton();
		return b;
	}
}
