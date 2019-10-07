package model;


/**
 * A model for an ADSR (aka Attack-Decay-Sustain-Release) envelope generator. Traditionally these envelopes are the 
 * basic elements that produce the variations of texture and amplitude of generated sounds (the so-called "timbre").
 *  
 * @links http://youtu.be/BLoM9bBr8lc A nice tutorial on what is a timbre and an ADSR envelope.
 * @links https://en.wikipedia.org/wiki/Envelope_(music) the ADSR page on wikipedia.
 * 
 * @see dac_board.c source file in the STM32 project contains useful constants
 * 
 * @author Bastien Fratta
 * @author Reynal
 */

public class ADSREnveloppe extends AbstractModule {
	
	private MIDIParameter attackMsParameter, decayMsParameter, sustainLevelParameter, releaseMsParameter;
	
	// should be coherent with STM32 code!!! TODO: how could we "synchronize" these data b/w both projects so that any change there is automatically reflected here?
	public static final double ATTACK_MIDI_TO_MS = 50.;
	public static final double DECAY_MIDI_TO_MS = 50.;
	public static final double RELEASE_MIDI_TO_MS = 50.;
	
	// list of label constants for use by clients:
	public static final String ATTACK = "Attack";
	public static final String DECAY = "Decay";
	public static final String SUSTAIN = "Sustain";
	public static final String RELEASE = "Release";

	

	public ADSREnveloppe() {

		attackMsParameter = new MIDIParameter(ATTACK);
		decayMsParameter = new MIDIParameter(DECAY);
		sustainLevelParameter = new MIDIParameter(SUSTAIN);
		releaseMsParameter = new MIDIParameter(RELEASE);
		parameterList.add(attackMsParameter);
		parameterList.add(decayMsParameter);
		parameterList.add(sustainLevelParameter);
		parameterList.add(releaseMsParameter);
		
	}
	
	// ---- value getters and setters --- (write operating may fire change events)
	
	public double getAttackMs() {
		return ATTACK_MIDI_TO_MS * attackMsParameter.getValueAsMIDICode();
	}

	public void setAttackMs(double attackMs) {
		this.attackMsParameter.setValueFromMIDICode((int)(attackMs/ATTACK_MIDI_TO_MS));
	}

	public double getDecayMs() {
		return DECAY_MIDI_TO_MS*decayMsParameter.getValueAsMIDICode();
	}

	public void setDecayMs(double decayMs) {
		this.decayMsParameter.setValueFromMIDICode((int)(decayMs/DECAY_MIDI_TO_MS));
	}

	public double getSustainLevel() {
		return sustainLevelParameter.getValueAsMIDICode() / 127.0;
	}

	public void setSustainLevel(double sustainLevel) {
		this.sustainLevelParameter.setValueFromMIDICode((int)(sustainLevel * 127.0));
	}

	public double getReleaseMs() {
		return RELEASE_MIDI_TO_MS*releaseMsParameter.getValueAsMIDICode();
	}

	public void setReleaseMs(double releaseMs) {
		this.releaseMsParameter.setValueFromMIDICode((int)(releaseMs/RELEASE_MIDI_TO_MS));
	}

	

	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
	public MIDIParameter getAttackMsParameter(){
		return attackMsParameter;
	}

	public MIDIParameter getDecayMsParameter(){
		return decayMsParameter;
	}
	
	public MIDIParameter getSustainLevelParameter(){
		return sustainLevelParameter;
	}
	
	public MIDIParameter getReleaseMsParameter(){
		return releaseMsParameter;
	}
	
}
