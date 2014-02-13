#include "Probability.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include "Config.h"

Probability::Probability(VecMap* vars, double initValue)
{
	variables  = vars;
	
	if (vars->size() > 0) {
	
		std::size_t dim = 1;
		for (std::vector<Variable*>::iterator var = vars->begin(); var != vars->end(); var++) {
			dim *= (*var)->getNStates();
		}

		// controllo per non allocare miliardi di elementi
		if (dim >= 100000000) {
			std::cout << "stai creando una tabella di " << dim << " elementi.";
			std::string s;
			std::cin >> s;
			//Config::tabellaScazza = true;
		} //else {

		table = new double[dim];
		tableSize = dim;

		for (std::size_t i = 0; i < dim; i++) {
			table[i] = initValue;
		}

		sommaTabella = dim * initValue;
		//}
	} else {
		// tabella vuota
		// NON DOVREBBE MAI ACCADERE
		sommaTabella = 0;
		table = new double[0];
		tableSize = 0;
	}

}

Probability::Probability(VecMap* vars, double* pot, std::size_t ts)
{
	variables  = vars;
	table = pot;
	tableSize = ts;
	sommaTabella = 0;
	// mi calcolo la somma degli elementi della tabella
	for (std::size_t i = 0; i < ts; i++) {
		sommaTabella += pot[i];
	}
}

Probability::Probability(VecMap* vars, double* pot, std::size_t ts, double sommaTab)
{
	variables  = vars;
	tableSize = ts;
	table = pot;
	sommaTabella = sommaTab;
}


Probability::~Probability()
{
	//delete variables; // non posso cancellarle che le condivido con altri fi
	delete table;
}

VecMap* Probability::getVariables()
{
	return variables;
}

double* Probability::getTable()
{
	return table;
}

void Probability::normalizza()
{
	//std::cout << "faccio una normalizzazione per somma = " << sommaTabella;
	// controllo la normalizzazione
	double sommaNormalizzata = 0;
	for (std::size_t i = 0; i < tableSize; i++) {
		table[i] /= sommaTabella;
		sommaNormalizzata += table[i];
	}
	//std::cout << " che diventa = " << sommaNormalizzata;
	//std::string s;
	//std::cin >> s;
}

void Probability::normalizzaCPSeServe()
{
	std::size_t nStatesLastVar = variables->back()->getNStates();

	double somma = 0;
	double sommaParziale;
	for (std::size_t i = 0; i < tableSize / nStatesLastVar; i++) {
		sommaParziale = 0;
		for (std::size_t j = 0; j < nStatesLastVar; j++) {
			sommaParziale += table[i * nStatesLastVar + j];
		}
		if (abs(sommaParziale - 1) > epsilon) {
			for (std::size_t j = 0; j < nStatesLastVar; j++) {
				table[i * nStatesLastVar + j] /= sommaParziale;
				somma += table[i * nStatesLastVar + j];
			}
		} else {
			somma += sommaParziale;
		}
	}
	sommaTabella = somma;
}

double* Probability::getAtConfig(std::vector<std::size_t*>* config)
{
	//std::size_t index = 0;
	//std::size_t nStati = 1;
	//for (std::size_t i = config->size(); i > 0; ) {
	//	--i;
	//	index += *(config->at(i)) * nStati;
	//	nStati *= variables->get(i)->getNStates();
	//}
	//return &table->at(index);

	return &table[getIndexOfConfig(config)];
}

double* Probability::getAtConfig(std::vector<std::size_t>* config)
{
	return &table[getIndexOfConfig(config)];
}

void Probability::setAtConfig(std::vector<std::size_t*>* config, double el)
{
	//std::size_t index = 0;
	//std::size_t nStati = 1;
	//for (std::size_t i = config->size(); i > 0; ) {
	//	--i;
	//	index += *(config->at(i)) * nStati;
	//	nStati *= variables->get(i)->getNStates();
	//}
	//sommaTabella = sommaTabella + el - table->at(index);
	table[getIndexOfConfig(config)] = el;
}

std::size_t Probability::getIndexOfConfig(std::vector<std::size_t*>* config)
{
	std::size_t index = 0;
	std::size_t nStati = 1;
	for (std::size_t i = config->size(); i > 0; ) {
		--i;
		index += *(config->at(i)) * nStati;
		nStati *= variables->get(i)->getNStates();
	}

	// DEBUG
	//std::cout << "indice " << index << std::endl;
	//

	return index;
}

