package model;

/** 
 * 
 */
public class VcoCEM3340 extends Vco{

	public static enum WaveShape {
		
		SQUARE,
		TRIANGLE,
		PULSE,
		SAWTOOTH;
	}
	
	private EnumParameter<WaveShape> shape;
	private BooleanParameter syncFrom13700;
	
	public VcoCEM3340() {
		super();
		shape = new EnumParameter<WaveShape>("WaveShape");
		parameterList.add(shape);
		// TODO : syncFrom13700
	}

	public EnumParameter<WaveShape> getWaveShape() {
		return shape;
	}
	
	public void setWaveShape(EnumParameter<WaveShape> waveshape) {
		this.shape = waveshape;
	}

}
