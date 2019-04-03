package view.touchscreen;

import java.awt.BasicStroke;
import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.Toolkit;
import java.awt.geom.AffineTransform;
import java.awt.geom.Path2D;
import java.awt.image.ImageObserver;

import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import model.Vca;

public class EnveloppeView implements TouchScreenView{
	
	private java.awt.Image imageEnveloppe;
	private Vca model;
	
	public EnveloppeView(Vca model){
		this.model = model;
		imageEnveloppe = Toolkit.getDefaultToolkit().getImage("src/resources/img/Enveloppe Mode.png");
		model.getVelocityTrackingParameter().addSynthParameterEditListener(e -> updateVelocityTrackingParameterView());
		model.getEnveloppeGeneratorDepthParameter().addSynthParameterEditListener(e -> updateEnveloppeGeneratorDepthParameterView());
		
	}
	
	private Object updateEnveloppeGeneratorDepthParameterView() {
		// TODO Auto-generated method stub
		return null;
	}

	private Object updateVelocityTrackingParameterView() {
		
		return null;
	}

	@Override
	public void render(Graphics2D g2, double scaleX, double scaleY, ImageObserver io) {
		
		AffineTransform at = AffineTransform.getTranslateInstance(-0.5, 0.5); // image rendering is always referenced to upper left corner => need translation
		at.scale(1.0/imageEnveloppe.getWidth(io), -1.0/imageEnveloppe.getHeight(io)); // let's scale down the image so that it is a 1 by 1 square !
		g2.drawImage(imageEnveloppe, at, io);
		/*Path2D.Float ligne = new Path2D.Float();
		BasicStroke s = new BasicStroke(10);
		ligne.moveTo(-0.4,-0.2);
		ligne.lineTo(-0.3,0.3);
		ligne.lineTo(-0.2,0.2);
		ligne.lineTo(0.3,0.2);
		ligne.lineTo(0.4,-0.2);
		g2.setStroke(s);
		g2.draw(ligne);*/
	}

	@Override
	public boolean isAnimated() {
		return false;
	}
}
