package model;

import java.util.List;

/** 
 * A model for a VCO based on the CEM or AS3340 device.
 * 
 * @author Bastien Fratta, S. Reynal
 */
public class Vco3340 extends AbstractModel {

	private MIDIParameter detuneParameter;
	private EnumParameter<Octave> octaveParameter;	
	private EnumParameter<WaveShape> waveShapeParameter;
	private MIDIParameter dutyParameter;
	private BooleanParameter syncFrom13700Parameter;
	
	public Vco3340() {
		super();
		parameterList.add(detuneParameter = new MIDIParameter("VCO Detune %"));
		parameterList.add(octaveParameter = new EnumParameter<Octave>(Octave.class, "VCO Octave"));
		parameterList.add(waveShapeParameter = new EnumParameter<WaveShape>(WaveShape.class, "WaveShape"));
		parameterList.add(dutyParameter = new MIDIParameter("Duty"));
		parameterList.add(syncFrom13700Parameter = new BooleanParameter("Sync"));
		for (SynthParameter<?> param : getParameters()) param.addSynthParameterEditListener(e -> System.out.println(e)); // for debug purpose only		
	}

	// ---- value getters and setters --- (write operating may fire change events)
	
	public WaveShape getWaveShape() {
		return waveShapeParameter.getValue();
	}
	
	public void setWaveShape(WaveShape waveshape) {
		this.waveShapeParameter.setValue(waveshape);
	}
	

	public boolean isSyncFrom13700() {
		return syncFrom13700Parameter.getValue();
	}


	public void setSyncFrom13700(boolean syncFrom13700) {
		this.syncFrom13700Parameter.setValue(syncFrom13700);
	}
	
	public double getDuty() {
		return dutyParameter.getValue();
	}

	/**
	 * @param duty b/w 0.0 and 1.0
	 */
	public void setDuty(double duty) {
		this.dutyParameter.setValue((int)(127.0*duty));
	}
	
	public Octave getOctave() { 
		return octaveParameter.getValue();
	}

	public void setOctave(Octave v) {
		octaveParameter.setValue(v);
	}
	
	public double getDetune() {
		return detuneParameter.getValue();
	}

	/**
	 * @param detune b/w -64 and 63 cents
	 */
	public void setDetune(int detune) {
		detuneParameter.setValue((detune + 64));
	}
	
	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
	public MIDIParameter getDetuneParameter() {
		return detuneParameter;
	}
	
	public EnumParameter<Octave> getOctaveParameter(){
		return octaveParameter;
	}	
	
	public BooleanParameter getSyncFrom13700Parameter() {
		return syncFrom13700Parameter;
	}
	
	public EnumParameter<WaveShape> getWaveShapeParameter() {
		return waveShapeParameter;
	}

	public MIDIParameter getDutyParameter() {
		return dutyParameter;
	}


	// ----------- enum -------------
	
	public static enum WaveShape {
		
		SQUARE,
		TRIANGLE,
		PULSE,
		SAWTOOTH;
	}


	
	// ------------ test -------------
	public static void main(String[] args) {
	
		Vco3340 vco1 = new Vco3340();
		List<SynthParameter<?>> paramsVCO1 = vco1.getParameters();
		for (SynthParameter<?> p : paramsVCO1) {
			System.out.println(p);
		}
	}

}


