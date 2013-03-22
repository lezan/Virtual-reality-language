#pragma once

#include "thinkgear.h"

class bci {

public:
	// COSTRUTTURE E DECOSTRUTTORE //
	bci(void); //Costruttore classe bci; inizializza tutte le variabili
	~bci(void); //Decostruttore classe bci; non implementato
	// FUNZIONE BCI //
	void getConnection(void); //Stablisce una connessione alla bci e setta connectionId
	void enableBlinkDetection(void); //Abilita il blink detection
	void disableBlinkDetection(void); //Disabilit il blink detection
	void readData(void); //Legge i dati dalla bci e aggiorna la variabili
	int getLevelAttention(void); //Restituisce il livello attuale di attenzione
	int getLevelMeditation(void); //Restituisce il livello attuale di meditazione
	int getBlinkDetection(void); //Restitisce il livello attuale del blink detection (o dell'ultimo blink)
	bool checkBlinkDetection(void); //Controlla se è avvenuto o meno un bink
	void disconnected(void); //Libera la connessione con la bci
	void printBciInfo(void); //Stampa alcune informazioni utili della classe
	// FUNZIONI FOG //
	void checkgAvailableFog(void); //Controlla se si può attivare la nebbia
	void updateFog(void); //Aggiorna lo stato della nebbia in base al valore di attenzione

private:

	int connectionId; //Id della connessione alla bci

	int attentionLevel; //Livello di attenzione attuale
	int blinkLevel; //Livello di blink attuale
	bool blink; //E' a 'true' se si è verificato un blink, 'false' altrimenti,
	int meditationLevel; //Livello di meditazione attuale
	int signalQuality; //Qualità del segnale ricevuto

	int error; //Handler per gli errori

	int packetsRead; //Handler per la lettura dei dati dalla bci
	int numPacketsRead; //Contatore per il numero totale di pacchetti letti dalla bci

	char levelAttention[40]; //Variabile per stampare il livello di attenzione
	char levelMeditation[40]; //Variabile per stampare il livello di meditazione
	char levelBlink[40]; //Variabile per stampare il livello di blink
	char blinkOn[40]; //Variabile per stampare se ci sia stato o meno blink
	char levelSignalQuality[40]; //Variabile per stampare il livello di qualità del segnale
	char totPacketsRead[40]; //Variabile per stampare il numero totale di pacchetti letti dalla bci

	bool fogActive; //Variabile che indica se la nebbia può essere attivata o meno
};
