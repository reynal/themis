package model;

/**
 * This class instantiate every parameters of a VCA type module and create a list of those parameters
 * As long as there is one type of VCA every of its parameters is set here.
 * 
 * @author Bastien Fratta
 */

public class Vca extends AbstractModel {
	
	private DoubleParameter enveloppeGeneratorDepth,keyboardTracking;
	
	public Vca() {
		
		keyboardTracking = new DoubleParameter("KBD Tracking", 0, 0, 0);
		enveloppeGeneratorDepth = new DoubleParameter("EG Depth", 0, 0, 0);
		parameterList.add(keyboardTracking);
		parameterList.add(enveloppeGeneratorDepth);
	}

	public double getEnveloppeGeneratorDepth() {
		return enveloppeGeneratorDepth.getValue();
	}

	/**
	 * @param egDepth b/w 0 and 1
	 */
	public void setEnveloppeGeneratorDepth(double egDepth) {
		this.enveloppeGeneratorDepth.setValue(egDepth);
	}

	public double getKeyboardTracking() {
		return keyboardTracking.getValue();
	}

	public void setKeyboardTracking(double kbdTracking) {
		this.keyboardTracking.setValue(kbdTracking);
	}
	
}