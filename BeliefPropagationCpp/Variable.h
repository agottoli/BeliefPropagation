#pragma once

#ifndef VARIABLE_H
#define VARIABLE_H

#include <unordered_set>
#include <vector>


// forward declaration (da usare se ho dei puntatori a queste classi e niente più)
class Probability;
//class ProbabilityMD;
class JTClique;
class States;

class Variable
{
	// nome della variabile
	std::string label;

	// stati della variabile
	//std::vector<std::string> states;
	States* states;

	// probabilità condizionale associata alla variabile
	//Probability* probability;
	Probability* probability;

	// padri della variabile nella rete bayesiana
	std::unordered_set<Variable*>* fathers;

	// figli della variabile nella rete bayesiana
	std::unordered_set<Variable*>* sons;

	// vicini (includono sia padri che figli) mantiene tutti le variabili a cui è collegata infatti aggiungo archi durante la trafsormazione in junction tree
	std::unordered_set<Variable*>* neighbors;

	//Probability* slice;
	std::size_t evidence; // < 0 significa nessuna evidenza (SIZE_MAX)

	// parametro usato per pesare le varie variabili durante la selezione per costruire le cricche
	// calcolo il numero di vicini non già selezionati
	std::size_t numNeighboursNotSelected;
	// oppure in fase iniziale gli do una posizione nell'ordine
	//std::size_t indiceInOrdinamentoImposto;

	// insieme delle cricche in cui compare la variabile 
	// (serve per assegnare la probabilità condizionale della variabile al potenziale di una cricca guardando solo quelle in cui compare la variabile) 
	std::unordered_set<JTClique*>* cliques;

	

public:
	Variable(std::string, States*);
	~Variable(void);

	// setto la probabilità condizionale della variabile
	void setConditionalProbability(Probability* prob);
	//void setConditionalProbability(ProbabilityMD* prob);
	// restituisco la probabilità condizionale
	Probability* getConditionalProbability(void);
	//ProbabilityMD* getConditionalProbability(void);

	// aggiungo il collegamento tra variabile padre e variabile figlio per costruire la rete bayesiana
	static void linkFatherToSon(Variable* f , Variable* s) {
		f->addSon(s);
		s->addFather(f);
	}
	// aggiungo il collegamento ad una variabile figlio
	void addSon(Variable*);
	// aggiungo il collegamento ad una variabile padre
	void addFather(Variable*);
	// aggiungo il collegamento ad una variabile (padri, figli e archi aggiunti durante la fase di moralizzazione (fase 1))
	void addNeighborIfNecessary(Variable*);
	// restituisce i figli
	std::unordered_set<Variable*>* getSons(void);
	// restituisce i padri
	std::unordered_set<Variable*>* getFathers(void);
	// restituisce i nodi ai quali è collegata (arco bidirezionale)
	std::unordered_set<Variable*>* getNeighbors(void);

	// evidence
	bool addEvidence(std::string);
	bool addEvidence(std::size_t);
	std::size_t getEvidence();
	bool isEvidence();

	// restituisce il numero di nodi ai quali è collegata
	std::size_t nNeighbors(void);

	States* getStates(void);
	std::size_t getNStates();

	// restituisce l'etichetta identificativa della variabile
	std::string getLabel(void);

	// restituisce una stringa per stampare la variabile
	std::string toString(void);

	void addClique(JTClique*);
	std::unordered_set<JTClique*>* getCliques();

	//void normalizzaCP();

//private:
	// restituisce il parametro usato per ordinare la coda di priorità usata per scegliere l'ordine si selezione durante la fase di triangolarizzazione e ricerca cricche massimali
	// nota: è il numero di nodi ai quali è collegata tolti i nodi già selezionati in precedenza
	std::size_t getNumNeighboursNotSelected(void);
	// aggiorna il parametro usato per ordinare la coda decrementandolo di un'unità
	// nota: valuta il fatto di modificarlo in modo da modificare il paramero dei vicini invece di chiamarlo per ogni vicino...
	void decrementNumNeighboursNotSelected(void);

	// inserito questo metodo per problema coda
	//void calcolaParPerComparare() {
	//	parametroPerComparare = neighbors->size();
	//	//parametroPerComparare = 
	//}

	//void setIndiceInOrdinamentoImposto(std::size_t numero) {
	//	indiceInOrdinamentoImposto = numero;
	//}

	//std::size_t getIndiceInOrdinamentoImposto() {
	//	return indiceInOrdinamentoImposto;
	//}
};

#endif /* VARIABLE_H */
