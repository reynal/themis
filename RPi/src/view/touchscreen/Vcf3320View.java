package view.touchscreen;

import java.awt.Graphics2D;
import java.awt.Toolkit;
import java.awt.geom.AffineTransform;
import java.awt.image.ImageObserver;

import model.Vcf3320;
import model.Vco13700;

public class Vcf3320View implements TouchScreenView{

	private java.awt.Image imageVCF;
	private Vcf3320 model;
	
	public Vcf3320View(Vcf3320 model){
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
		AffineTransform at = AffineTransform.getTranslateInstance(-0.5, 0.5); // image rendering is always referenced to upper left corner => need translation
		at.scale(1.0/imageVCF.getWidth(io), -1.0/imageVCF.getHeight(io)); // let's scale down the image so that it is a 1 by 1 square !
		g2.drawImage(imageVCF, at, io);
		
	}
	
	@Override
	public boolean isAnimated() {
		return true;
	}
}
