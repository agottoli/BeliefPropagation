#include "JTClique.h"
#include "Probability.h"
//#include "ProbabilityMD.h"
//#include "Separator.h";
#include "Variable.h"
#include "VecMap.h"

/**
 * Implementa i metodi per le cricche del junction tree.
 *
 * @author Alessandro Gottoli vr352595
 */

JTClique::JTClique(std::unordered_set<Variable*>* vs)
{
	vars = vs;
	psi = NULL;
}


JTClique::~JTClique(void)
{
	//vars->~unordered_set();
	//psi->~Probability();
}

std::unordered_set<Variable*>* JTClique::getVars() 
{
	return vars;
}

std::unordered_map<JTClique*, Separator*>* JTClique::getLinks()
{
	return &links;
}

Probability* JTClique::getPsi()
{
	return psi;
}

void JTClique::setPsi(Probability* psiNuovo)
{
	psi = psiNuovo;
}

bool JTClique::containsAll(std::unordered_set<Variable*>* nodiCricca)
{
	
	for (std::unordered_set<Variable*>::iterator I = nodiCricca->begin(); I != nodiCricca->end(); ++I) {
		if (vars->find(*I) == vars->end()) { // se almeno una variabile non è in vars do false!!!
			return false;
		}
	}

	return true;
}

bool JTClique::containsAll(VecMap* nodiCricca)
{
	
	for (std::vector<Variable*>::iterator I = nodiCricca->begin(); I != nodiCricca->end(); ++I) {
		if (vars->find(*I) == vars->end()) { // se almeno una variabile non è in vars do false!!!
			return false;
		}
	}

	return true;
}

void JTClique::aggiungiLinkSeparatore(JTClique* c, Separator* s)
{
	links[c] = s;
}

//void JTClique::initPotenziale(ProbabilityMD* p)
//{		
//	ProbabilityMD* old = psi;
//	psi = psi->mul(p);
//	delete old;
//}

std::string JTClique::toString() 
{
	std::string s = "{ ";
	for (std::unordered_set<Variable*>::iterator I = vars->begin(); I != vars->end(); ++I) {
		s.append((*I)->toString()).append(", ");
	}
	s.append("}");
	// si può aggiungere la stampa del potenziale
	//s.append("\npot:\n").append(psi->toString());

	return s;
}
