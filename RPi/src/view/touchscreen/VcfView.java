package view.touchscreen;

import java.awt.*;
import java.awt.geom.*;
import java.awt.image.*;

import model.VcfModule;

/**
 * A view appropriate for a VCF module.
 * 
 * TODO : write doc ; class not working yet
 * 
 * @author bastien
 *
 */
public class VcfView implements TouchScreenView{

	private Image imageVCF;
	private VcfModule model;
	
	public VcfView(VcfModule model){
		this.model = model;
		imageVCF = Toolkit.getDefaultToolkit().getImage("src/resources/img/Filter Mode 1.png");
		// or Image image = new ImageIcon(this.getClass().getResource("/images/img.png")).
		
		model.getCutoffParameter().addModuleParameterChangeListener(e -> updateCutoffParameterView());
		model.getResonanceParameter().addModuleParameterChangeListener(e -> updateResonanceParameterView());
		model.getKbdTrackingParameter().addModuleParameterChangeListener(e -> updateKbdTrackingParameterView());
		model.getEgDepthParameter().addModuleParameterChangeListener(e -> updateEgDepthParameterView());
	}

	private void updateEgDepthParameterView() {
	}

	private void updateKbdTrackingParameterView() {
	}

	private void updateResonanceParameterView() {
	}

	private void updateCutoffParameterView() {
	}

	@Override
	public void render(Graphics2D g2, double scaleX, double scaleY, ImageObserver io) {
		AffineTransform at = AffineTransform.getTranslateInstance(-0.5, 0.5); // image rendering is always referenced to upper left corner => need translation
		at.scale(1.0/imageVCF.getWidth(io), -1.0/imageVCF.getHeight(io)); // let's scale down the image so that it is a 1 by 1 square !
		g2.drawImage(imageVCF, at, io);
		
	}
	
	@Override
	public boolean isAnimated() {
		return false;
	}
}
