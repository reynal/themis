package model;

import java.util.List;

/** 
 * 
 */
public class VcoCEM3340 extends Vco{

	
	private EnumParameter<WaveShape> shape;
	private BooleanParameter syncFrom13700;
	
	public VcoCEM3340() {
		super();
		parameterList.add(shape = new EnumParameter<WaveShape>(WaveShape.class, "WaveShape"));
		// TODO : add "syncFrom13700"
	}

	public EnumParameter<WaveShape> getWaveShape() {
		return shape;
	}
	
	public void setWaveShape(EnumParameter<WaveShape> waveshape) {
		this.shape = waveshape;
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


