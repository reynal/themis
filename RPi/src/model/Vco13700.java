package model;

/**
 * Model for a VCO based on the LM13700 OTA.
 * 
 * @author Bastien Fratta, S. Reynal
 *
 */
public class Vco13700 extends AbstractModel {

	private DoubleParameter detuneParameter;
	private EnumParameter<Octave> octaveParameter;
	private EnumParameter<WaveShape> waveShapeParameter;
	
	public Vco13700() {
		super();
		parameterList.add(detuneParameter = new DoubleParameter("VCO Detune %", -5, 5, 0.1));
		parameterList.add(octaveParameter = new EnumParameter<Octave>(Octave.class, "VCO Octave"));
		parameterList.add(waveShapeParameter = new EnumParameter<WaveShape>(WaveShape.class, "WaveShape"));
	}
		
	enum WaveShape {
		
		SQUARE,
		TRIANGLE;
	}

	// ---- value getters and setters --- (write operating may fire change events)
	
	public Octave getOctave() { 
		return octaveParameter.getValue();
	}

	public void setOctave(Octave v) {
		octaveParameter.setValue(v);
	}
	
	public double getDetune() {
		return detuneParameter.getValue();
	}

	public void setDetune(double v) {
		detuneParameter.setValue(v);
	}
	
	public WaveShape getWaveShape() {
		return waveShapeParameter.getValue();
	}
	
	public void setWaveShape(WaveShape shape) {
		this.waveShapeParameter.setValue(shape);
	}
	
	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
	public DoubleParameter getDetuneParameter() {
		return detuneParameter;
	}
	
	public EnumParameter<Octave> getOctaveParameter(){
		return octaveParameter;
	}

	public EnumParameter<WaveShape> getWaveShapeParameter() {
		return waveShapeParameter;
	}
}
