package model;

import java.util.List;

import model.event.*;

/** 
 * 
 */
public class VcoCEM3340 extends Vco {

	
	private EnumParameter<WaveShape> shape;
	private DoubleParameter duty;
	private BooleanParameter syncFrom13700;
	
	public VcoCEM3340() {
		super();
		parameterList.add(shape = new EnumParameter<WaveShape>(WaveShape.class, "WaveShape"));
		parameterList.add(duty = new DoubleParameter("Duty", 0, 100, 1));
		parameterList.add(syncFrom13700 = new BooleanParameter("Sync"));
		
	}

	public WaveShape getWaveShape() {
		return shape.getValue();
	}
	
	public void setWaveShape(WaveShape waveshape) {
		this.shape.setValue(waveshape);
	}
	
	public EnumParameter<WaveShape> getShapeParameter() {
		return shape;
	}

	public DoubleParameter getDutyParameter() {
		return duty;
	}


	public boolean getSyncFrom13700() {
		return syncFrom13700.getValue();
	}

	public BooleanParameter getSyncFrom13700Parameter() {
		return syncFrom13700;
	}

	public void setSyncFrom13700(boolean syncFrom13700) {
		this.syncFrom13700.setValue(syncFrom13700);
	}
	
	public double getDuty() {
		return duty.getValue();
	}

	public void setDuty(double duty) {
		this.duty.setValue(duty);
	}

	// ----------- enum -------------
	
	public static enum WaveShape {
		
		SQUARE,
		TRIANGLE,
		PULSE,
		SAWTOOTH;
	}


	
	// ------------ test -------------
	public static void main(String[] args) {
	
		VcoCEM3340 vco1 = new VcoCEM3340();
		List<SynthParameter<?>> paramsVCO1 = vco1.getParameters();
		for (SynthParameter<?> p : paramsVCO1) {

			System.out.println(p);
			System.out.println(p.createControl());
			//System.out.println(p.createControl().getJavaFXView());
		}
	}

}


