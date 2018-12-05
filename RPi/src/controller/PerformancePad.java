package controller;

import controller.component.PushButton;
import javafx.scene.Node;

/**
 * 
 * @author Bastien Fratta
 *
 */
public class PerformancePad {
	
	
	private PushButton[][] pushButtons;
	private int lenght;
	
	/**
	 * 
	 */
	public PerformancePad(){
		
		pushButtons = new PushButton[16][4];
		
	}
	
	/**
	 * 
	 * @return
	 */
	public Node getView() {
		
		return null;
	}
	
	/**
	 * 
	 * @param pad
	 * @return
	 */
	public int getSize() {
		
		return pushButtons.length;
		
	}
	
	 /**
	  * Returns the push button at the given row and col
	  */
	 public PushButton getPerformancePadPushButton(int row, int col){
		 
		 return null; // TODO
		 
	 }
	 
}
