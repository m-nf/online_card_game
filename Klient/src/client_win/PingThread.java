package client_win;

public class PingThread extends Thread{
	
	Communication communication;
	Gui gui;
	GameHandler gameHandler;
	
	/**
	 * Constructor
	 * @param communication
	 * @param gui
	 * @param gameHandler
	 */
	public void preparePingThread(Communication communication, Gui gui, GameHandler gameHandler) {
		this.communication = communication;
		this.gui = gui;
		this.gameHandler = gameHandler;
	}
	
	/**
	 * Pings server. Flag connected is set to false, ping msg send, if there is an answer to 5 seconds
	 * connected is set to true. Else reconnect thread is created.
	 */
	public void run() {			
		while(true) {
			if(communication.isReconnectRunning() == false) {
				communication.sendMsg("vb|connection|ping\n");
				communication.setConnected(false);
				try {
					Thread.sleep(5000); // 5 s
				} catch (InterruptedException e) {
					System.out.println("Error while waiting for ping answer.");
				}
				
				if(communication.getKillPingThread() == true) {
					break;
				}
				
				if(communication.getConnected() == false) {
					ReconnectThread reconnectThread = new ReconnectThread();
					reconnectThread.prepareReconnectThread(communication, gui, gameHandler);
					reconnectThread.start();					
				}
				try {
					Thread.sleep(10000); // 10 s
				} catch (InterruptedException e) {
					System.out.println("Error while waiting to send ping question.");
				}
			}
		}		
	}
}