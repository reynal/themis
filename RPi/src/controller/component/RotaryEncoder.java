package controller.component;

import java.awt.Color;
import java.awt.Component;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.io.IOException;

import javax.swing.*;
import javax.swing.event.EventListenerList;

import com.pi4j.io.gpio.PinState;

import device.MCP23017.InterruptEvent;
import device.MCP23017.InterruptListener;
import device.MCP23017;
import controller.event.*;
import javafx.geometry.Orientation;
import javafx.scene.Node;
import javafx.scene.control.Slider;
import com.pi4j.io.gpio.*;

/**
 * A physical quadratic encoder that can fire UP or DOWN change events upon rotation.
 * 
 * @author SR
 * 
 */
public class RotaryEncoder extends Control  {

	// labels for UI buttons
	static final String CW_LBL = "->";
	static final String CCW_LBL = "<-";
	
	private MCP23017.Pin channelA; // pin GPIO entree A sur MCP23017
	private MCP23017.Pin channelB; // pin GPIO entree B sur MCP23017

	private PinState levelA; // dernier niveau logique enregistré sur entree A
	private PinState levelB; // dernier niveau logique enregistré sur entree B
	private Channel previousTriggeringChannel; // dernier port ayant changé d'état (soit gpioA soit gpioB ; pour le debounce)
	public static enum Channel {A,B};
	
	
	/**
	 * 
	 * @param mcpDevice le MCP23017 sur lequel sont connectées les broches de l'encoder
	 * @param gpioA broche (port A) du MCP32017 connectée à la sortie A de l'encodeur
	 * @param gpioB broche (port A) du MCP32017 connectée à la sortie B de l'encodeur
	 * @param gpioIntRpi broche de la RPi connectée à la sortie INTA du MCP23017
	 * @throws IOException 
	 */
	public RotaryEncoder(String label, MCP23017 mcpDevice, MCP23017.Pin gpioA, MCP23017.Pin gpioB) throws IOException{

		super(label);
		this.channelA = gpioA;
		this.channelB = gpioB;
		levelA=PinState.LOW;
		levelB=PinState.LOW;
		previousTriggeringChannel=null;
		listenerList = new EventListenerList();

		mcpDevice.setInput(MCP23017.Port.A); // encoder
		mcpDevice.setPullupResistors(MCP23017.Port.A, true); // Port A pull up enabled (le bouton doit connecter le port a la masse)
		mcpDevice.setInterruptOnChange(MCP23017.Port.A, true); // Port A : enables GPIO input pin for interrupt-on-change
		mcpDevice.addInterruptListener(new PhysicalEncoderChangeListener());
	}
	
	
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
	  * 	Listens to changes event coming from a real encoder through the MCP23017 GPIO expander
	  * @author sydxrey
	  */
	 class PhysicalEncoderChangeListener implements InterruptListener {
		 
		/**
		 * Callback lorsque la pin INTA du MCP23017 est asserted ; signifie qu'une des pins du PORT A a changé, donc qu'un encodeur a tourné, mais pas forcément celui-ci !
		 * First item: vérifier que c'est bien cet encodeur qui a tourné ! 
		 */
		@Override
		public void interruptOccured(InterruptEvent event) {

			//System.out.println(event);

			// on cherche à savoir quel GPx du MCP23017 a bougé :
			Channel triggeringChannel; // quel canal a changé, A ou B ?
			if (event.getPin() == channelA) { // C'est A qui a changé
				triggeringChannel = Channel.A;
				levelA = event.getLevel();
			} else if (event.getPin() == channelB) { // C'est B qui a changé
				triggeringChannel = Channel.B;
				levelB = event.getLevel();
			}
			else return; // ok c'était pas pour nous !!!

			//System.out.printf("mcp23017_int_handler:\tINTF=%02X \t GPIO input = %02X\t gpioMCP23017=%d \t levelMCP32017=%d\n", intfRegister, captureRegister, gpioMCP23017, channelLevel); // also clears INTB flag ce qui va provoquer le re-appel du handler mais avec level=1

			// debounce code: si c'est le premier front sur le nouvel encodeur (A ou B) on execute le code suivant
			// car sinon c'est que c'est un rebond sur le meme GPIO (auquel cas on fait rien)
			// en d'autres termes, on ne prend en compte que le premier front sur tout nouvel encoder
			// CW : 11 -> 01 -> 00 -> 10
			// CCW : 11 -> 10 -> 00 -> 01

			if (triggeringChannel != previousTriggeringChannel){ // c'est pas un rebond !
				previousTriggeringChannel = triggeringChannel;
				if ((triggeringChannel == Channel.A) && (levelA == PinState.LOW) && (levelB==PinState.LOW)){ 
					//++position; // transition 10 -> 00
					fireRotaryEncoderEvent(RotaryEncoderDirection.UP);
				}
				else if ((triggeringChannel == Channel.B) && (levelB == PinState.HIGH) && (levelA==PinState.HIGH)){ 
					//--position; // transition 10 -> 11
					fireRotaryEncoderEvent(RotaryEncoderDirection.DOWN);
				}
				//System.out.println("pos="+position);
			}
		}	 
	 }

	 /**
	  * Listens to change event coming from the simulator UI ; this is just an event forwarder
	  * to RotaryEncoderChangeListener's.
	  * @author sydxrey
	  */
	class VirtualEncoderChangeListener implements java.awt.event.ActionListener {

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
