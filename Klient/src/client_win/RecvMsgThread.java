package client_win;

public class RecvMsgThread extends Thread{
	String msg;
	Gui gui;	
	GameHandler gameHandler;
	
	/**
	 * Constructor.
	 */
	public void prepareRecvMsgThread(String msg, GameHandler gameHandler, Gui gui) {
		this.msg = msg;
		this.gameHandler = gameHandler;
		this.gui = gui;
	}	
	
	/**
	 * Thread action.
	 */
	public void run() {
		parseMsg();
	}
	
	/**
	 * Parse recieved message.
	 */
	public void parseMsg() {		
		String[] msgArray = msg.split("\\|");
		int numberOfSplits = msgArray.length;		
		
		if((numberOfSplits >= 1) && (msgArray[0].compareTo("vb") == 0)) {
			if((numberOfSplits == 3) && msgArray[1].compareTo("nick") == 0) {
				gameHandler.nick(msgArray);
			}
			else if((numberOfSplits >= 3) && msgArray[1].compareTo("game") == 0) {				
				gameHandler.game(msgArray, numberOfSplits);
			}
			else if((numberOfSplits >= 3) && msgArray[1].compareTo("round") == 0) {
				gameHandler.round(msgArray, numberOfSplits);				
			}			
			else if((numberOfSplits >= 3) && msgArray[1].compareTo("connection") == 0) {
				gameHandler.connection(msgArray, numberOfSplits);
			}		
			
		}
		
	}
}
