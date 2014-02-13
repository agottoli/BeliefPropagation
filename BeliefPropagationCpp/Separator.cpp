#include "Separator.h"
#include <iostream>
#include <string>
#include <fstream>
// tempistiche somme
#include <time.h>

// per cuda
#include "../BeliefPropagationCUDA/myLibKernelSmallN.h"
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

	std::cout << "numero elementi sommo m: " << dimMapToSame << " e gli arrai sono N: " << fi->getTableSize() << std::endl;
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

std::size_t* Separator::createIndexingTableCUDA(JTClique* cliqueToIndex)
{
	Probability* cliquePsi = cliqueToIndex->getPsi();

	std::size_t dimCliqueTable = cliquePsi->getTableSize();
	std::size_t* indexT = new size_t[dimCliqueTable];

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

	//std::cout << "numero elementi sommo: " << dimMapToSame << " e gli arrai sono: " << fi->getTableSize() << std::endl;
	//std::string s;
	//std::cin >> s;


	std::size_t fiIndex;
	for (std::size_t i = 0; i < dimCliqueTable; i++) {
		// NOTA: configClique si riferisce esattamente alla configurazione dell'indice i nella tabella della cricca
		fiIndex = fi->getIndexOfConfig(configSep);
		indexT[fiIndex * dimMapToSame + (offset[fiIndex])++] = i; //cliquePsi->getAtConfig(&configClique);

		// aggiorno alla prossima configurazione
		cliquePsi->nextConfig(&configClique);
	}


	delete offset;
	delete configSep;

	return indexT;
}

Probability* Separator::sumOnIndexingTableOf(JTClique* cli, JTClique* cliScrivo, double* elapsedSum, double* elapsedDivMul)
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
	clock_t begin;
	clock_t end;

	std::size_t numeroElemDaSommare = cli->getPsi()->getTableSize() / dimFiStarTable;
	std::size_t numeroElemDaAggiornareConUgualeValore = cliScrivo->getPsi()->getTableSize() / dimFiStarTable;

	double sommaTabella = 0;
	for (std::size_t i = 0; i < dimFiStarTable; i++) {
		//std::cout << " i = " << i;

		// rilevamento tempo occupato dalle somme!!!
		begin = clock();
		//

		// calcolo il valore di fiStar[i]
		for (std::size_t j = 0; j < numeroElemDaSommare; j++) {
			//std::cout << " j = " << j << std::endl;
			//std::cout << fiStarTable[i];
			fiStarTable[i] += *indexingTableLeggo[i * numeroElemDaSommare + j];
			//std::cout << " --> " << fiStarTable[i] << std::endl;
			
		}

		// rilevamento tempo occupato dalle somme!!!
		end = clock();
		*elapsedSum += (double)(end - begin) / CLOCKS_PER_SEC;
		//

		sommaTabella += fiStarTable[i];

		double divisioneTraFi;

		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		begin = clock();
		//

		// calcolo solo se la divisione 
		if (fi->getTable()[i] > zero) {
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
		end = clock();
		*elapsedDivMul += (double)(end - begin) / CLOCKS_PER_SEC;
		//

	}

	//std::string sss;
	//std::cin >> sss;

	return new Probability(vars, fiStarTable, dimFiStarTable, sommaTabella);

}

Probability* Separator::sumOnIndexingTableOfCUDA(JTClique* cli, JTClique* cliScrivo, double* elapsedSum, double* elapsedDivMul)
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
	clock_t begin;
	clock_t end;

	std::size_t sizeTableLeggo = cli->getPsi()->getTableSize();
	std::size_t numeroElemDaSommare = sizeTableLeggo / dimFiStarTable;
	std::size_t numeroElemDaAggiornareConUgualeValore = cliScrivo->getPsi()->getTableSize() / dimFiStarTable;

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
	
	double* fiStarTable = runSmallN(sizeTableLeggo, dimFiStarTable, cli->getPsi()->getTable(), (long unsigned int *) indexingTableLeggo); 
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*int size = sizeTableLeggo; //32;    // number of elements to reduce -> default:  16777216						// ALE
	int nArray = dimFiStarTable; //2;	// m = 16384 
    unsigned int bytesValuesInput = size * sizeof(double);										// ALE
    double *h_idata = (double *) malloc(bytesValuesInput);										// ALE
	unsigned int bytesIndexInput = size * sizeof(long unsigned int);										// ALE
	long unsigned int *h_iIndexData = (long unsigned int *) malloc(bytesIndexInput);										// ALE
		
	for (int i=0; i<size; i++) { h_idata[i] = 1; h_iIndexData[i]=i; }
	
	double* fiStarTable = runSmallN(size, nArray, h_idata, h_iIndexData);
	*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	//
	double* tableCliScrivo = cliScrivo->getPsi()->getTable();
	
	
	for (std::size_t i = 0; i < dimFiStarTable; i++) {

		double divisioneTraFi;

		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		begin = clock();
		//

		// calcolo solo se la divisione 
		if (fi->getTable()[i] > zero) {
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
				tableCliScrivo[indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j]] *= divisioneTraFi;
				//std::cout << *indexingTableScrivo[i * numeroElemDaAggiornareConUgualeValore + j];
				//std::string sss;
				//std::cin >> sss;
			}
		}

		// rilevamento tempo occupato dalla divisione e moltiplicazione!!!
		end = clock();
		*elapsedDivMul += (double)(end - begin) / CLOCKS_PER_SEC;
		//

	}

	//std::string sss;
	//std::cin >> sss;
	
	//return new Probability(vars, fiStarTable, dimFiStarTable, sommaTabella);
	return new Probability(vars, fiStarTable, dimFiStarTable, cli->getPsi()->getSommaTabella());

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
