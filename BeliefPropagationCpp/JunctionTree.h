#pragma once

#ifndef JUNCTIONTREE_H
#define JUNCTIONTREE_H

#include <unordered_set>
#include "JTClique.h"
#include "Separator.h"

class JunctionTree
{
	// le cricche che compongono l'albero
	std::unordered_set<JTClique*>* cliques;
	// i separatori che compongono l'albero
	std::unordered_set<Separator*>* links;

	// la cricca scelta come radice dell'albero (quella su cui si applica la BP)
	JTClique* root;

	std::unordered_set<Variable*>*allVariables;

public:
	// costruisce l'albero dati l'insieme di cricche e i separatori che le uniscono
	JunctionTree(std::unordered_set<JTClique*>*, std::unordered_set<Separator*>*);
	JunctionTree(std::unordered_set<JTClique*>*, std::unordered_set<Separator*>*, std::unordered_set<Variable*>*);
	~JunctionTree(void);

	// calcola la radice
	void calcolaRootMigliore();
	// restituisce la radice (se non � gi� stata calcolata, la calcola pure usndo il metodo createRootMigliore)
	JTClique* getRoot();

	// rappresentazione dell'albero in dot
	std::string getDOT();
	
	// calcola la probabilit� congiunta del junction tree (moltiplica tutti i potenziali delle cricche, moltiplica i potenziali dei separatori e poi fa la divisione tra i due)
	Probability* calcolaJointProbability();

	// rappresentazione dell'albero (TODO non ha molto senso, c'� gi� il getDOT)
	std::string toString();

// prova che non mi convince
std::unordered_set<JTClique*>* getCliques()
{
	return cliques;
}

private:
	// getDOT private recursive
	std::string getDOT2(JTClique*, JTClique*);
};

#endif /* JUNCTIONTREE_H */
