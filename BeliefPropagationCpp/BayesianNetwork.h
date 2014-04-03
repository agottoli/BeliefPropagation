#pragma once

#ifndef BAYESIANNETWORK_H
#define BAYESIANNETWORK_H

#include <unordered_set>
#include <queue>
#include <unordered_map>
#include <string>
#include "Variable.h"
#include "JTClique.h"
#include "JunctionTree.h"
#include "DisjointSet.h"
#include "Variable.h"

// comparatore per le variabili (ordina in modo da dare priorità a quelle che hanno meno vicini in modo da creare cricche più piccole)
//class MyComparator
//{	
//
//public:
//	bool operator() (Variable* v1, Variable* v2) {
//		// siccome la coda di priorità prende per primi quelli con elemento maggiore e a me interessa prendere prima quelli con numero minore devo mettere maggiore!!!
//		return v1->getIndiceInOrdinamentoImposto() > v2->getIndiceInOrdinamentoImposto(); // <=
//		// TODO nota che l'ho invertito per prova
//	}
//};

// comparatore per i separatori (ordina in modo da dare priorità a quelli che hanno più variabili in modo da far valere la running intersection property)
class MyComparatorSeparator
{

public:
	bool operator() (Separator* s1, Separator* s2) {
		// siccome la coda di priorità prende per primi quelli con elemento maggiore e a me interessa prendere prima quelli con numero minore devo mettere maggiore!!!
		//return s1->size() < s2->size();
		if (s1->nVars() != s2->nVars())
			return s1->nVars() < s2->nVars();

		//std::cout << "i separatori hanno stesso peso!!!\n";
		
		// se hanno stesso peso scelgo per ordinamento scelto da me
		if (s1->getSoggetto()->getId() != s2->getSoggetto()->getId())
			return s1->getSoggetto()->getId() > s2->getSoggetto()->getId();

		return s1->getOggetto()->getId() > s2->getOggetto()->getId();
		
	}
};

/**
 * Classe che implementa una bayesian network.
 *
 * @author Alessandro Gottoli vr352595
 */
class BayesianNetwork
{
	// insieme che contiene tutte le variabili che compongono la rete
	std::unordered_set<Variable*> nodes;
	// nuovo 2014-01-24
	std::unordered_map<std::string, Variable*> findNodes;
	// metto globale i done per contare la dimensione delle cricche in min...
	// insieme dei nodi già elaborati, serve per non inserirli nelle cricche successive
	//std::unordered_set<Variable*> done;

	// evidenza (coppia Variabile e numero dello stato che so che vale)
	std::unordered_map<Variable*, unsigned> evidence;
	
	// DEBUG
	std::size_t numeroElementiDelleTabelleProbability;
	std::size_t numeroCricche;
	std::size_t numeroCriccheMassimali;

	std::vector<std::string>* ordinamento;

public:
	BayesianNetwork(void);
	~BayesianNetwork(void);

	void setOrdinamento(std::vector<std::string>* ord) {
		ordinamento = ord;
	}


	// aggiunge una variabile alla rete
	void addVariable(Variable* v);
	// restituisce tutte le variabili della rete
	std::unordered_set<Variable*>* getVariables(void);

	// FASE 1 MORALIZZAZIONE
	void moralize();

	//FASE 1b introducing evidence
	//bool addVariableEvidence(Variable*, std::string);
	//bool addVariableEvidence(Variable*, int);
	//void introduceEvidence();

	// FASE 2: TRIANGOLAZIONE e IDENDIFICAZIONE CRICCHE MASSIMALI
	std::unordered_set<JTClique*>* triangolateMaxClique();

	// FASE 3: COSTRUZIONE JUNCTION TREE TRAMITE KRUSKAL
	JunctionTree* createJunctionTreeMST(std::unordered_set<JTClique*>*);
	
	// restituisce una stringa che rappresenta in dot l'albero
	std::string getDOT();

	// calcola la probabilità congiunta dela rete bayesiana moltiplicando tutte le probabilità condizionali delle variabili
	Probability* calcolaJointProbability();
	
	//DEBUG
	std::size_t getNumeroElementiDelleTabelleProbability() {
		return numeroElementiDelleTabelleProbability;
	}
	std::size_t getNumeroCricche() {
		return numeroCricche;
	}
	std::size_t getNumeroCriccheMassimali() {
		return numeroCriccheMassimali;
	}
	//

	// restituisce l'insieme di tutte le variabili
	std::unordered_set<Variable*>* getAllVariables()
	{
		return &nodes;
	}

private:
	// FASE 2 metodi di supporto
	static Variable* min(std::unordered_set<Variable*>*, std::unordered_set<Variable*>* done);
	//static Variable* minSupport(std::vector<Variable*>* minsPartenza);
	static Variable* minSupport(Variable** minsPartenza, std::size_t dim);

	// creo una nuova cricca data dall'insieme delle variabili solo se non sono un sottoinsieme delle variabili di una cricca già presente nell'insieme
	void createCliqueAndAddToSetIfMaximal(std::unordered_set<JTClique*>*, std::unordered_set<Variable*>*);
	// restituisce true se e solo se l'insieme delle variabili solo se non sono un sottoinsieme delle variabili di una cricca già presente nell'insieme
	bool isMaximalCliqueInSetOfMaximalClique(std::unordered_set<JTClique*>*, std::unordered_set<Variable*>*);
	// crea delle probabilità (vuote) per inizializzare i potenziali iniziali delle cricche create
	void initPotentialsToMaximalCliques();
	//

	// FASE 2 metodi di supporto
	// applica kruskal per trovare l'albero di ricoprimento massimo (massimo perché voglio i link con separatori massimi)
	//std::unordered_set<Separator*>* kruskal(std::priority_queue<Separator*, std::vector<Separator*>, MyComparatorSeparator>*, std::unordered_set<JTClique*>*);
	JunctionTree* kruskal(std::priority_queue<Separator*, std::vector<Separator*>, MyComparatorSeparator>*, std::unordered_set<JTClique*>*);
	//


	// metodo di supporto per la triangolarizzazione
	void triangolarizzaSupport(Variable*, std::unordered_set<Variable*>*, std::unordered_set<JTClique*>*);

};

#endif /* BAYESIANNETWORK_H */
