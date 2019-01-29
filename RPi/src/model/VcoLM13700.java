package model;

public class VcoLM13700 extends Vco {

	private EnumParameter<WaveShape> shape;
	
	public VcoLM13700() {
		super();
		parameterList.add(shape = new EnumParameter<WaveShape>(WaveShape.class, "WaveShape"));
	}
		
	enum WaveShape {
		
		SQUARE,
		TRIANGLE;
	}


}
