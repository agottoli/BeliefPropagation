#pragma once 

#ifndef PROBABILITY_H
#define PROBABILITY_H

#include <vector>
#include <map>
//#include <boost/lexical_cast.hpp>
#include <cmath> // per std::abs
#include "Variable.h"
#include "VecMap.h"

#include "Config.h"

/*
#ifndef EPSILON_SOGLIA
#define EPSILON_SOGLIA 0.0000000001 //L // 10^-9 per il confronto dei valori delle probabilit�, pu� esserci una leggera differenza per arrotondamenti vari
#endif

#ifndef ZERO_DIVISIONE
#define ZERO_DIVISIONE 0.0 //L // per gestire il fatto della divisione 0/0 che da 0 invece di NaN
#endif
*/

/**
 * Classe che implementa la probabilit�.
 *
 * @author Alessandro Gottoli vr352595
 */

class Probability
{
	// variabili in ordine in cui compaiono nella tabella
	VecMap* variables;
	// vettore che contiene tutte le probabilit� delle varie configurazioni
	//std::vector<double>* table;
	double* table;
	std::size_t tableSize;

	// contiene la somma dei valori della tabella (se = 1 ok, altrumenti andrebbe normalizzata)
	double sommaTabella;

public:

	// inizializzata tutta a initValue
	Probability(VecMap* vars, double initValue);

	//Probability::Probability(VecMap* vars, std::vector<double>* pot);
	Probability(VecMap* vars, double* pot, std::size_t ts);

	//Probability::Probability(VecMap* vars, std::vector<double>* pot, double sommaTab);
	Probability(VecMap* vars, double* pot, std::size_t ts, double sommaTab);

	~Probability();

	// restituisce le variabili
	VecMap* getVariables();
	// restituisce la tabella delle probabilit�
	//std::vector<double>* getTable();
	double* getTable();
	void setTable(double* newTable) {
		table = newTable;
	}

	// calcola la nuova probabilit� moltiplicando la probabilit� per quella passata
	// NOTA: aggiorna la tabella this e quindi tutte le variabili presenti in other devono comparire anche in this!!!
	void mul(Probability* other);

	// le variabili devono avere lo stesso ordine per funzionare!!!
	void divOrdinato(Probability* other);
	void aggiornaOrdinato(Probability* fiStar, Probability* fi);
	//

	//
	std::string toString();

	// restituisce true se e solo se i valori di ogni probabilit� con la stessa configurazione differiscono di < epsilon (10^-9)
	bool confronta(Probability* other);

	bool isNormalized() {
		//std::cout << sommaTabella << '\t';
		return std::abs(sommaTabella - 1) <= EPSILON_SOGLIA;
	}

	// normalizza la tabella della probabilit� attuale e tengo la somma dentro il parametro sommaTabella quando la costruisco???
	void normalizza();

	double getSommaTabella() {
		return sommaTabella;
	}
	void setSommaTabella(double somma) {
		sommaTabella = somma;
	}

	std::size_t getTableSize() {
		return tableSize;
	}

	Probability* sumOnNotPresent(Probability* fi);
	double* getAtConfig(std::vector<std::size_t*>* config);
	double* getAtConfig(std::vector<std::size_t>* config);
	bool nextConfig(std::vector<std::size_t>* config);
	bool nextConfig(std::vector<std::size_t*>* config);
	void setAtConfig(std::vector<std::size_t*>* config, double el);
	void addAtConfig(std::vector<std::size_t*>* config, double el);
	std::size_t getIndexOfConfig(std::vector<std::size_t*>* config);
	std::size_t getIndexOfConfig(std::vector<std::size_t>* config);

	void normalizzaCPSeServe();

	Probability* copy();

};

#endif
