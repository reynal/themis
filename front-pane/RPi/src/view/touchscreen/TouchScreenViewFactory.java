package view.touchscreen;

import application.ModuleFactory;

/**
 * A factory that can build model views for the Raspberry touch screen.
 *  
 * @author reynal
 *
 */
public class TouchScreenViewFactory {

	private Vco3340View vco3340View;
	private Vco13700View vco13700View;
	private VcaView vcaView;
	private VcfView vcfView;
	
	public TouchScreenViewFactory(ModuleFactory f) {

		vco13700View = new Vco13700View(f.getVco13700());
		vco3340View = new Vco3340View(f.getVco3340A());
		vcfView = new VcfView(f.getVcf());
		vcaView = new VcaView(f.getVca());	
	}

	public Vco3340View getVco3340View() {
		return vco3340View;
	}

	public Vco13700View getVco13700View() {
		return vco13700View;
	}

	public VcaView getVcaView() {
		return vcaView;
	}

	public VcfView getVcfView() {
		return vcfView;
	}

	
}
