package model;

import java.util.*;

//import controller.FrontPane;
//import controller.PerformancePad;
//import controller.component.Control;
//import controller.component.PushButton;
import controller.event.PushButtonActionEvent;
import controller.event.PushButtonActionListener;

/**
 * 
 * @author Bastien Fratta
 *
 */
public abstract class Vco extends AbstractModel implements PushButtonActionListener {
	
	protected DoubleParameter detune;
	protected EnumParameter<Octave> octave;
	
	/**
	 * 
	 */
	public Vco(){
		parameterList.add(detune = new DoubleParameter("VCO Detune %", -5, 5, 0.1));
		parameterList.add(octave = new EnumParameter<Octave>(Octave.class, "VCO Octave"));
	}
	
	public EnumParameter<Octave> getOctave() { 
		return octave;
	}

	public void setOctave(EnumParameter<Octave> octave) {
		this.octave = octave;
	}
	
	public DoubleParameter getDetune() {
		return detune;
	}

	public void setDetune(DoubleParameter detune) {
		this.detune = detune;
	}
	
	

	@Override
	public void actionPerformed(PushButtonActionEvent e) {
		// TODO Auto-generated method stub
		
		
	}
	
}