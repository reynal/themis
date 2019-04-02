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
		vcf = new Vcf3320();
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
		return vcf;
	}
	
	
}
