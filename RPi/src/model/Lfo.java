package model;

public class Lfo extends AbstractModel {
	
	private DoubleParameter frequencyParameter;
	private EnumParameter<WaveShape> waveShapeParameter;
	
	enum WaveShape {
		
		SQUARE,
		TRIANGLE,
		PULSE,
		SAWTOOTH;
	}
		
	// ---- value getters and setters --- (write operating may fire change events)
	
	public double getFrequency() {
		return frequencyParameter.getValue();
	}

	public void setFrequency(double frequency) {
		this.frequencyParameter.setValue(frequency);
	}
	
	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
}
