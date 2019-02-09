package model;


/**
 * This class instantiate every parameters of a ADSR type module and create a list of those parameters
 * As long as there is one type of ADSR every of its parameters is set here.
 * 
 * @author Bastien Fratta
 */

public class ADSREnveloppe extends AbstractModel {
	
	private DoubleParameter attackMs, decayMs, sustainLevel, releaseMs;
	//what about attackLevel or decayLevel ?

	public ADSREnveloppe() {

		attackMs = new DoubleParameter("Attack", 0, 200, 0.1);
		decayMs = new DoubleParameter("Decay", 0, 0, 0);
		sustainLevel = new DoubleParameter("Sustain", 0, 0, 0);
		releaseMs = new DoubleParameter("Release", 0, 0, 0);
		parameterList.add(attackMs);
		parameterList.add(decayMs);
		parameterList.add(sustainLevel);
		parameterList.add(releaseMs);
	}
	
	// ---- value getters and setters --- (write operating may fire change events)
	
	public double getAttackMs() {
		return attackMs.getValue();
	}

	public void setAttackMs(double attackMs) {
		this.attackMs.setValue(attackMs);
	}

	public double getDecayMs() {
		return decayMs.getValue();
	}

	public void setDecayMs(double decayMs) {
		this.decayMs.setValue(decayMs);
	}

	public DoubleParameter getSustainLevel() {
		return sustainLevel;
	}

	public void setSustainLevel(DoubleParameter sustainLevel) {
		this.sustainLevel = sustainLevel;
	}

	public DoubleParameter getReleaseMs() {
		return releaseMs;
	}

	public void setReleaseMs(DoubleParameter releaseMs) {
		this.releaseMs = releaseMs;
	}


	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
	
}