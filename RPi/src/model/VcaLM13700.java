package model;

import java.util.ArrayList;
import java.util.List;

/**
 * This class instantiate every parameters of a VCA type module and create a list of those parameters
 * As long as there is one type of VCA every of its parameters is set here.
 * 
 * @author Bastien Fratta
 */

public class VcaLM13700 {
	private DoubleParameter egDepth,kbdTracking;
	protected final List<SynthParameter<?>> parameterList = new ArrayList<SynthParameter<?>>();
	
	public VcaLM13700() {
		kbdTracking = new DoubleParameter("Keyboard", 0, 0, 0);
		egDepth = new DoubleParameter("EG Depth", 0, 0, 0);
		parameterList.add(kbdTracking);
		parameterList.add(egDepth);
	}

	public DoubleParameter getEnvGenDepth() {
		return egDepth;
	}

	public void setEnvGenDepth(DoubleParameter egDepth) {
		this.egDepth = egDepth;
	}

	public DoubleParameter getKbdTracking() {
		return kbdTracking;
	}

	public void setKbdTracking(DoubleParameter kbdTracking) {
		this.kbdTracking = kbdTracking;
	}
	
	public List<SynthParameter<?>> getParameters() {
		return parameterList;
	}
}