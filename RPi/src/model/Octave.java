package model;

public enum Octave {
	
	TWO_INCHES,
	FOUR_INCHES;

	
	public static void main(String[] args) {
		Octave o = Octave.TWO_INCHES;
		int i = Octave.values().length;
		Octave.class.isEnum();
	}
}

