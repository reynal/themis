package view.touchscreen;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.geom.AffineTransform;
import java.awt.geom.CubicCurve2D;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Line2D;
import java.awt.image.ImageObserver;

public class FilterXY extends Filter{

	private Image image;
	private double animTheta;
	
	public FilterXY() {
		image = Toolkit.getDefaultToolkit().getImage("src/resources/img/Filter Mode 2.png");
	}

	@Override
	public void render(Graphics2D g2, double scaleX, double scaleY, ImageObserver io) {

		// display logo:
		AffineTransform at = AffineTransform.getTranslateInstance(-0.5, 0.5); // image rendering is always referenced to upper left corner => need translation
		at.scale(1.0/image.getWidth(io), -1.0/image.getHeight(io)); // let's scale down the image so that it is a 1 by 1 square !
		g2.drawImage(image, at, io);
	}


	@Override
	public boolean isAnimated() {
		return true;
	}
}