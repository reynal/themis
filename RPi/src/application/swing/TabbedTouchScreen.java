package application.swing;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.io.IOException;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MidiUnavailableException;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTabbedPane;
import javax.swing.SwingConstants;

import application.ModuleFactory;
import model.ModuleParameter;
import model.midi.MidiInHandler;
import model.serial.DebugTransmitter;

/**
 * Another implementation of the TouchScreen interface based on a JTabbedPane
 * mostly for debugging purpose. Note that JSlider's here don't respond to ModuleParameter changes... 
 * 
 * @author reynal
 *
 */
@SuppressWarnings("serial")
public class TabbedTouchScreen extends JTabbedPane {
	
	private TouchScreen ts;

	public TabbedTouchScreen(MidiInHandler midiInHandler) throws IOException, InvalidMidiDataException, MidiUnavailableException {
		super();
		addTab("VCO 3340A", new Vco3340APane());
		addTab("VCO 3340B", new Vco3340BPane());
		addTab("VCO 13700", new Vco13700Pane());
		addTab("VCA", new VcaPane());
		addTab("VCF", new VcfPane());
		addTab("Piano", new VirtualPiano(midiInHandler));
	}
	
	/**
	 * Creates a tabbed pane with the given touchscreen appended to the list of tabs.
	 * @param midiInHandler
	 * @param ts
	 */
	public TabbedTouchScreen(MidiInHandler midiInHandler, TouchScreen ts) throws IOException, InvalidMidiDataException, MidiUnavailableException {
		
		this(midiInHandler);
		this.ts = ts;
		addTab("TouchScreen", ts);
	}
	
	/*
	 *  Utility that creates a slider that can control the given parameter. 
	 */
	private JPanel createMidiCCSlider(ModuleParameter<?> parameter) {
		
		JPanel p = new JPanel();
		p.setLayout(new BorderLayout());
		
		JLabel title = new JLabel(parameter.getLabel(), SwingConstants.CENTER);
		p.add(title, BorderLayout.NORTH);

		JSlider js = new JSlider(JSlider.VERTICAL, 0, parameter.getValuesCount()-1, 0);
		p.add(js, BorderLayout.CENTER);
		
		JLabel lbl = new JLabel(parameter.getValue().toString(), SwingConstants.CENTER);
		p.add(lbl, BorderLayout.SOUTH);
		
		js.addChangeListener(e -> {
			//System.out.println(e);
			int midiValue = ((JSlider)e.getSource()).getValue();			
			parameter.setValueFromMIDICode(midiValue);
			//lbl.setText(Integer.toString(midiValue));
			lbl.setText(parameter.getValue().toString());
			});
		return p;
	}
	
	/*
	 * 
	 */
	private class Vco3340APane extends Box {
		
		Vco3340APane(){
			
			super(BoxLayout.X_AXIS);
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340A().getDetuneParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340A().getSemitonesParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340A().getOctaveParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340A().getWaveShapeParameter()));
			//add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340().getSyncFrom13700Parameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340A().getDutyParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340A().getLevelParameter()));
		}
		
	}
	
	/*
	 * 
	 */
	private class Vco3340BPane extends Box {
		
		Vco3340BPane(){
			
			super(BoxLayout.X_AXIS);
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340B().getDetuneParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340B().getSemitonesParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340B().getOctaveParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340B().getPulseLevelParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340B().getSawLevelParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340B().getTriLevelParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340B().getDutyParameter()));
		}
		
	}

	/*
	 * 
	 */
	private class Vco13700Pane extends Box {
		
		Vco13700Pane(){
			
			super(BoxLayout.X_AXIS);
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco13700().getDetuneParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco13700().getSemitonesParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco13700().getOctaveParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco13700().getSquLevelParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco13700().getTriLevelParameter()));
					
		}
	}	
	
	/*
	 * 
	 */
	private class VcaPane extends Box {
		
		VcaPane(){
			
			super(BoxLayout.X_AXIS);
			add(createMidiCCSlider(ModuleFactory.getDefault().getVca().getAdsrEnveloppe().getAttackMsParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVca().getAdsrEnveloppe().getDecayMsParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVca().getAdsrEnveloppe().getSustainLevelParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVca().getAdsrEnveloppe().getReleaseMsParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVca().getVelocitySensitivityParameter()));
					
		}
	}	

	/*
	 * 
	 */
	private class VcfPane extends Box {
		
		VcfPane(){
			
			super(BoxLayout.X_AXIS);
			add(createMidiCCSlider(ModuleFactory.getDefault().getVcf().getAdsrEnveloppe().getAttackMsParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVcf().getAdsrEnveloppe().getDecayMsParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVcf().getAdsrEnveloppe().getSustainLevelParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVcf().getAdsrEnveloppe().getReleaseMsParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVcf().getCutoffParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVcf().getResonanceParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVcf().getFilterOrderParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVcf().getEgDepthParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVcf().getKbdTrackingParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVcf().getVelocitySensitivityParameter()));
		}
	}	
	
	/**
	 * Caller need to call setVisible(true) on returned JFrame.
	 * @return
	 */
	public JFrame createWrappingJFrame() {

		JFrame f = new JFrame("Themis TabbedTouchScreen");
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		//f.setLayout(new BorderLayout(10, 10));
		//f.setBackground(Color.black); // #222

		f.setContentPane(this);
		f.setPreferredSize(new Dimension(800, 400)); // RPi 7" screen resolution
		f.setLocation(850,0);


		if (ts != null) 
			f.setJMenuBar(ts.createJMenuBar());

		f.pack();
		return f;
	}	
	
	public void openJFrame() {
		JFrame f = createWrappingJFrame();
		f.setVisible(true);
		
	}
	
	// ---------------------------------------------------------------------------
	
	public static void main(String[] args) throws MidiUnavailableException, IOException, InvalidMidiDataException {
		
		MidiInHandler midiInHandler = new MidiInHandler(new DebugTransmitter(), 1);
		TabbedTouchScreen tts = new TabbedTouchScreen(midiInHandler);
		tts.openJFrame();
	}

}
