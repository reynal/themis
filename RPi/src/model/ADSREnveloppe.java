package model;


/**
 * This class instantiate every parameters of a ADSR type module and create a list of those parameters
 * As long as there is one type of ADSR every of its parameters is set here.
 * 
 * @author Bastien Fratta
 */

public class ADSREnveloppe extends AbstractModel {
	
	private MIDIParameter attackMsParameter, decayMsParameter, sustainLevelParameter, releaseMsParameter;
	//what about attackLevel or decayLevel ?
	
	public static final double ATTACK_MIDI_TO_MS = 50.;
	public static final double DECAY_MIDI_TO_MS = 50.;
	public static final double RELEASE_MIDI_TO_MS = 50.;
	
	// list of label constant for use by clients:
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
		this.attackMsParameter.setValueAsMIDICode((int)(attackMs/ATTACK_MIDI_TO_MS));
	}

	public double getDecayMs() {
		return DECAY_MIDI_TO_MS*decayMsParameter.getValueAsMIDICode();
	}

	public void setDecayMs(double decayMs) {
		this.decayMsParameter.setValueAsMIDICode((int)(decayMs/DECAY_MIDI_TO_MS));
	}

	public double getSustainLevel() {
		return sustainLevelParameter.getValueAsMIDICode() / 127.0;
	}

	public void setSustainLevel(double sustainLevel) {
		this.sustainLevelParameter.setValueAsMIDICode((int)(sustainLevel * 127.0));
	}

	public double getReleaseMs() {
		return RELEASE_MIDI_TO_MS*releaseMsParameter.getValueAsMIDICode();
	}

	public void setReleaseMs(double releaseMs) {
		this.releaseMsParameter.setValueAsMIDICode((int)(releaseMs/RELEASE_MIDI_TO_MS));
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
