package view.touchscreen;

import java.awt.*;
import java.awt.geom.*;
import java.awt.image.*;

import model.Vca;

public class EnvAmp{

	private Image image;
	private double animTheta;
	
	
	public EnvAmp(){
		
		image = Toolkit.getDefaultToolkit().getImage("src/resources/img/Enveloppe Mode 1.png");
	}

	public void render(Graphics2D g2, double scaleX, double scaleY, ImageObserver io) {

		// display logo:
		AffineTransform at = AffineTransform.getTranslateInstance(-0.5, 0.5); // image rendering is always referenced to upper left corner => need translation
		at.scale(1.0/image.getWidth(io), -1.0/image.getHeight(io)); // let's scale down the image so that it is a 1 by 1 square !
		g2.drawImage(image, at, io);

	}


	public boolean isAnimated() {
		return true;
	}
}
