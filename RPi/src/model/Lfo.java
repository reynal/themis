package model;

public class Lfo extends AbstractModel {
	
	protected DoubleParameter frequency;
	
	enum WaveShape {
		
		SQUARE,
		TRIANGLE,
		PULSE,
		SAWTOOTH;
	}
	WaveShape shape;
	
	public double getFrequency() {
		return frequency.getValue();
	}

	public void setFrequency(double frequency) {
		this.frequency.setValue(frequency);
	}
}
