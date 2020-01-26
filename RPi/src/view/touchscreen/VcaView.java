package view.touchscreen;

import java.awt.*;
import java.awt.geom.*;
import java.awt.image.*;
import model.VcaModule;

public class VcaView implements TouchScreenView{
	
	private Image imageEnveloppe;
	
	public VcaView(VcaModule model){
		imageEnveloppe = Toolkit.getDefaultToolkit().getImage("src/resources/img/Enveloppe Mode.png");
		model.getVelocitySensitivityParameter().addChangeListener(e -> updateVelocitySensitivityParameterView());
		
	}
	
	private void updateVelocitySensitivityParameterView() {
		
	}

	@Override
	public void render(Graphics2D g2, double scaleX, double scaleY, ImageObserver io) {
		
		/*AffineTransform at = AffineTransform.getTranslateInstance(-0.5, 0.5); // image rendering is always referenced to upper left corner => need translation
		at.scale(1.0/imageEnveloppe.getWidth(io), -1.0/imageEnveloppe.getHeight(io)); // let's scale down the image so that it is a 1 by 1 square !
		g2.drawImage(imageEnveloppe, at, io);*/
		
		Path2D.Float ligne = new Path2D.Float();
		BasicStroke s = new BasicStroke(10);
		ligne.moveTo(-0.4,-0.2);
		ligne.lineTo(-0.3,0.3);
		ligne.lineTo(-0.2,0.2);
		ligne.lineTo(0.3,0.2);
		ligne.lineTo(0.4,-0.2);
		g2.setStroke(s);
		g2.draw(ligne);
	}

	@Override
	public boolean isAnimated() {
		return false;
	}
}
