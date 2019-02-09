package model;

/**
 * This class instantiate every parameters of a VCA type module and create a list of those parameters
 * As long as there is one type of VCA every of its parameters is set here.
 * 
 * @author Bastien Fratta
 */

public class Vca extends AbstractModel {
	
	private DoubleParameter enveloppeGeneratorDepthParameter,velocityTrackingParameter;
	private ADSREnveloppe adsrEnveloppe;
	
	public Vca() {
		
		adsrEnveloppe = new ADSREnveloppe();
		parameterList.addAll(adsrEnveloppe.getParameters());
		velocityTrackingParameter = new DoubleParameter("KBD Tracking", 0, 0, 0);
		enveloppeGeneratorDepthParameter = new DoubleParameter("EG Depth", 0, 0, 0);
		parameterList.add(velocityTrackingParameter);
		parameterList.add(enveloppeGeneratorDepthParameter);
	}
	
	// ---- value getters and setters --- (write operating may fire change events)

	public double getEnveloppeGeneratorDepth() {
		return enveloppeGeneratorDepthParameter.getValue();
	}

	/**
	 * @param egDepth b/w 0 and 1
	 */
	public void setEnveloppeGeneratorDepth(double egDepth) {
		this.enveloppeGeneratorDepthParameter.setValue(egDepth);
	}

	public double getKeyboardTracking() {
		return velocityTrackingParameter.getValue();
	}

	public void setKeyboardTracking(double kbdTracking) {
		this.velocityTrackingParameter.setValue(kbdTracking);
	}
	
	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
}