#include "Separator.h"
#include <iostream>
#include <string>
#include <fstream>
// tempistiche somme
//#include <time.h>
#include <chrono>

// per cuda
//#include "../BeliefPropagationCUDA/sumKernelSmallN.h"
#include "../BeliefPropagationCUDA/margAndScatt.h"
//

/**
 * Implementa i metodi del separatore.
 *
 * @author Alessandro Gottoli vr352595
 */
Separator::Separator(JTClique* s, JTClique* o, VecMap* v)
{
	soggetto = s;
	oggetto = o;
	vars = v;
}


Separator::~Separator(void)
{
	//soggetto->~JTClique();
	//oggetto->~JTClique();
	//vars->~VecMap();
	//fi->~Probability();
}

JTClique* Separator::getSoggetto()
{
	return soggetto;
}

JTClique* Separator::getOggetto()
{
	return oggetto;
}

Probability* Separator::getFi()
{
	return fi;
}

void Separator::setFi(Probability* fiNuovo)
{
	fi = fiNuovo;
}

void Separator::sceltoPerMST()
{
	soggetto->aggiungiLinkSeparatore(oggetto, this);
	oggetto->aggiungiLinkSeparatore(soggetto, this);
}

VecMap* Separator::getVars()
{
	return vars;
}

std::size_t Separator::size()
{
	return vars->size();
}

std::string Separator::toString()
{
	// TODO temporaneo
	return soggetto->toString().append("---").append(oggetto->toString()).append(": ").append(vars->toString());
}

double** Separator::createIndexingTable(JTClique* cliqueToIndex)
{
	Probability* cliquePsi = cliqueToIndex->getPsi();

	std::size_t dimCliqueTable = cliquePsi->getTableSize();
	double** indexT = new double*[dimCliqueTable];

	std::size_t* offset = new std::size_t[fi->getTableSize()];
	// dicono che un array ha già tutti elementi a zero... MA NON E' VERO!!!
	for (std::size_t i = 0; i < fi->getTableSize(); i++) {
		offset[i] = 0;
	}


	// controllo che tutte le variabili della tabella other siano presenti in this
	// e così mi salvo dove si trovano
	VecMap* cliqueVars = cliquePsi->getVariables();

	std::vector<std::size_t> configClique (cliqueVars->size());
		
	//std::vector<std::size_t>* indexOtherInThis = new std::vector<std::size_t>();
	std::vector<std::size_t*>* configSep = new std::vector<std::size_t*>();
	configSep->reserve(vars->size());
	for(std::vector<Variable*>::iterator el = vars->begin(); el !=  vars->end(); el++) {
		std::size_t index = cliqueVars->indexOf(*el);

		// tolgo il controllo che non può mai capitare!!!
		//if (index == SIZE_MAX) {
		//	// non c'è la variabile allora ferma tutto che c'è un errore!!!
		//	std::cout << "Probability::mul: La variabile " << *elOther << " non è presente!!" << std::endl;
		//	std::string stop;
		//	std::cin >> stop;
		//}

		//indexOtherInThis->push_back(index);
		configSep->push_back(&configClique.at(index));
	}

	std::size_t dimMapToSame = dimCliqueTable / fi->getTableSize();

	// STATISTICHE!!!!
	// DEBUG
	//std::string file = "meno1024.txt";
	//if (fi->getTableSize() > 1024) {
	//	file = "piu1024.txt";
	//}
	/*
	std::string file = "m.txt";
	std::ofstream f(file, std::ios::app); //apre il file in modalità append, lasciando intatto quello che c'è e scrivendo alla fine
    if(!f) {
        std::cout << "Errore nell'apertura del file!";
        std::string s;
		std::cin >> s;
    }

    f << dimMapToSame << std::endl;

    f.close(); //chiudo il file

	std::string fileN = "N.txt";
	std::ofstream fN(fileN, std::ios::app); //apre il file in modalità append, lasciando intatto quello che c'è e scrivendo alla fine
    if(!fN) {
        std::cout << "Errore nell'apertura del file!";
        std::string s;
		std::cin >> s;
    }

    fN << fi->getTableSize() << std::endl;

    fN.close(); //chiudo il file
	*/
	/////////////////////////////////////

	//std::cout << "numero elementi sommo m: " << dimMapToSame << " e gli arrai sono N: " << fi->getTableSize() << std::endl;
	// END DEBUG
	//std::string s;
	//std::cin >> s;


	std::size_t fiIndex;
	for (std::size_t i = 0; i < dimCliqueTable; i++) {
		fiIndex = fi->getIndexOfConfig(configSep);
		indexT[fiIndex * dimMapToSame + (offset[fiIndex])++] = cliquePsi->getAtConfig(&configClique);

		// aggiorno alla prossima configurazione
		cliquePsi->nextConfig(&configClique);
	}


	delete offset;
	delete configSep;

	return indexT;
}

inline std::size_t
pow2roundup (std::size_t x)
{
    //if (x < 0)
    //    return 0; // non capiterà mai perché le tabelle hanno tutte dimensione positiva!!!

    --x; // da problemi con unsigned se x = 0, ma non capiterà mai
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}

