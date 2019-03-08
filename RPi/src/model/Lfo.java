package model;

public class Lfo extends AbstractModel {
	
	private MIDIParameter frequencyParameter;
	private EnumParameter<WaveShape> waveShapeParameter;
	
	public static final double FREQ_MIDI_TO_HZ = 0.1;
	
	enum WaveShape {
		
		SQUARE,
		TRIANGLE,
		PULSE,
		SAWTOOTH;
	}
		
	// ---- value getters and setters --- (write operating may fire change events)
	
	public double getFrequency() {
		return frequencyParameter.getValueAsMIDICode() * FREQ_MIDI_TO_HZ;
	}

	public void setFrequency(double frequency) {
		this.frequencyParameter.setValueAsMIDICode((int)(frequency / FREQ_MIDI_TO_HZ));
	}

	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
	/**
	 * @return the frequencyParameter
	 */
	public MIDIParameter getFrequencyParameter() {
		return frequencyParameter;
	}

	/**
	 * @return the waveShapeParameter
	 */
	public EnumParameter<WaveShape> getWaveShapeParameter() {
		return waveShapeParameter;
	}
	
	
	
}
