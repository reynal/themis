package model;

import model.Vco3340.WaveShape;

/**
 * A model for the CEM or AS3320 VCF
 * @author sydxrey
 *
 */
public class Vcf3320 extends AbstractModel {

	private DoubleParameter cutoffParameter, egDepthParameter, kbdTrackingParameter, resonanceParameter;
	private EnumParameter<FilterOrder> filterOrderParameter;  
	
	public Vcf3320() {
		
		cutoffParameter = new DoubleParameter("Cutoff");
		resonanceParameter = new DoubleParameter("Reso");
		kbdTrackingParameter = new DoubleParameter("Keyboard");
		egDepthParameter = new DoubleParameter("EG.Depth");
		parameterList.add(filterOrderParameter = new EnumParameter<FilterOrder>(FilterOrder.class, "FilterOrder"));
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

	// ----------- enum -------------
	
	public static enum FilterOrder {
		
		SECOND_ORDER,
		FOURTH_ORDER;
	}
	
	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
	/**
	 * @return the cutoffParameter
	 */
	public DoubleParameter getCutoffParameter() {
		return cutoffParameter;
	}

	/**
	 * @return the egDepthParameter
	 */
	public DoubleParameter getEgDepthParameter() {
		return egDepthParameter;
	}

	/**
	 * @return the kbdTrackingParameter
	 */
	public DoubleParameter getKbdTrackingParameter() {
		return kbdTrackingParameter;
	}

	/**
	 * @return the resonanceParameter
	 */
	public DoubleParameter getResonanceParameter() {
		return resonanceParameter;
	}

	/**
	 * @return the filterOrderParameter
	 */
	public EnumParameter<FilterOrder> getFilterOrderParameter() {
		return filterOrderParameter;
	}
	
	
	
	
	
}