std::size_t* Separator::createIndexingTableCUDA(JTClique* cliqueToIndex)
{
	Probability* cliquePsi = cliqueToIndex->getPsi();

	std::size_t dimCliqueTable = cliquePsi->getTableSize();

	std::size_t fiTableSize = fi->getTableSize();

	std::size_t dimMapToSame = dimCliqueTable / fiTableSize;

	// provo a costruire la tabella già pronta con m e N potenze di 2
	std::size_t dimMapToSamePow2 = pow2roundup(dimMapToSame);
	std::size_t nArrayPow2 = pow2roundup(fiTableSize);
	std::size_t psiTableSizePow2 = dimMapToSamePow2 * nArrayPow2;
	/*
	std::size_t* offset = new std::size_t[fiTableSize];
	// dicono che un array ha già tutti elementi a zero... MA NON E' VERO!!!
	for (std::size_t i = 0; i < fiTableSize; i++) { 
		offset[i] = 0;
	}
	*/
	///*
	// diventa...
	std::size_t* offset = new std::size_t[nArrayPow2];
	// dicono che un array ha già tutti elementi a zero... MA NON E' VERO!!!
	for (std::size_t i = 0; i < nArrayPow2; i++) { 
		offset[i] = 0;
	}
	//*/
	//

	// controllo che tutte le variabili della tabella other siano presenti in this
	// e così mi salvo dove si trovano
	VecMap* cliqueVars = cliquePsi->getVariables();

	std::vector<std::size_t> configClique (cliqueVars->size());
		
	//std::vector<std::size_t>* indexOtherInThis = new std::vector<std::size_t>();
	std::vector<std::size_t*>* configSep = new std::vector<std::size_t*>();
	configSep->reserve(vars->size());
	for(std::vector<Variable*>::iterator el = vars->begin(); el !=  vars->end(); el++) {
		std::size_t index = cliqueVars->indexOf(*el);

		// tolgo il controllo che non può mai capitare!!!
		//if (index == SIZE_MAX) {
		//	// non c'è la variabile allora ferma tutto che c'è un errore!!!
		//	std::cout << "Probability::mul: La variabile " << *elOther << " non è presente!!" << std::endl;
		//	std::string stop;
		//	std::cin >> stop;
		//}

		//indexOtherInThis->push_back(index);
		configSep->push_back(&configClique.at(index));
	}


	//std::cout << "numero elementi sommo: " << dimMapToSame << " e gli arrai sono: " << fi->getTableSize() << std::endl;
	//std::string s;
	//std::cin >> s;

	
	
	// provo a costruire la tabella già pronta con m e N potenze di 2
	std::size_t* indexT = new size_t[psiTableSizePow2];
	for (std::size_t i = 0; i < psiTableSizePow2; i++) {
		indexT[i] = SIZE_MAX;
	}
	/*
	std::size_t* indexT = new size_t[dimCliqueTable];
	*/

	std::size_t fiIndex;
	for (std::size_t i = 0; i < dimCliqueTable; i++) {
		// NOTA: configClique si riferisce esattamente alla configurazione dell'indice i nella tabella della cricca
		fiIndex = fi->getIndexOfConfig(configSep);

#if INDEXINGSUM_ON_ROW		
//		if (Config::indexingSumOnRow) {
			// QUA C'E' LA DIFFERENZA DI INSERIRE I DATI IN [0000][1111][2222]
			// provo a costruire la tabella già pronta con m e N potenze di 2
			/*
			indexT[fiIndex * dimMapToSame + (offset[fiIndex])++] = i; //cliquePsi->getAtConfig(&configClique);
			*/
			///*
			// diventa...
			indexT[fiIndex * dimMapToSamePow2 + (offset[fiIndex])++] = i; //cliquePsi->getAtConfig(&configClique);
			//*/
			//
#else
//		} else {
			// OPPURE IN [012][012][012][012]
			// provo a costruire la tabella già pronta con m e N potenze di 2
			/*
			indexT[fiIndex + fiTableSize * (offset[fiIndex])++] = i;
			*/
			///*
			// diventa...
			indexT[fiIndex + nArrayPow2 * (offset[fiIndex])++] = i;
			//*/
			//
//		}
#endif

		// aggiorno alla prossima configurazione
		cliquePsi->nextConfig(&configClique);
	}


	delete offset;
	delete configSep;

	// DEBUG
	//for (std::size_t i = 0; i < dimCliqueTable; i++) {
	//	std::cout << indexT[i] << " ";
	//}
	//std::cout << std::endl;
	//

	return indexT;
}

void Separator::updatePotentials(JTClique* cli, JTClique* cliScrivo, long long* elapsedSum, long long* elapsedDivMul)
{

#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
	// rilevamento tempo occupato dalle somme
	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;
#endif

#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
		// rilevamento tempo occupato dalle somme!!!
		begin = std::chrono::high_resolution_clock::now();
		//
#endif

	double** indexingTableLeggo;
	double** indexingTableScrivo;
	if (cli == soggetto) {
		indexingTableLeggo = indexingSoggetto;
		indexingTableScrivo = indexingOggetto;
	} else {
		// sicuramente è dell'oggetto, non ammetto che uno sbagli a passarmi la cli!!!
		indexingTableLeggo = indexingOggetto;
		indexingTableScrivo = indexingSoggetto;
	}

	std::size_t dimFiStarTable = fi->getTableSize();

	double* fiStarTable = new double[dimFiStarTable];
	//for (std::size_t i = 0; i < dimFiStarTable; i++) {
	//	fiStarTable[i] = 0.0;
	//}

	//std::cout << "sto eseguendo il metodo con la index" << std::endl;

	std::size_t numeroElemDaSommare = cli->getPsi()->getTableSize() / dimFiStarTable;
	std::size_t numeroElemDaAggiornareConUgualeValore = cliScrivo->getPsi()->getTableSize() / dimFiStarTable;

	double sommaTabella = 0.0;
	double sommaTabellaCri = 0.0;

#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
		// rilevamento tempo occupato dalle somme!!!
		end = std::chrono::high_resolution_clock::now();
		*elapsedSum += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		//
#endif

	for (std::size_t i = 0; i < dimFiStarTable; i++) {
		//std::cout << " i = " << i;
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
		// rilevamento tempo occupato dalle somme!!!
		begin = std::chrono::high_resolution_clock::now();
		//
#endif

		fiStarTable[i] = 0.0;

		// calcolo il valore di fiStar[i]
		for (std::size_t j = 0; j < numeroElemDaSommare; j++) {
			//std::cout << " j = " << j << std::endl;
			//std::cout << fiStarTable[i];
			fiStarTable[i] += *indexingTableLeggo[i * numeroElemDaSommare + j];
			//std::cout << " --> " << fiStarTable[i] << std::endl;
			
		}

		sommaTabella += fiStarTable[i];

#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
		// rilevamento tempo occupato dalle somme!!!
		end = std::chrono::high_resolution_clock::now();
		*elapsedSum += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		//
//#endif	

//#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		begin = std::chrono::high_resolution_clock::now();
		//
#endif

		double divisioneTraFi;

		// calcolo solo se la divisione 
		if (fi->getTable()[i] > ZERO_DIVISIONE) {
			// calcolo quanto vale fiStar[i]/fi[i]
			divisioneTraFi = fiStarTable[i] / fi->getTable()[i];
		} else
			divisioneTraFi = 0;

		//std::cout << "fiStar[" << i << "]/fi[" << i << "] = " << divisioneTraFi << std::endl;

		// se devo moltiplicare per 1 allora mi risparmio il giro
		if (divisioneTraFi != 1) {

			// provo a sbagliare per vedere se il confronto lo rileva
			//divisioneTraFi = 0.5;

			for (std::size_t j = 0; j < numeroElemDaAggiornareConUgualeValore; j++) {
				//std::cout << "iT[" << i * numeroElemDaAggiornareConUgualeValore + j << "] = " << *indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j] << " --> ";
				*indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j] *= divisioneTraFi;
				//std::cout << *indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j];
				//std::string sss;
				//std::cin >> sss;

				// SUMT
				sommaTabellaCri += *indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j];
			}
		} else {
			for (std::size_t j = 0; j < numeroElemDaAggiornareConUgualeValore; j++) {
				// SUMT
				sommaTabellaCri += *indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j];
			}
		}

