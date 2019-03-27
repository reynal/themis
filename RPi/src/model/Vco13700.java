package model;


/**
 * Model for a VCO based on the LM13700 OTA.
 * 
 * @author Bastien Fratta, S. Reynal
 *
 */
public class Vco13700 extends AbstractModel {

	private MIDIParameter detuneParameter;
	private EnumParameter<Octave> octaveParameter;	
	private MIDIParameter waveShapeParameter;
	
	public static final String DETUNE = "VCO13700 Detune";
	public static final String OCTAVE = "VCO13700 Octave";
	public static final String WAVE = "VCO13700 WaveShape";
	
	public Vco13700() {
		super();
		parameterList.add(detuneParameter = new MIDIParameter("VCO13700 Detune"));
		parameterList.add(octaveParameter = new EnumParameter<Octave>(Octave.class, "VCO13700 Octave"));
		parameterList.add(waveShapeParameter = new MIDIParameter("VCO13700 WaveShape"));
		for (SynthParameter<?> param : getParameters()) param.addSynthParameterEditListener(e -> System.out.println(e)); // for debug purpose only		
	}
		
	enum WaveShape {
		
		SQUARE,
		TRIANGLE;
	}

	// ---- value getters and setters --- (write operating may fire change events)
	
	public int getWaveShape() {
		return waveShapeParameter.getValueAsMIDICode();
	}
	
	public void setWaveShape(int waveshape) {
		waveShapeParameter.setValue(waveshape);
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
		
	public MIDIParameter getWaveShapeParameter() {
		return waveShapeParameter;
	}

}
