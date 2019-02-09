package model;

import java.util.ArrayList;
import java.util.List;

public class VcfCEM3320 extends AbstractModel {

	private DoubleParameter cutoffParameter, egDepthParameter, kbdTrackingParameter, resonanceParameter;
	
	public VcfCEM3320() {
		
		cutoffParameter = new DoubleParameter("Cutoff", 0, 1, 0.01);
		resonanceParameter = new DoubleParameter("Reso", 0, 1, 0.01);
		kbdTrackingParameter = new DoubleParameter("Keyboard", 0, 1, 0.01);
		egDepthParameter = new DoubleParameter("EG.Depth", 0, 1, 0.01);
		parameterList.add(cutoffParameter);
		parameterList.add(resonanceParameter);
		parameterList.add(kbdTrackingParameter);
		parameterList.add(egDepthParameter);
	}
	
	// ---- value getters and setters --- (write operating may fire change events)
	
	public double getCutoff() {
		return cutoffParameter.getValue();
	}

	public void setCutoff(double f) {
		this.cutoffParameter.setValue(f);
	}

	public double getEnvGenDepth() {
		return egDepthParameter.getValue();
	}

	public void setEnvGenDepth(double d) {
		this.egDepthParameter.setValue(d);
	}

	public double getKbdTracking() {
		return kbdTrackingParameter.getValue();
	}

	public void setKbdTracking(double k) {
		this.kbdTrackingParameter.setValue(k);
	}

	public double getResonance() {
		return resonanceParameter.getValue();
	}

	public void setResonance(double r) {
		this.resonanceParameter.setValue(r);
	}
	
	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
	
	
}