#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		end = std::chrono::high_resolution_clock::now();
		*elapsedDivMul += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		//
#endif

	}


#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		begin = std::chrono::high_resolution_clock::now();
		//
#endif

	//std::string sss;
	//std::cin >> sss;
	delete fi->getTable(); // free(fi->getTable());
	fi->setTable(fiStarTable);
	fi->setSommaTabella(sommaTabella);

	//SUMT
	cliScrivo->getPsi()->setSommaTabella(sommaTabellaCri);

#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		end = std::chrono::high_resolution_clock::now();
		*elapsedDivMul += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		//
#endif

	//return new Probability(vars, fiStarTable, dimFiStarTable, sommaTabella);

}

// sostituito sotto
//Probability* Separator::sumOnIndexingTableOfCUDA(JTClique* cli, JTClique* cliScrivo, double* elapsedSum, double* elapsedDivMul)
//{
//	std::size_t* indexingTableLeggo;
//	std::size_t* indexingTableScrivo;
//	
//	if (cli == soggetto) {
//		indexingTableLeggo = indexingSoggettoCUDA;
//		indexingTableScrivo = indexingOggettoCUDA;
//	} else {
//		// sicuramente è dell'oggetto, non ammetto che uno sbagli a passarmi la cli!!!
//		indexingTableLeggo = indexingOggettoCUDA;
//		indexingTableScrivo = indexingSoggettoCUDA;
//	}
//
//	std::size_t dimFiStarTable = fi->getTableSize();
//
//	//double* fiStarTable = new double[dimFiStarTable];
//
//	// rilevamento tempo occupato dalle somme
//	clock_t begin;
//	clock_t end;
//
//	std::size_t sizeTableLeggo = cli->getPsi()->getTableSize();
//	std::size_t numeroElemDaSommare = sizeTableLeggo / dimFiStarTable;
//
//	// da
//	//double* fiStarTable = runSmallN(size, int nArray, double *h_idata, int *h_iIndexData);
//	// a
//	/*
//	std::cout << "sizeTableLeggo: " << sizeTableLeggo << std::endl;
//	std::cout << "dimFiStarTable: " << dimFiStarTable << std::endl;
//	std::cout << "table vera dim: " << cli->getPsi()->getTableSize() << std::endl;
//	for (std::size_t i = 0; i < cli->getPsi()->getTableSize(); i++) {
//		std::cout << indexingTableLeggo[i] << " ";
//	}
//	*/
//	
//	// provo a costruire la tabella già pronta con m e N potenze di 2
//	std::size_t numeroElemDaSommarePow2 = pow2roundup(numeroElemDaSommare);
//	std::size_t dimFiStarTablePow2 = pow2roundup(dimFiStarTable); // nArrayPow2 
//	std::size_t sizeTableLeggoPow2 = numeroElemDaSommarePow2 * dimFiStarTablePow2;
//	/*
//	double* fiStarTable = runSmallN(sizeTableLeggo, dimFiStarTable, cli->getPsi()->getTable(), (long unsigned int *) indexingTableLeggo); 
//	*/
//	///*
//	// diventa...
//	double* fiStarTable = //new double[dimFiStarTable];
//		runBigN(sizeTableLeggoPow2, dimFiStarTablePow2, cli->getPsi()->getTable(), indexingTableLeggo, sizeTableLeggo, dimFiStarTable);
//	// mi restituirà un array più lungo, ma modificando la dimensione del risultato in cuda, mi da solo il numero di elementi validi. 
//	//*/
//	//
//	
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	/*int size = sizeTableLeggo; //32;    // number of elements to reduce -> default:  16777216						// ALE
//	int nArray = dimFiStarTable; //2;	// m = 16384 
//    unsigned int bytesValuesInput = size * sizeof(double);										// ALE
//    double *h_idata = (double *) malloc(bytesValuesInput);										// ALE
//	unsigned int bytesIndexInput = size * sizeof(long unsigned int);										// ALE
//	long unsigned int *h_iIndexData = (long unsigned int *) malloc(bytesIndexInput);										// ALE
//		
//	for (int i=0; i<size; i++) { h_idata[i] = 1; h_iIndexData[i]=i; }
//	
//	double* fiStarTable = runSmallN(size, nArray, h_idata, h_iIndexData);
//	*/
//	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	
//	
//	//
//	double* tableCliScrivo = cliScrivo->getPsi()->getTable();
//	std::size_t sizeTableScrivo = cliScrivo->getPsi()->getTableSize();
//	
//	std::size_t numeroElemDaAggiornareConUgualeValore = cliScrivo->getPsi()->getTableSize() / dimFiStarTable;
//
//	// provo a costruire la tabella già pronta con m e N potenze di 2
//	std::size_t numeroElemDaAggiornareConUgualeValorePow2 = pow2roundup(numeroElemDaAggiornareConUgualeValore);
//	std::size_t sizeTableScrivoPow2 = numeroElemDaAggiornareConUgualeValorePow2 * dimFiStarTablePow2;
//	//
//
//	//for (std::size_t i = 0; i < sizeTableScrivo; i++) {
//	//	std::cout << "indexingTableScrivo[i + dimFiStarTablePow2 * j] = " << indexingTableScrivo[i] << std::endl;
//	//}
//	
//	for (std::size_t i = 0; i < dimFiStarTable; i++) {
//
//		double divisioneTraFi;
//
//		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
//		begin = clock();
//		//
//
//		// calcolo solo se la divisione 
//		if (fi->getTable()[i] > zero) {
//			// calcolo quanto vale fiStar[i]/fi[i]
//			divisioneTraFi = fiStarTable[i] / fi->getTable()[i];
//		} else
//			divisioneTraFi = 0;
//
//		//std::cout << "fiStar[" << i << "]/fi[" << i << "] = " << divisioneTraFi << std::endl;
//
//		// se devo moltiplicare per 1 allora mi risparmio il giro
//		if (divisioneTraFi != 1) {
//
//			// provo a sbagliare per vedere se il confronto lo rileva
//			//divisioneTraFi = 0.5;
//
//			for (std::size_t j = 0; j < numeroElemDaAggiornareConUgualeValore; j++) {
//				//std::cout << "iT[" << i * numeroElemDaAggiornareConUgualeValore + j << "] = " << *indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j] << " --> ";
//				
//				if (Config::indexingSumOnRow) {
//					// [0][1][2] * [0000][1111][2222]
//					/*
//					tableCliScrivo[indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j]] *= divisioneTraFi;
//					*/
//					///*
//					// diventa...
//					tableCliScrivo[indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValorePow2 + j]] *= divisioneTraFi;
//					//*/
//					//
//				} else {
//					// [0][1][2] * [012][012][012][012]
//					/*
//					tableCliScrivo[indexingTableScrivo[i + dimFiStarTable * j]] *= divisioneTraFi;
//					*/
//					///*
//					// diventa...
//					tableCliScrivo[indexingTableScrivo[i + dimFiStarTablePow2 * j]] *= divisioneTraFi;
//					//*/
//					//
//				}
//				//std::cout << *indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j];
//				//std::string sss;
//				//std::cin >> sss;
//			}
//		}
//
//		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
//		end = clock();
//		*elapsedDivMul += (double)(end - begin) / CLOCKS_PER_SEC;
//		//
//
//	}
//
//	//std::string sss;
//	//std::cin >> sss;
//	
//	//return new Probability(vars, fiStarTable, dimFiStarTable, sommaTabella);
//	return new Probability(vars, fiStarTable, dimFiStarTable, cli->getPsi()->getSommaTabella());
//
//}

