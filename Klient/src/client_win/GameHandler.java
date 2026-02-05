package client_win;

import java.util.HashMap;
import java.util.Map;

public class GameHandler {
	private Gui gui;
	private Communication communication;
	private MyMouseListener myMouseListener;
	private GameHandler gameHandler;
	
	private int[] cardsInHand;
	private int playedLabel;
	private Map<String, Boolean> msgActive;
	private boolean lastcard;
	
	public void prepareGameHandler(Gui gui, Communication communication, MyMouseListener myMouseListener, GameHandler gameHandler) {
		this.gui = gui;
		this.communication = communication;
		this.myMouseListener = myMouseListener;
		this.gameHandler = gameHandler;
		
		lastcard = false;
		cardsInHand = new int[4]; // count of cards in hand
		msgActive = new HashMap<String, Boolean>();
		
		msgActive.put("ok", true);
		msgActive.put("duplicate", true);
		msgActive.put("full", false);
		msgActive.put("wait", false);
		msgActive.put("error", false);		
		msgActive.put("prepare", false);
		msgActive.put("play", false);
		msgActive.put("winner", false);
		msgActive.put("forced", false);
		msgActive.put("end", false);
		msgActive.put("oppplayedcard", false);
		msgActive.put("nickname", false);
		msgActive.put("tie", false);
		msgActive.put("lastcard", false);
		msgActive.put("start", false);		
	}
	
	/*
	 * sets icon of MyCardWindow on played card
	 * sets icon of label of played card on null
	 * call a method which send played card to server  
	 */
	public void playedLabelReaction(int i) {
		playedLabel = i;
    	int playedCard = getPlayedCard();
    	
    	if(playedCard != -1) { // for caution
	    	gui.getMyCardWindow().setIcon(gui.getCards().get(playedCard));
	    	
	    	if(playedLabel == 0) gui.getMyCard1().setIcon(null);
	    	else if(playedLabel == 1) gui.getMyCard2().setIcon(null);
	    	else if(playedLabel == 2) gui.getMyCard3().setIcon(null);
	    	else if(playedLabel == 3) gui.getMyCard4().setIcon(null);
	    	
	    	communication.sendMsg("vb|round|playedcard|" + playedCard + "\n");
    	}    	
	}
	
	synchronized public void nick(String[] msgArray) { 
		if(msgArray[2].compareTo("ok") == 0 && msgActive.get("ok") == true) {
			gui.getNickWarningLabel().setText("");
			
			communication.setKillPingThread(false);
			PingThread pingThread = new PingThread();
			pingThread.preparePingThread(communication, gui, gameHandler);
			pingThread.start();			
			
			gui.setContentPane(gui.getLobbyPanel());
			gui.revalidate();
			gui.repaint();
			gui.getMyPlayer().setText(gui.getNickField().getText());			
			
			msgActive.put("ok", false);
			msgActive.put("duplicate", false);
			msgActive.put("full", true);
			msgActive.put("wait", true);
			msgActive.put("error", true);
			msgActive.put("lastcard", true);	
			msgActive.put("forced", true);
			msgActive.put("winner", true);			
		}		
		else if(msgArray[2].compareTo("duplicate") == 0 && msgActive.get("duplicate") == true) {
			gui.getNickWarningLabel().setText("The nickname is already used. Please, choose another.");
		}

	}
	
