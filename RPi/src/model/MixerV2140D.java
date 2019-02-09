package model;


public class MixerV2140D extends AbstractModel {

	private DoubleParameter gain1dBParameter,gain2dBParameter, gain3dBParameter, gain4dBParameter;

	/**
	 * Builds a mixer model with default channel values
	 */
	public MixerV2140D(){
		this("Gain 1", "Gain 2", "Gain 3", "Gain 4");
	}
	
	/**
	 * 
	 * @param lbl1 first channel label
	 * @param lbl2 second channel label
	 * @param lbl3 third channel label
	 * @param lbl4 fourth channel label
	 */
	public MixerV2140D(String lbl1, String lbl2, String lbl3, String lbl4){
		super();
		parameterList.add(gain1dBParameter = new DoubleParameter(lbl1,-48,6,0.2)); //TODO verification of mixer cutting/gain values
		parameterList.add(gain2dBParameter = new DoubleParameter(lbl2,-48,6,0.2));
		parameterList.add(gain3dBParameter = new DoubleParameter(lbl3,-48,6,0.2));
		parameterList.add(gain4dBParameter = new DoubleParameter(lbl4,-48,6,0.2));
	}
	
	// ---- value getters and setters --- (write operating may fire change events)
	
	public double getGain1dB() {
		return gain1dBParameter.getValue();
	}
	public void setGain1dB(double gain1dB) {
		this.gain1dBParameter.setValue(gain1dB);
	}
	public double getGain2dB() {
		return gain2dBParameter.getValue();
	}
	public void setGain2dB(double gain2dB) {
		this.gain2dBParameter.setValue(gain2dB);
	}
	public double getGain3dB() {
		return gain3dBParameter.getValue();
	}
	public void setGain3dB(double gain3dB) {
		this.gain3dBParameter.setValue(gain3dB);
	}
	public double getGain4dB() {
		return gain4dBParameter.getValue();
	}
	public void setGain4dB(double gain4dB) {
		this.gain4dBParameter.setValue(gain4dB);
	}



	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
	/**
	 * @return the gain1dBParameter
	 */
	public DoubleParameter getGain1dBParameter() {
		return gain1dBParameter;
	}

	/**
	 * @return the gain2dBParameter
	 */
	public DoubleParameter getGain2dBParameter() {
		return gain2dBParameter;
	}

	/**
	 * @return the gain3dBParameter
	 */
	public DoubleParameter getGain3dBParameter() {
		return gain3dBParameter;
	}

	/**
	 * @return the gain4dBParameter
	 */
	public DoubleParameter getGain4dBParameter() {
		return gain4dBParameter;
	}
	
}
