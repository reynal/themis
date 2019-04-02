package model;

/**
 * This class instantiate every parameters of a VCA type module and create a list of those parameters
 * As long as there is one type of VCA every of its parameters is set here.
 * 
 * @author Bastien Fratta
 */

public class Vca extends AbstractModel {
	
	private MIDIParameter enveloppeGeneratorDepthParameter,velocityTrackingParameter, velocitySensitivityParameter;
	private ADSREnveloppe adsrEnveloppe;
	
	// list of label constant for use by clients:
	public static final String EG_DEPTH = "VCA EG.Detph";
	public static final String VEL_TRACKING = "VCA Velocity Tracking";
	
	public Vca() {
		
		adsrEnveloppe = new ADSREnveloppe();
		parameterList.addAll(adsrEnveloppe.getParameters());
		velocityTrackingParameter = new MIDIParameter(VEL_TRACKING);
		enveloppeGeneratorDepthParameter = new MIDIParameter(EG_DEPTH);
		parameterList.add(velocityTrackingParameter);
		parameterList.add(enveloppeGeneratorDepthParameter);
	}
	
	// ---- value getters and setters --- (write operating may fire change events)

	public double getEnveloppeGeneratorDepth() {
		return enveloppeGeneratorDepthParameter.getValue() / 127.0;
	}

	/**
	 * @param egDepth b/w 0 and 1
	 */
	public void setEnveloppeGeneratorDepth(double egDepth) {
		this.enveloppeGeneratorDepthParameter.setValue((int)(127.0*egDepth));
	}

	public double getKeyboardTracking() {
		return velocityTrackingParameter.getValue() / 127.0;
	}

	public void setKeyboardTracking(double kbdTracking) {
		this.velocityTrackingParameter.setValue((int)(127.0*kbdTracking));
	}
	
	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	


	public MIDIParameter getEnveloppeGeneratorDepthParameter() {
		return enveloppeGeneratorDepthParameter;
	}

	public MIDIParameter getVelocityTrackingParameter() {
		return velocityTrackingParameter;
	}

	public ADSREnveloppe getAdsrEnveloppe() {
		return adsrEnveloppe;
	}

	
}