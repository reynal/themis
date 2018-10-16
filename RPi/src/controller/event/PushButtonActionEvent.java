package controller.event;

import java.util.EventObject;

public class PushButtonActionEvent extends EventObject {

		private static final long serialVersionUID = 1L;
		
		boolean state;

		public PushButtonActionEvent(Object source) {
			super(source);
			// TODO Auto-generated constructor stub
		}
}