void Separator::updatePotentialsCUDA(JTClique* cli, JTClique* cliScrivo, long long* elapsedSum, long long* elapsedDivMul)
{
	std::size_t* indexingTableLeggoCUDA;
	std::size_t* indexingTableScrivoCUDA;

//#if UTILIZZA_CPU_PER_TABELLE_PICCOLE
//	double** indexingTableLeggo;
//	double** indexingTableScrivo;
//#endif
	
	if (cli == soggetto) {
		indexingTableLeggoCUDA = indexingSoggettoCUDA;
		indexingTableScrivoCUDA = indexingOggettoCUDA;
		
//#if UTILIZZA_CPU_PER_TABELLE_PICCOLE
//		indexingTableLeggo = indexingSoggetto;
//		indexingTableScrivo = indexingOggetto;
//#endif
		
	} else {
		// sicuramente è dell'oggetto, non ammetto che uno sbagli a passarmi la cli!!!
		indexingTableLeggoCUDA = indexingOggettoCUDA;
		indexingTableScrivoCUDA = indexingSoggettoCUDA;
		
//#if UTILIZZA_CPU_PER_TABELLE_PICCOLE
//		indexingTableLeggo = indexingOggetto;
//		indexingTableScrivo = indexingSoggetto;
//#endif
		
	}

	std::size_t dimFiStarTable = fi->getTableSize();

	//double* fiStarTable = new double[dimFiStarTable];

#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE //&& CONSIDERA_TRASFERIMENTI_MEMORIA
	// rilevamento tempo occupato dalle somme
	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;
#endif

	std::size_t sizeTableLeggo = cli->getPsi()->getTableSize();
	std::size_t numeroElemDaSommare = sizeTableLeggo / dimFiStarTable;

	// da
	//double* fiStarTable = runSmallN(size, int nArray, double *h_idata, int *h_iIndexData);
	// a
	/*
	std::cout << "sizeTableLeggo: " << sizeTableLeggo << std::endl;
	std::cout << "dimFiStarTable: " << dimFiStarTable << std::endl;
	std::cout << "table vera dim: " << cli->getPsi()->getTableSize() << std::endl;
	for (std::size_t i = 0; i < cli->getPsi()->getTableSize(); i++) {
		std::cout << indexingTableLeggo[i] << " ";
	}
	*/
	
	// provo a costruire la tabella già pronta con m e N potenze di 2
	std::size_t numeroElemDaSommarePow2 = pow2roundup(numeroElemDaSommare);
	std::size_t dimFiStarTablePow2 = pow2roundup(dimFiStarTable); // nArrayPow2 
	std::size_t sizeTableLeggoPow2 = numeroElemDaSommarePow2 * dimFiStarTablePow2;
	/*
	double* fiStarTable = runSmallN(sizeTableLeggo, dimFiStarTable, cli->getPsi()->getTable(), (long unsigned int *) indexingTableLeggo); 
	*/
	///*
	// diventa...
	//double* fiStarTable = //new double[dimFiStarTable];
	//	runBigN(sizeTableLeggoPow2, dimFiStarTablePow2, cli->getPsi()->getTable(), indexingTableLeggo, sizeTableLeggo, dimFiStarTable,);
	// mi restituirà un array più lungo, ma modificando la dimensione del risultato in cuda, mi da solo il numero di elementi validi. 
	//*/
	//
	
	//
	double* tableCliScrivo = cliScrivo->getPsi()->getTable();
	std::size_t sizeTableScrivo = cliScrivo->getPsi()->getTableSize();
	
	std::size_t numeroElemDaAggiornareConUgualeValore = cliScrivo->getPsi()->getTableSize() / dimFiStarTable;

	// provo a costruire la tabella già pronta con m e N potenze di 2
	std::size_t numeroElemDaAggiornareConUgualeValorePow2 = pow2roundup(numeroElemDaAggiornareConUgualeValore);
	std::size_t sizeTableScrivoPow2 = numeroElemDaAggiornareConUgualeValorePow2 * dimFiStarTablePow2;
	//

	
		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		//begin = clock();
		//

	// MARGINALIZATION
	double* fiStarTable; // se uso l'approccio ibrido
	double* fiTable = fi->getTable();

	bool is_fiStarTableOnHost;

#if UTILIZZA_CPU_PER_TABELLE_PICCOLE
	//double* fiStarOnCPU;
	if (sizeTableLeggo <= SIZE_MAX_CPU_MARGINALIZATION || numeroElemDaSommarePow2 <= 4) {
		is_fiStarTableOnHost = true;
		
		// DEBUG
		//std::cout << "marginalization su CPU." << std::endl;
		//

		#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE // && CONSIDERA_TRASFERIMENTI_MEMORIA
		begin = std::chrono::high_resolution_clock::now();
		#endif
		
		fiStarTable = marginalizationCPUindexCUDA(dimFiStarTable, numeroElemDaSommare, indexingTableLeggoCUDA, cli->getPsi()->getTable(), numeroElemDaSommarePow2, dimFiStarTablePow2); //, this);

		#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE // && CONSIDERA_TRASFERIMENTI_MEMORIA
		end = std::chrono::high_resolution_clock::now();
		*elapsedSum += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		#endif

		// la somma della tabella di fi viene settata da marginalization
		fi->setTable(fiStarTable); // se ce l'ho già la tabella la setto, in caso andassi su gpu che non salva più

	} else {
#endif

	is_fiStarTableOnHost = false;
	
	if (dimFiStarTablePow2 > SIZE_MAX_SMALL_N) {
		// BIG N
		// DEBUG
		//std::cout << "marginalization su CUDA (BIG N)." << std::endl;
		//
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && CONSIDERA_TRASFERIMENTI_MEMORIA
		begin = std::chrono::high_resolution_clock::now();
#endif
		fiStarTable = marginalizationBigN(sizeTableLeggoPow2, dimFiStarTablePow2, cli->getPsi()->getTable(), indexingTableLeggoCUDA, sizeTableLeggo, dimFiStarTable, elapsedSum, elapsedDivMul);
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && CONSIDERA_TRASFERIMENTI_MEMORIA
		end = std::chrono::high_resolution_clock::now();
		*elapsedSum += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
#endif
	} else {
		// SMALL N
		// DEBUG
		//std::cout << "marginalization su CUDA (Small N)." << std::endl;
		//
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && CONSIDERA_TRASFERIMENTI_MEMORIA
		begin = std::chrono::high_resolution_clock::now();
#endif
		fiStarTable = marginalizationSmallN(sizeTableLeggoPow2, dimFiStarTablePow2, cli->getPsi()->getTable(), indexingTableLeggoCUDA, sizeTableLeggo, dimFiStarTable, elapsedSum, elapsedDivMul);
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && CONSIDERA_TRASFERIMENTI_MEMORIA
		end = std::chrono::high_resolution_clock::now();
		*elapsedSum += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
#endif
	}

#if UTILIZZA_CPU_PER_TABELLE_PICCOLE
	} // fine della marginalizzazione
#endif


	// SCATTERING

#if UTILIZZA_CPU_PER_TABELLE_PICCOLE
	if (sizeTableScrivo <= SIZE_MAX_CPU_SCATTERING) {
		// DEBUG
		//std::cout << "scattering su CPU." << std::endl;
		//
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE // && CONSIDERA_TRASFERIMENTI_MEMORIA
	begin = std::chrono::high_resolution_clock::now();
#endif
	
	scatteringCPUindexCUDA(dimFiStarTable, numeroElemDaAggiornareConUgualeValore, indexingTableScrivoCUDA, fiStarTable, 
		fiTable, cliScrivo, is_fiStarTableOnHost, tableCliScrivo, numeroElemDaAggiornareConUgualeValorePow2, dimFiStarTablePow2); // QUIIIIII!!!!


#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && CONSIDERA_TRASFERIMENTI_MEMORIA
	end = std::chrono::high_resolution_clock::now();
	*elapsedDivMul += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
#endif

	// cancello dopo lo scattering che non mi serve più
	delete fiTable;
	// per me è giusto qui il set della tabella
	//fi->setTable(fiStarTable); // lo faccio da dentro il scatteringCPU
	// la somma della tabella di fi viene settata da marginalization

#if NORMALIZZA_AD_OGNI_PASSO
	// normalizzo le cricche
	if (!cliScrivo->getPsi()->isNormalized())
		cliScrivo->getPsi()->normalizza();
#endif

	} else {
#endif
		// DEBUG
		//std::cout << "scattering su CUDA." << std::endl;
		//
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && CONSIDERA_TRASFERIMENTI_MEMORIA
	begin = std::chrono::high_resolution_clock::now();
#endif
	
	scattering(sizeTableScrivoPow2, dimFiStarTablePow2, fiStarTable, fiTable, tableCliScrivo, indexingTableScrivoCUDA, sizeTableScrivo, dimFiStarTable, elapsedSum, elapsedDivMul, is_fiStarTableOnHost);
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && CONSIDERA_TRASFERIMENTI_MEMORIA
	end = std::chrono::high_resolution_clock::now();
	*elapsedDivMul += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
#endif

	// setto i nuovi valori delle tabelle che ho modificato
	cliScrivo->getPsi()->setSommaTabella(1.0); // perché faccio la normalizzazione direttamente nel kernel
	fi->setSommaTabella(cli->getPsi()->getSommaTabella());
	// qui in cuda, la fi viene già settata se su device
	//                    altrimenti devo settarla ma lo faccio su all'uscita dalla marginalization

#if UTILIZZA_CPU_PER_TABELLE_PICCOLE
	} // fine scattering
#endif

	//std::string sss;
	//std::cin >> sss;
	//std::cout << "fine update\n";


	/* TUTTO COMPLETO 
	margAndScatt(sizeTableLeggoPow2, dimFiStarTablePow2, cli->getPsi()->getTable(), indexingTableLeggo, sizeTableLeggo, dimFiStarTable,
					sizeTableScrivoPow2, fi->getTable(), tableCliScrivo, indexingTableScrivo, sizeTableScrivo
					);
	*/

		// calcolo solo se la divisione 
		//if (fi->getTable()[i] > zero) {
		//	// calcolo quanto vale fiStar[i]/fi[i]
		//	divisioneTraFi = fiStarTable[i] / fi->getTable()[i];
		//} else
		//	divisioneTraFi = 0;

		//std::cout << "fiStar[" << i << "]/fi[" << i << "] = " << divisioneTraFi << std::endl;

		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		//end = clock();
		//*elapsedDivMul += (double)(end - begin) / CLOCKS_PER_SEC;
		//


	//std::string sss;
	//std::cin >> sss;
	
	//return new Probability(vars, fiStarTable, dimFiStarTable, sommaTabella);
	//return new Probability(vars, fiStarTable, dimFiStarTable, cli->getPsi()->getSommaTabella());

}

