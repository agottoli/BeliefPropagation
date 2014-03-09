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
	// variabili che compongono la cricca
	std::unordered_set<Variable*>* vars;
	// potenziale associata alla cricca
	Probability* psi;

	// cricche alle quali è collegata la cricca con anche i separatori associati all'arco
	std::unordered_map<JTClique*, Separator*> links;

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
};

#endif /* JTCLIQUE_H */
