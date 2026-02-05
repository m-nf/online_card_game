package client_win;

import java.awt.*;
import java.util.HashMap;
import java.util.Map;

import javax.swing.*;


public class Gui extends JFrame{
		
	private Map<Integer, ImageIcon> cards;
	private ImageIcon backsideIcon;
	private boolean infoResultLock;
	
	/* panel open */
	private JLabel ipaddressLabel;
	private JLabel portLabel;
	private JLabel nickLabel;
	private JLabel nickWarningLabel;
	private JLabel connectWarningLabel;
	private JLabel ipaddressWarningLabel;
	private JLabel portWarningLabel;
    private JTextField nickField;
    private JTextField ipaddressField;
    private JTextField portField;
    private JButton okBut;
    private JPanel openPanel;
    
    /* panel lobby */
    private JLabel lobbyInfo;
    private JLabel lobbyWaiting;
    private JLabel lobbyWinGames;
    private JPanel lobbyPanel;
    private JButton playBut;   
    
    /* panel game */
    private JLabel myCard1;
    private JLabel myCard2;
    private JLabel myCard3;
    private JLabel myCard4;
    private JLabel myCardWindow;
    private JLabel oppCard1;
    private JLabel oppCard2;
    private JLabel oppCard3;
    private JLabel oppCard4;
    private JLabel oppCardWindow;
    private JLabel stock;
    private JLabel myPoints;
    private JLabel oppPoints;
    private JLabel infoResult;
    private JLabel turn;
    private JLabel myPlayer;
    private JLabel oppPlayer;    
	private JPanel gamePanel;    
	
    public void prepareGui(GuiReactions guiReactions, MyMouseListener myMouseListener, MyWindowListener myWindowListener) {    	
    	setTitle("Vetsi bere"); // title of window	        
		//setSize(1000, 700); // widht, height
    	setExtendedState(JFrame.MAXIMIZED_BOTH);
		setLocationRelativeTo(null); // in the center of screen
		setDefaultCloseOperation(DO_NOTHING_ON_CLOSE); // end of programme when cross clicked		
		addWindowListener(myWindowListener);		
    	
    	prepareOpenPanel(guiReactions);    	
    	prepareLobbyPanel(guiReactions);    	
    	prepareGamePanel(myMouseListener);    	
    	prepareIcons();
	    gamePanel.revalidate();
	    gamePanel.repaint();

		infoResultLock = false;
	}	 
	
    /**
     * Prepares Open Panel
     */
    public void prepareOpenPanel(GuiReactions guiReactions) {
    	openPanel = new JPanel(new GridBagLayout());
    	//add(openPanel);
		GridBagConstraints cst = new GridBagConstraints();
		cst.insets = new Insets(10, 10, 10, 10); // spaces between components
		
		/*
         * fields
         */	        
        ipaddressField = new JTextField("147.228.67.112", 12);        
        cst.gridx = 1;
        cst.gridy = 0;
        openPanel.add(ipaddressField, cst);
        
        
        portField = new JTextField("10000", 12);
        cst.gridx = 1;
        cst.gridy = 1;
        openPanel.add(portField, cst);
        
        nickField = new JTextField("max", 12);
        cst.gridx = 1;
        cst.gridy = 2;
        openPanel.add(nickField, cst);
        
        /*
         * labels
         */	        
        ipaddressLabel = new JLabel("IP address:");
        cst.gridx = 0;
        cst.gridy = 0;
        openPanel.add(ipaddressLabel, cst);
        
        portLabel = new JLabel("Port:");
        cst.gridx = 0;
        cst.gridy = 1;
        openPanel.add(portLabel, cst);  
        
        nickLabel = new JLabel("Nickname (maximal lenght 10 characters):");
        cst.gridx = 0;
        cst.gridy = 2;
        openPanel.add(nickLabel, cst);
        
        ipaddressWarningLabel = new JLabel("");
        ipaddressWarningLabel.setForeground(Color.red);
        cst.gridx = 2;
        cst.gridy = 0;
        openPanel.add(ipaddressWarningLabel, cst);
        
        portWarningLabel = new JLabel("");
        portWarningLabel.setForeground(Color.red);
        cst.gridx = 2;
        cst.gridy = 1;
        openPanel.add(portWarningLabel, cst);
        
        nickWarningLabel = new JLabel("");        
        nickWarningLabel.setForeground(Color.red);
        cst.gridx = 2;
        cst.gridy = 2;
        openPanel.add(nickWarningLabel, cst);      
        
        connectWarningLabel = new JLabel("");
        connectWarningLabel.setForeground(Color.red);
        cst.gridx = 1;
        cst.gridy = 4;
        openPanel.add(connectWarningLabel, cst);     
        
        /*
         * butttons
         */
        okBut = new JButton("Ok");
        cst.gridx = 1;
        cst.gridy = 3;        
        openPanel.add(okBut, cst);
        okBut.addActionListener(guiReactions);        
		
        setContentPane(openPanel);
	}
    
    