double* Separator::marginalizationCPU(size_t dimFiStarTable, // sarebbe sia la dimensione dell'output, sia il nostro caro nArray.
							  size_t numeroElemDaSommare, // sarebbe il nostro caro e vecchio m
							  double** indexingTableLeggo // la indexing table che punta agli elementi su cui fare la marginalizzazione
							  //,Separator* sep // quello che si becca la tabella appena calcolata... // QUI!!!
							  ) {



	double* fiStarTable = new double[dimFiStarTable];
	//for (std::size_t i = 0; i < dimFiStarTable; i++) {
	//	fiStarTable[i] = 0.0;
	//}

	//std::cout << "sto eseguendo il metodo con la index" << std::endl;

	//std::size_t numeroElemDaSommare = cli->getPsi()->getTableSize() / dimFiStarTable;


	double sommaTabella = 0.0;
	for (std::size_t i = 0; i < dimFiStarTable; i++) {
		//std::cout << " i = " << i;

		fiStarTable[i] = 0.0;

		// calcolo il valore di fiStar[i]
		for (std::size_t j = 0; j < numeroElemDaSommare; j++) {
			//std::cout << " j = " << j << std::endl;
			//std::cout << fiStarTable[i];
			fiStarTable[i] += *indexingTableLeggo[i * numeroElemDaSommare + j];
			//std::cout << " --> " << fiStarTable[i] << std::endl;
			
		}

		sommaTabella += fiStarTable[i];

	}

	// per aiutare la normalizzazione
	//sep->getFi()->setSommaTabella(sommaTabella);
	fi->setSommaTabella(sommaTabella);

	return fiStarTable;
}

