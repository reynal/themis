package view.touchscreen;

import java.awt.*;
import java.awt.geom.*;
import java.awt.image.*;

/**
 * a default implementation of TouchScreenView that displays an animated THeMiS logo. 
 * @author SR
 *
 */
public class DefaultView implements TouchScreenView {

	private Image image;
	private double animTheta;
	
	public DefaultView() {
		image = Toolkit.getDefaultToolkit().getImage("src/resources/img/logo.png");
	}

	@Override
	public void render(Graphics2D g2, double scaleX, double scaleY, ImageObserver io) {

		animTheta += 0.1;
		
		// animate clipping:
		g2.setPaint(Color.orange);
		g2.clip(new Ellipse2D.Double(0.3*Math.sin(0.24*animTheta)-.25,0.3*Math.cos(0.4*animTheta)-.25,.5,.5));

		// display logo:
		AffineTransform at = AffineTransform.getTranslateInstance(-0.5, 0.5); // image rendering is always referenced to upper left corner => need translation
		at.scale(1.0/image.getWidth(io), -1.0/image.getHeight(io)); // let's scale down the image so that it is a 1 by 1 square !
		g2.drawImage(image, at, io);
		
		// display animated shapes:		
		g2.setPaint(Color.red);
		g2.draw(new Line2D.Double(0,0,0.5*Math.cos(animTheta),0.5*Math.sin(animTheta)));		
		g2.setPaint(Color.cyan);
		g2.draw(new CubicCurve2D.Double(-0.5, 0, -0.25, 0.5*Math.cos(0.6*animTheta), 0.25, 0.5*Math.sin(0.85*animTheta), 0.5, 0));
		
		

	}


	@Override
	public boolean isAnimated() {
		return true;
	}

}
