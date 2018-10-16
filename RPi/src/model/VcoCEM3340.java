package model;

public class VcoCEM3340 extends Vco {
	
	enum WaveShape {
		
		SQUARE,
		TRIANGLE,
		PULSE,
		SAWTOOTH;
	}
	
	WaveShape shape;
	boolean syncFrom13700;

}