double* Separator::marginalizationCPUindexCUDA(size_t dimFiStarTable, // sarebbe sia la dimensione dell'output, sia il nostro caro nArray.
							  size_t numeroElemDaSommare, // sarebbe il nostro caro e vecchio m
							  size_t* indexingTableLeggo // la indexing table che punta agli elementi su cui fare la marginalizzazione
							  , double* tableCliLeggo
							  , size_t numeroElemDaSommarePow2
							  , size_t dimFiStarTablePow2
							  //,Separator* sep // quello che si becca la tabella appena calcolata... // QUI!!!
							  ) {



	double* fiStarTable = new double[dimFiStarTable];


	/*
	double sommaTabella = 0.0;
	for (std::size_t i = 0; i < dimFiStarTable; i++) {
		//std::cout << " i = " << i;

		fiStarTable[i] = 0.0;

		// calcolo il valore di fiStar[i]
		for (std::size_t j = 0; j < numeroElemDaSommare; j++) {
			//std::cout << " j = " << j << std::endl;
			//std::cout << fiStarTable[i];
			fiStarTable[i] += *indexingTableLeggo[i * numeroElemDaSommare + j];
			//std::cout << " --> " << fiStarTable[i] << std::endl;
			
		}

		sommaTabella += fiStarTable[i];

	}

	// per aiutare la normalizzazione
	//sep->getFi()->setSommaTabella(sommaTabella);
	fi->setSommaTabella(sommaTabella);

	return fiStarTable;
	*/


	//////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	double sommaTabella = 0.0;
	for (std::size_t i = 0; i < dimFiStarTable; i++) {
		//std::cout << " i = " << i;

		fiStarTable[i] = 0.0;

		// calcolo il valore di fiStar[i]
		for (std::size_t j = 0; j < numeroElemDaSommare; j++) {
#if SUM_ON_ROW
				//if (Config::indexingSumOnRow) {
					// [0][1][2] * [0000][1111][2222]
			fiStarTable[i] += tableCliLeggo[indexingTableLeggo[i * numeroElemDaSommarePow2 + j]];
#else
				//} else {
					// [0][1][2] * [012][012][012][012]
			fiStarTable[i] += tableCliLeggo[indexingTableLeggo[i + dimFiStarTablePow2 * j]];
				//}
#endif
			//std::cout << " j = " << j << std::endl;
			//std::cout << fiStarTable[i];
			//fiStarTable[i] += *indexingTableLeggo[i * numeroElemDaSommare + j];
			//std::cout << " --> " << fiStarTable[i] << std::endl;
			
		}

		sommaTabella += fiStarTable[i];

	}


	fi->setSommaTabella(sommaTabella);

	return fiStarTable;
}