	synchronized public void game(String[] msgArray, int numberOfSplits) {
		if(numberOfSplits == 8 && msgArray[2].compareTo("prepare") == 0 && msgActive.get("prepare") == true) {//opp_nickname, cards 1-4
			/* setting gui */
			gui.getOppPlayer().setText(msgArray[3]);
			gui.getMyCard1().setIcon(gui.getCards().get(Integer.parseInt(msgArray[4])));
			gui.getMyCard2().setIcon(gui.getCards().get(Integer.parseInt(msgArray[5])));
			gui.getMyCard3().setIcon(gui.getCards().get(Integer.parseInt(msgArray[6])));
			gui.getMyCard4().setIcon(gui.getCards().get(Integer.parseInt(msgArray[7])));				
			
			/* setting things from previous game */
			gui.getInfoResult().setText("");
			gui.getMyCardWindow().setIcon(null);
			gui.getOppCardWindow().setIcon(null);
			gui.getStock().setIcon(gui.getBacksideIcon());			
			gui.getOppPoints().setText("0");
			gui.getMyPoints().setText("0");
			
			gui.getOppCard1().setIcon(gui.getBacksideIcon());
			gui.getOppCard2().setIcon(gui.getBacksideIcon());	
			gui.getOppCard3().setIcon(gui.getBacksideIcon());	
			gui.getOppCard4().setIcon(gui.getBacksideIcon());
			
			/* setting cards in hand */
			cardsInHand[0] = Integer.parseInt(msgArray[4]);
			cardsInHand[1] = Integer.parseInt(msgArray[5]);
			cardsInHand[2] = Integer.parseInt(msgArray[6]);
			cardsInHand[3] = Integer.parseInt(msgArray[7]);
			
			msgActive.put("full", false);
			msgActive.put("wait", false);
			msgActive.put("error", false);
			msgActive.put("prepare", false);
			msgActive.put("play", true);
		}
		else if(numberOfSplits == 3 && msgArray[2].compareTo("full") == 0 && msgActive.get("full") == true) {
			gui.getLobbyWaiting().setText("We are sorry. Game rooms are full.");
		}
		else if(numberOfSplits == 3 && msgArray[2].compareTo("wait") == 0 && msgActive.get("wait") == true) {
			gui.getLobbyWaiting().setText("Waiting for opponent.");
			gui.getLobbyPanel().revalidate();
			gui.getLobbyPanel().repaint();
			
			msgActive.put("prepare", true);
		}
		else if(numberOfSplits == 4 && msgArray[2].compareTo("wait") == 0 && msgActive.get("error") == true) {
			if(msgArray[3].compareTo("error") == 0) {
				gui.getLobbyWaiting().setText("No opponent found.");
				gui.getLobbyPanel().revalidate();
				gui.getLobbyPanel().repaint();
			}
		}
		else if(numberOfSplits == 4 && msgArray[2].compareTo("play") == 0 && msgActive.get("play") == true) {			
			if(msgArray[3].length() <= 10 && 
				(msgArray[3].compareTo(gui.getOppPlayer().getText()) == 0 || msgArray[3].compareTo(gui.getMyPlayer().getText()) == 0)) {
				lastcard = false;
				
				gui.getTurn().setText("Plays: " + msgArray[3]);
				gui.getLobbyWaiting().setText("");
				gui.setContentPane(gui.getGamePanel());
				gui.repaint();
				gui.revalidate();
				
				if(msgArray[3].compareTo(gui.getMyPlayer().getText()) == 0) {
					myMouseListener.setActive(true);
				}
				
				msgActive.put("play", false);
				msgActive.put("oppplayedcard", true);
			}			
		}
		else if(numberOfSplits == 4 && msgArray[2].compareTo("winner") == 0 && msgActive.get("winner") == true) {			
			if(msgArray[3].compareTo(gui.getOppPlayer().getText()) == 0 || msgArray[3].compareTo(gui.getMyPlayer().getText()) == 0 ||
				msgArray[3].compareTo("tie") == 0) {
				if(msgArray[3].compareTo("tie") == 0) {					
					gui.getInfoResult().setText("Game result: draw." + msgArray[3]);					
				}
				else {					
					gui.getInfoResult().setText("Winner of game is " + msgArray[3] + ".");					
				}
				gui.getTurn().setText("");
				
				msgActive.put("forced", false);
				msgActive.put("winner", false);				
				msgActive.put("full", false);
				msgActive.put("wait", false);
				msgActive.put("error", false);		
				msgActive.put("prepare", false);
				msgActive.put("play", false);				
				msgActive.put("oppplayedcard", false);
				msgActive.put("nickname", false);
				msgActive.put("tie", false);				
				msgActive.put("start", false);

				msgActive.put("end", true);
			}
		}
		else if(numberOfSplits == 5 && msgArray[2].compareTo("winner") == 0 && msgActive.get("forced") == true) {
			if(msgArray[3].compareTo("forced") == 0 && 
				(msgArray[4].compareTo(gui.getOppPlayer().getText()) == 0 || msgArray[4].compareTo(gui.getMyPlayer().getText()) == 0)) {
				
				gui.getInfoResult().setText("Opponent left the game. Winner is " + msgArray[4]);
				gui.getTurn().setText("");				
				
				msgActive.put("forced", false);
				msgActive.put("winner", false);				
				msgActive.put("full", false);
				msgActive.put("wait", false);
				msgActive.put("error", false);		
				msgActive.put("prepare", false);
				msgActive.put("play", false);				
				msgActive.put("oppplayedcard", false);
				msgActive.put("nickname", false);
				msgActive.put("tie", false);				
				msgActive.put("start", false);

				msgActive.put("end", true);
			}
		}
		else if(numberOfSplits == 4 && msgArray[2].compareTo("end") == 0 && msgActive.get("end") == true) {
			gui.getLobbyWinGames().setText("Games won: " + msgArray[3]);
			gui.setContentPane(gui.getLobbyPanel());
			gui.getLobbyWaiting().setText("");
			gui.repaint();
			gui.revalidate();
			
			msgActive.put("full", true);
			msgActive.put("wait", true);
			msgActive.put("error", true);
		}
		gui.getGamePanel().revalidate();
		gui.getGamePanel().repaint();
		gui.getLobbyPanel().revalidate();
		gui.getLobbyPanel().repaint();		
	}

