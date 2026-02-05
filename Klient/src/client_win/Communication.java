package client_win;

import java.awt.Color;
import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;

public class Communication extends Thread{
	
	private GameHandler gameHandler;
	private Gui gui;
	private Communication communication;
	
	private Socket socket;
    private BufferedReader reader;
    private PrintWriter writer;    
    private int port;
    private String host;
    private boolean connected;
    private boolean reconnectRunning;
	private boolean killPingThread;
    
	/**
	 * Constructor.
	 */
    public void prepareCommunication(GameHandler gameHandler, Gui gui, Communication communication) {
		this.gameHandler = gameHandler;
		this.gui = gui;
		this.communication = communication;
		connected = false;
		reconnectRunning = false;
		killPingThread = false;
	}
    
	/**
	 * Create socket for communication. Connect to server. Initializace reader and writer.
	 * @return
	 */	 
	public boolean connect() {		
		host = gui.getIpaddressField().getText();		
		try {
			// is port a number ?
			port = Integer.parseInt(gui.getPortField().getText());
			
			// create a socket to communicate to the specified host and port
	        try {
	            socket = new Socket( host, port);
	            reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));	           
	            writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));                       
	            
	            gui.getConnectWarningLabel().setText("Connected.");
	            gui.getConnectWarningLabel().setForeground(Color.black);
	            gui.getPortWarningLabel().setText("");
	            gui.getIpaddressWarningLabel().setText("");	            
	            connected = true;
	            System.out.println("Connected to: " + host);
	            
	            
	            return true;
	        }
	        catch (IOException e) {
	        	gui.getConnectWarningLabel().setText("Connection to " + host + ":" + port + " refused");         	           
	        }
	        catch (IllegalArgumentException e) {
	        	gui.getPortWarningLabel().setText("Invalid port!");        	
	        }
	        catch (NullPointerException e) {
	        	gui.getIpaddressWarningLabel().setText("Invalid IP address!");
	        }
		} catch (NumberFormatException e1) {			
			gui.getPortWarningLabel().setText("Invalid port!");
			
		}
		return false;		
	}	
	
	/**
	 * Sends msg given by parameter.
	 * @param msg
	 */
	synchronized public void sendMsg(String msg) {
		try {
    	    writer.print(msg);
            writer.flush();
            System.out.println("Send: " + msg);
        }
        catch (Exception e) {
           // System.err.println("Write error");
        }
	}	
	
	/**
	 * @param reader the reader to set
	 * @throws IOException 
	 */
	public void setReader() throws IOException {
		this.reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
	}

	/**
	 * @param writer the writer to set
	 * @throws IOException 
	 */
	public void setWriter() throws IOException {
		this.writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
	}

	/**
	 * @param socket the socket to set
	 * @throws IOException 
	 * @throws UnknownHostException 
	 */
	public void setSocket() throws UnknownHostException, IOException {		
		socket = new Socket(host,port);		
	}

	/**
	 * @param reconnectRunning the reconnectRunning to set
	 */
	public void setReconnectRunning(boolean reconnectRunning) {
		this.reconnectRunning = reconnectRunning;
	}

	/**
	 * @return the reconnectRunning
	 */
	public boolean isReconnectRunning() {
		return reconnectRunning;
	}

	/**
	 * @return the connected
	 */
	public boolean getConnected() {
		return connected;
	}
	
	/**
	 * setting variable Connected
	 */
	public void setConnected(boolean connected) {
		this.connected = connected;
	}

	/**
	 * @return the port
	 */
	public int getPort() {
		return port;
	}

	/**
	 * @return the host
	 */
	public String getHost() {
		return host;
	}

	public Socket getSocket() {
		return socket;
	}
	
	public BufferedReader getReader() {
		return reader;
	}

	public boolean getKillPingThread() {
		return killPingThread;
	}

	public void setKillPingThread(boolean arg) {
		killPingThread = arg;
	}

}
