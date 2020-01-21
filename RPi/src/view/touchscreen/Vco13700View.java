package view.touchscreen;

import java.awt.Graphics2D;
import java.awt.Toolkit;
import java.awt.geom.AffineTransform;
import java.awt.image.ImageObserver;

import model.Vco13700Module;

public class Vco13700View implements TouchScreenView {
	
	private java.awt.Image imageVCO;
	private Vco13700Module model;
	
	public Vco13700View(Vco13700Module model){
		this.model = model;
		imageVCO = Toolkit.getDefaultToolkit().getImage("src/resources/img/13700.png");
		model.getDetuneParameter().addModuleParameterChangeListener(e -> updateDetuneParameterView());
		model.getOctaveParameter().addModuleParameterChangeListener(e -> updateOctaveParameterView());
		//model.getWaveShapeParameter().addModuleParameterChangeListener(e -> updateWaveShapeParameterView());
	}
	
	private Object updateWaveShapeParameterView() {
		// TODO Auto-generated method stub
		return null;
	}

	private Object updateOctaveParameterView() {
		// TODO Auto-generated method stub
		return null;
	}

	private void updateDetuneParameterView() {
		// TODO Auto-generated method stub
	}

	@Override
	public void render(Graphics2D g2, double scaleX, double scaleY, ImageObserver io) {
		AffineTransform at = AffineTransform.getTranslateInstance(-0.5, 0.5); // image rendering is always referenced to upper left corner => need translation
		at.scale(1.0/imageVCO.getWidth(io), -1.0/imageVCO.getHeight(io)); // let's scale down the image so that it is a 1 by 1 square !
		g2.drawImage(imageVCO, at, io);
		
	}

	@Override
	public boolean isAnimated() {
		return false;
	}

}

