package model;

/**
 * A model for the CEM or AS3320 VCF
 * @author sydxrey
 *
 */
public class Vcf3320 extends AbstractModel {

	private DoubleParameter cutoffParameter, egDepthParameter, kbdTrackingParameter, resonanceParameter;
	
	public Vcf3320() {
		
		cutoffParameter = new DoubleParameter("Cutoff");
		resonanceParameter = new DoubleParameter("Reso");
		kbdTrackingParameter = new DoubleParameter("Keyboard");
		egDepthParameter = new DoubleParameter("EG.Depth");
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