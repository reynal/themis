package model;

import java.awt.*;
import java.util.ArrayList;
import java.util.List;

import javax.swing.*;

import application.SwingMain;
import controller.component.Control;
import javafx.geometry.Insets;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.layout.GridPane;

public class AbstractModel implements SynthParameterProvider {

	protected final List<SynthParameter<?>> parameterList = new ArrayList<SynthParameter<?>>();

	public List<SynthParameter<?>> getParameters() {		
		return parameterList;
	}
	
	/**
	 * @return a pane that contains every parameter for this model
	 */
	public GridPane createSimulatorJavaFXControlGroup() {
		
		GridPane group = new GridPane();
		group.setStyle("-fx-background-color: black;"+"-fx-border-color: magenta;");
		group.setHgap(10);
		group.setVgap(10);
		group.setMinSize(250,200);
        //group.setMaxSize(250,200);
		group.setPadding(new Insets(0, 10, 0, 10));
		
		int i=0;
		for (SynthParameter<?> p : getParameters()) {
			i=i+1;
			//Label title = new Label(((SynthParameter<?>) params).getLabel()); //comment avoir un label pour la liste de paramtres
			//title.setStyle("-fx-text-fill: magenta;");

			Label label = new Label(p.getLabel());
			label.setStyle("-fx-text-fill: lightpink;");

			Control c = p.getControl();
			//System.out.println(p);
			
			Node n = c.getJavaFXView();
			//System.out.println(c.getJavaFXView());

			//group.add(title,2,0);
			group.add(n,i,1);
			//layout.add(lbl,i,1);
			group.add(label,i,2);
			
		}
		return group;
	}

	public JComponent createSimulatorSwingControlGroup() {
		JPanel group = SwingMain.createDecoratedPanel(getClass().getName());
		group.setLayout(new GridLayout(getParameters().size(), 1, 10, 10));
		//group.setStyle("-fx-background-color: black;"+"-fx-border-color: magenta;");
		//group.setBackground(Color.black);
		//group.setBorder(BorderFactory.createLineBorder(java.awt.Color.PINK));
		//group.setHgap(10);
		//group.setVgap(10);
		//group.setMinSize(250,200);
		//group.setPreferredSize(250,200);
        //group.setMaxSize(250,200);
		//group.setPadding(new Insets(0, 10, 0, 10));
		
		int i=0;
		for (SynthParameter<?> p : getParameters()) {
			i=i+1;
			//Label title = new Label(((SynthParameter<?>) params).getLabel()); //comment avoir un label pour la liste de paramtres
			//title.setStyle("-fx-text-fill: magenta;");

			JLabel label = new JLabel(p.getLabel());
			label.setForeground(Color.pink);

			Control c = p.getControl();
			//System.out.println(p);
			
			JComponent n = c.getSwingView();
			//System.out.println(c.getJavaFXView());

			//group.add(title,2,0);
			group.add(n);
			//layout.add(lbl,i,1);
			group.add(label);
			
		}
		return group;
	}	
		
}