std::size_t Probability::getIndexOfConfig(std::vector<std::size_t>* config)
{
	std::size_t index = 0;
	std::size_t nStati = 1;
	for (std::size_t i = config->size(); i > 0; ) {
		--i;
		index += config->at(i) * nStati;
		nStati *= variables->get(i)->getNStates();
	}

	// DEBUG
	//std::cout << "indice " << index << std::endl;
	//

	return index;
}

void Probability::addAtConfig(std::vector<std::size_t*>* config, double el)
{
	//std::size_t index = 0;
	//std::size_t nStati = 1;
	//for (std::size_t i = config->size(); i > 0; ) {
	//	--i;
	//	index += *(config->at(i)) * nStati;
	//	nStati *= variables->get(i)->getNStates();
	//}
	//sommaTabella = sommaTabella + el - table->at(index);
	table[getIndexOfConfig(config)] += el;
}

bool Probability::nextConfig(std::vector<std::size_t>* config)
{
	for (std::size_t i = config->size(); i > 0; ) {
		--i;
		if (config->at(i) == variables->get(i)->getNStates() - 1) {
			config->at(i) = 0;
		} else {
			config->at(i)++;
			//break;
			return true;
		}
	}

	// se arrivo qua significa che ho fatto il giro
	return false;
}

bool Probability::nextConfig(std::vector<std::size_t*>* config)
{
	for (std::size_t i = config->size(); i > 0; ) {
		--i;
		if (*config->at(i) == variables->get(i)->getNStates() - 1) {
			*config->at(i) = 0;
		} else {
			(*config->at(i))++;
			//break;
			return true;
		}
	}

	// se arrivo qua significa che ho fatto il giro
	return false;
}

void Probability::mul(Probability* other)
{

	// DEBUG
	//std::cout << "MUL:\nP1)\n" << this->toString() << "\nP2)\n" << other->toString() << std::endl;
	//


	std::vector<std::size_t> config (variables->size());

	// controllo che tutte le variabili della tabella other siano presenti in this
	// e così mi salvo dove si trovano
	VecMap* otherVars = other->getVariables();
	//std::vector<std::size_t>* indexOtherInThis = new std::vector<std::size_t>();
	std::vector<std::size_t*>* configOther = new std::vector<std::size_t*>();
	configOther->reserve(otherVars->size());
	for(std::vector<Variable*>::iterator elOther = otherVars->begin(); elOther !=  otherVars->end(); elOther++) {
		std::size_t index = variables->indexOf(*elOther);
		if (index == SIZE_MAX) {
			// non c'è la variabile allora ferma tutto che c'è un errore!!!
			std::cout << "Probability::mul: La variabile " << *elOther << " non è presente!!" << std::endl;
			std::string stop;
			std::cin >> stop;
		}
		//indexOtherInThis->push_back(index);
		configOther->push_back(&config.at(index));
	}

	double somma = 0;
	
	for (std::size_t i = 0; i < tableSize; i++) {
	
		table[i] *= *(other->getAtConfig(configOther));
		somma += table[i];

		// DEBUG
		/*if (*(other->getAtConfig(configOther)) < 0 || *(other->getAtConfig(configOther)) > 1) {
			std::cout << "errore elemento < 0";
			std::string sss;
			std::cin >> sss;
		}*/
		//

		nextConfig(&config);

	}

	// DEBUG
	//std::cout << "RIS:\nP1)\n" << this->toString() << std::endl;
	//std::string sss;
	//std::cin >> sss;
	//

	sommaTabella = somma;

	
}

Probability* Probability::sumOnNotPresent(Probability* fi)
{
	std::vector<std::size_t> config (variables->size());

	// controllo che tutte le variabili della tabella other siano presenti in this
	// e così mi salvo dove si trovano
	VecMap* fiVars = fi->getVariables();
	std::vector<std::size_t*>* configOther = new std::vector<std::size_t*>();
	configOther->reserve(fiVars->size());
	for(std::vector<Variable*>::iterator elOther = fiVars->begin(); elOther != fiVars->end(); elOther++) {
		std::size_t index = variables->indexOf(*elOther);
		if (index == SIZE_MAX) {
			// non c'è la variabile allora ferma tutto che c'è un errore!!!
			std::cout << "Probability::mul: La variabile " << *elOther << " non è presente!!" << std::endl;
			std::string stop;
			std::cin >> stop;
		}
		//indexOtherInThis->push_back(index);
		configOther->push_back(&config.at(index));
	}

	Probability* fiStar = new Probability(fiVars, 0.0);

	for (std::size_t i = 0; i < tableSize; i++) {
		
		fiStar->addAtConfig(configOther, table[i]);

		nextConfig(&config);

	}

	fiStar->setSommaTabella(sommaTabella);

	return fiStar;
}

