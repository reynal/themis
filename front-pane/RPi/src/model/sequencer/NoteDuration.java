package model.sequencer;

public enum NoteDuration {
	
	WHOLE(4), // ronde
	HALF(2), // blanche
	QUARTER(1), // noire
	EIGHTH(0.5), // croche
	SIXTEENTH(0.25);
	
	//  TODO : notes pointées ! 
	public static final int TICKS_PER_QUARTER = 200;	
	private double duration;
	
	NoteDuration(double duration){
		this.duration = duration;
	}
	
	public double getDuration() {
		return duration;
	}
	
	public long getTicks() {
		return (long)(duration * TICKS_PER_QUARTER);
	}
}
