package client_win;

import java.io.IOException;

public class ListeningThread extends Thread {
	
	private GameHandler gameHandler;
	private Gui gui;
	private Communication communication;
	private StringBuffer sBuffer;
	
	public void prepareListeningThread(GameHandler gameHandler, Gui gui, Communication communication) {
		this.gameHandler = gameHandler;
		this.gui = gui;
		this.communication = communication;
	}
	
	/**
	 * Thread for recieving communication. Recieves till \n occured.
	 * Then creates new thread recvMsgThread for processing msg.
	 */
	public void run() {
		System.out.println("nasloucahci vlakno bezi.");
		
		String msg;
		int character = 0;
		int index = 0;
		boolean invalid_char = false;
		
		while(true) {
			character = 0;
			sBuffer = new StringBuffer();
			index = 0;
			invalid_char = false;
			try {
				while((character = communication.getReader().read()) != 10) {
					if (index == 0 && (char)character != 'v') {
						invalid_char = true;
						break;
					}
					if (index == 1 && (char)character != 'b') {
						invalid_char = true;
						break;				
					}
					if (index == 2 && (char)character != '|') {
						invalid_char = true;
						break;
					}
					sBuffer.append((char) character);
					index++;
				}
				if (invalid_char == false) {
					System.out.println("Recieved: " + sBuffer.toString());
					RecvMsgThread recvMsgThread = new RecvMsgThread();
					recvMsgThread.prepareRecvMsgThread(sBuffer.toString(), gameHandler, gui);
					recvMsgThread.start();
				}
				else { // invalid char situation	
					try {
						communication.getSocket().close();
					}
					catch (Exception e) {
						System.out.println("Closing socket error (after invalid char).\n");      	           
					}
					System.out.println("Recieved: invalid char. Socket closed.\n");					
					
					communication.setKillPingThread(true);
					communication.setConnected(false);
					
					gameHandler.getMsgActive().put("ok", true);
					gameHandler.getMsgActive().put("duplicate", true);
					gameHandler.getMsgActive().put("full", false);
					gameHandler.getMsgActive().put("wait", false);
					gameHandler.getMsgActive().put("error", false);		
					gameHandler.getMsgActive().put("prepare", false);
					gameHandler.getMsgActive().put("play", false);
					gameHandler.getMsgActive().put("winner", false);
					gameHandler.getMsgActive().put("forced", false);
					gameHandler.getMsgActive().put("end", false);
					gameHandler.getMsgActive().put("oppplayedcard", false);
					gameHandler.getMsgActive().put("nickname", false);
					gameHandler.getMsgActive().put("tie", false);
					gameHandler.getMsgActive().put("lastcard", false);
					gameHandler.getMsgActive().put("start", false);
					
					gui.getConnectWarningLabel().setText("Please, connect again.");
					gui.setContentPane(gui.getOpenPanel());					
					gui.repaint();
					gui.revalidate();
					gui.getOpenPanel().repaint();
					gui.getOpenPanel().revalidate();

					break;
				}
				
			} catch (IOException e) {
				System.out.println("Reading: IO exception.");
				ReconnectThread reconnectThread = new ReconnectThread();
				reconnectThread.prepareReconnectThread(communication, gui, gameHandler);
				reconnectThread.start();
				break;
				
			} catch (NullPointerException e) {
	        	System.out.println("Reading: null pointer exception.");
	        	ReconnectThread reconnectThread = new ReconnectThread();
				reconnectThread.prepareReconnectThread(communication, gui, gameHandler);
				reconnectThread.start();
				break;
	        }
			
		}	
		
		System.out.println("naslouchaci smycka vypnuta\n");
	}
}

/*
 * public void run() {
		String msg;
		System.out.println("nasloucahci vlakno bezi.");
		while(true) {	
			try {
				communication.getReader().re
				msg = new String(communication.getReader().readLine());
				System.out.println("Recieved: " + msg);
				RecvMsgThread recvMsgThread = new RecvMsgThread();
				recvMsgThread.prepareRecvMsgThread(msg, gameHandler, gui);
				recvMsgThread.start();
				
			} catch (IOException e) {
				System.out.println("Reading: IO exception.");
				ReconnectThread reconnectThread = new ReconnectThread();
				reconnectThread.prepareReconnectThread(communication, gui, gameHandler);
				reconnectThread.start();
				break;
				
			} catch (NullPointerException e) {
	        	System.out.println("Reading: null pointer exception.");
	        	ReconnectThread reconnectThread = new ReconnectThread();
				reconnectThread.prepareReconnectThread(communication, gui, gameHandler);
				reconnectThread.start();
				break;
	        }	        
		}
		System.out.println("naslouchaci smycka vypnuta\n");
	}
	*/
