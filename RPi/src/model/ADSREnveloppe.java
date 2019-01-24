package model;

import java.util.ArrayList;
import java.util.List;

/**
 * This class instantiate every parameters of a ADSR type module and create a list of those parameters
 * As long as there is one type of ADSR every of its parameters is set here.
 * 
 * @author Bastien Fratta
 */

public class ADSREnveloppe {
	
	protected final List<SynthParameter<?>> parameterList = new ArrayList<SynthParameter<?>>();
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
	
	public DoubleParameter getAttackMs() {
		return attackMs;
	}

	public void setAttackMs(DoubleParameter attackMs) {
		this.attackMs = attackMs;
	}

	public DoubleParameter getDecayMs() {
		return decayMs;
	}

	public void setDecayMs(DoubleParameter decayMs) {
		this.decayMs = decayMs;
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

	public List<SynthParameter<?>> getParameters() {
		return parameterList;
	}
	
	
	
}