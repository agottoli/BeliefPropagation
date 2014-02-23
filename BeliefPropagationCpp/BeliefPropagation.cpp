#include <iostream>
#include <string> // serve per la stampa
#include "BeliefPropagation.h"
#include "States.h"
#include "Config.h"
#include <time.h>

static int livello;

BeliefPropagation::BeliefPropagation(void)
{
}


BeliefPropagation::~BeliefPropagation(void)
{
}

void BeliefPropagation::collectEvidence(JTClique* root, double* elapsedSum, double* elapsedDivMul)
{
	livello = 0;
	for (std::unordered_map<JTClique*, Separator*>::iterator entry = root->getLinks()->begin(); entry != root->getLinks()->end(); ++entry) {
		// STAMPA ESECUZIONE inizio
		//std::cout << "collectEvidence(" << entry->first->toString() << ", " << root->toString() << ")\n";
		// STAMPA ESECUZIONE fine
		collectEvidence(entry->first, root, elapsedSum, elapsedDivMul);

		// STAMPA ESECUZIONE inizio
		//std::cout << "update(" << root->toString() << ", " << entry->first->toString() << ")\n";
		// STAMPA ESECUZIONE fine
		update(root, entry->first, entry->second, elapsedSum, elapsedDivMul);
	}
}

void BeliefPropagation::collectEvidence(JTClique* node, JTClique* father, double* elapsedSum, double* elapsedDivMul)
{
	//livello++;
	for (std::unordered_map<JTClique*, Separator*>::iterator entry = node->getLinks()->begin(); entry != node->getLinks()->end(); ++entry) {
		if (entry->first != father) {
			// STAMPA ESECUZIONE inizio
			//std::cout << tabLivello() << "collectEvidence(" << entry->first->toString() << ", " << node->toString() << ")\n";
			// STAMPA ESECUZIONE fine
			collectEvidence(entry->first, node, elapsedSum, elapsedDivMul);

			// STAMPA ESECUZIONE inizio
			//std::cout << tabLivello() << "update(" << node->toString() << ", " << entry->first->toString() << ")\n";
			// STAMPA ESECUZIONE fine
			update(node, entry->first, entry->second, elapsedSum, elapsedDivMul);
		}
	}
	//livello--;
}

// sommo su first
// modifico node
void BeliefPropagation::update(JTClique* node, JTClique* first, Separator* second, double* elapsedSum, double* elapsedDivMul)
{
	//livello++;

	// STAMPA ESECUZIONE inizio
	//std::cout << tabLivello() << "fiS* = sumOn{varsC\\varsS}(psiCfiglio)\n";
	// STAMPA ESECUZIONE fine

	Probability* fiSeparatore = second->getFi();
	
	Probability* fiSeparatoreStar;
	
	if (!Config::useIndexingTable) {
		fiSeparatoreStar = first->getPsi()->sumOnNotPresent(fiSeparatore);
		node->getPsi()->aggiornaOrdinato(fiSeparatoreStar, fiSeparatore);

		// aggiorno la fi
		second->setFi(fiSeparatoreStar);
		delete fiSeparatore;

		/* PROVA SENZA NORMALIZZAZIONE * /
		if (!node->getPsi()->isNormalized())
			node->getPsi()->normalizza();
		/ * */
	} else {
		// DEBUG calcolo te tabelle anche col metodo normale e le confronto con il nuovo metodo
		/*Probability* copiaFiSeparatoreStar = first->getPsi()->sumOnNotPresent(fiSeparatore);
		Probability* copiaPsiCricca = node->getPsi()->copy();
		copiaPsiCricca->aggiornaOrdinato(copiaFiSeparatoreStar, fiSeparatore);
		if (!copiaPsiCricca->isNormalized())
			copiaPsiCricca->normalizza();*/
		//
		if (!Config::useCUDA) {
			fiSeparatoreStar = second->sumOnIndexingTableOf(first, node, elapsedSum, elapsedDivMul);

			// aggiorno la fi
			second->setFi(fiSeparatoreStar);
			delete fiSeparatore;

			//if (!node->getPsi()->isNormalized())
			//	node->getPsi()->normalizza();
		} else {
			//fiSeparatoreStar = 
			second->updateCUDA(first, node, elapsedSum, elapsedDivMul);

			//if (!node->getPsi()->isNormalized())
			//	node->getPsi()->normalizza();
		}

		// DEBUG confronta fiStar e psiStar
		
		//bool ok = copiaFiSeparatoreStar->confronta(fiSeparatoreStar);
		/*bool ok = copiaFiSeparatoreStar->confronta(second->getFi());
		if (ok)
			std::cout << "separatore OK!!! :D" << std::endl;
		else {
			std::cout << "separatore ERROR!!! :(" << std::endl;
			std::string sss;
			std::cin >> sss;
		}
		std::cout << "vars copia: " << copiaPsiCricca->getVariables()->toString() << std::endl;
		std::cout << "vars origi: " << node->getPsi()->getVariables()->toString() << std::endl;
		ok = copiaPsiCricca->confronta(node->getPsi());
		if (ok)
			std::cout << "cricca OK!!! :D" << std::endl;
		else {
			std::cout << "cricca ERROR!!! :(" << std::endl;
			std::string sss;
			std::cin >> sss;
		}*/
		//	
	}

	

	//livello--;
}


