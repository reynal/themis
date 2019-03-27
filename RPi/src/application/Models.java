package application;

import model.*;

/**
 * This class is responsible for creating all the models needed by the entire application.
 * @author bastien
 *
 */
public class Models {
	
	private Vco3340 vco3340;
	private Vco13700 vco13700;
	private Vca vca;
	private Vcf3320 vcf;
	
	Models(){
		vco3340 = new Vco3340();
		vco13700 = new Vco13700();
		vca = new Vca();
		vcf3320 = new Vcf3320();
	}
}
