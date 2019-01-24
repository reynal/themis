package model;

import java.util.ArrayList;
import java.util.List;

public class VcfCEM3320 {

	private DoubleParameter cutoff, egDepth, kbdTracking, resonance;
	protected final List<SynthParameter<?>> parameterList = new ArrayList<SynthParameter<?>>();
	
	public VcfCEM3320() {
		cutoff = new DoubleParameter("Cutoff", 0, 1, 0.01);
		resonance = new DoubleParameter("Reso", 0, 1, 0.01);
		kbdTracking = new DoubleParameter("Keyboard", 0, 1, 0.01);
		egDepth = new DoubleParameter("EG.Depth", 0, 1, 0.01);
		parameterList.add(cutoff);
		parameterList.add(resonance);
		parameterList.add(kbdTracking);
		parameterList.add(egDepth);
	}
	public DoubleParameter getCutoff() {
		return cutoff;
	}

	public void setCutoff(DoubleParameter cutoff) {
		this.cutoff = cutoff;
	}

	public DoubleParameter getEnvGenDepth() {
		return egDepth;
	}

	public void setEnvGenDepth(DoubleParameter egDepth) {
		this.egDepth = egDepth;
	}

	public DoubleParameter getKbdTracking() {
		return kbdTracking;
	}

	public void setKbdTracking(DoubleParameter kbdTracking) {
		this.kbdTracking = kbdTracking;
	}

	public DoubleParameter getResonance() {
		return resonance;
	}

	public void setResonance(DoubleParameter resonance) {
		this.resonance = resonance;
	}
	
	public List<SynthParameter<?>> getParameters() {
		return parameterList;
	}
}