package application;

import javafx.application.Application;
import javafx.geometry.Insets;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;
import javafx.scene.layout.GridPane;
import model.*;

import java.util.*;
import java.util.List;

import com.sun.prism.paint.Color;

import controller.component.Control;
import model.midi.*;
import model.spi.*;

import javax.imageio.ImageIO;
import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

/**
 * SR -> BF : Documentation a ecrire !
 */
public class Main2 extends JFrame {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	static final boolean RUNNING_ON_RPI = false;
	
	//Stage windows;
	static JWindow screen;
	static JDialog controls;
	static JPanel controlsPanel;
	static FlowLayout controlsLayout,screenLayout;
	

	
	/*
	private void createControlsGroup(GridPane group) {
		group.setStyle("-fx-background-color: black;" + "-fx-border-color: magenta;");
		group.setHgap(10);
		group.setVgap(10);
		group.setMinSize(250, 200);
		// group.setMaxSize(250,200);
		group.setPadding(new Insets(0, 10, 0, 10));
	}*/

	private static void fillControlsGroup(JPanel panel, List<SynthParameter<?>> params) {
		int i = 0;
		for (SynthParameter<?> p : params) {
			i = i + 1;
			// Label title = new Label(((SynthParameter<?>) params).getLabel()); //comment
			// avoir un label pour la liste de paramtres
			// title.setStyle("-fx-text-fill: magenta;");
			JLabel label = new JLabel(p.getLabel());
			//label.setStyle("-fx-text-fill: lightpink;");
			Control c = p.getControl();
			System.out.println(p);
			
			// n = c.getJavaSwingView();
			//System.out.println(c.getJavaFXView());
			Component n = c.createJavaSwingView();
			panel.add(n);
			panel.add(label);
			if (n instanceof JSlider && p instanceof DoubleParameter) { // TODO SR bad practice
				JSlider slider = (JSlider) n;
				slider.addChangeListener(new ChangeListener() {
				      public void stateChanged(ChangeEvent event) {
				        int value = slider.getValue();
				          System.out.println(value);}
				    });
			}
			if (n instanceof JSlider && p instanceof EnumParameter) { // TODO SR bad practice
				JSlider slider = (JSlider) n;
				slider.addChangeListener(new ChangeListener() {
				      public void stateChanged(ChangeEvent event) {
				        int value = slider.getValue();
				          System.out.println(value);}
				    });
			}
		}
	}

	public static void main(String[] args) {
		SwingUtilities.invokeLater(new Runnable(){
			
			private VcoCEM3340 vco3340;
			private VcoLM13700 vco13700;
			private VcoCEM3340 vcoDig;
			private MixerV2140D mixer;
			private VcfCEM3320 vcf;
			private VcaLM13700 vca;
			private ADSREnveloppe adsr;
			
			private List<SynthParameter<?>> paramsVco3340,paramsVco13700,paramsVcoDig,paramsMixer,paramsVcf,paramsVca,paramsAdsr;
			
			private JPanel controlsGroup1;
			private JPanel controlsGroup2;
			private JPanel controlsGroup3;
			private JPanel controlsGroup4;
			private JPanel controlsGroup5;
			private JPanel controlsGroup6;
			private JPanel controlsGroup7;

			public void run(){
				
				screen = new JWindow();
				screen.setSize(800,480);
				screen.setVisible(true);
				screen.setLocation(0,0);
				
				controls = new JDialog();
				controls.setSize(1920,500);
				controls.setTitle("Themis");
				controls.setVisible(true);
				controls.setLocation(0,500);				

				controlsPanel = new JPanel ();
				controlsLayout = new FlowLayout();
				controlsPanel.setLayout(controlsLayout);
				//JSlider slider = new JSlider();
				//controlsPanel.add(slider);
				
				controls.setContentPane(controlsPanel);
				
				vco3340 = new VcoCEM3340();
				vco13700 = new VcoLM13700();
				vcoDig = new VcoCEM3340();
				mixer = new MixerV2140D();
				vcf = new VcfCEM3320();
				vca = new VcaLM13700();
				adsr = new ADSREnveloppe();

				paramsVco3340 = vco3340.getParameters();
				paramsVco13700 = vco13700.getParameters();
				paramsVcoDig = vcoDig.getParameters();
				paramsMixer = mixer.getParameters();
				paramsVcf = vcf.getParameters();
				paramsVca = vca.getParameters();
				paramsAdsr = adsr.getParameters();
				
				controlsGroup1 = new JPanel();
				controlsGroup1.setBorder(BorderFactory.createLineBorder(java.awt.Color.BLACK));
				controlsGroup2 = new JPanel();
				controlsGroup2.setBorder(BorderFactory.createLineBorder(java.awt.Color.BLACK));
				controlsGroup3 = new JPanel();
				controlsGroup3.setBorder(BorderFactory.createLineBorder(java.awt.Color.BLACK));
				controlsGroup4 = new JPanel();
				controlsGroup4.setBorder(BorderFactory.createLineBorder(java.awt.Color.BLACK));
				controlsGroup5 = new JPanel();
				controlsGroup5.setBorder(BorderFactory.createLineBorder(java.awt.Color.BLACK));
				controlsGroup6 = new JPanel();
				controlsGroup6.setBorder(BorderFactory.createLineBorder(java.awt.Color.BLACK));
				controlsGroup7 = new JPanel();
				controlsGroup7.setBorder(BorderFactory.createLineBorder(java.awt.Color.BLACK));
				
				
				fillControlsGroup(controlsGroup1,paramsVco3340);
				fillControlsGroup(controlsGroup2,paramsVco13700);
				fillControlsGroup(controlsGroup3,paramsVcoDig);
				fillControlsGroup(controlsGroup4,paramsMixer);
				fillControlsGroup(controlsGroup5,paramsVcf);
				fillControlsGroup(controlsGroup6,paramsVca);
				fillControlsGroup(controlsGroup7,paramsAdsr);
				
				controlsPanel.add(controlsGroup1);
				controlsPanel.add(controlsGroup2);
				controlsPanel.add(controlsGroup3);
				controlsPanel.add(controlsGroup4);
				controlsPanel.add(controlsGroup5);
				controlsPanel.add(controlsGroup6);
				controlsPanel.add(controlsGroup7);
				
				//setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			}
		});
	}

}
