package view.component;

import javax.swing.JComponent;

import device.*;

/**
 * Base abstract class for View's based on the IS31FL3731 device. 
 * @author sydxrey
 *
 */
public abstract class AbstractView implements View {

	public final int LED_MIN_VALUE = 0;
	public final int LED_MAX_VALUE = 7;
	public final int MIDI_MIN_VALUE = 0;
	public final int MIDI_MAX_VALUE = 127;
	public final int HIGH = 1;
	public final int LOW = 0;

	protected IS31FL3731 is31fl3731;

	public AbstractView(IS31FL3731 is31fl3731) { //if we want to impose the state
		
		super();
		this.is31fl3731 = is31fl3731;
	}	
	
	public abstract JComponent getUIForSimulator();
	
}
