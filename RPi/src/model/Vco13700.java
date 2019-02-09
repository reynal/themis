package model;

/**
 * 
 * @author Bastien Fratta, S. Reynal
 *
 */
public class Vco13700 extends AbstractModel {

	protected DoubleParameter detune;
	protected EnumParameter<Octave> octave;
	private EnumParameter<WaveShape> shape;
	
	public Vco13700() {
		super();
		parameterList.add(detune = new DoubleParameter("VCO Detune %", -5, 5, 0.1));
		parameterList.add(octave = new EnumParameter<Octave>(Octave.class, "VCO Octave"));
		parameterList.add(shape = new EnumParameter<WaveShape>(WaveShape.class, "WaveShape"));
	}
		
	enum WaveShape {
		
		SQUARE,
		TRIANGLE;
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
}
