package controller.event;

import java.util.EventListener;

public interface PushButtonActionListener extends EventListener {
	
	void actionPerformed(PushButtonActionEvent e);
	
	}
