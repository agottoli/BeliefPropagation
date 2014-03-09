#include <iostream>
//#include <boost/lexical_cast.hpp>
#include <algorithm>
#include "Variable.h"
#include "Probability.h"
//#include "ProbabilityMD.h"
#include "States.h"

/**
 * Implementa i metodi delle variabili.
 *
 * @author Alessandro Gottoli vr352595
 */
Variable::Variable(std::string l, States* s)
{
	// DEBUG
	//std::cout << "creata variabile: " << l << std::endl;
	//
	
	//indiceInOrdinamentoImposto = SIZE_MAX; // chi non è nell'ordinamento lo tengo per ultimo!!!
	numNeighboursNotSelected = 0;
	label = l;
	states = s;
	evidence = SIZE_MAX;

	fathers = new std::unordered_set<Variable*>();
	sons = new std::unordered_set<Variable*>();
	neighbors = new std::unordered_set<Variable*>();
	cliques = new std::unordered_set<JTClique*>();
}


Variable::~Variable(void)
{
	//probability->~Probability();
	//fathers->~unordered_set();
	//sons->~unordered_set();
	//neighbors->~unordered_set();
	//cliques->~unordered_set();
	//states->~States();
}

void Variable::setConditionalProbability(Probability* prob)
{
	probability = prob;
}

//void Variable::setConditionalProbability(ProbabilityMD* prob)
//{
//	probability = prob;
//}

Probability* Variable::getConditionalProbability()
{
	return probability;
}

//static void linkFatherToSon(Variable* f, Variable* s)
//{
//	f->addSon(s);
//	s->addFather(f);
//}

void Variable::addSon(Variable* v) {
	sons->insert(v);
	addNeighborIfNecessary(v);
}

void Variable::addFather(Variable* v)
{
	fathers->insert(v);
	addNeighborIfNecessary(v);
}

void Variable::addNeighborIfNecessary(Variable* v)
{
	// aggiunge ai vicini e aumenta di uno il parametro per comparare neighbors.
	if (neighbors->find(v) == neighbors->end()) {
		neighbors->insert(v);
		// TODO commentato per problema coda
		numNeighboursNotSelected++;
	}
}

std::unordered_set<Variable*>* Variable::getSons(void) {
	return sons;
}

std::unordered_set<Variable*>* Variable::getFathers(void) {
	return fathers;
}

std::unordered_set<Variable*>* Variable::getNeighbors(void) {
	return neighbors;
}

bool Variable::addEvidence(std::string stato) 
{
	std::size_t indiceStato = states->indexOf(stato); // gli stati devo implementarli con anche una map per la ricerca veloce (solo in caso di evidenza)
	//if ((itStato = std::find(states.begin(), states.end(), stato)) != states.end()) {
	if (indiceStato == SIZE_MAX) {
		//std::cout << "stato trovato: " << indiceStato;
		evidence = indiceStato;
		return true;
	}
	//std::cout << "stato NON trovato: :(";
	return false;
}

bool Variable::addEvidence(std::size_t indiceStato)
{
	//if (indiceStato >= 0 && 
	if (indiceStato < states->size()) {
		//std::cout << "indice stato trovato: " << indiceStato;
		evidence = indiceStato;
		return true;
	}
	//std::cout << "indice stato NON trovato: :(";
	return false;
}

std::size_t Variable::getEvidence()
{
	return evidence;
}

bool  Variable::isEvidence()
{
	return evidence != SIZE_MAX;
}

std::size_t Variable::nNeighbors(void) {
	return neighbors->size();
}

States* Variable::getStates(void) {
	return states;
}

std::size_t Variable::getNStates() {
	return states->size();
}

void Variable::addClique(JTClique* c) {
	cliques->insert(c);
}

std::unordered_set<JTClique*>* Variable::getCliques() {
	return cliques;
}
	
std::size_t Variable::getNumNeighboursNotSelected(void) {
	return numNeighboursNotSelected;
}
	
void Variable::decrementNumNeighboursNotSelected(void) {
	if (--numNeighboursNotSelected < 0) {
		//exit(1);
		std::cerr << "Variable.decrementParPerComparare: parametro va sotto 0 :(\n";
		std::string sss;
		std::cin >> sss;
	}
}
	
std::string Variable::getLabel(void) {
	return label;
}
	
std::string Variable::toString(void) {
	return label;
	// stampo senza stati???
}

//void Variable::normalizzaCP() {
//	for (int i = 0; i < probability->getTable()->size() / states->size(); i++) {
//		double sumStessaVar = 0;
//		for (int j = 0; j < states->size(); j++) {
//			sumStessaVar += probability->getTable()->at(i*states->size() + j);
//		}
//		if (sumStessaVar != 1) {
//			for (int j = 0; j < states->size(); j++) {
//				probability->getTable()->at(i*states->size() + j) /= sumStessaVar;
//			}
//		}
//	}
//}
