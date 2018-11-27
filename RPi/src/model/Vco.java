package model;

import java.util.*;

import controller.FrontPane;
import controller.PerformancePad;
import controller.component.Control;
import controller.component.PushButton;
import controller.event.PushButtonActionEvent;
import controller.event.PushButtonActionListener;

/**
 * 
 * @author Bastien Fratta
 *
 */
public class Vco implements PushButtonActionListener, SynthParameterProvider {
	
	protected DoubleParameter detune;
	protected SynthParameter waveform; //TODO: syd create EnumParameter class
	protected SynthParameter octave;
	
	Vco(){
		detune = new DoubleParameter();
		//octave = Octave.FOUR_INCHES;
		/*PushButton pushBut = PerformancePad.getPerformancePadPushButton(5, 4);
		pushBut.addPushButtonActionListener(this);
		*/
		
	}
	
	public SynthParameter getOctave() { // TODO Sylvain utiliser une enum
		return octave;
	}

	public void setOctave(SynthParameter octave) {
		this.octave = octave;
	}
	
	public void setDetune(DoubleParameter detune) {
		this.detune = detune;
	}
	
	public DoubleParameter getDetune() {
		return detune;
	}
	
	public void setWaveform(SynthParameter waveform) {
		this.waveform = waveform;
	}
	
	public SynthParameter getWaveform() {
		return waveform;
	}

	@Override
	public void actionPerformed(PushButtonActionEvent e) {
		// TODO Auto-generated method stub
		
		
	}

	@Override
	public List<SynthParameter> getParameters() {
		List<SynthParameter> l =new ArrayList<SynthParameter>();
		l.add(detune);
		//l.add(waveform);
		return l;
	}
	
}