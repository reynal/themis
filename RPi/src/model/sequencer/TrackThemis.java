package model.sequencer;


import java.util.*;
import javax.sound.midi.*;
import static model.sequencer.EventDuration.*;
/**
 * TrackThemis
 * 
 */

// cas à traiter: fin de boucle note on, début de boucle note off
public class TrackThemis{
	

	// adaptor design pattern
	
ArrayList<TrackThemis> history;
	
	private Track track;

	public TrackThemis(Track t) {
		this.track = t;
	}
	
	public boolean add(MidiEvent event){
		return track.add(event);		
	}
	
	
	public MidiEvent	get(int index){
		return track.get(index);				
	}
	
	public boolean	remove(MidiEvent event){
		return track.remove(event);
	}
	
	
	public int	size(){
		return track.size();
	}
	
	public long	ticks(){
		return track.ticks();
	}
	
	// my own stuff
	
	public enum EventType {  // éventuellement à sortir si on ne peut pas mélanger CC et NOTE sur une track
        NOTE,
        CC;
	}
	
	/**
	 * type
	 * Défini le message midi
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
	 * randomize whole line (réutiliser move)
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
