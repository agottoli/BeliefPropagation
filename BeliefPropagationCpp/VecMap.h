#pragma once

#ifndef VECMAP_H
#define VECMAP_H

#include <vector>
#include <unordered_map>
//#include "Variable.h"
// da togliere
#include <iostream>

class Variable;

//const bool useMap = true;

class VecMap
{
	// numero di elementi contenuti
	std::size_t siza;
	// variabili contenute:
	// vettore per averli ordinati per inserimento
	std::vector<Variable*>* variables;
	// map per avere la ricerca efficiente in tempo costante
	// nota: il value è l'indice in cui si trova la variabile nel vettore

	/*// MEMORIA 2014-01-14 provo a togliere le hashmap per vedere se la memoria utilizzata diminuisce drasticamente oppure no
	la lascio tanto è un puntatore, ma non la uso */
	std::unordered_map<Variable*,std::size_t>* variablesMap;
	
	//bool useMap;

public:
	// costruttore con zero elementi, ma con capacity data dal parametro
	VecMap(std::size_t);
	// costruzione per copy
	VecMap(std::vector<Variable*>*, std::unordered_map<Variable*,std::size_t>*);
	~VecMap(void);

	std::vector<Variable*>::reference get(std::size_t); //std::vector<Variable*>::size_type);

	// restituisce l'indice in cui si trova la variabile nel vettore, -1 se non esiste
	//int indexOf(Variable*);
	std::size_t indexOf(Variable* var);

	// restituisce true se la variabile esiste tra le variabili
	bool exists(Variable*);

	// iteratori per il vector :)
	std::vector<Variable*>::iterator begin();
	std::vector<Variable*>::iterator end();

	// inserisco una variabile
	void push_back(Variable*);

	// ultimo elemento inserito
	std::vector<Variable*>::reference back();
	// numero di elementi presenti
	std::vector<Variable*>::size_type size();

	// copia il VecMap corrente
	VecMap* copy();

	// rappresentazione delle variabili presenti
	std::string toString();

	// prova per capire come funziona con la riserva dello spazio
	void statistiche() {
		std::cout << "siza: " << siza << " size(): " << variables->size() << " capacity(): " << variables->capacity();
	}

	std::size_t numeroElementiDellaRelativaTabella(); //{
		//std::size_t spazio = 1;
		//for (std::vector<Variable*>::iterator it = variables->begin(); it != variables->end(); it++) {
			//spazio *= (*it)->getNStates();
		//}
		//return spazio; // * (std::sizeof(double));
	//}
};

#endif /* VECMAP_H */

