package client_win;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class GuiReactions implements ActionListener {
	
	private Communication communication;
	private Gui gui;
	private GameHandler gameHandler;
	
	/**
	 * Constructor.
	 * @param communication
	 * @param gui
	 */
	public void prepareGuiReactions(Communication communication, Gui gui, GameHandler gameHandler) {
		this.communication = communication;
		this.gui = gui;
		this.gameHandler = gameHandler;
	}
	
	/**
	 * 
	 */
	public void actionPerformed(ActionEvent e) {
		String nick = gui.getNickField().getText();
		if(e.getActionCommand().equals("Ok")) {            
			gui.getIpaddressWarningLabel().setText("");
			gui.getPortWarningLabel().setText("");
			gui.getNickWarningLabel().setText("");
			gui.getConnectWarningLabel().setText("");
			
			// if ip, port, nick arent empty
			if(gui.getIpaddressField().getText().length() == 0 || gui.getPortField().getText().length() == 0 || (nick.length() > 10 || nick.length() == 0)) {
				if(gui.getIpaddressField().getText().length() == 0) {
					gui.getIpaddressWarningLabel().setText("Please, enter the IP address!");
				}				
				if(gui.getPortField().getText().length() == 0) {
					gui.getPortWarningLabel().setText("Please, enter the port!");
				}
				if (nick.length() == 0) {
					gui.getNickWarningLabel().setText("Please, enter the nickname!");
				}
				else {
					gui.getNickWarningLabel().setText("Nickname is too long!");
				}
			}
			else {
				if(communication.getConnected() == false) {
					if(communication.connect() == true) {
						ListeningThread listeningThread = new ListeningThread();
						listeningThread.prepareListeningThread(gameHandler, gui, communication);
						listeningThread.start();
						//communication.start(); // starts listening						
					}
				}				
				communication.sendMsg("vb|nick|" + nick + "\n");
			}			
			/*
			else if (communication.getConnected() == false) {				
				communication.connect();
				communication.sendMsg("vb|nick|" + nick + "\n");
			}			
			*/			
        }
		if(e.getActionCommand().equals("Play")) {			
			communication.sendMsg("vb|game|play\n");
			
        }
    }
		
}
