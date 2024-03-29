#include <iostream>
#include <string> // serve per la stampa
#include "BeliefPropagation.h"
#include "States.h"
#include "Config.h"
//#include <time.h>
//#include <sys/timeb.h>
#include <chrono>

/**
 * Implementa i metodi necessari della Belief Propagation.
 *
 * @author Alessandro Gottoli vr352595
 */

static int livello;

BeliefPropagation::BeliefPropagation(void)
{
}


BeliefPropagation::~BeliefPropagation(void)
{
}

void BeliefPropagation::collectEvidence(JTClique* root, long long* elapsedSum, long long* elapsedDivMul)
{
	livello = 0;
	for (std::unordered_map<JTClique*, Separator*>::iterator entry = root->getLinks()->begin(); entry != root->getLinks()->end(); ++entry) {
#if PRINT_EXECUTION_BP
		// STAMPA ESECUZIONE inizio
		std::cout << "collectEvidence(" << entry->first->toString() << ", " << root->toString() << ")\n";
		// STAMPA ESECUZIONE fine
#endif
		collectEvidence(entry->first, root, elapsedSum, elapsedDivMul);

#if PRINT_EXECUTION_BP
		// STAMPA ESECUZIONE inizio
		std::cout << "update(" << root->toString() << ", " << entry->first->toString() << ")\n";
		// STAMPA ESECUZIONE fine
#endif
		update(root, entry->first, entry->second, elapsedSum, elapsedDivMul);
	}
}

void BeliefPropagation::collectEvidence(JTClique* node, JTClique* father, long long* elapsedSum, long long* elapsedDivMul)
{
#if PRINT_EXECUTION_BP
	livello++;
#endif

	for (std::unordered_map<JTClique*, Separator*>::iterator entry = node->getLinks()->begin(); entry != node->getLinks()->end(); ++entry) {
		if (entry->first != father) {
#if PRINT_EXECUTION_BP
			// STAMPA ESECUZIONE inizio
			std::cout << tabLivello() << "collectEvidence(" << entry->first->toString() << ", " << node->toString() << ")\n";
			// STAMPA ESECUZIONE fine
#endif
			collectEvidence(entry->first, node, elapsedSum, elapsedDivMul);

#if PRINT_EXECUTION_BP
			// STAMPA ESECUZIONE inizio
			std::cout << tabLivello() << "update(" << node->toString() << ", " << entry->first->toString() << ")\n";
			// STAMPA ESECUZIONE fine
#endif
			update(node, entry->first, entry->second, elapsedSum, elapsedDivMul);
		}
	}
#if PRINT_EXECUTION_BP
	livello--;
#endif
}

// sommo su first
// modifico node
void BeliefPropagation::update(JTClique* node, JTClique* first, Separator* second, long long* elapsedSum, long long* elapsedDivMul)
{
#if PRINT_EXECUTION_BP
	livello++;
#endif

	// STAMPA ESECUZIONE inizio
	//std::cout << tabLivello() << "fiS* = sumOn{varsC\\varsS}(psiCfiglio)\n";
	// STAMPA ESECUZIONE fine

	Probability* fiSeparatore = second->getFi();
	
	//Probability* fiSeparatoreStar;

#if !TEMPO_COMPLESSIVO && !CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;
#endif
	
#if !USE_INDEXING_TABLE && !USA_CUDA
//	if (!Config::useIndexingTable) {

#if !TEMPO_COMPLESSIVO && !CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
	begin = std::chrono::high_resolution_clock::now();
#endif
		Probability* fiSeparatoreStar = first->getPsi()->sumOnNotPresent(fiSeparatore);
		node->getPsi()->aggiornaOrdinato(fiSeparatoreStar, fiSeparatore);

		// aggiorno la fi
		second->setFi(fiSeparatoreStar);
		delete fiSeparatore;

#if !TEMPO_COMPLESSIVO && !CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
	end = std::chrono::high_resolution_clock::now();
	*elapsedSum += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
#endif

#if NORMALIZZA_AD_OGNI_PASSO
		/* PROVA SENZA NORMALIZZAZIONE */
		if (!node->getPsi()->isNormalized())
			node->getPsi()->normalizza();
		/* */
#endif
#else
//	} else {
#if CONTROLLA_UPDATE
		// DEBUG calcolo te tabelle anche col metodo normale e le confronto con il nuovo metodo
		/* */
		Probability* copiaFiSeparatoreStar = first->getPsi()->sumOnNotPresent(fiSeparatore);
		Probability* copiaPsiCricca = node->getPsi()->copy();
		copiaPsiCricca->aggiornaOrdinato(copiaFiSeparatoreStar, fiSeparatore);
		/* */
#if NORMALIZZA_AD_OGNI_PASSO
		if (!copiaPsiCricca->isNormalized())
			copiaPsiCricca->normalizza();
#endif
		//
#endif


#if !TEMPO_COMPLESSIVO && !CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
	begin = std::chrono::high_resolution_clock::now();
#endif

#if !USA_CUDA
		//if (!Config::useCUDA) { // TODO aggiungi dimensione minima delle tabelle!!!
			//fiSeparatoreStar = 
			second->updatePotentials(first, node, elapsedSum, elapsedDivMul);

			// aggiorno la fi
			//second->setFi(fiSeparatoreStar);
			//delete fiSeparatore;
#if NORMALIZZA_AD_OGNI_PASSO
			if (!node->getPsi()->isNormalized())
				node->getPsi()->normalizza();
#endif
#else
		//} else {
			//fiSeparatoreStar = 
				second->updatePotentialsCUDA(first, node, elapsedSum, elapsedDivMul);

			//if (!node->getPsi()->isNormalized())
			//	node->getPsi()->normalizza();
//		}
#endif

#if !TEMPO_COMPLESSIVO && !CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
	end = std::chrono::high_resolution_clock::now();
	*elapsedSum += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
#endif


#if CONTROLLA_UPDATE
		// DEBUG confronta fiStar e psiStar
		/* */
		//bool ok = copiaFiSeparatoreStar->confronta(fiSeparatoreStar);
		bool ok = copiaFiSeparatoreStar->confronta(second->getFi());
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
		}
