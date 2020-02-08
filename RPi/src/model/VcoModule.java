package model;

import java.util.logging.Logger;

/**
 * A generic model for a VCO (aka Voltage Controled Oscillator) module.
 * 
 * @author reynal
 *
 */
public abstract class VcoModule extends AbstractModule {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	protected MIDIParameter detuneParameter;
	protected EnumParameter<Octave> octaveParameter;
	protected EnumParameter<Semitones> semitonesParameter;

	public static final String DETUNE = "Detune";
	public static final String OCTAVE = "Octave";
	public static final String SEMITONES = "Semitones";
	
	public VcoModule() {
		super();
		parameterList.add(detuneParameter = new MIDIParameter(getVcoName() + DETUNE));
		parameterList.add(octaveParameter = new EnumParameter<Octave>(Octave.class, getVcoName() + OCTAVE));
		parameterList.add(semitonesParameter = new EnumParameter<Semitones>(Semitones.class, getVcoName() + SEMITONES));
		
		// debug:
		detuneParameter.addChangeListener(e -> LOGGER.info(e.toString())); 
		octaveParameter.addChangeListener(e -> LOGGER.info(e.toString()));
	}
	
	protected abstract String getVcoName();
	
	// ---- value getters and setters --- (write operating may fire change events)
	
	public Octave getOctave() { 
		return octaveParameter.getValue();
	}

	public void setOctave(Octave v) {
		octaveParameter.setValue(v);
	}
	
	public int getDetune() {
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
	
	public EnumParameter<Semitones> getSemitonesParameter(){
		return semitonesParameter;
	}	
		
}
