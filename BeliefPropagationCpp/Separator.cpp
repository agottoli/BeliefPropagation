#include "Separator.h"
#include <iostream>
#include <string>
#include <fstream>
// tempistiche somme
//#include <time.h>
#include <chrono>

// per cuda
//#include "../BeliefPropagationCUDA/sumKernelSmallN.h"
#include "../BeliefPropagationCUDA/myLibKernelBigN.h"
//

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

		
		if (Config::indexingSumOnRow) {
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
		} else {
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
		}

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
	for (std::size_t i = 0; i < dimFiStarTable; i++) {
		fiStarTable[i] = 0.0;
	}

	//std::cout << "sto eseguendo il metodo con la index" << std::endl;
	

	// rilevamento tempo occupato dalle somme
	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;

	std::size_t numeroElemDaSommare = cli->getPsi()->getTableSize() / dimFiStarTable;
	std::size_t numeroElemDaAggiornareConUgualeValore = cliScrivo->getPsi()->getTableSize() / dimFiStarTable;

	double sommaTabella = 0;
	for (std::size_t i = 0; i < dimFiStarTable; i++) {
		//std::cout << " i = " << i;

		// rilevamento tempo occupato dalle somme!!!
		begin = std::chrono::high_resolution_clock::now();
		//

		// calcolo il valore di fiStar[i]
		for (std::size_t j = 0; j < numeroElemDaSommare; j++) {
			//std::cout << " j = " << j << std::endl;
			//std::cout << fiStarTable[i];
			fiStarTable[i] += *indexingTableLeggo[i * numeroElemDaSommare + j];
			//std::cout << " --> " << fiStarTable[i] << std::endl;
			
		}

		// rilevamento tempo occupato dalle somme!!!
		end = std::chrono::high_resolution_clock::now();
		*elapsedSum += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
		//

		sommaTabella += fiStarTable[i];

		double divisioneTraFi;

		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		begin = std::chrono::high_resolution_clock::now();
		//

		// calcolo solo se la divisione 
		if (fi->getTable()[i] > zeroALE) {
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
			}
		}

		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		end = std::chrono::high_resolution_clock::now();
		*elapsedDivMul += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
		//

	}

	//std::string sss;
	//std::cin >> sss;
	delete fi->getTable(); // free(fi->getTable());
	fi->setTable(fiStarTable);
	fi->setSommaTabella(sommaTabella);
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
	std::size_t* indexingTableLeggo;
	std::size_t* indexingTableScrivo;
	
	if (cli == soggetto) {
		indexingTableLeggo = indexingSoggettoCUDA;
		indexingTableScrivo = indexingOggettoCUDA;
	} else {
		// sicuramente è dell'oggetto, non ammetto che uno sbagli a passarmi la cli!!!
		indexingTableLeggo = indexingOggettoCUDA;
		indexingTableScrivo = indexingSoggettoCUDA;
	}

	std::size_t dimFiStarTable = fi->getTableSize();

	//double* fiStarTable = new double[dimFiStarTable];

	// rilevamento tempo occupato dalle somme
	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;

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
	begin = std::chrono::high_resolution_clock::now();
	double* fiStarOnGPU = marginalizationBigN(sizeTableLeggoPow2, dimFiStarTablePow2, cli->getPsi()->getTable(), indexingTableLeggo, sizeTableLeggo, dimFiStarTable);
	end = std::chrono::high_resolution_clock::now();
	*elapsedSum += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

	// SCATTERING
	begin = std::chrono::high_resolution_clock::now();
	scattering(sizeTableScrivoPow2, dimFiStarTablePow2, fiStarOnGPU, fi->getTable(), tableCliScrivo, indexingTableScrivo, sizeTableScrivo, dimFiStarTable);
	end = std::chrono::high_resolution_clock::now();
	*elapsedDivMul += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
		
	
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
