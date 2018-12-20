package model;

import java.util.List;

import controller.component.Control;
import controller.event.RotaryEncoderChangeListener;
import controller.event.RotaryEncoderEvent;

public class MixerV2140D extends AbstractModel implements RotaryEncoderChangeListener {

	protected DoubleParameter gain1dB;
	protected DoubleParameter gain2dB;
	protected DoubleParameter gain3dB;
	protected DoubleParameter gain4dB;

	public double getGain1dB() {
		return gain1dB.getValue();
	}
	public void setGain1dB(double gain1dB) {
		this.gain1dB.setValue(gain1dB);
	}
	public double getGain2dB() {
		return gain2dB.getValue();
	}
	public void setGain2dB(double gain2dB) {
		this.gain2dB.setValue(gain2dB);
	}
	public double getGain3dB() {
		return gain3dB.getValue();
	}
	public void setGain3dB(double gain3dB) {
		this.gain3dB.setValue(gain3dB);
	}
	public double getGain4dB() {
		return gain4dB.getValue();
	}
	public void setGain4dB(double gain4dB) {
		this.gain4dB.setValue(gain4dB);
	}
	
	List<Control> getControl() {
		return null; //new Control[] {new RotaryEncoderEv
	}
	

	@Override
	public void encoderRotated(RotaryEncoderEvent e) {
		//e.getValue();
	}
}
