package view.component;

import java.io.IOException;

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

import device.IS31FL3731;

public class LED extends IS31FL3731 {
	
	//CONSTRUCTORS
	
		public LED(int row, int AB, int state) throws Exception { //we are free to choose the state
			
			super();
			this.row = row;
			this.col = AB;
			this.stateValue = state;
		}

	//ATTRIBUTES

		public final int LED_MIN_VALUE = 0;
		public final int LED_MAX_VALUE = 7;
		public final int MIDI_MIN_VALUE = 0;
		public final int MIDI_MAX_VALUE = 127;
		public final int HIGH = 1;
		public final int LOW = 0;
		
		private int ledValue;
		private int midiValue; // must be b/w 0 (no LED active) and 128
		private int stateValue; //
		private int row;
		private int col;
		//Accessor
		
		//LED VALUE
		public void setLEDValue(int v) {
			if (v > LED_MAX_VALUE)
				throw new IllegalArgumentException(v + " greater than " + LED_MAX_VALUE);
			else if (v < LED_MIN_VALUE)
				throw new IllegalArgumentException(v + " lower than " + LED_MIN_VALUE);
			ledValue = v;
		}
		
		public int getLEDValue() {
			return ledValue;	
		}
		
		//MIDI VALUE
		
		public void setMidiValue(int v) {
			if (v > MIDI_MAX_VALUE)
				throw new IllegalArgumentException(v + " greater than " + MIDI_MAX_VALUE);
			else if (v < MIDI_MIN_VALUE)
				throw new IllegalArgumentException(v + " lower than " + MIDI_MIN_VALUE);
			midiValue = v;
		}
		
		public int getMidiValue() {
			return midiValue;
			
		}
		
		//STATE VALUE
		
		public void setStateValue(int v) {
			if (v ==0 | v==1)
				stateValue = v;
			else
				throw new IllegalArgumentException(v + " lower than " + this.MIDI_MIN_VALUE);
		}
		
		public int getStateValue() {
			return stateValue;
			
		}
		
		//Binary way to turn on LEDs
		
		private void updateBargraphBinary(int row ,int val, int col) throws IOException, InterruptedException, UnsupportedBusNumberException  {

			val = val | 7;
			if(col==1) {
				val=val+this.LED_MAX_VALUE;
			}
			int i;
			for (i=this.LED_MIN_VALUE;i<val;i++) {
				switchLED(row,i,HIGH);
				Thread.sleep(50);
			}
			for (i=val;i<this.LED_MAX_VALUE;i++) {
				switchLED(row,i,LOW);
				Thread.sleep(50);
			}
		}
		
		
		//Change state of LED by a smooth way thanks to PWM when we use MIDI
		private void updateBargraphPWM(int row ,int val, int col) throws IOException, InterruptedException, UnsupportedBusNumberException  {

			val = val | this.MIDI_MAX_VALUE;
			
			if(col==1) {
				col=col+this.LED_MAX_VALUE;
			}
			
			int position = val/16;
			int i;
			for (i=this.LED_MIN_VALUE;i<position+1;i++) {
				switchLED(row,i,HIGH);
				Thread.sleep(50);
			}
			for (i=position;i<this.LED_MAX_VALUE;i++) {
				switchLED(row,i,LOW);
				Thread.sleep(50);
			}
			
			int pwmVal = (val-(position*16-1))*16;
			setLEDpwm(row, position+1, pwmVal);
			
		}

		
		public static void main (String args[]) throws Exception {
			
			LED led1 = new LED(1,1,0); //information of B 8-LED bargraph of the row 2, on state 0 (Turn on or turn of a LED)
			LED led2 = new LED(2,0,1); //information of A 8-LED bargraph of the row 3, on state 1 (Use midi information, then PWM)
			
			while(true) {
				
				led1.setLEDValue(3); //received value by the listener (TODO), Here in example, the value is 3)
				led1.updateBargraphBinary(led1.row ,led1.ledValue, led1.col);
				
				Thread.sleep(50);
				
				led2.setMidiValue(48); //received value by the listener (TODO), Here in example, the value is 48)
				led2.updateBargraphPWM(led2.row ,led2.midiValue, led2.col);
				
				Thread.sleep(50);
				
			}
		}

	
	
	
}
