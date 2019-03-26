package application;

import model.Vco13700;
import model.Vco3340;

public class Models {
	
	public Vco3340 vco3340;
	public Vco13700 vco13700;
	Models(){
		vco3340 = new Vco3340();
		vco13700 = new Vco13700();
	}
}
