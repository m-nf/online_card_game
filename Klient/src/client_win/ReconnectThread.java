package client_win;

public class ReconnectThread extends Thread {
	
	Communication communication;
	Gui gui;
	GameHandler gameHandler;
	
	/**
	 * Constructor.
	 * @param communication
	 * @param gui
	 * @param gameHandler
	 */
	public void prepareReconnectThread(Communication communication, Gui gui, GameHandler gameHandler) {
		this.communication = communication;
		this.gui = gui;
		this.gameHandler = gameHandler;
	}
	
	/**
	 * Sends server requests for reconnect for 30 seconds or till flag connected is true.
	 */
	public void run() {
		if(communication.isReconnectRunning() == false) {
			communication.setReconnectRunning(true);
			
			System.out.println("Reconnect thread created.\n");
			try {
	            communication.getSocket().close();
	        }
	        catch (Exception e) {
	        	System.out.println("Closing socket error. (Before reconnect)\n");      	           
	        }

			//System.out.println("Reconnecting.");
			int timeCounter = 0;			
			
			
			gui.getInfoResult().setText("Connection lost. Reconnecting, please wait.");
		    gui.getLobbyWaiting().setText("Connection lost. Reconnecting, please wait.");

			while (communication.getConnected() == false && timeCounter <= 12) {
				
				try {
					communication.setSocket();					
					communication.setReader();					
					communication.setWriter();
					
										
					ListeningThread listeningThread = new ListeningThread();
					listeningThread.prepareListeningThread(gameHandler, gui, communication);
					listeningThread.start();
					
					communication.sendMsg("vb|connection|reconnect|" + gui.getNickField().getText() + "\n");								
					
					
					sleep(6);
					if(communication.getConnected() == false) {
						communication.sendMsg("vb|connection|reconnect|" + gui.getNickField().getText() + "\n");
					}
								
				}
				catch (Exception e) {
										 
				}
						
				timeCounter++;
				try {
					Thread.sleep(5000);
				} catch (InterruptedException e) {
					System.out.println("Reconnecting sleep error.");
				}
			}
			
			if(communication.getConnected() == false) {
				gui.getInfoResult().setText("Connection lost, close and open client.");
				gui.getLobbyWaiting().setText("Connection lost, close and open client.");
				gui.getGamePanel().repaint();
				gui.getGamePanel().revalidate();
				gui.getLobbyPanel().repaint();
				gui.getLobbyPanel().revalidate();
			}
		}
	}	
}
