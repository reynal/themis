package model;

import java.util.logging.Logger;

/**
 * A model for a VCF (aka Voltage Controled Filter) module, whatever the underlying hardware.
 * 
 * This model currently comprises:
 * - an ADSR enveloppe.
 * - a Cutoff and a Resonance parameters for the filter
 * - an EG (aka Enveloppe Generator) Depth parameter that sets the actual variation of the cutoff value vs the enveloppe
 * - a Keyboard Tracking parameter that sets how fast does the cutoff frequency change follow the current MIDI note 
 * - a filter order, e.g., the number of poles (2 or 4)
 * 
 * @author reynal
 *
 */
public class VcfModule extends AbstractModule {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private MIDIParameter cutoffParameter, egDepthParameter, kbdTrackingParameter, resonanceParameter, velocitySensitivityParameter;
	private EnumParameter<FilterOrder> filterOrderParameter;  
	private ADSREnveloppe adsrEnveloppe;
	
	// list of label constant for use by clients:
	public static final String CUTOFF = "VCF Cutoff";
	public static final String RESONANCE = "VCF Res";
	public static final String KBD_TRACKING = "VCF Kbd";
	public static final String EG_DEPTH = "VCF EG Depth";
	public static final String VEL_SENSITIVITY= "VCF Vel Sens";
	
	public VcfModule() {
		
		cutoffParameter = new MIDIParameter(CUTOFF);
		resonanceParameter = new MIDIParameter(RESONANCE);
		kbdTrackingParameter = new MIDIParameter(KBD_TRACKING);
		egDepthParameter = new MIDIParameter(EG_DEPTH);
		adsrEnveloppe = new ADSREnveloppe();
		velocitySensitivityParameter = new MIDIParameter(VEL_SENSITIVITY);
		
		parameterList.addAll(adsrEnveloppe.getParameters());
		parameterList.add(filterOrderParameter = new EnumParameter<FilterOrder>(FilterOrder.class, "FilterOrder"));
		parameterList.add(cutoffParameter);
		parameterList.add(resonanceParameter);
		parameterList.add(kbdTrackingParameter);
		parameterList.add(egDepthParameter);
		parameterList.add(velocitySensitivityParameter);
		
		for (ModuleParameter<?> param : getParameters()) param.addModuleParameterChangeListener(e -> LOGGER.info(e.toString()));  // for debug purpose only

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
	 * @return the velocitySensitivityParameter
	 */
	public MIDIParameter getVelocitySensitivityParameter() {
		return velocitySensitivityParameter;
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
	
	public ADSREnveloppe getAdsrEnveloppe() {
		return adsrEnveloppe;
	}
	
}
