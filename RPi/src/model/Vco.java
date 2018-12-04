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
public abstract class Vco implements PushButtonActionListener, SynthParameterProvider {
	
	protected DoubleParameter detune;
	protected EnumParameter<Octave> octave;
	protected final List<SynthParameter<?>> parameterList = new ArrayList<SynthParameter<?>>();
	
	/**
	 * 
	 */
	public Vco(){
		detune = new DoubleParameter("VCO Detune");
		octave = new EnumParameter<Octave>("VCO Octave");
		parameterList.add(detune);
		parameterList.add(octave);

		/*PushButton pushBut = PerformancePad.getPerformancePadPushButton(5, 4);
		pushBut.addPushButtonActionListener(this);
		*/
		
	}
	
	public EnumParameter<Octave> getOctave() { // TODO Sylvain utiliser une enum
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

	@Override
	public List<SynthParameter<?>> getParameters() {		
		return parameterList;
	}
	
}