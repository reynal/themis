package model.sequencer;


import java.util.*;
import javax.sound.midi.*;
import static model.sequencer.EventDuration.*;
import model.sequencer.TrackThemis.*;
/**
 * SequenceThemis
 * 
 */

public class SequenceThemis extends Sequence {
 

	public SequenceThemis(float divisionType, int resolution) throws InvalidMidiDataException {
		super(divisionType, resolution);
		// TODO Auto-generated constructor stub
	}
	
	public SequenceThemis(float divisionType, int resolution, int numTracks) throws InvalidMidiDataException {
		super(divisionType, resolution, numTracks);
		// TODO Auto-generated constructor stub
	}

	
	public TrackThemis createTrackThemis() {
		
		return new TrackThemis(createTrack());		
		
	}  

	
	public boolean deleteTrackThemis(TrackThemis trackThemis) {
		
		//return deleteTrack(trackThemis(t));		
		return false;
	} 

	
	public TrackThemis[] getTracksThemis() {
		
		//return new TrackThemis(getTracks());
		return null;
		
	}   
	
	public float getDivisionTypeThemis() {
		
		//return getDivisionType(trackThemis(t));
		return 0.0f;
		
	} 
	
	
	// my own stuff
	
	
	public enum EventType {  // �ventuellement � sortir si on ne peut pas m�langer CC et NOTE sur une track
        NOTE,
        CC;
	}
	
	/**
	 * type
	 * Defini le message midi
	 */
	void type() {
		
}
	
	/**
	 * addEvent
	 * add a midi event
	 */
	public void addEvent(int startTick, EventDuration length, EventType type, int data1, int data2) {
		//if (type == NOTE) {
			
		//}
}
	
	/**
	 * deleteEvent
	 * delete a midi event
	 */
	public void deleteEvent(int selectTick) {
		//TODO
	}
	
	/**
	 * deleteEvent
	 * delete many midi events
	 */
	public void deleteEvent(int selectTick, EventDuration selection) {
		//TODO
	}
	
	/**
	 * moveEvent
	 * move a midi event
	 */
	public void moveEvent(int selectTick) {
		//TODO
	}
	
	/**
	 * moveEvent
	 * move many midi events
	 */
	public void moveEvent(int selectTick, EventDuration selection) {
		//TODO
	}
	
	/**
	 * quantize
	 * quantize whole line
	 */
	public void quantize(EventDuration step) {
		//TODO
	}
	
	/**
	 * quantize
	 * quantize selected part
	 */
	public void quantize(EventDuration step, int selectedTick, EventDuration selection) {
		//TODO
	}

	/**
	 * randomize
	 * randomize whole line (r�utiliser move)
	 */
	public void randomize(EventDuration range) {
		// TODO
	}

	/**
	 * randomize
	 * randomize selected part
	 */
	public void randomize(EventDuration range, int selectedTick, EventDuration selection) {
		// TODO
	}

}
