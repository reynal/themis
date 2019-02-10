package application;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.*;

/**
 * The Raspberry Pi touchscreen
 */
public class TouchScreen extends JPanel {

	private static final long serialVersionUID = 3697461493768524120L;

	public TouchScreen() {
		
		super();
		setBackground(Color.black);
		//setPadding(new Insets(10));
	    //add(createSimulatorEncoders());
		//dimension(800,480);
	       //AnchorPane screen = new AnchorPane();
        //AnchorPane.setLeftAnchor(screen, 410.0);
        //AnchorPane.setLeftAnchor(screen, 230.0);
        
        // 	TODO : il faut pas encoder les resources comme ça, il faut utiliser getResource()
        //ImageView iv1 = new ImageView(new Image("file:src/resources/img/logo.png")); 
        //iv1.setStyle("-fx-border-color : grey");
		
		try {
			BufferedImage image = ImageIO.read(new File("src/resources/img/logo.png"));
		    JLabel label = new JLabel(new ImageIcon(image));
		    add(label);        
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}	

}
