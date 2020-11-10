package application.javafx;

/*import controller.component.Control;
import controller.component.PushButton;
import controller.component.RotaryEncoder;
import javafx.application.Application;
import javafx.scene.*;
import model.*;
import javafx.geometry.*;
import javafx.stage.*;
import javafx.scene.control.*;
import javafx.scene.image.*;
import javafx.scene.layout.*;
*/

/**
 * UI Factory when using JavaFX API
 */
public class JavaFXMain { // extends Application {
	// implements UIFactory<Node> {
	/*
	 * 
	 * @Override public void start(Stage windows) throws Exception {
	 * 
	 * System.out.println("starting JavaFX Themis application");
	 * windows.setTitle("Themis"); windows.setX(0); windows.setY(0);
	 * windows.setResizable(false);
	 * 
	 * if (Main.SIMULATOR) windows.setScene(createSimulator()); else
	 * windows.setScene(createRaspberryTouchscreen());
	 * 
	 * 
	 * //TODO gestion message erreur communication bus SPI (fenetre ou pop up)
	 * 
	 * windows.show();
	 * 
	 * }
	 * 
	 * 
	 * // real themis private Scene createRaspberryTouchscreen(){
	 * 
	 * BorderPane touchScreenLayout = new BorderPane();
	 * touchScreenLayout.setStyle("-fx-background-color: #000;");
	 * touchScreenLayout.setPadding(new Insets(10));
	 * //touchScreenLayout.getChildren().add(createSimulatorEncoders()); return new
	 * Scene(touchScreenLayout,800,480); }
	 * 
	 * // simulator private Scene createSimulator(){
	 * 
	 * BorderPane simulatorLayout = new BorderPane();
	 * simulatorLayout.setStyle("-fx-background-color: #222;");
	 * simulatorLayout.setPadding(new Insets(10));
	 * simulatorStage.setScene(simulator);
	 * simulatorStage.setTitle("Themis Simulator");
	 * simulatorStage.setResizable(true);
	 * //simulatorLayout.setBottom(createSimulatorEncoders());
	 * simulatorLayout.setLeft(createSimulatorTouchscreen());
	 * simulatorLayout.setRight(createSimulatorPads()); return new
	 * Scene(simulatorLayout,1600,910);
	 * 
	 * }
	 * 
	 * private AnchorPane createSimulatorTouchscreen(){
	 * 
	 * AnchorPane screen = new AnchorPane(); //AnchorPane.setLeftAnchor(screen,
	 * 410.0); //AnchorPane.setLeftAnchor(screen, 230.0);
	 * 
	 * // TODO : il faut pas encoder les resources comme ça, il faut utiliser
	 * getResource() ImageView iv1 = new ImageView(new
	 * Image("file:src/resources/img/logo.png"));
	 * iv1.setStyle("-fx-border-color : grey"); screen.getChildren().add(iv1);
	 * 
	 * return screen; }
	 * 
	 * 
	 * private GridPane createSimulatorPads(){
	 * 
	 * GridPane pads = new GridPane(); pads.setPadding(new Insets(80));
	 * pads.setHgap(10); pads.setVgap(10);
	 * 
	 * // pads: int i=0; for(int x = 0; x <8; x++){ for(int y = 0; y <4; y++){
	 * i=i+1; Button butt = new Button(); butt.setMinSize(70.0,70.0);
	 * butt.setStyle("-fx-background-color : white;"); pads.add(butt,x,y); } }
	 * 
	 * return pads; }
	 * 
	 *//**
		 * Return an appropriate Swing component for the given physical control that may
		 * be used inside an interface simulator.
		 * 
		 * @return actually an instance of JavaFX Node
		 */
	/*
	 * //@Override public Node createUIForControl(Control c) {
	 * 
	 * if (c instanceof PushButton) {
	 * 
	 * Button b = new Button(); b.setMinSize(20.0,20.0);
	 * b.setStyle("-fx-background-color : white;"); // TODO : add listener that
	 * forwards to PushButton
	 * 
	 * }
	 * 
	 * else if (c instanceof RotaryEncoder) {
	 * 
	 * Slider slider = new Slider(0,127,1);
	 * slider.setOrientation(Orientation.VERTICAL); slider.setShowTickMarks(true);
	 * slider.setShowTickLabels(true); slider.setSnapToTicks(true);
	 * slider.setMajorTickUnit(1f); slider.setBlockIncrement(1f);
	 * //slider.valueProperty().addListener(new VirtualEncoderChangeListener());
	 * return slider; }
	 * 
	 * 
	 * return null; }
	 * 
	 *//**
		 * Add the given synthetizer parameter to the given container
		 * 
		 * @param container any Node that can hosts components, e.g., a Group
		 */
	/*
	 * //@Override public void addToContainer(Node container, SynthParameter<?>
	 * param) {
	 * 
	 * Group group = (Group)container; Control c = null; //param.getControl();
	 * group.getChildren().add(createUIForControl(c)); Label label = new
	 * Label(param.getLabel()); //label.setForeground(Color.pink);
	 * group.getChildren().add(label); param.addSynthParameterEditListener(e ->
	 * System.out.println(e)); }
	 *//**
		 * @return a pane that contains every parameter for this model
		 *//*
			 * //@Override public Node createSimulatorControlPane(AbstractModel model) {
			 * 
			 * GridPane group = new GridPane();
			 * group.setStyle("-fx-background-color: black;"+"-fx-border-color: magenta;");
			 * group.setHgap(10); group.setVgap(10); group.setMinSize(250,200);
			 * //group.setMaxSize(250,200); group.setPadding(new Insets(0, 10, 0, 10));
			 * 
			 * int i=0; for (SynthParameter<?> p : model.getParameters()) { i=i+1; //Label
			 * title = new Label(((SynthParameter<?>) params).getLabel()); //comment avoir
			 * un label pour la liste de paramtres
			 * //title.setStyle("-fx-text-fill: magenta;");
			 * 
			 * Label label = new Label(p.getLabel());
			 * label.setStyle("-fx-text-fill: lightpink;");
			 * 
			 * Control c = null; // p.getControl(); //System.out.println(p);
			 * 
			 * Node n = (Node)createUIForControl(c);
			 * //System.out.println(c.getJavaFXView());
			 * 
			 * //group.add(title,2,0); group.add(n,i,1); //layout.add(lbl,i,1);
			 * group.add(label,i,2);
			 * 
			 * // debug only: listen to model change: p.addSynthParameterEditListener(e ->
			 * System.out.println(e));
			 * 
			 * } return group; }
			 */

}