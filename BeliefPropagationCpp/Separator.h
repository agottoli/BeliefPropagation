#pragma once

#ifndef SEPARATOR_H
#define SEPARATOR_H

#include "JTClique.h"
#include "VecMap.h"
#include "Probability.h"
//#include "ProbabilityMD.h"
#include "Config.h"

/**
 * Classe che implementa il separatore, cioè l'arco che collega due cricche nel junction tree con la propria probabilità.
 *
 * @author Alessandro Gottoli vr352595
 */
class Separator
{
	// le due cricche collegate dal link di questo separatore
	JTClique* soggetto;
	JTClique* oggetto;

	// le variabili in comune tra le due cricche (in ordine conforme la tabella fi del potenziale)
	VecMap* vars;

	// indexing (uso puntatori per accedere direttamente agli elementi della tabella)
	double** indexingSoggetto; // le dimensioni sono quelle della tabella del soggetto
	double** indexingOggetto; // le dimensioni sono quelle della tabella del oggetto
	// indexing per CUDA (uso gli indici degli elementi nella tabella) --> serve perché una volta sulla scheda non possiamo usare i puntatori in ram
	std::size_t* indexingSoggettoCUDA;
	std::size_t* indexingOggettoCUDA;

	// potenziale associato a questo separatore
	Probability* fi;

public:
	Separator(JTClique*, JTClique*, VecMap*);
	~Separator(void);

	// restituisce le cricche a cui si riferisce questo separatore
	JTClique* getSoggetto();
	JTClique* getOggetto();

	// restituisce il potenziale del separatore
	Probability* getFi();
	// setta il potenziale del separatore (in update della BP)
	void setFi(Probability*);

	// indico che questo separatore è stato scelto per far parte del junction tree
	// in pratica ad ognuna delle cricche aggiungo l'altra con il separatore (this) tra i suoi link
	void sceltoPerMST();

	// restituisce le variabili del separatore
	VecMap* getVars();

	// restituisce il numero di variabili in comune tra le due cricche (serve per pesare il separatore in kruskal)
	std::size_t size();

	// rappresentazione del separatore
	std::string toString();

	void updatePotentials(JTClique* cli, JTClique* cliScrivo, long long* elapsedSum, long long* elapsedDivMul);
	//Probability* sumOnIndexingTableOfCUDA(JTClique* cli, JTClique* cliScrivo, double* elapsedSum, double* elapsedDivMul);
	void updatePotentialsCUDA(JTClique* cli, JTClique* cliScrivo, long long* elapsedSum, long long* elapsedDivMul);
	void updatePotentialsCUDAonCPU(JTClique* cli, JTClique* cliScrivo, long long* elapsedSum, long long* elapsedDivMul);
	// metodi per gestire il caso misto in cui c'è possibilità che una tabella è fatta su cpu e l'altra su gpu...
	double* marginalizationCPU(size_t dimFiStarTable, // sarebbe sia la dimensione dell'output, sia il nostro caro nArray.
							  size_t numeroElemDaSommare, // sarebbe il nostro caro e vecchio m
							  double** indexingTableLeggo // la indexing table che punta agli elementi su cui fare la marginalizzazione
							  //,Separator* sep // quello che si becca la tabella appena calcolata... // QUI!!!
							  );
	double* marginalizationCPUindexCUDA(size_t dimFiStarTable, // sarebbe sia la dimensione dell'output, sia il nostro caro nArray.
							  size_t numeroElemDaSommare, // sarebbe il nostro caro e vecchio m
							  size_t* indexingTableLeggo // la indexing table che punta agli elementi su cui fare la marginalizzazione
							  , double* tableCliLeggo
							  , size_t numeroElemDaSommarePow2
							  , size_t dimFiStarTablePow2
							  //,Separator* sep // quello che si becca la tabella appena calcolata... // QUI!!!
							  );
	void scatteringCPU(size_t dimFiStarTable, // sarebbe la dimsione del separatore su cui iteriamo per leggere i valori da dividere
				size_t numeroElemDaAggiornareConUgualeValore,
				double** indexingTableScrivo,
				double* fiStarTable,
				double* fiTable,
				JTClique* cliScrivo,
				bool fiStarTableOnCPU
				);
	void scatteringCPUindexCUDA(size_t dimFiStarTable, // sarebbe la dimsione del separatore su cui iteriamo per leggere i valori da dividere
				size_t numeroElemDaAggiornareConUgualeValore,
				size_t* indexingTableScrivo,
				double* fiStarTable,
				double* fiTable,
				JTClique* cliScrivo,
				bool fiStarTableOnCPU
				, double* tableCliScrivo
				, size_t numeroElemDaAggiornareConUgualeValorePow2
				, size_t dimFiStarTablePow2
				);
	//

	void createIndexingTable() {
		// creo tabelle di indexing
		// DEBUG
		//std::cout << "costruisco la indexing table per soggetto!!! " << ++(Config::numIndexing) << std::endl;
		//
		indexingSoggetto = createIndexingTable(soggetto);
		// DEBUG
		//std::cout << "costruisco la indexing table per oggetto!!!  " << ++(Config::numIndexing) << std::endl;
		//
		indexingOggetto = createIndexingTable(oggetto);
	}

	void createIndexingTableCUDA() {
/*	
#if UTILIZZA_CPU_PER_TABELLE_PICCOLE
		if (soggetto->getPsi()->getTableSize() <= SIZE_MAX_CPU) {
			// DEBUG
			//std::cout << "Siccome la tabella ha dimensione piccola, decido di fare su cpu e quindi costruisco le indexing table per il metodo con cpu.\n"; // << ++(Config::numIndexing) << std::endl;
			//
			indexingSoggetto = createIndexingTable(soggetto);
		} else {
#endif
*/
			// creo tabelle di indexing
			// DEBUG
			//std::cout << "costruisco la indexing table CUDA per soggetto!!! " << ++(Config::numIndexing) << std::endl;
			//
			indexingSoggettoCUDA = createIndexingTableCUDA(soggetto);
/*			
#if UTILIZZA_CPU_PER_TABELLE_PICCOLE
		}
		
		if (oggetto->getPsi()->getTableSize() <= SIZE_MAX_CPU) {
			// DEBUG
			//std::cout << "Siccome la tabella ha dimensione piccola, decido di fare su cpu e quindi costruisco le indexing table per il metodo con cpu.\n"; // << ++(Config::numIndexing) << std::endl;
			//
			indexingOggetto = createIndexingTable(oggetto);
		} else {
#endif
*/
			// DEBUG
			//std::cout << "costruisco la indexing table CUDA per oggetto!!!  " << ++(Config::numIndexing) << std::endl;
			//
			indexingOggettoCUDA = createIndexingTableCUDA(oggetto);
/*			
#if UTILIZZA_CPU_PER_TABELLE_PICCOLE
		}
#endif
*/
	}

	void aggiornaPotenzialeOrdinato(JTClique* node, Probability* fiSeparatoreStar, Probability* fiSeparatore);

private:
	// per sequenziale
	double** createIndexingTable(JTClique*);
	// per CUDA
	std::size_t* createIndexingTableCUDA(JTClique*);

};

#endif /* SEPARATOR_H */
