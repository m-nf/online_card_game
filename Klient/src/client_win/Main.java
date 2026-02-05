package client_win;

public class Main {
	
	/**
	 * Main method of programme. Creates instances, prepares them and runs gui.
	 * @param args
	 */
	public static void main(String[] args) {
		Gui gui = new Gui();
		MyMouseListener myMouseListener = new MyMouseListener();
		GameHandler gameHandler = new GameHandler();
		GuiReactions guiReactions = new GuiReactions();
		Communication communication = new Communication();
		MyWindowListener myWindowListener = new MyWindowListener();
		
		gui.prepareGui(guiReactions, myMouseListener, myWindowListener);
		myMouseListener.prepareMyMouseListener(gameHandler);
		gameHandler.prepareGameHandler(gui, communication, myMouseListener, gameHandler);
		guiReactions.prepareGuiReactions(communication, gui, gameHandler);
		communication.prepareCommunication(gameHandler, gui, communication);
		myWindowListener.prepareMyWindowListener(communication);
				
	    gui.setVisible(true);
	}

}
