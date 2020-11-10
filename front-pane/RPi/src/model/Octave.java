package model;

/**
 * an enumeration of constants useful for VCO's
 * @author SR
 */
public enum Octave {
	
	ONE_INCHES, // 1"
	TWO_INCHES, // 2"
	FOUR_INCHES, // 4"
	EIGHT_INCHES;  // 8"

	// -------------- test --------------
	
	public static void main(String[] args) {
		
		//Octave o = Octave.TWO_INCHES;
		//int i = Octave.values().length;
		Octave.class.isEnum();
		
	}
}

