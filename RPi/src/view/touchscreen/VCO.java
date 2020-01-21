import java.util.Calendar;
import java.awt.event.*;
import javax.swing.*;
import java.awt.*;
import java.awt.geom.Line2D;
 
public class VCO extends JFrame 
{
 
  public VCO( ) {
    super("VCO"); //Nom de la fenetre
    setSize(400,500);//Taille de la fenetre
    setDefaultCloseOperation(EXIT_ON_CLOSE);
 
    VcoLabel clock = new VcoLabel( );
    getContentPane( ).add(clock, BorderLayout.CENTER);
  }
 
  public static void main(String args[]) {
    VCO ct = new VCO( );
    ct.setVisible(true);
  }
 
  class VcoLabel extends JLabel implements ActionListener 
  	{
		//Creation du carre 
		public void paintComponent (Graphics g){	
			super.paintComponent(g);
			Graphics2D g2d = (Graphics2D)g;
			g2d.setColor(Color.red);			
			// A simple triangle.
			int x[]= {100,350,100}; 
			int y[]= {50,200,350}; 
			int n = 3;
			Polygon p = new Polygon(x, y, n);   // This polygon represents a triangle with the above
												// vertices.
			g2d.fill(p);     // Fills the triangle above.
			
			//lines
			g2d.setColor(Color.black);
			double x1,x2,x3,x4;
			x1=70;x2=100;x3=350;x4=380;
			double y1,y2,y3;
			y1=100;y2=300;y3=200;
			Line2D.Double l1 = new Line2D.Double(x1, y1, x2, y1);
			Line2D.Double l2 = new Line2D.Double(x1, y2, x2, y2);
			Line2D.Double l3 = new Line2D.Double(x3, y3, x4, y3);
			g2d.draw(l1);g2d.draw(l2);g2d.draw(l3);g2d.draw(p);
		}
		
		 public void actionPerformed(ActionEvent ae) {
			setText(String.format("%tT", Calendar.getInstance()));
			repaint();
		}
   }
 
}
