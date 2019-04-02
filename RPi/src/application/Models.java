package application;

import model.*;

/**
 * This class is responsible for creating all the models needed by the entire application.
 * @author bastien
 *
 */
public class Models {
	
	public static Vco3340 vco3340;
	public static Vco13700 vco13700;
	public static Vca vca;
	public static Vcf3320 vcf3320;
	
	Models(){
		vco3340 = new Vco3340();
		vco13700 = new Vco13700();
		vca = new Vca();
		vcf3320 = new Vcf3320();
	}

	public Vco3340 getVco3340() {
		return vco3340;
	}

	public Vco13700 getVco13700() {
		return vco13700;
	}

	public Vca getVca() {
		return vca;
	}

	public Vcf3320 getVcf() {
		return vcf3320;
	}
	
	
}