#endif
		/* */
		//	
//	}
#endif

	
#if PRINT_EXECUTION_BP
	livello--;
#endif
}


void BeliefPropagation::distributeEvidence(JTClique* root, long long* elapsedSum, long long* elapsedDivMul)
{
	for (std::unordered_map<JTClique*, Separator*>::iterator entry = root->getLinks()->begin(); entry != root->getLinks()->end(); ++entry) {
#if PRINT_EXECUTION_BP
		// STAMPA ESECUZIONE inizio
		std::cout << "update(" << entry->first->toString() << ", " << root->toString() << ")\n";
		// STAMPA ESECUZIONE fine
#endif
		//update(entry->first, entry->second, root, elapsedSum, elapsedDivMul);
		update(entry->first, root, entry->second, elapsedSum, elapsedDivMul);

#if PRINT_EXECUTION_BP
		// STAMPA ESECUZIONE inizio
		std::cout << "distributeEvidence(" << entry->first->toString() << ", " << root->toString() << ")\n";
		// STAMPA ESECUZIONE fine
#endif
		distributeEvidence(entry->first, root, elapsedSum,  elapsedDivMul);
	}
}

void BeliefPropagation::distributeEvidence(JTClique* node, JTClique* father, long long* elapsedSum, long long* elapsedDivMul)
{
#if PRINT_EXECUTION_BP
	livello++;
#endif
	for (std::unordered_map<JTClique*, Separator*>::iterator entry = node->getLinks()->begin(); entry != node->getLinks()->end(); ++entry) {
		if (entry->first != father) {
#if PRINT_EXECUTION_BP
			// STAMPA ESECUZIONE inizio
			std::cout << tabLivello() << "update(" << entry->first->toString() << ", " << node->toString() << ")\n";
			// STAMPA ESECUZIONE fine
#endif
			//update(entry->first, entry->second, node, elapsedSum, elapsedDivMul);
			update(entry->first, node, entry->second, elapsedSum, elapsedDivMul);

#if PRINT_EXECUTION_BP
			// STAMPA ESECUZIONE inizio
			std::cout << tabLivello() << "distributeEvidence(" << entry->first->toString() << ", " << node->toString() << ")\n";
			// STAMPA ESECUZIONE fine
#endif
			distributeEvidence(entry->first, node, elapsedSum, elapsedDivMul);
		}
	}
#if PRINT_EXECUTION_BP
	livello--;
#endif
}

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

	long long* elapsedSum = new long long(0);
	long long* elapsedDivMul = new long long(0);

	auto begin = std::chrono::high_resolution_clock::now();

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

	// NORMALIZZO QUA ALLA FINE
#if !USA_CUDA && !NORMALIZZA_AD_OGNI_PASSO
//	if (!Config::useCUDA)
		jt->normalizeAllPotentials();
#endif
	//

	//m = difftime(time(NULL), now);
	auto end = std::chrono::high_resolution_clock::now();
	//double elapsedTime = 
	

	// STAMPA ESECUZIONE inizio

	std::cout << "valori delle tabelle aggiornate.\n";

std::cout << "\n-------------------------------\nModalit� scelta:\n";

#if USA_CUDA
	std::cout << "CUDA\n";
#if UTILIZZA_CPU_PER_TABELLE_PICCOLE
	std::cout << "utilizzando la cpu per tabelle fino a " << SIZE_MAX_CPU_MARGINALIZATION << " elementi per MARGINALIZZAZIONE e fino a " << SIZE_MAX_CPU_SCATTERING << " elementi per SCATTERING.\n";
#endif

#if TEMPO_COMPLESSIVO || CONSIDERA_TRASFERIMENTI_MEMORIA
	std::cout << "misurazione dei tempi COMPRESI i trasferimenti in memoria\n";
#else
	std::cout << "misurazione dei tempi SENZA i trasferimenti in memoria\n";
#endif

#else
	std::cout << "sequenziale\n";
#endif



#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
	std::cout << "Per eseguire la marginalizzazione: " << *elapsedSum / 1000000.0 << " ms." << std::endl;
	std::cout << "per eseguire lo scattering:        " << *elapsedDivMul / 1000000.0 << " ms." << std::endl;
#elif !TEMPO_COMPLESSIVO
	std::cout << "Tempi compresi i trasferimenti in memoria:\n";
	std::cout << "Per eseguire gli update delle tabelle: " << *elapsedSum / 1000000.0 << " ms." << std::endl;
#else
	std::cout << "Tempi compresi i trasferimenti in memoria:\n";
	std::cout << "Per eseguire tutta la Belief Propagation: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms." << std::endl;
#endif

	std::cout << "\n-------------------------------\n";
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
