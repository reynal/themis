package model;

import java.util.ArrayList;
import java.util.List;

public class VcfCEM3320 extends AbstractModel {

	private DoubleParameter cutoff, egDepth, kbdTracking, resonance;
	
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
	public double getCutoff() {
		return cutoff.getValue();
	}

	public void setCutoff(double f) {
		this.cutoff.setValue(f);
	}

	public double getEnvGenDepth() {
		return egDepth.getValue();
	}

	public void setEnvGenDepth(double d) {
		this.egDepth.setValue(d);
	}

	public double getKbdTracking() {
		return kbdTracking.getValue();
	}

	public void setKbdTracking(double k) {
		this.kbdTracking.setValue(k);
	}

	public double getResonance() {
		return resonance.getValue();
	}

	public void setResonance(double r) {
		this.resonance.setValue(r);
	}
	
}