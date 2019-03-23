package view.touchscreen;

import java.awt.Graphics2D;
import java.awt.Toolkit;
import java.awt.geom.AffineTransform;
import java.awt.image.ImageObserver;

import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;

public class EnveloppeView implements TouchScreenView{
	
	private java.awt.Image imageFilter1,imageFilter2;
	public EnveloppeView(){
		imageFilter1 = Toolkit.getDefaultToolkit().getImage("src/resources/img/Filter_Mode_1.png");
		imageFilter2 = Toolkit.getDefaultToolkit().getImage("src/resources/img/Filter_Mode_2.png");
	}
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

	@Override
	public void render(Graphics2D g2, double scaleX, double scaleY, ImageObserver io) {
		AffineTransform at = AffineTransform.getTranslateInstance(-0.5, 0.5); // image rendering is always referenced to upper left corner => need translation
		at.scale(1.0/imageFilter1.getWidth(io), -1.0/imageFilter1.getHeight(io)); // let's scale down the image so that it is a 1 by 1 square !
		g2.drawImage(imageFilter1, at, io);
	}

	@Override
	public boolean isAnimated() {
		return false;
	}
}
