#pragma once

#ifndef JTCLIQUE_H
#define JTCLIQUE_H

#include <unordered_set>
#include <unordered_map>
#include <vector>


class Variable;
class Probability;
class Separator;
class VecMap;

/**
 * Classe che implementa le cricche del junction tree.
 *
 * @author Alessandro Gottoli vr352595
 */

class JTClique
{
	size_t id;

	// variabili che compongono la cricca
	std::unordered_set<Variable*>* vars;
	// potenziale associata alla cricca
	Probability* psi;

	// cricche alle quali è collegata la cricca con anche i separatori associati all'arco
	std::unordered_map<JTClique*, Separator*> links;

	bool visited; // solo per debug e controllare che sia un albero

public:
	JTClique(std::unordered_set<Variable*>*);
	~JTClique(void);

	// restituisce le variabili della cricca
	std::unordered_set<Variable*>* getVars();
	// restituisce i vicini con il separatore associato
	std::unordered_map<JTClique*, Separator*>* getLinks();
	// restituisce il potenziale
	Probability* getPsi();
	// setta il potenziale
	void setPsi(Probability*);

	// restituisce true se le variabili passate sono contenute nelle variabili della cricca
	bool containsAll(std::unordered_set<Variable*>*);
	bool containsAll(VecMap*);

	// 
	void aggiungiLinkSeparatore(JTClique*, Separator*);

	// equivalente a mul + setPsi
	//void initPotenziale(ProbabilityMD*);

	// rappresentazione della stringa
	std::string toString();

	void setVisited() { // solo per debug e controllare che sia un albero
		visited = true;
	}
	void setNotVisited() { // solo per debug e controllare che sia un albero
		visited = false;
	}
	bool isVisited() { // solo per debug e controllare che sia un albero
		return visited;
	}

	size_t getId() {
		return id;
	}
};

#endif /* JTCLIQUE_H */