void BeliefPropagation::distributeEvidence(JTClique* root, double* elapsedSum, double* elapsedDivMul)
{
	for (std::unordered_map<JTClique*, Separator*>::iterator entry = root->getLinks()->begin(); entry != root->getLinks()->end(); ++entry) {
		// STAMPA ESECUZIONE inizio
		//std::cout << "update(" << entry->first->toString() << ", " << root->toString() << ")\n";
		// STAMPA ESECUZIONE fine
		//update(entry->first, entry->second, root, elapsedSum, elapsedDivMul);
		update(entry->first, root, entry->second, elapsedSum, elapsedDivMul);

		// STAMPA ESECUZIONE inizio
		//std::cout << "distributeEvidence(" << entry->first->toString() << ", " << root->toString() << ")\n";
		// STAMPA ESECUZIONE fine
		distributeEvidence(entry->first, root, elapsedSum,  elapsedDivMul);
	}
}

void BeliefPropagation::distributeEvidence(JTClique* node, JTClique* father, double* elapsedSum, double* elapsedDivMul)
{
	//livello++;
	for (std::unordered_map<JTClique*, Separator*>::iterator entry = node->getLinks()->begin(); entry != node->getLinks()->end(); ++entry) {
		if (entry->first != father) {
			// STAMPA ESECUZIONE inizio
			//std::cout << tabLivello() << "update(" << entry->first->toString() << ", " << node->toString() << ")\n";
			// STAMPA ESECUZIONE fine
			//update(entry->first, entry->second, node, elapsedSum, elapsedDivMul);
			update(entry->first, node, entry->second, elapsedSum, elapsedDivMul);

			// STAMPA ESECUZIONE inizio
			//std::cout << tabLivello() << "distributeEvidence(" << entry->first->toString() << ", " << node->toString() << ")\n";
			// STAMPA ESECUZIONE fine
			distributeEvidence(entry->first, node, elapsedSum, elapsedDivMul);
		}
	}
	//livello--;
}

