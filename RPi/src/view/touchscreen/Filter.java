package view.touchscreen;

import java.awt.Graphics2D;
import java.awt.image.ImageObserver;

public class Filter implements TouchScreenView{

	
	public Filter() {
	}

	@Override
	public void render(Graphics2D g2, double scaleX, double scaleY, ImageObserver io) {
	}
	
	@Override
	public boolean isAnimated() {
		return true;
	}
}
