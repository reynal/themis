package model;

/**
 * A model for a Quad VCA (voltage control amplifier, aka "mixer") module based on a V2140 device.
 * 
 * @author reynal
 *
 */
public class V2140QuadVCAModule extends AbstractModule {

	private MIDIParameter gain1dBParameter,gain2dBParameter, gain3dBParameter, gain4dBParameter;
	
	public static final double GAIN_MIDI_TO_dB = 0.5;

	/**
	 * Builds a mixer model with default channel values
	 */
	public V2140QuadVCAModule(){
		this("Gain 1", "Gain 2", "Gain 3", "Gain 4");
	}
	
	/**
	 * @param lbl1 first channel label
	 * @param lbl2 second channel label
	 * @param lbl3 third channel label
	 * @param lbl4 fourth channel label
	 */
	public V2140QuadVCAModule(String lbl1, String lbl2, String lbl3, String lbl4){
		super();
		parameterList.add(gain1dBParameter = new MIDIParameter(lbl1)); 
		parameterList.add(gain2dBParameter = new MIDIParameter(lbl2));
		parameterList.add(gain3dBParameter = new MIDIParameter(lbl3));
		parameterList.add(gain4dBParameter = new MIDIParameter(lbl4));
	}
	
	// ---- value getters and setters --- (write operating may fire change events)
	
	public double getGain1dB() {
		return gain1dBParameter.getValue() * GAIN_MIDI_TO_dB;
	}
	public void setGain1dB(double gain1dB) {
		this.gain1dBParameter.setValue((int)(gain1dB / GAIN_MIDI_TO_dB));
	}
	public double getGain2dB() {
		return gain2dBParameter.getValue() * GAIN_MIDI_TO_dB;
	}
	public void setGain2dB(double gain2dB) {
		this.gain2dBParameter.setValue((int)(gain2dB / GAIN_MIDI_TO_dB));
	}
	public double getGain3dB() {
		return gain3dBParameter.getValue() * GAIN_MIDI_TO_dB;
	}
	public void setGain3dB(double gain3dB) {
		this.gain3dBParameter.setValue((int)(gain3dB / GAIN_MIDI_TO_dB));
	}
	public double getGain4dB() {
		return gain4dBParameter.getValue() * GAIN_MIDI_TO_dB;
	}
	public void setGain4dB(double gain4dB) {
		this.gain4dBParameter.setValue((int)(gain4dB / GAIN_MIDI_TO_dB));
	}



	// ---- SynthParameter getters ---- (write access is forbidden so as to maintain the associated listener mechanism integrity)
	
	/**
	 * @return the gain1dBParameter
	 */
	public MIDIParameter getGain1dBParameter() {
		return gain1dBParameter;
	}

	/**
	 * @return the gain2dBParameter
	 */
	public MIDIParameter getGain2dBParameter() {
		return gain2dBParameter;
	}

	/**
	 * @return the gain3dBParameter
	 */
	public MIDIParameter getGain3dBParameter() {
		return gain3dBParameter;
	}

	/**
	 * @return the gain4dBParameter
	 */
	public MIDIParameter getGain4dBParameter() {
		return gain4dBParameter;
	}
	
}
