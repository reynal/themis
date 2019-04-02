package view.touchscreen;

import java.awt.Graphics2D;
import java.awt.Toolkit;
import java.awt.image.ImageObserver;

import model.Vcf3320;
import model.Vco13700;

public class VCF3320 implements TouchScreenView{

	private java.awt.Image imageVCF;
	private Vcf3320 model;
	
	public VCF3320(Vcf3320 model){
		this.model = model;
		imageVCF = Toolkit.getDefaultToolkit().getImage("src/resources/img/Filter Mode 1.png");
		model.getCutoffParameter().addSynthParameterEditListener(e -> updateCutoffParameterView());
	}

	private Object updateCutoffParameterView() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void render(Graphics2D g2, double scaleX, double scaleY, ImageObserver io) {
	}
	
	@Override
	public boolean isAnimated() {
		return true;
	}
}
