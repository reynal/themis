package controller.event;

import java.util.EventObject;

/**
 * An event that indicated that a press on a button occured.
 * @author sydxrey
 *
 */
public class PushButtonActionEvent extends EventObject {

		private static final long serialVersionUID = 1L;
		
		boolean state;

		public PushButtonActionEvent(Object source) {
			super(source);
			
		}

		public PushButtonActionEvent(Object source, boolean state) {
			super(source);
			this.state = state;
		}
}