void Separator::scatteringCPU(size_t dimFiStarTable, // sarebbe la dimsione del separatore su cui iteriamo per leggere i valori da dividere
				size_t numeroElemDaAggiornareConUgualeValore,
				double** indexingTableScrivo,
				double* fiStarTable,
				double* fiTable,
				JTClique* cliScrivo,
				bool fiStarTableOnCPU
				)
{

	if (!fiStarTableOnCPU)
		fiStarTable = recuperaDaGPU(fiStarTable, dimFiStarTable);

	double divisioneTraFi;

	double sommaTabellaCri = 0.0;

	for (std::size_t i = 0; i < dimFiStarTable; i++) {
		

		// calcolo solo se la divisione 
		if (fiTable[i] > ZERO_DIVISIONE) {
			// calcolo quanto vale fiStar[i]/fi[i]
			divisioneTraFi = fiStarTable[i] / fiTable[i];
		} else
			divisioneTraFi = 0;

		//std::cout << "fiStar[" << i << "]/fi[" << i << "] = " << divisioneTraFi << std::endl;

		// se devo moltiplicare per 1 allora mi risparmio il giro
		if (divisioneTraFi != 1) {

			// provo a sbagliare per vedere se il confronto lo rileva
			//divisioneTraFi = 0.5;

			for (std::size_t j = 0; j < numeroElemDaAggiornareConUgualeValore; j++) {
				//std::cout << "iT[" << i * numeroElemDaAggiornareConUgualeValore + j << "] = " << *indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j] << " --> ";
				*indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j] *= divisioneTraFi;
				//std::cout << *indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j];
				//std::string sss;
				//std::cin >> sss;

				// SUMT
				sommaTabellaCri += *indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j];
			}
		} else {
			for (std::size_t j = 0; j < numeroElemDaAggiornareConUgualeValore; j++) {
				// SUMT
				sommaTabellaCri += *indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j];
			}
		}
	}

	//delete fiTable;
	fi->setTable(fiStarTable);
	cliScrivo->getPsi()->setSommaTabella(sommaTabellaCri);
}

void Separator::scatteringCPUindexCUDA(size_t dimFiStarTable, // sarebbe la dimsione del separatore su cui iteriamo per leggere i valori da dividere
				size_t numeroElemDaAggiornareConUgualeValore,
				size_t* indexingTableScrivo,
				double* fiStarTable,
				double* fiTable,
				JTClique* cliScrivo,
				bool fiStarTableOnCPU
				, double* tableCliScrivo
				, size_t numeroElemDaAggiornareConUgualeValorePow2
				, size_t dimFiStarTablePow2
				)
{

	if (!fiStarTableOnCPU)
		fiStarTable = recuperaDaGPU(fiStarTable, dimFiStarTable);

	double divisioneTraFi;

	double sommaTabellaCri = 0.0;

	for (std::size_t i = 0; i < dimFiStarTable; i++) {

		// calcolo solo se la divisione 
		if (fi->getTable()[i] > ZERO_DIVISIONE) {
			// calcolo quanto vale fiStar[i]/fi[i]
			divisioneTraFi = fiStarTable[i] / fiTable[i]; // CAMBIARE
		} else
			divisioneTraFi = 0;

		//std::cout << "fiStar[" << i << "]/fi[" << i << "] = " << divisioneTraFi << std::endl;

		// se devo moltiplicare per 1 allora mi risparmio il giro
		if (divisioneTraFi != 1) {

			// provo a sbagliare per vedere se il confronto lo rileva
			//divisioneTraFi = 0.5;

			for (std::size_t j = 0; j < numeroElemDaAggiornareConUgualeValore; j++) {
#if !USA_CUDA && SUM_ON_ROW
				// NON VIENE MAI USATO!!!
				//if (Config::indexingSumOnRow) {
					// [0][1][2] * [0000][1111][2222]
					/*
					tableCliScrivo[indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j]] *= divisioneTraFi;
					*/
					///*
					// diventa...
					tableCliScrivo[indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValorePow2 + j]] *= divisioneTraFi;

					// SUMT
					sommaTabellaCri += tableCliScrivo[indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValorePow2 + j]];


					//std::cout << sommaTabellaCri << std::endl;
					//*/
					//
#else
				//} else {
					// [0][1][2] * [012][012][012][012]
					/*
					tableCliScrivo[indexingTableScrivo[i + dimFiStarTable * j]] *= divisioneTraFi;
					*/
					///*
					// diventa...
					tableCliScrivo[indexingTableScrivo[i + dimFiStarTablePow2 * j]] *= divisioneTraFi;

					// SUMT
					sommaTabellaCri += tableCliScrivo[indexingTableScrivo[i + dimFiStarTablePow2 * j]];


					//std::cout << sommaTabellaCri << std::endl;
					//*/
					//
				//}
#endif
				

			}
		} else {
			for (std::size_t j = 0; j < numeroElemDaAggiornareConUgualeValore; j++) {
#if SUM_ON_ROW
					sommaTabellaCri += tableCliScrivo[indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValorePow2 + j]];
#else
					// SUMT
					sommaTabellaCri += tableCliScrivo[indexingTableScrivo[i + dimFiStarTablePow2 * j]];
					//std::cout << sommaTabellaCri << std::endl;
#endif
			}
		}


	}


	fi->setTable(fiStarTable); // CAMBIARE

	// SUMT
	cliScrivo->getPsi()->setSommaTabella(sommaTabellaCri);

}

