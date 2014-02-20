#pragma once

#ifndef BELIEFPROPAGATION_H
#define BELIEFPROPAGATION_H

#include "JTClique.h"
#include "Separator.h"
#include "JTClique.h"
#include "JunctionTree.h"
#include "BayesianNetwork.h"

class BeliefPropagation
{

public:
	BeliefPropagation(void);
	~BeliefPropagation(void);

	// posso applicare l'algoritmo di belief propagation partendo dal junction tree...
	static void BP(JunctionTree* jt);
	// ... oppure dalla rete bayesiana (che implica la creazione del junction tree)
	static JunctionTree* BP(BayesianNetwork* bn);

private:
	// andata
	static void collectEvidence(JTClique* root, double* elapsedSum, double* elapsedDivMul);
	static void collectEvidence(JTClique* node, JTClique* father, double* elapsedSum, double* elapsedDivMul);
	static void update(JTClique* node, JTClique* first, Separator* second, double* elapsedSum, double* elapsedDivMul);

	// ritorno
	static void distributeEvidence(JTClique* root, double* elapsedSum, double* elapsedDivMul);
	static void distributeEvidence(JTClique* node, JTClique* father, double* elapsedSum, double* elapsedDivMul);
	//static void update(JTClique* first, Separator* second, JTClique* node, double* elapsedSum, double* elapsedDivMul);

	// per stampa livello
	static std::string tabLivello();
};

#endif /* BELIEFPROPAGATION_H */
