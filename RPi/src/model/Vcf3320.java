package model;


/**
 * A model for the CEM or AS3320 VCF
 * @author sydxrey
 *
 */
public class Vcf3320 extends AbstractModel {

	private MIDIParameter cutoffParameter, egDepthParameter, kbdTrackingParameter, resonanceParameter;
	private EnumParameter<FilterOrder> filterOrderParameter;  
	private ADSREnveloppe adsrEnveloppe;
	
	// list of label constant for use by clients:
	public static final String CUTOFF = "VCO3320 Cutoff";
	public static final String RESO = "VCO3320 Reso";
	public static final String KBD = "VCO3320 Keyboard";
	public static final String EG_DEPTH = "VCO3320 EG.Depth";
	
	public Vcf3320() {
		
		cutoffParameter = new MIDIParameter(CUTOFF);
		resonanceParameter = new MIDIParameter(RESO);
		kbdTrackingParameter = new MIDIParameter(KBD);
		egDepthParameter = new MIDIParameter(EG_DEPTH);
		adsrEnveloppe = new ADSREnveloppe();
		parameterList.addAll(adsrEnveloppe.getParameters());
		parameterList.add(filterOrderParameter = new EnumParameter<FilterOrder>(FilterOrder.class, "FilterOrder"));
		parameterList.add(cutoffParameter);
		parameterList.add(resonanceParameter);
		parameterList.add(kbdTrackingParameter);
		parameterList.add(egDepthParameter);
		for (SynthParameter<?> param : getParameters()) param.addSynthParameterEditListener(e -> System.out.println(e));  // for debug purpose only

	}
	
	// ---- value getters and setters --- (write operating may fire change events)
	
	public double getCutoff() {
		return cutoffParameter.getValue();
	}

	/**
	 * @param f b/w 0.0 and 1.0 (aka percentage)
	 */
	public void setCutoff(double f) {
		this.cutoffParameter.setValue((int)(127.0 * f));
	}

	public double getEnvGenDepth() {
		return egDepthParameter.getValue();
	}

	/**
	 * @param f b/w 0.0 and 1.0 (aka percentage)
	 */
	public void setEnvGenDepth(double d) {
		this.egDepthParameter.setValue((int)(127.0 * d));
	}

	public double getKbdTracking() {
		return kbdTrackingParameter.getValue();
	}

	/**
	 * @param f b/w 0.0 and 1.0 (aka percentage)
	 */
	public void setKbdTracking(double k) {
		this.kbdTrackingParameter.setValue((int)(127.0 * k));
	}

	public double getResonance() {
		return resonanceParameter.getValue();
	}

	/**
	 * @param f b/w 0.0 and 1.0 (aka percentage)
	 */
	public void setResonance(double r) {
		this.resonanceParameter.setValue((int)(127.0 * r));
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
	public MIDIParameter getCutoffParameter() {
		return cutoffParameter;
	}

	/**
	 * @return the egDepthParameter
	 */
	public MIDIParameter getEgDepthParameter() {
		return egDepthParameter;
	}

	/**
	 * @return the kbdTrackingParameter
	 */
	public MIDIParameter getKbdTrackingParameter() {
		return kbdTrackingParameter;
	}

	/**
	 * @return the resonanceParameter
	 */
	public MIDIParameter getResonanceParameter() {
		return resonanceParameter;
	}

	/**
	 * @return the filterOrderParameter
	 */
	public EnumParameter<FilterOrder> getFilterOrderParameter() {
		return filterOrderParameter;
	}
	
	public ADSREnveloppe getAdsrEnveloppeParameter() {
		return adsrEnveloppe;
	}
	
}