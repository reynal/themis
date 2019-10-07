package model;

/**
 * A model for a LFO (aka Low Frequency Oscillator) module. Those modules are usually dedicated
 * to producing various modulations, for example, vibrator or tremolo, thus imparting dynamic
 * variations to the sound.
 * 
 * TODO :
 * - add a "MIDI sync" field that represents a synchronization of the LFO frequency to the MIDI clock
 *  
 * @author reynal
 *
 */
public class LfoModule extends AbstractModule {
	
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
		this.frequencyParameter.setValueFromMIDICode((int)(frequency / FREQ_MIDI_TO_HZ));
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