void Separator::updatePotentialsCUDAonCPU(JTClique* cli, JTClique* cliScrivo, long long* elapsedSum, long long* elapsedDivMul)
{
	// OCCHIO: le tabelle degli indici adesso hanno anche i valori SIZE_MAX!!!!
	size_t* indexingTableLeggo;
	size_t* indexingTableScrivo;
	if (cli == soggetto) {
		indexingTableLeggo = indexingSoggettoCUDA;
		indexingTableScrivo = indexingOggettoCUDA;
	} else {
		// sicuramente è dell'oggetto, non ammetto che uno sbagli a passarmi la cli!!!
		indexingTableLeggo = indexingOggettoCUDA;
		indexingTableScrivo = indexingSoggettoCUDA;
	}

	std::size_t dimFiStarTable = fi->getTableSize();

	double* fiStarTable = new double[dimFiStarTable];
	//for (std::size_t i = 0; i < dimFiStarTable; i++) {
	//	fiStarTable[i] = 0.0;
	//}

	//std::cout << "sto eseguendo il metodo con la index" << std::endl;
	double* tableCliScrivo = cliScrivo->getPsi()->getTable();
	double* tableCliLeggo = cli->getPsi()->getTable();
	
#if !TEMPO_COMPLESSIVO  && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
	// rilevamento tempo occupato dalle somme
	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;
#endif

	std::size_t numeroElemDaSommare = cli->getPsi()->getTableSize() / dimFiStarTable;
	std::size_t numeroElemDaAggiornareConUgualeValore = cliScrivo->getPsi()->getTableSize() / dimFiStarTable;



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// provo a costruire la tabella già pronta con m e N potenze di 2
	std::size_t numeroElemDaSommarePow2 = pow2roundup(numeroElemDaSommare);
	std::size_t dimFiStarTablePow2 = pow2roundup(dimFiStarTable); // nArrayPow2 
	std::size_t sizeTableLeggoPow2 = numeroElemDaSommarePow2 * dimFiStarTablePow2;
	// provo a costruire la tabella già pronta con m e N potenze di 2
	std::size_t numeroElemDaAggiornareConUgualeValorePow2 = pow2roundup(numeroElemDaAggiornareConUgualeValore);
	std::size_t sizeTableScrivoPow2 = numeroElemDaAggiornareConUgualeValorePow2 * dimFiStarTablePow2;
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	double sommaTabella = 0.0;
	double sommaTabellaCri = 0.0;
	for (std::size_t i = 0; i < dimFiStarTable; i++) {
		//std::cout << " i = " << i;

		fiStarTable[i] = 0.0;

#if !TEMPO_COMPLESSIVO  && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
		// rilevamento tempo occupato dalle somme!!!
		begin = std::chrono::high_resolution_clock::now();
		//
#endif

		// calcolo il valore di fiStar[i]
		for (std::size_t j = 0; j < numeroElemDaSommare; j++) {
#if SUM_ON_ROW
				//if (Config::indexingSumOnRow) {
					// [0][1][2] * [0000][1111][2222]
			fiStarTable[i] += tableCliLeggo[indexingTableLeggo[i * numeroElemDaSommarePow2 + j]];
#else
				//} else {
					// [0][1][2] * [012][012][012][012]
			fiStarTable[i] += tableCliLeggo[indexingTableLeggo[i + dimFiStarTablePow2 * j]];
				//}
#endif
			//std::cout << " j = " << j << std::endl;
			//std::cout << fiStarTable[i];
			//fiStarTable[i] += *indexingTableLeggo[i * numeroElemDaSommare + j];
			//std::cout << " --> " << fiStarTable[i] << std::endl;
			
		}

#if !TEMPO_COMPLESSIVO  && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
		// rilevamento tempo occupato dalle somme!!!
		end = std::chrono::high_resolution_clock::now();
		*elapsedSum += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		//
#endif

		sommaTabella += fiStarTable[i];

		double divisioneTraFi;

#if !TEMPO_COMPLESSIVO  && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		begin = std::chrono::high_resolution_clock::now();
		//
#endif

		// calcolo solo se la divisione 
		if (fi->getTable()[i] > ZERO_DIVISIONE) {
			// calcolo quanto vale fiStar[i]/fi[i]
			divisioneTraFi = fiStarTable[i] / fi->getTable()[i];
		} else
			divisioneTraFi = 0;

		//std::cout << "fiStar[" << i << "]/fi[" << i << "] = " << divisioneTraFi << std::endl;

		// se devo moltiplicare per 1 allora mi risparmio il giro
		if (divisioneTraFi != 1) {

			// provo a sbagliare per vedere se il confronto lo rileva
			//divisioneTraFi = 0.5;

			for (std::size_t j = 0; j < numeroElemDaAggiornareConUgualeValore; j++) {
#if !USA_CUDA && SUM_ON_ROW
				// NON VIENE MAI USATO!!!
				//if (Config::indexingSumOnRow) {
					// [0][1][2] * [0000][1111][2222]
					/*
					tableCliScrivo[indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j]] *= divisioneTraFi;
					*/
					///*
					// diventa...
					tableCliScrivo[indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValorePow2 + j]] *= divisioneTraFi;

					// SUMT
					sommaTabellaCri += tableCliScrivo[indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValorePow2 + j]];


					//std::cout << sommaTabellaCri << std::endl;
					//*/
					//
#else
				//} else {
					// [0][1][2] * [012][012][012][012]
					/*
					tableCliScrivo[indexingTableScrivo[i + dimFiStarTable * j]] *= divisioneTraFi;
					*/
					///*
					// diventa...
					tableCliScrivo[indexingTableScrivo[i + dimFiStarTablePow2 * j]] *= divisioneTraFi;

					// SUMT
					sommaTabellaCri += tableCliScrivo[indexingTableScrivo[i + dimFiStarTablePow2 * j]];


					//std::cout << sommaTabellaCri << std::endl;
					//*/
					//
				//}
#endif
				

			}
		} else {
			for (std::size_t j = 0; j < numeroElemDaAggiornareConUgualeValore; j++) {
#if SUM_ON_ROW
					sommaTabellaCri += tableCliScrivo[indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValorePow2 + j]];
#else
					// SUMT
					sommaTabellaCri += tableCliScrivo[indexingTableScrivo[i + dimFiStarTablePow2 * j]];
					//std::cout << sommaTabellaCri << std::endl;
#endif
			}
		}

#if !TEMPO_COMPLESSIVO  && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		end = std::chrono::high_resolution_clock::now();
		*elapsedDivMul += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		//
#endif

	}

	//std::string sss;
	//std::cin >> sss;
	delete fi->getTable(); // free(fi->getTable());
	fi->setTable(fiStarTable);
	fi->setSommaTabella(sommaTabella);

	// SUMT
	cliScrivo->getPsi()->setSommaTabella(sommaTabellaCri);

	//return new Probability(vars, fiStarTable, dimFiStarTable, sommaTabella);

}

	
	
//void Separator::aggiornaPotenzialeOrdinato(JTClique* node, Probability* fiSeparatoreStar, Probability* fiSeparatore)
//{
//	double** indexingTable;
//	if (node == soggetto) {
//		indexingTable = indexingSoggetto;
//	} else {
//		// sicuramente è dell'oggetto, non ammetto che uno sbagli a passarmi la cli!!!
//		indexingTable = indexingOggetto;
//	}
//
//	for (std::size_t i = 0; i < node->getPsi()->getTableSize(); i++) {
//		indexingTable
//	}
//}
