#pragma once

#ifndef STATES_H
#define STATES_H

#include <vector>
#include <string>
#include <unordered_map>

//const bool useMapS = true;

/**
 * Classe che implementa l'insieme dei valori che può avere una variabile.
 *
 * @author Alessandro Gottoli vr352595
 */
class States
{
	// numero di elementi contenuti
	int siza;
	// variabili contenute:
	// vettore per averli ordinati per inserimento
	std::vector<std::string>* states;
	// map per avere la ricerca efficiente in tempo costante
	// nota: il value è l'indice in cui si trova la variabile nel vettore
	/*// MEMORIA 2014-01-14 provo a togliere le hashmap per vedere se la memoria utilizzata diminuisce drasticamente oppure no
	la lascio tanto è un puntatore, ma non la uso */
	std::unordered_map<std::string,int>* statesMap;

	//bool useMap;

public:
	States(int);
	~States(void);

	std::vector<std::string>::reference get(std::size_t);

	// inserisco una variabile
	void push_back(std::string);

	// restituisce l'indice in cui si trova la variabile nel vettore, SIZE_MAX se non esiste
	std::size_t indexOf(std::string);

	// iteratori per il vector :)
	std::vector<std::string>::iterator begin();
	std::vector<std::string>::iterator end();

	// numero di elementi presenti
	std::vector<std::string>::size_type size();

	// rappresentazione degli stati presenti
	std::string toString();
};

#endif /* STATES_H */