	/**
     * Prepares Lobby Panel.
     */
    public void prepareLobbyPanel(GuiReactions guiReactions) {
    	lobbyPanel = new JPanel(new GridBagLayout());    	
    	GridBagConstraints cst = new GridBagConstraints();    	
    	cst.insets = new Insets(10, 10, 10, 10);
    	
    	/* labels */
    	lobbyInfo = new JLabel("You are in lobby. For a game click on \"Play\".");
    	cst.gridx = 0;
    	cst.gridy = 0;
    	lobbyPanel.add(lobbyInfo, cst);
    	
    	lobbyWaiting = new JLabel("");    	
    	cst.gridx = 0;
    	cst.gridy = 2;
    	lobbyPanel.add(lobbyWaiting, cst);
    	
    	lobbyWinGames = new JLabel("Won games: 0");    	
    	cst.gridx = 0;
    	cst.gridy = 3;
    	lobbyPanel.add(lobbyWinGames, cst);
    	
    	/* buttons */
    	playBut = new JButton("Play");
    	cst.gridx = 0;
    	cst.gridy = 1;
    	lobbyPanel.add(playBut, cst);
    	playBut.addActionListener(guiReactions); 	
    	
    }
	
    /**
     * Prepares Game Panel.
     */
    public void prepareGamePanel(MyMouseListener myMouseListener) {
    	gamePanel = new JPanel(new BorderLayout());    	
    	JPanel myCards = new JPanel(new FlowLayout(FlowLayout.CENTER, 10, 10));
    	JPanel oppCards = new JPanel(new FlowLayout(FlowLayout.CENTER, 10, 10));
    	JPanel gameWindow = new JPanel(new GridBagLayout());
    	JPanel players = new JPanel(new BorderLayout());
    	GridBagConstraints cst = new GridBagConstraints();    	
    	
    	cst.insets = new Insets(10, 10, 10, 10);
    	
    	/*
    	 * panel myCards
    	 */
    	myCard1 = new JLabel();
    	myCard1.setName("myCard1");
    	myCards.add(myCard1);
    	myCard1.addMouseListener(myMouseListener);
    	
    	myCard2 = new JLabel();
    	myCard2.setName("myCard2");
    	myCards.add(myCard2);
    	myCard2.addMouseListener(myMouseListener);
    	
    	myCard3 = new JLabel();
    	myCard3.setName("myCard3");
    	myCards.add(myCard3);
    	myCard3.addMouseListener(myMouseListener);
    	
    	myCard4 = new JLabel();
    	myCard4.setName("myCard4");
    	myCards.add(myCard4);
    	myCard4.addMouseListener(myMouseListener);
    	
    	/*
    	 * panel oppCards
    	 */
    	oppCard1 = new JLabel();
    	oppCards.add(oppCard1);
    	
    	oppCard2 = new JLabel();
    	oppCards.add(oppCard2);
    	
    	oppCard3 = new JLabel();
    	oppCards.add(oppCard3);
    	
    	oppCard4 = new JLabel();
    	oppCards.add(oppCard4);
    	
    	/*
    	 * panel gameWindow
    	 */
    	oppCardWindow = new JLabel();
    	cst.gridx = 1;
    	cst.gridy = 0;
    	gameWindow.add(oppCardWindow, cst);
    	
    	myCardWindow = new JLabel();
    	cst.gridx = 1;
    	cst.gridy = 3;
    	gameWindow.add(myCardWindow, cst);   	
    	
    	infoResult = new JLabel();
    	cst.gridx = 2;
    	cst.gridy = 2;
    	cst.anchor = GridBagConstraints.LINE_END;
    	gameWindow.add(infoResult, cst);
    	
    	turn = new JLabel();
    	cst.gridx = 2;
    	cst.gridy = 1;
    	cst.anchor = GridBagConstraints.LINE_END;
    	gameWindow.add(turn, cst);
    	
    	stock = new JLabel();
    	cst.gridx = 0;
    	cst.gridy = 2;
    	cst.anchor = GridBagConstraints.LINE_START;
    	gameWindow.add(stock, cst);
    	
    	
    	oppPoints = new JLabel("0");
    	cst.gridx = 2;
    	cst.gridy = 0;
    	cst.anchor = GridBagConstraints.LINE_END;
    	gameWindow.add(oppPoints, cst);
    	
    	myPoints = new JLabel("0");
    	cst.gridx = 2;
    	cst.gridy = 4;
    	cst.anchor = GridBagConstraints.LINE_END;
    	gameWindow.add(myPoints, cst);
    	
    	
    	/*
    	 * panel players
    	 */
    	myPlayer = new JLabel();
    	players.add(myPlayer, BorderLayout.SOUTH);
    	
    	//myPoints = new JLabel("0");    	
    	//players.add(myPoints, BorderLayout.SOUTH);
    	
    	oppPlayer = new JLabel();
    	players.add(oppPlayer, BorderLayout.NORTH);
    	
    	//oppPoints = new JLabel("0");    	
    	//players.add(oppPoints, BorderLayout.NORTH);   	
    	
    	
    	gamePanel.add(myCards, BorderLayout.SOUTH);
    	gamePanel.add(oppCards, BorderLayout.NORTH);
    	gamePanel.add(gameWindow, BorderLayout.CENTER);
    	gamePanel.add(players, BorderLayout.EAST);  	
    	
    }
    
