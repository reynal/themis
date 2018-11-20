package controller;

import controller.component.PushButton;
import javafx.scene.Node;

public class PerformancePad {
	
	
	static PushButton[][] pad;
	private int lenght;
	
	void init(){
		
		pad = new PushButton[16][4];
		
	}
	
	public static Node getView() {
		
		return null;
	}
	int getSize(PerformancePad pad) {
		return pad.lenght;
	}
	
	 /**
	 * Returns the push button at the given row and col
	 */
	 public static PushButton getPerformancePadPushButton(int row, int col){
		 return null; // TODO
	 }
	 
}
