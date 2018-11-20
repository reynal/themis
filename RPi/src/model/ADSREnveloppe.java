package model;


/**
 * Contains the parameters for an ADSR enveloppe, Attack, Decay, Sustain, Release
 *
 * @author sydxrey
 *
 */
public class ADSREnveloppe {
	
	private double attackMs, decayMs, sustainLevel, releaseMs;
	//what about attackLevel or decayLevel ?

	public double getAttackMs() {
		return attackMs;
	}

	public void setAttackMs(double attackMs) {
		this.attackMs = attackMs;
	}

	public double getDecayMs() {
		return decayMs;
	}

	public void setDecayMs(double decayMs) {
		this.decayMs = decayMs;
	}

	public double getSustainLevel() {
		return sustainLevel;
	}

	public void setSustainLevel(double sustainLevel) {
		this.sustainLevel = sustainLevel;
	}

	public double getReleaseMs() {
		return releaseMs;
	}

	public void setReleaseMs(double releaseMs) {
		this.releaseMs = releaseMs;
	}
	
	
	
}