    /**
     * adding icon to hashmap
     */
    public void prepareIcons() {
    	GraphicsDevice gd = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();
		int width = gd.getDisplayMode().getWidth() / 18;
        int height = gd.getDisplayMode().getHeight() / 6;
        
        cards = new HashMap<Integer, ImageIcon>();
        backsideIcon = new ImageIcon(new ImageIcon("Karty/rub.png").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH));
        
        stock.setIcon(backsideIcon);
    	
        oppCard1.setIcon(backsideIcon);
        oppCard2.setIcon(backsideIcon);
        oppCard3.setIcon(backsideIcon);
        oppCard4.setIcon(backsideIcon);        
        
        cards.put(0, new ImageIcon(new ImageIcon("Karty/c_desitka.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(1, new ImageIcon(new ImageIcon("Karty/c_devitka.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(2, new ImageIcon(new ImageIcon("Karty/c_eso.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(3, new ImageIcon(new ImageIcon("Karty/c_kral.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(4, new ImageIcon(new ImageIcon("Karty/c_osma.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(5, new ImageIcon(new ImageIcon("Karty/c_sedma.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(6, new ImageIcon(new ImageIcon("Karty/c_spodek.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(7, new ImageIcon(new ImageIcon("Karty/c_svrsek.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(8, new ImageIcon(new ImageIcon("Karty/e_desitka.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(9, new ImageIcon(new ImageIcon("Karty/e_devitka.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(10, new ImageIcon(new ImageIcon("Karty/e_eso.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(11, new ImageIcon(new ImageIcon("Karty/e_kral.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(12, new ImageIcon(new ImageIcon("Karty/e_osma.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(13, new ImageIcon(new ImageIcon("Karty/e_sedma.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(14, new ImageIcon(new ImageIcon("Karty/e_spodek.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(15, new ImageIcon(new ImageIcon("Karty/e_svrsek.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(16, new ImageIcon(new ImageIcon("Karty/k_desitka.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(17, new ImageIcon(new ImageIcon("Karty/k_devitka.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(18, new ImageIcon(new ImageIcon("Karty/k_eso.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(19, new ImageIcon(new ImageIcon("Karty/k_kral.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(20, new ImageIcon(new ImageIcon("Karty/k_osma.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(21, new ImageIcon(new ImageIcon("Karty/k_sedma.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));        
        cards.put(22, new ImageIcon(new ImageIcon("Karty/k_spodek.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(23, new ImageIcon(new ImageIcon("Karty/k_svrsek.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(24, new ImageIcon(new ImageIcon("Karty/z_desitka.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(25, new ImageIcon(new ImageIcon("Karty/z_devitka.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(26, new ImageIcon(new ImageIcon("Karty/z_eso.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(27, new ImageIcon(new ImageIcon("Karty/z_kral.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(28, new ImageIcon(new ImageIcon("Karty/z_osma.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(29, new ImageIcon(new ImageIcon("Karty/z_sedma.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(30, new ImageIcon(new ImageIcon("Karty/z_spodek.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
        cards.put(31, new ImageIcon(new ImageIcon("Karty/z_svrsek.jpg").getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH)));
 
    }
    
    /**
	 * @return the stock
	 */
	public JLabel getStock() {
		return stock;
	}

	/**
	 * @return the myCard1
	 */
	public JLabel getMyCard1() {
		return myCard1;
	}	

	/**
	 * @return the myCard2
	 */
	public JLabel getMyCard2() {
		return myCard2;
	}

	/**
	 * @return the myCard3
	 */
	public JLabel getMyCard3() {
		return myCard3;
	}	

	/**
	 * @return the myCard4
	 */
	public JLabel getMyCard4() {
		return myCard4;
	}	

	/**
	 * @return the myCardWindow
	 */
	public JLabel getMyCardWindow() {
		return myCardWindow;
	}	

	/**
	 * @return the oppCard4
	 */
	public JLabel getOppCard4() {
		return oppCard4;
	}

	/**
	 * @return the oppCardWindow
	 */
	public JLabel getOppCardWindow() {
		return oppCardWindow;
	}

	/**
	 * @return the myPlayer
	 */
	public JLabel getMyPlayer() {
		return myPlayer;
	}	

	/**
	 * @return the oppPlayer
	 */
	public JLabel getOppPlayer() {
		return oppPlayer;
	}
	
	/**
	 * @return the cards
	 */
	public Map<Integer, ImageIcon> getCards() {
		return cards;
	}

	/**
	 * @return the nickField
	 */
	public JTextField getNickField() {
		return nickField;
	}

	/**
	 * @return the ipaddressField
	 */
	public JTextField getIpaddressField() {
		return ipaddressField;
	}

	/**
	 * @return the portField
	 */
	public JTextField getPortField() {
		return portField;
	}

	/**
	 * @return the nickWarningLabel
	 */
	public JLabel getNickWarningLabel() {
		return nickWarningLabel;
	}

	/**
	 * @return the connectWarningLabel
	 */
	public JLabel getConnectWarningLabel() {
		return connectWarningLabel;
	}

	/**
	 * @return the ipaddressWarningLabel
	 */
	public JLabel getIpaddressWarningLabel() {
		return ipaddressWarningLabel;
	}

	/**
	 * @return the porttWarningLabel
	 */
	public JLabel getPortWarningLabel() {
		return portWarningLabel;
	}

	/**
	 * @return the openPanel
	 */
	public JPanel getOpenPanel() {
		return openPanel;
	}

	/**
	 * @return the lobbyPanel
	 */
	public JPanel getLobbyPanel() {
		return lobbyPanel;
	}

	/**
	 * @return the gamePanel
	 */
	public JPanel getGamePanel() {
		return gamePanel;
	}

	/**
	 * @return the myPoints
	 */
	public JLabel getMyPoints() {
		return myPoints;
	}

	/**
	 * @return the oppPoints
	 */
	public JLabel getOppPoints() {
		return oppPoints;
	}

	/**
	 * @return the backsideIcon
	 */
	public ImageIcon getBacksideIcon() {
		return backsideIcon;
	}  
    
	/**
	 * @return the lobbyWaiting
	 */
	public JLabel getLobbyWaiting() {
		return lobbyWaiting;
	}

	/**
	 * @return the turn
	 */
	public JLabel getTurn() {
		return turn;
	}
	
	/**
	 * @return the lobbyWinGames
	 */
	public JLabel getLobbyWinGames() {
		return lobbyWinGames;
	}

	/**
	 * @return the roundResult
	 */
	public JLabel getInfoResult() {
		return infoResult;
	}

	/**
	 * @return the oppCard1
	 */
	public JLabel getOppCard1() {
		return oppCard1;
	}

	/**
	 * @return the oppCard2
	 */
	public JLabel getOppCard2() {
		return oppCard2;
	}

	/**
	 * @return the oppCard3
	 */
	public JLabel getOppCard3() {
		return oppCard3;
	}

	public boolean getInfoResultLock() {
		return infoResultLock;
	}

	public void setInfoResultLock(boolean arg) {
		infoResultLock = arg;
	}
}
