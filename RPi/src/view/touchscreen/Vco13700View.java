package view.touchscreen;

import java.awt.*;
import java.awt.geom.*;
import java.awt.image.*;

import model.SynthParameter;
import model.Vco13700;
import model.Vco3340;
import model.event.SynthParameterEditEvent;
import model.event.SynthParameterEditListener;

public class Vco13700View implements TouchScreenView {
	
	private java.awt.Image imageVCO;
	private Vco13700 model;
	
	public Vco13700View(Vco13700 model){
		this.model = model;
		imageVCO = Toolkit.getDefaultToolkit().getImage("src/resources/img/13700.png");
		model.getDetuneParameter().addSynthParameterEditListener(e -> updateDetuneParameterView());
		model.getOctaveParameter().addSynthParameterEditListener(e -> updateOctaveParameterView());
		model.getWaveShapeParameter().addSynthParameterEditListener(e -> updateWaveShapeParameterView());
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
		// TODO Auto-generated method stub
		return false;
	}

}

