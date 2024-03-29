#pragma once

#ifndef JUNCTIONTREE_H
#define JUNCTIONTREE_H

#include <unordered_set>
#include "JTClique.h"
#include "Separator.h"

/**
 * Classe che implementa il junction tree.
 *
 * @author Alessandro Gottoli vr352595
 */
class JunctionTree
{
	// le cricche che compongono l'albero
	std::unordered_set<JTClique*>* cliques;
	// i separatori che compongono l'albero
	std::unordered_set<Separator*>* links;

	// la cricca scelta come radice dell'albero (quella su cui si applica la BP)
	//JTClique* root;
	std::unordered_set<JTClique*>* roots;

	std::unordered_set<Variable*>*allVariables;

public:
	// costruisce l'albero dati l'insieme di cricche e i separatori che le uniscono
	//JunctionTree(std::unordered_set<JTClique*>*, std::unordered_set<Separator*>*);
	JunctionTree(std::unordered_set<JTClique*>*, std::unordered_set<Separator*>*, std::unordered_set<Variable*>*, std::unordered_set<JTClique*>*);
	~JunctionTree(void);

	// calcola la radice
	//void calcolaRootMigliore();
	// restituisce la radice (se non � gi� stata calcolata, la calcola pure usndo il metodo createRootMigliore)
	//JTClique* getRoot();
	std::unordered_set<JTClique*>* getRoots();

	// rappresentazione dell'albero in dot
	std::string getDOT();

	void normalizeAllPotentials() {
		for (std::unordered_set<JTClique*>::iterator cli = cliques->begin(); cli != cliques->end(); cli++) {
			if (!(*cli)->getPsi()->isNormalized())
				(*cli)->getPsi()->normalizza();
		}
		for (std::unordered_set<Separator*>::iterator sep = links->begin(); sep != links->end(); sep++) {
			if (!(*sep)->getFi()->isNormalized())
				(*sep)->getFi()->normalizza();
		}
	}

	/* CONVIENE FARLA OGNI VOLTA QUANDO AGGIORNO LE TABELLE PERCHE' ALTRIMENTI ADESSO DEVO RITRASFERIRE TUTTE LE TABELLE IN GPU
	void normalizeAllPotentialsCUDA() {
		for (std::unordered_set<JTClique*>::iterator cli = cliques->begin(); cli != cliques->end(); cli++) {
			if (!(*cli)->getPsi()->isNormalized())
				(*cli)->getPsi()->normalizza();
		}
		for (std::unordered_set<Separator*>::iterator sep = links->begin(); sep != links->end(); sep++) {
			if (!(*sep)->getFi()->isNormalized())
				(*sep)->getFi()->normalizza();
		}
	}*/
	
	// calcola la probabilit� congiunta del junction tree (moltiplica tutti i potenziali delle cricche, moltiplica i potenziali dei separatori e poi fa la divisione tra i due)
	Probability* calcolaJointProbability();

	// rappresentazione dell'albero (TODO non ha molto senso, c'� gi� il getDOT)
	std::string toString();

// prova che non mi convince
std::unordered_set<JTClique*>* getCliques()
{
	return cliques;
}

bool unicoAlbero() {
	std::cout << cliques->size() << "cliques con " << links->size() << " links." << std::endl;
	return cliques->size() == 1 + links->size();
}

void getStatistics();

	bool checkRunningIntersectionProperty();
	bool findPathSup(JTClique* father, JTClique* node, JTClique* dst, std::vector<Separator*>* path);
	std::vector<Separator*>* findPath(JTClique* src, JTClique* dst, std::size_t nCliques);
	bool isATreeWithDFSSupport(JTClique* padre, JTClique* node);
	bool isATreeWithDFS();
	//void aggiungiUnArcoACaso();

private:
	// getDOT private recursive
	std::string getDOT2(JTClique*, JTClique*);
};

#endif /* JUNCTIONTREE_H */
