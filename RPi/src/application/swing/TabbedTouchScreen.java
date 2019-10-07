package application.swing;

import java.awt.BorderLayout;

import javax.swing.*;

import application.ModuleFactory;
import model.ModuleParameter;

/**
 * Another implementation of the TouchScreen interface based on a JTabbedPane
 * mostly for debugging purpose. Note that JSlider's here don't respond to ModuleParameter changes... 
 * 
 * @author reynal
 *
 */
@SuppressWarnings("serial")
public class TabbedTouchScreen extends JTabbedPane {

	public TabbedTouchScreen() {
		super();
		addTab("VCO 3340", new Vco3340Pane());
		addTab("VCO 13700", new Vco13700Pane());
		addTab("VCA", new VcaPane());
		addTab("VCF", new VcfPane());
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
		
		JLabel lbl = new JLabel("0", SwingConstants.CENTER);
		p.add(lbl, BorderLayout.SOUTH);
		
		js.addChangeListener(e -> {
			//System.out.println(e);
			int midiValue = ((JSlider)e.getSource()).getValue();
			lbl.setText(Integer.toString(midiValue));
			parameter.setValueFromMIDICode(midiValue);
			});
		return p;
	}
	
	/*
	 * 
	 */
	private class Vco3340Pane extends Box {
		
		Vco3340Pane(){
			
			super(BoxLayout.X_AXIS);
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340().getDetuneParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340().getOctaveParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340().getWaveShapeParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340().getSyncFrom13700Parameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco3340().getDutyParameter()));
		}
		
	}
	
	/*
	 * 
	 */
	private class Vco13700Pane extends Box {
		
		Vco13700Pane(){
			
			super(BoxLayout.X_AXIS);
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco13700().getDetuneParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco13700().getOctaveParameter()));
			add(createMidiCCSlider(ModuleFactory.getDefault().getVco13700().getWaveShapeParameter()));
					
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
	
	// ---------------------------------------------------------------------------
	
	public static void main(String[] args) {
		
		JFrame f = new JFrame("test");
		f.setContentPane(new TabbedTouchScreen());
		f.pack();
		f.setVisible(true);
	}

}