	synchronized public void round(String[] msgArray, int numberOfSplits) {
		if(numberOfSplits == 4 && msgArray[2].compareTo("oppplayedcard") == 0 && msgActive.get("oppplayedcard") == true) {						
			try {
				int oppPlayedCard = Integer.parseInt(msgArray[3]);
				if (oppPlayedCard >= 0 && oppPlayedCard <= 31) {
					gui.getOppCardWindow().setIcon(gui.getCards().get(oppPlayedCard));
					
					msgActive.put("oppplayedcard", false);
					msgActive.put("nickname", true);
					msgActive.put("tie", true);					
				}
				if(lastcard == false) {
					gui.getOppCard4().setIcon(null);
				}
				if(lastcard == true) {
					if(gui.getOppCard1().getIcon() != null) {
						gui.getOppCard1().setIcon(null);
					}
					else if(gui.getOppCard2().getIcon() != null) {
						gui.getOppCard2().setIcon(null);
					}
					else if(gui.getOppCard3().getIcon() != null) {
						gui.getOppCard3().setIcon(null);
					}
					else if(gui.getOppCard4().getIcon() != null) {
						gui.getOppCard4().setIcon(null);
					}
				}
			} catch (NumberFormatException e) {
				
			}
			
		}
		else if(numberOfSplits == 7 && msgArray[2].compareTo("roundresult") == 0 && msgActive.get("nickname") == true) {
			if ((msgArray[3].compareTo("nickname") == 0 && 
				(msgArray[4].compareTo(gui.getOppPlayer().getText()) == 0 || msgArray[4].compareTo(gui.getMyPlayer().getText()) == 0))) {
				if(gui.getInfoResultLock() == false) {
					gui.getInfoResult().setText("Winner of round is " + msgArray[4]);			
				}
				gui.getMyPoints().setText(msgArray[5]);
				gui.getOppPoints().setText(msgArray[6]);
				
				msgActive.put("nickname", false);
				msgActive.put("tie", false);
				msgActive.put("lastcard", true);
				msgActive.put("start", true);
			}		
		}
		else if(numberOfSplits == 6 && msgArray[2].compareTo("roundresult") == 0 && msgActive.get("tie") == true) {
			if(msgArray[3].compareTo("tie") == 0) {
				if(gui.getInfoResultLock() == false) {
					gui.getInfoResult().setText("It's a draw.");
				}
				gui.getMyPoints().setText(msgArray[4]);
				gui.getOppPoints().setText(msgArray[5]);
				
				msgActive.put("nickname", false);
				msgActive.put("tie", false);
				msgActive.put("lastcard", true);
				msgActive.put("start", true);
			}
		}
		else if(numberOfSplits == 5 && msgArray[2].compareTo("start") == 0 && msgActive.get("start") == true) {
			try {
				int cardToDraw = Integer.parseInt(msgArray[3]);
				
				if ((cardToDraw >= -1 && cardToDraw <= 31) && 
					(msgArray[4].compareTo(gui.getOppPlayer().getText()) == 0 || msgArray[4].compareTo(gui.getMyPlayer().getText()) == 0)) {				
					
					if(gui.getInfoResultLock() == false) {
						gui.getInfoResult().setText("");
					}
					gui.getMyCardWindow().setIcon(null);
					gui.getOppCardWindow().setIcon(null);
					
					if(lastcard == false) {
						gui.getOppCard4().setIcon(gui.getBacksideIcon());
					}
					
					if(cardToDraw != -1) {
						if(playedLabel == 0) {
							gui.getMyCard1().setIcon(gui.getCards().get(cardToDraw));
						}
						else if(playedLabel == 1) {
							gui.getMyCard2().setIcon(gui.getCards().get(cardToDraw));
						}
						else if(playedLabel == 2) {
							gui.getMyCard3().setIcon(gui.getCards().get(cardToDraw));
						}
						else if(playedLabel == 3) {
							gui.getMyCard4().setIcon(gui.getCards().get(cardToDraw));
						}
					}
					cardsInHand[playedLabel] = cardToDraw;
					
					gui.getTurn().setText("Plays: " + msgArray[4]);
					if(msgArray[4].compareTo(gui.getMyPlayer().getText()) == 0) {
						myMouseListener.setActive(true);
					}
					
					msgActive.put("start", false);					
					msgActive.put("oppplayedcard", true);					
				}				
			} catch (NumberFormatException e) {
				
			}			
		}
		else if(numberOfSplits == 3 && msgArray[2].compareTo("lastcard") == 0 && msgActive.get("lastcard") == true) {
			lastcard = true;
			
			gui.getStock().setIcon(null);
			
			msgActive.put("lastcard", false);
		}
		else if(numberOfSplits == 4 && msgArray[2].compareTo("turn") == 0 &&
				(msgArray[3].compareTo(gui.getOppPlayer().getText()) == 0 || msgArray[3].compareTo(gui.getMyPlayer().getText()) == 0)) {
			gui.getTurn().setText("Plays: " + msgArray[3]);
			if(msgArray[3].compareTo(gui.getMyPlayer().getText()) == 0) {
				myMouseListener.setActive(true);
			}
		}
		gui.repaint();
		gui.revalidate();
		gui.getGamePanel().revalidate();
		gui.getGamePanel().repaint();
		gui.getLobbyPanel().revalidate();
		gui.getLobbyPanel().repaint();
	}
	