// le variabili devono essere uguali e in ordine
void Probability::aggiornaOrdinato(Probability* fiStar, Probability* fi) {

	double* tableFiStar = fiStar->getTable();
	double* tableFi = fi->getTable();

	if (fiStar->getTableSize() != fi->getTableSize()) {
		std::cout << "dimensioni delle fi diverse -> qualcosa non va!" <<std::endl;
		std::string sss;
		std::cin >> sss;
	}

	//Probability probabilityDiv (fiStar->getVariables(), 0.0);
	double* tabellaDiv = new double[fiStar->getTableSize()]; //probabilityDiv.getTable();

	double somma = 0;

	for (std::size_t i = 0; i < fiStar->getTableSize(); i++) {
		if (tableFi[i] <= zero) {
			//std::cout << "divisione per zero!" << std::endl;
			tabellaDiv[i] = 0;
		} else {

			tabellaDiv[i] = tableFiStar[i] / tableFi[i];
			somma += tabellaDiv[i];
		}

	}
	Probability probabilityDiv (fiStar->getVariables(), tabellaDiv, fiStar->getTableSize());

	this->mul(&probabilityDiv);
}

void Probability::divOrdinato(Probability* other)
{
	double* otherTable = other->getTable();
	double somma = 0;
	for (std::size_t i = 0; i < tableSize; i++) {
		
		if (otherTable[i] <= zero) {
			//std::cout << "divisione per zero!" << std::endl;
			table[i] = 0;
		} else {

			table[i] /= otherTable[i];
			somma += table[i];

		}
	}
	sommaTabella = somma;
}

bool Probability::confronta(Probability* other) {

	bool esito = true;

	// devono avere la stessa dimensione, altrimenti sono diverse per forza
	if (variables->size() != other->variables->size())
		return false;


	std::vector<std::size_t> config (variables->size());

	// controllo che tutte le variabili della tabella other siano presenti in this
	// e così mi salvo dove si trovano
	VecMap* otherVars = other->getVariables();
	//std::vector<std::size_t>* indexOtherInThis = new std::vector<std::size_t>();
	std::vector<std::size_t*>* configOther = new std::vector<std::size_t*>();
	configOther->reserve(otherVars->size());
	for(std::vector<Variable*>::iterator elOther = otherVars->begin(); elOther !=  otherVars->end(); elOther++) {
		std::size_t index = variables->indexOf(*elOther);
		if (index == SIZE_MAX) {
			// non c'è la variabile allora ferma tutto che c'è un errore!!!
			std::cout << "Probability::confronta: La variabile " << (*elOther)->toString() << " non è presente!!" << std::endl;
			std::string stop;
			std::cin >> stop;
			return false;
		}
		//indexOtherInThis->push_back(index);
		configOther->push_back(&config.at(index));
	}

	
	for (std::size_t i = 0; i < tableSize; i++) {

		if (table[i] < 0 || table[i] > 100000) {
			std::cout << "valore tabella 1 strano: " << table[i];
			esito = false;
			std::string sss;
			std::cin >> sss;
		}

		if (*(other->getAtConfig(configOther)) < 0 || *(other->getAtConfig(configOther)) > 100000) {
			std::cout << "valore tabella 2 strano: " << *(other->getAtConfig(configOther));
			esito = false;
			std::string sss;
			std::cin >> sss;
		}

		if (std::abs(table[i] - *(other->getAtConfig(configOther))) > epsilon) {
			// diversi
			esito = false;
			std::cout << "\tERROR\t";
			std::string sss;
			std::cin >> sss;
		} else {
			// uguali
			//std::cout << "\tOK\t";
		}
		//std::cout << table[i] << " - " << *(other->getAtConfig(configOther)) << "\n";

		nextConfig(&config);

	}

	return esito;
}

std::string Probability::toString()
{
	std::string s = variables->toString().append("\n");

	for (std::size_t i = 0; i < tableSize; i++) {
		s.append(std::to_string(table[i])).append("\n");
	}
	return s;
}

Probability* Probability::copy()
{
	double* newTable = new double[tableSize];
	//memcpy(newTable, table, tableSize);
	for (std::size_t i = 0; i < tableSize; i++) {
		newTable[i] = table[i];
	}
	return new Probability(variables, newTable, tableSize, sommaTabella);
}
