package view.touchscreen;

import java.awt.Graphics2D;
import java.awt.Toolkit;
import java.awt.geom.AffineTransform;
import java.awt.image.ImageObserver;


public class EncodersView implements TouchScreenView{

	private java.awt.Image imageEncoders;
	
	public EncodersView(){
		imageEncoders = Toolkit.getDefaultToolkit().getImage("src/resources/img/Encodeurs.png");
	}

	@Override
	public void render(Graphics2D g2, double scaleX, double scaleY, ImageObserver io) {
		AffineTransform at = AffineTransform.getTranslateInstance(-0.5, 0.5); // image rendering is always referenced to upper left corner => need translation
		at.scale(1.0/imageEncoders.getWidth(io), -1.0/imageEncoders.getHeight(io)); // let's scale down the image so that it is a 1 by 1 square !
		g2.drawImage(imageEncoders, at, io);
		
	}
	
	@Override
	public boolean isAnimated() {
		return true;
	}
}
