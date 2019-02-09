package application;
	
import model.*;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.*;
import javax.swing.border.*;


/**
 * SR -> BF :  Documentation a ecrire !
 */	
public class SwingMain extends JFrame {
	
	private static final long serialVersionUID = 1L;

	public SwingMain() throws HeadlessException {
		super("Themis");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		if (Main.SIMULATOR) setContentPane(createSimulator());
		else setContentPane(createRaspberryTouchscreen());        
		pack();		
		setLocation(0,0);
		setResizable(false);
		setVisible(true);
		
	}
	
	
	// real themis
	private JPanel createRaspberryTouchscreen(){

		JPanel touchScreenLayout = new JPanel();
		touchScreenLayout.setBackground(Color.black);
		//touchScreenLayout.setPadding(new Insets(10));
	    //touchScreenLayout.getChildren().add(createSimulatorEncoders());
		//return new Scene(touchScreenLayout,800,480);
		return touchScreenLayout;
	}
	
	// simulator
	private JPanel createSimulator(){

		JPanel p = new JPanel();
		p.setLayout(new GridLayout(2,2,10,10));
		p.setBackground(Color.black); // #222
		p.setBorder(new EmptyBorder(10,10,10,10));
		
	    //p.add(createSimulatorTouchscreen());
	    //p.add(createSimulatorPads());
	    p.add(createSimulatorEncoders());
	    
	    p.setPreferredSize(new Dimension(1600,910));
	    
	    return p;
		
	}
	
	private JPanel createSimulatorTouchscreen(){
        
        //AnchorPane screen = new AnchorPane();
        //AnchorPane.setLeftAnchor(screen, 410.0);
        //AnchorPane.setLeftAnchor(screen, 230.0);
        
        // 	TODO : il faut pas encoder les resources comme ça, il faut utiliser getResource()
        //ImageView iv1 = new ImageView(new Image("file:src/resources/img/logo.png")); 
        //iv1.setStyle("-fx-border-color : grey");
		JPanel p = createDecoratedPanel("RPi touchscreen");
		try {
			BufferedImage image = ImageIO.read(new File("src/resources/img/logo.png"));
		    JLabel label = new JLabel(new ImageIcon(image));
		    p.add(label);        
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        return p;
	}
	
	
	private JPanel createSimulatorEncoders(){
		
        JPanel p = createDecoratedPanel("Encoders");
        p.setLayout(new GridLayout(2,4,10,10));
        for (AbstractModel m : Main.createModels()){
        		p.add(m.createSimulatorSwingControlGroup());
        }
		return p;
	}
	
	private JPanel createSimulatorPads(){
		
		JPanel p = createDecoratedPanel("PADS");
		p.setLayout(new GridLayout(4,8));

        // pads:
		int i=0;
	    for(int x = 0; x <8; x++){
	    	for(int y = 0; y <4; y++){
	           	JButton butt = new JButton();
	           	//butt.setMinSize(70.0,70.0);
	           	//butt.setStyle("-fx-background-color : white;");
	           	butt.setForeground(Color.white);
	           	p.add(butt);
	        }
	    }
	    
	    return p;
	}

	// --- utilities ---
	public static JPanel createDecoratedPanel(String title) {
		JPanel p = new JPanel();
        //pads.setPadding(new Insets(80));
        //pads.setHgap(10);
        //pads.setVgap(10);
		p.setBackground(Color.black);
		p.setBorder(BorderFactory.createTitledBorder(
				new LineBorder(Color.gray, 1), 
				title, 
				TitledBorder.CENTER, 
				TitledBorder.DEFAULT_POSITION, 
				new Font("SansSerif", Font.BOLD, 10), 
				Color.white ));
		return p;
	}
	

}