package client_win;

import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JOptionPane;

public class MyWindowListener extends WindowAdapter {
	
	Communication communication;
	
    /**
     * Constructor.
     * @param communication
     */
	public void prepareMyWindowListener(Communication communication) {
		this.communication = communication;
	}
	
    /**
     * Sends disconnect msg to server before closing window by "cross".
     */
	public void windowClosing(WindowEvent e) {
        int confirm = JOptionPane.showOptionDialog(
             null, "Are You Sure to Close Application?", 
             "Exit Confirmation", JOptionPane.YES_NO_OPTION, 
             JOptionPane.QUESTION_MESSAGE, null, null, null);
        if (confirm == 0) {
        	communication.sendMsg("vb|connection|disconnect\n");
        	System.exit(0);
        }
    }

}
