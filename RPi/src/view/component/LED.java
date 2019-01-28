package view.component;

import java.io.IOException;

import com.pi4j.io.i2c.I2CFactory.*;

/**
 * SR : Je ne comprend pas l'interet de cette classe par rapport à IS31FL3731 du coup
 * Pourquoi ne pas rajouter ces méthodes dans IS31FL3731 ?
 * 
 *
 */
public class LED extends IS31FL3731 {
	
	//CONSTRUCTEUR
	
	public LED() throws Exception {
		
		super(DeviceAddress.AD_GND);
		
	}
	
	//ATTRIBUTS
	public static final boolean HIGH = true;
	public static final boolean LOW = false;
	
	int MS =1;
	
	protected int val;
	protected int state;
	
	
	// 														METHODES
	
	// Shutdowns LEDs 
	void resetLED(int row) throws IOException, InterruptedException, UnsupportedBusNumberException  {
		
		int i;
		for (i=0;i<8;i++) {
			
			switchLED(row,i,LOW);
			Thread.sleep(20); // SR : pourquoi ce delay ?
			switchLED(row+1,i,LOW);
			Thread.sleep(20);
		}
	}
	
	// Allume plus ou moins de LED, de façon binary (SR : hein ????) 
	void LedInDecBinary(int row, int inc,int val, int state) throws IOException, InterruptedException, UnsupportedBusNumberException  {

		if (state == 0){
			if (inc == 0){}
			else {		
				val = val | 7;
				if(val==0) { //All LEDs are turned off
					if(inc==-1) resetLED(row);
					else switchLED(row,0,HIGH);
				}
				else { 
					if (inc == -1) switchLED(row, val-1, LOW);
					else switchLED(row,val+1,HIGH);
				}
			}
		}
	}
	
	// SR : et celle la elle fait quoi ?
	void LedInDecPWM(int row, int col, int inc,int val, int state) throws IOException, InterruptedException, UnsupportedBusNumberException  {

		if (state == 0){
			
			val = val & 127;
			if(val==0) { //All LEDs are turned off
				if(inc==-1) resetLED(row);
				else setLEDpwm(row, col, val+2);
			}
			else { 
				if (inc == -1) setLEDpwm(row, col, val-2);
				else setLEDpwm(row, col, val+2);
			}
		}
	}
}