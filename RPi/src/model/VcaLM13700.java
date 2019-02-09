package model;

/**
 * This class instantiate every parameters of a VCA type module and create a list of those parameters
 * As long as there is one type of VCA every of its parameters is set here.
 * 
 * @author Bastien Fratta
 */

public class VcaLM13700 extends AbstractModel {
	
	private DoubleParameter egDepth,kbdTracking;
	
	public VcaLM13700() {
		
		kbdTracking = new DoubleParameter("Keyboard", 0, 0, 0);
		egDepth = new DoubleParameter("EG Depth", 0, 0, 0);
		parameterList.add(kbdTracking);
		parameterList.add(egDepth);
	}

	public double getEnvGenDepth() {
		return egDepth.getValue();
	}

	public void setEnvGenDepth(double egDepth) {
		this.egDepth.setValue(egDepth);
	}

	public double getKbdTracking() {
		return kbdTracking.getValue();
	}

	public void setKbdTracking(double kbdTracking) {
		this.kbdTracking.setValue(kbdTracking);
	}
	
}