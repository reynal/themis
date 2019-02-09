package model;

import java.util.List;

import model.event.*;

/** 
 * @author Bastien Fratta, S. Reynal
 */
public class Vco3340 extends AbstractModel {

	protected DoubleParameter detune;
	protected EnumParameter<Octave> octave;	
	private EnumParameter<WaveShape> shape;
	private DoubleParameter duty;
	private BooleanParameter syncFrom13700;
	
	public Vco3340() {
		super();
		parameterList.add(detune = new DoubleParameter("VCO Detune %", -5, 5, 0.1));
		parameterList.add(octave = new EnumParameter<Octave>(Octave.class, "VCO Octave"));
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
	
	public Octave getOctave() { 
		return octave.getValue();
	}

	public void setOctave(Octave v) {
		octave.setValue(v);
	}
	
	public double getDetune() {
		return detune.getValue();
	}

	public void setDetune(double v) {
		detune.setValue(v);
	}
	
	public DoubleParameter getDetuneParameter() {
		return detune;
	}
	
	public EnumParameter<Octave> getOctaveParameter(){
		return octave;
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
	
		Vco3340 vco1 = new Vco3340();
		List<SynthParameter<?>> paramsVCO1 = vco1.getParameters();
		for (SynthParameter<?> p : paramsVCO1) {

			System.out.println(p);
			System.out.println(p.createControl());
			//System.out.println(p.createControl().getJavaFXView());
		}
	}

}


