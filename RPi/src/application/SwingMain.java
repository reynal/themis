package application;
	
import model.*;

import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.imageio.*;
import javax.swing.*;
import javax.swing.border.*;
import controller.component.*;


/**
 * UI Factory when using Swing API
 */	
public class SwingMain extends JFrame {
	
	private static final long serialVersionUID = 1L;

	public SwingMain() throws HeadlessException {
		
		super("Themis");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		if (Main.SIMULATOR) setContentPane(createSimulator());
		else setContentPane(new TouchScreen());        
		pack();		
		setLocation(0,0);
		setResizable(false);
		System.out.println("Starting Swing Themis application");
		setVisible(true);
		
	}
	
	
	
	/**
	 * Simulator
	 * @return
	 */
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
	
	/**
	 * 
	 * @return
	 */
	private JPanel createSimulatorTouchscreen(){
        
		JPanel p = createDecoratedPanel("Simulated RPi touchscreen");
		p.add(new TouchScreen());
		return p;
	}
	
	/**
	 * 
	 * @return
	 */
	private JPanel createSimulatorEncoders(){
		
        JPanel p = createDecoratedPanel("Encoders");
        p.setLayout(new GridLayout(2,4,10,10));
        for (AbstractModel m : Main.createModels()){
        		p.add(createSimulatorControlPane(m));
        }
		return p;
	}
	
	/**
	 * 
	 * @return
	 */
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


	/**
	 * 
	 * @param model
	 * @return
	 */
	public JComponent createSimulatorControlPane(AbstractModel model) {
		
		JPanel group = SwingMain.createDecoratedPanel(getClass().getName());
		group.setLayout(new GridLayout(model.getParameters().size(), 1, 10, 10));
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
		for (SynthParameter<?> p : model.getParameters()) {
			i=i+1;
			//Label title = new Label(((SynthParameter<?>) params).getLabel()); //comment avoir un label pour la liste de paramtres
			//title.setStyle("-fx-text-fill: magenta;");

			JLabel label = new JLabel(p.getLabel());
			label.setForeground(Color.pink);

			Control c = p.getControl();
			JComponent n = createUIForControl(c);

			group.add(n);
			group.add(label);
			
			// debug only: listen to model change:
			p.addSynthParameterEditListener(e -> System.out.println(e));
			
			
		}
		return group;
	}	
	
	/**
	 * Return an appropriate Swing component for the given physical control
	 * that may be used inside an interface simulator.
	 */
	public static JComponent createUIForControl(Control c) {
		
		if (c instanceof PushButton) {
			
			JButton b = new JButton(c.getLabel());
			b.addActionListener(e -> ((PushButton)c).firePushButtonActionEvent());
			return b;

		}
		else if (c instanceof RotaryEncoder) {
			
			JPanel p = new JPanel();
			p.setBackground(Color.black);
			p.setLayout(new GridLayout(1,3));
			JButton butMinus = new JButton("<-");
			p.add(butMinus);
			JLabel lbl = new JLabel(c.getLabel());
			lbl.setForeground(Color.white);
			p.add(lbl);
			JButton butPlus = new JButton("->");
			p.add(butPlus);
			butPlus.addActionListener(e -> ((RotaryEncoder)c).fireRotaryEncoderEvent(RotaryEncoderDirection.UP));
			butMinus.addActionListener(e -> ((RotaryEncoder)c).fireRotaryEncoderEvent(RotaryEncoderDirection.DOWN));
			return p;			
			
		}
		else return null;
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