	synchronized public void connection(String[] msgArray, int numberOfSplits) {
		if(numberOfSplits == 3 && msgArray[2].compareTo("ping") == 0) {
			communication.sendMsg("vb|connection|ping|ok\n");
		}
		else if(numberOfSplits == 4 && msgArray[2].compareTo("ping") == 0) {
			communication.setConnected(true);
		}
		else if(numberOfSplits == 3 && msgArray[2].compareTo("oppdisconnected") == 0) {
			gui.getInfoResult().setText("Opponent is temporarily disconnected. Please wait.");
			
			//gui.setInfoResultLock(true);

		}
		else if(numberOfSplits == 3 && msgArray[2].compareTo("oppconnected") == 0) {
			//gui.setInfoResultLock(false);
			gui.getInfoResult().setText("Opponent is reconnected.");		
		}
		/*
		else if(numberOfSplits == 4 && msgArray[2].compareTo("reconnect") == 0) {
			if(msgArray[3].compareTo("ok") == 0) {
				communication.setConnected(true);
				gui.getInfoResult().setText("Reconnected.");				
			}
		}
		*/
		else if(numberOfSplits == 5 && msgArray[2].compareTo("reconnect") == 0) {
			if(msgArray[3].compareTo("lobby") == 0) {
				communication.setConnected(true);				
				communication.setReconnectRunning(false);
				
				gui.getLobbyWinGames().setText("Won games: " + msgArray[4]);
	            gui.getLobbyWaiting().setText("Reconnected.");
				gui.setContentPane(gui.getLobbyPanel());
				gui.repaint();
				gui.revalidate();				
			}
			
		}		
		else if(numberOfSplits == 12 && msgArray[2].compareTo("reconnect") == 0) {
			int card1, card2, card3, card4, playedCard;
			
			try {
				communication.setConnected(true);
				communication.setReconnectRunning(false);
				gui.getInfoResult().setText("Reconnected.");
				
				card1 = Integer.parseInt(msgArray[3]); // cards can be -1
				card2 = Integer.parseInt(msgArray[4]);
				card3 = Integer.parseInt(msgArray[5]);
				card4 = Integer.parseInt(msgArray[6]);
				playedCard = Integer.parseInt(msgArray[10]);			
				
				gui.getTurn().setText("Plays: " + msgArray[7]);
				gui.getMyPoints().setText(msgArray[8]);
				gui.getOppPoints().setText(msgArray[9]);
				gui.getOppPlayer().setText(msgArray[11]);

				if(playedCard != -1 && (msgArray[7].compareTo(gui.getMyPlayer().getText()) == 0)) {
					gui.getOppCardWindow().setIcon(gui.getCards().get(playedCard));
				}
				else if(playedCard != -1 && (msgArray[7].compareTo(gui.getMyPlayer().getText()) != 0)) {
					gui.getMyCardWindow().setIcon(gui.getCards().get(playedCard));
				}
				else {
					gui.getMyCardWindow().setIcon(null);
					gui.getOppCardWindow().setIcon(null);
				}
				
				if(card1 != -1) {
					gui.getMyCard1().setIcon(gui.getCards().get(card1));
					cardsInHand[0] = card1;
				}
				else if(card1 == -1) {
					gui.getMyCard1().setIcon(null);
				}
				if(card2 != -1) {
					gui.getMyCard2().setIcon(gui.getCards().get(card2));
					cardsInHand[1] = card2;
				}
				else if(card2 == -1) {
					gui.getMyCard2().setIcon(null);
				}
				if(card3 != -1) {
					gui.getMyCard3().setIcon(gui.getCards().get(card3));
					cardsInHand[2] = card3;
				}
				else if(card3 == -1) {
					gui.getMyCard3().setIcon(null);
				}
				if(card4 != -1) {
					gui.getMyCard4().setIcon(gui.getCards().get(card4));
					cardsInHand[3] = card4;
				}
				else if(card4 == -1) {
					gui.getMyCard4().setIcon(null);
				}
				
				int myplayedcard = getPlayedCard();
				//System.out.println("played " + playedcard + "karty" + card1 + " " + card2 + " "  + card3 + " " + card4);
				if(myplayedcard == card1 || myplayedcard == card2 || myplayedcard == card3 || myplayedcard == card4) {
					System.out.println("nastavuju moje okno\n");
					gui.getMyCardWindow().setIcon(null);
				}
				
				if(msgArray[7].compareTo(gui.getMyPlayer().getText()) == 0) {
					myMouseListener.setActive(true);
				}
				
				gui.setContentPane(gui.getGamePanel());
				gui.revalidate();
				gui.repaint();
				
				msgActive.put("reconnect", false);
				msgActive.put("full", true);
				msgActive.put("wait", true);
				msgActive.put("error", true);		
				msgActive.put("prepare", true);
				msgActive.put("play", true);
				msgActive.put("winner", true);
				msgActive.put("forced", true);
				msgActive.put("end", true);		
				msgActive.put("oppplayedcard", true);
				msgActive.put("nickname", true);
				msgActive.put("tie", true);
				msgActive.put("lastcard", true);
				msgActive.put("start", true);
			} catch (NumberFormatException e) {
				
			}			
		}

		gui.getGamePanel().revalidate();
		gui.getGamePanel().repaint();
		gui.getLobbyPanel().revalidate();
		gui.getLobbyPanel().repaint();
	}
	
	public void setPlayedLabel(int playedLabel) {
		this.playedLabel = playedLabel;
	}
	
	public int getPlayedLabel() {
		return playedLabel;
	}
	
	public void setCardsInHand(int card) {
		cardsInHand[playedLabel] = card;
	}
	
	public int getPlayedCard() {		
		return cardsInHand[playedLabel];
	}
	
	/**
	 * @return the msgActive
	 */
	public Map<String, Boolean> getMsgActive() {
		return msgActive;
	}
			

}
