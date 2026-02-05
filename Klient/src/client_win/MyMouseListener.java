package client_win;

import java.awt.event.*;

import javax.swing.*;

public class MyMouseListener extends MouseAdapter {
	
	private boolean active;
	private GameHandler gameHandler;
	
	/**
	 * Constructor
	 * @param gameHandler
	 */
	public void prepareMyMouseListener(GameHandler gameHandler) {
		this.gameHandler = gameHandler;
		active = false;		
	}	
	
	/**
	 * Handling reaction, when is clicked on card.
	 * After clicking on card is active sets on false.
	 * Specific msg from server sets active on true.
	 */
	public void mouseClicked(MouseEvent e) {
        JLabel label = (JLabel) e.getSource();
        
        if(active == true) {
        	active = false;       	
        	
	        if(label.getName().equals("myCard1")) {	        	
	        	gameHandler.playedLabelReaction(0);	        	
	        }
	        else if(label.getName().equals("myCard2")) {
	        	gameHandler.playedLabelReaction(1);	        	
	        }
	        else if(label.getName().equals("myCard3")) {
	        	gameHandler.playedLabelReaction(2);
	        }
	        else if(label.getName().equals("myCard4")) {
	        	gameHandler.playedLabelReaction(3);
	        }            
        }
    }
	
	/**
	 * Sets variable active.
	 * @param b
	 */
	public void setActive(boolean b) {
		active = b;
	}
}
