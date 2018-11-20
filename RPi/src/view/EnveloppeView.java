package view;

import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;

public class EnveloppeView extends Page{
	
	public static Scene env;{
		
		env.setFill(Color.BLACK);
		
		Group layout1 = new Group();
		Group layout2 = new Group();
		
		Image img = new Image("file:src/resources/img/filter2.png");
		ImageView imgView = new ImageView();
		imgView.setImage(img);
		imgView.setCache(true);
		
		layout1.setAutoSizeChildren(true);
		layout1.getChildren().addAll(imgView);
		
		Image filter1 = new Image("file:src/resources/img/filter1.png");
		ImageView IVFilter1 = new ImageView(filter1);
		
		layout2.getChildren().addAll(IVFilter1);
		
		
		}
}
