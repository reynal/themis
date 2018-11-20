package model;

import java.util.List;

import controller.component.Control;
import controller.event.RotaryEncoderChangeListener;
import controller.event.RotaryEncoderEvent;

public class MixerV2140D implements RotaryEncoderChangeListener {

	private double gain1dB;
	private double gain2dB;
	private double gain3dB;
	private double gain4dB;

	public double getGain1dB() {
		return gain1dB;
	}
	public void setGain1dB(double gain1dB) {
		this.gain1dB = gain1dB;
	}
	public double getGain2dB() {
		return gain2dB;
	}
	public void setGain2dB(double gain2dB) {
		this.gain2dB = gain2dB;
	}
	public double getGain3dB() {
		return gain3dB;
	}
	public void setGain3dB(double gain3dB) {
		this.gain3dB = gain3dB;
	}
	public double getGain4dB() {
		return gain4dB;
	}
	public void setGain4dB(double gain4dB) {
		this.gain4dB = gain4dB;
	}
	
	List<Control> getControl() {
		return null; //new Control[] {new RotaryEncoderEv
	}
	

	@Override
	public void encoderRotated(RotaryEncoderEvent e) {
		e.getValue();
	}
}
