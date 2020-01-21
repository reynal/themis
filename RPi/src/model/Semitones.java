package model;

public enum Semitones {

	MAJOR_SEVENTH_BELOW(-11),
	MINOR_SEVENTH_BELOW(-10),
	MAJOR_SIXTH_BELOW(-9),
	MINOR_SIXTH_BELOW(-8),
	FIFTH_BELOW(-7),
	AUG_FOURTH_BELOW(-6),
	PERFECT_FOURTH_BELOW(-5),
	MAJOR_THIRD_BELOW(-4),
	MINOR_THIRD_BELOW(-3),
	TONE_BELOW(-2),
	SEMITONE_BELOW(-1),
	UNISON(0),
	SEMITONE(1),
	TONE(2),
	MINOR_THIRD(3),
	MAJOR_THIRD(4),
	PERFECT_FOURTH(5),
	AUG_FOURTH(6),
	FIFTH(7),
	MINOR_SIXTH(8),
	MAJOR_SIXTH(9),
	MINOR_SEVENTH(10),
	MAJOR_SEVENTH(11);
	
	
	int interval;
	
	Semitones(int interval){
		this.interval = interval;
	}
	
	int getInterval() {
		return this.interval;
	}
	
}