// sommo su node
// modifico first
// metodo INUTILE!!!
/*
void BeliefPropagation::update(JTClique* first, Separator* second, JTClique* node, double* elapsedSum, double* elapsedDivMul)
{
	//livello++;

	// STAMPA ESECUZIONE inizio
	//std::cout << tabLivello() << "fiS* = sumOn{varsC\\varsS}(psiCpadre)\n";
	// STAMPA ESECUZIONE fine

	Probability* fiSeparatore = second->getFi();
	
	Probability* fiSeparatoreStar;

	if (!Config::useIndexingTable) {
		fiSeparatoreStar = node->getPsi()->sumOnNotPresent(fiSeparatore);
		first->getPsi()->aggiornaOrdinato(fiSeparatoreStar, fiSeparatore);
	} else {
		// DEBUG calcolo te tabelle anche col metodo normale e le confronto con il nuovo metodo
		Probability* copiaFiSeparatoreStar = node->getPsi()->sumOnNotPresent(fiSeparatore);
		Probability* copiaPsiCricca = first->getPsi()->copy();
		copiaPsiCricca->aggiornaOrdinato(copiaFiSeparatoreStar, fiSeparatore);
		//
		if (!Config::useCUDA)
			fiSeparatoreStar = second->sumOnIndexingTableOf(node, first, elapsedSum, elapsedDivMul);
		else
			fiSeparatoreStar = second->sumOnIndexingTableOfCUDA(node, first, elapsedSum, elapsedDivMul);

		// DEBUG confronta fiStar e psiStar
		bool ok = copiaFiSeparatoreStar->confronta(fiSeparatoreStar);
		if (ok)
			std::cout << "OK!!! :D" << std::endl;
		else {
			std::cout << "ERROR!!! :(" << std::endl;
			std::string sss;
			std::cin >> sss;
		}
		ok = copiaPsiCricca->confronta(first->getPsi());
		if (ok)
			std::cout << "OK!!! :D" << std::endl;
		else {
			std::cout << "ERROR!!! :(" << std::endl;
			std::string sss;
			std::cin >> sss;
		}
		//

	}

	

	// aggiorno la fi
	second->setFi(fiSeparatoreStar);
	delete fiSeparatore;

	if (!first->getPsi()->isNormalized())
		first->getPsi()->normalizza();

	//livello--;
}
*/

JunctionTree* BeliefPropagation::BP(BayesianNetwork* bn)
{
	bn->moralize();
	std::unordered_set<JTClique*>* cliques = bn->triangolateMaxClique();
	JunctionTree* jt = bn->createJunctionTreeMST(cliques);
	BP(jt);
	return jt;
}

void BeliefPropagation::BP(JunctionTree* jt)
{
	//std::cout << "inizia la belief propagation..." << std::endl;
	//std::string sss;
	//std::cin >> sss;

	//time_t m;
	//time_t now = time(NULL);
	double* elapsedSum = new double(0.0);
	double* elapsedDivMul = new double(0.0);
	clock_t begin = clock();
	
	/*
	jt->calcolaRootMigliore();
	
	JTClique* root = jt->getRoot();
	*/

	//std::cout << "le roots sono: " << jt->getRoots()->size() << std::endl;

	for (JTClique* root : *jt->getRoots()) {

		//std::cout << root->toString() << std::endl;

		// STAMPA ESECUZIONE inizio
		//std::cout << "root..." << root->toString();
		//std::cout << "FASE di ANDATA\n";
		// STAMPA ESECUZIONE fine
		collectEvidence(root, elapsedSum, elapsedDivMul);

		// STAMPA ESECUZIONE inizio
		//std::cout << "FASE di RITORNO\n";
		// STAMPA ESECUZIONE fine
		distributeEvidence(root, elapsedSum, elapsedDivMul);

	}

	//m = difftime(time(NULL), now);
	clock_t end = clock();
	double elapsedTime = (double)(end - begin) / CLOCKS_PER_SEC;

	// STAMPA ESECUZIONE inizio

	double elapsedSumPrint = *elapsedSum / CLOCKS_PER_SEC;
	std::cout << "valori delle tabelle aggiornate.\n";
	std::cout << "BeliefPropagation eseguito in: " << elapsedTime << ".\n";
	std::cout << "di cui " << elapsedSumPrint << " per eseguire le somme." << std::endl;
	std::cout << "     e " << *elapsedDivMul << " per eseguire le div-mul." << std::endl;
	// STAMPA ESECUZIONE fine
	//std::cin >> sss;
}

std::string BeliefPropagation::tabLivello()
{
	std::string s;
	for (int i = 0; i < livello; i++) {
		s.append("\t");
	}
	return s;
}
