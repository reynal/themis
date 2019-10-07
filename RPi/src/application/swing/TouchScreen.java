package application.swing;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;

import javax.swing.*;

import application.ModuleFactory;
import model.event.ModuleParameterChangeEvent;
import model.event.ModuleParameterChangeListener;
import view.touchscreen.*;

/**
 * The Raspberry Pi touchscreen
 */
public class TouchScreen extends JPanel implements ActionListener, ModuleParameterChangeListener {

	private static final long serialVersionUID = 1L;
	private TouchScreenView view;
	private Timer animationTimer;
	private static final int ANIMATION_TIMER = 20; // ms (a tester sur RPi, attention � ne pas consommer trop de ressources)

    private static final Logger LOGGER = Logger.getLogger("confLogger");


	/**
	 * 
	 */
	public TouchScreen() {
		
		super();
		//setPreferredSize(new Dimension(500,500));
		setBackground(Color.black);
		animationTimer = new Timer(ANIMATION_TIMER , this);
		//view = new FilterVal();
		view = new DefaultView();
		setView(view);

		
	}
	
	
	/**
	 * 
	 * @param view
	 */
	public void setView(TouchScreenView view) {
		
		this.view = view;
		if (view.isAnimated()) animationTimer.start();
		else animationTimer.stop();
		repaint();
		
		
	}

	
	@Override
	protected void paintComponent(Graphics g) {
		
		super.paintComponent(g);
		Graphics2D g2 = (Graphics2D)g;
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON); 
		g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON); // lissage rendu texte
		g2.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR); // lissage de rendu d'image VALUE_INTERPOLATION_BICUBIC better but slower
		// scale so that a 1x1 square now covers the whole hosting JPanel: 
		double sx = getWidth();
		double sy = -getHeight();
		g2.scale(sx, sy);
		// translate so that the (0,0) point is at the center of the hosting JPanel:
		g2.translate(0.5, -0.5);
		// shrinks stroke down to make up for the previous scaling:
		g2.setStroke(new BasicStroke((float)(1.0/Math.max(sx, sy))));
		view.render(g2, sx, sy, this);
		
	}	
	
	@Override
	public void actionPerformed(ActionEvent e) {
		
		repaint();
		
	}
	
	@Override
	public void moduleParameterChanged(ModuleParameterChangeEvent e) {
		// TODO check if this edit is for us, and if this is the case, 
		// possibly change the view and/or forward change to the current view 
		// and finally call repaint()
		
	}
	

	
	

}
