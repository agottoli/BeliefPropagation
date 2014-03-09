#include <iostream>
#include <vector>
#include <string>
//#include <boost/lexical_cast.hpp>
#include <iterator>
#include <set>
#include <queue>
#include "Variable.h"
#include "BayesianNetwork.h"
#include "JunctionTree.h"
#include "JTClique.h"
#include "RetiEsempio.cpp"
#include "BeliefPropagation.h"
#include "huginParser/JavaCC.h"
#include "huginParser/CharStream.h"
#include "huginParser/HuginParser.h"
#include "huginParser/HuginParserTokenManager.h"
#include <stdlib.h>

// include per leggere un file
#include <fstream>
#include <iomanip>

// includo gli header di cuda
#include "../BeliefPropagationCUDA/margAndScatt.h"

#include <limits.h> // per la misurazione del tempo

/**
 * E' la partenza di tutto!
 *
 * @author Alessandro Gottoli vr352595
 */
int main(int argc, char* argv[]) 
{
	bool VS = false;
	/* Per farlo partire senza passare i file da linea di comando */
	/* * /
	VS = true;
	argc = 3;
	char* argv2[3]; 
	argv2[0] = argv[0];
	argv2[1] = "..\\RetiEsempio\\Diabetes.net";
	argv2[2] = "..\\RetiEsempio\\Diabetes.num";
	//argv2[1] = "..\\RetiEsempio\\powerplant.net";
	//argv2[2] = "..\\RetiEsempio\\powerplant.num";
	argv = argv2;
	/*  */

	std::string sss;

	// se non gli passo un file in input esce con le itruzioni per l'esecuzione
	if (argc < 2) {
		std::cout << "Specificare file di input!" << std::endl;
		std::cout << "USAGE: " << argv[0] << " file_input.net [file_ordinamento.num]" << std::endl;
		//std::cin >> sss;
        return 1;
	}

	std::ifstream f(argv[1]);
	// controllo se il file esiste
    if(!f) {
        std::cout << "Il file della rete non esiste! (" << argv[1] << ")";
		//std::cin >> sss;
        return 2;
    }

	// leggo il file della rete
	std::string input;
    std::string s;
    while(f.good()) //fino a quando c'è qualcosa da leggere ..
    {
        //legge tutta la riga dal file e la mette nella variabile s
        getline(f, s);
        input.append(s).append("\n");
    }

    f.close(); //chiude il file della rete
	//std::cout << "letto file\n";

	// preparo il parsing della stringa in cui ho memorizzato la rete 
	hugin_parser::CharStream *stream = new hugin_parser::CharStream(input, input.size() - 1, 1, 1); // stringa, buffer???=fino a che colonna guardare, riga, colonna
	hugin_parser::HuginParserTokenManager *scanner = new hugin_parser::HuginParserTokenManager(stream);
	hugin_parser::HuginParser parser(scanner);
	//DEBUG
	std::cout << "Leggo la rete passata in input...";
	//
	// faccio partire il parsing
	parser.Start();
	// e mi recupero la rete bayesiana
	BayesianNetwork* bn = parser.getBayesianNetwork();
	// DEBUG
	std::cout << " OK." << std::endl;
	//

	// LEGGO L'ORDINAMENTO
	std::vector<std::string>* ordinamento = new std::vector<std::string>();
	if (argc > 2) {
		
		std::cout << "E' stato specificato un file di ordinamento." << std::endl;

		std::ifstream f2(argv[2]);
		std::string elem;

		if(!f2) {
			std::cout << "Il file dell'ordinamento specificato non esiste! (" << argv[2] << ")" << std::endl;
			std::cout << "continuo calcolandomi io l'ordinamento utilizzando un'euristica." << std::endl;

			//if (strcmp(argv[2], "indexing") == 0)
			//	Config::useIndexingTable = true;
			//std::cin >> sss;
			
			//return -1;
		} else {
	
			while (std::getline(f2, elem, ' ')) {
				ordinamento->push_back(elem);
			}
		}
	} else {
		std::cout << "non è stato specificato nessun ordinamento... lo decido io con un'euristica." << std::endl;
	}

	bn->setOrdinamento(ordinamento);
	
	std::cout << "E' stato scelto di ";
	//if (argc > 3)
	//	if (strcmp(argv[3], "indexing") == 0)
	//		Config::useIndexingTable = true;
#if !USE_INDEXING_TABLE
	//if (!Config::useIndexingTable) {
		std::cout << "NON ";
	//}
#endif
	std::cout << "utilizzare le index mapping table." << std::endl;
	//std::cin >> sss;


	std::unordered_set<Variable*>* vars = bn->getVariables();
	//std::size_t nStatiJoint = 1;
	std::cout << "normalizzo le tabelle delle probabilità condizionali...";
	for (std::unordered_set<Variable*>::iterator I = vars->begin(); I != vars->end(); I++) {
		//nStatiJoint *= (*I)->getStates()->size();
		//std::cout << (*I)->toString() << " " << (*I)->getStates()->size() << std::endl;
		(*I)->getConditionalProbability()->normalizzaCPSeServe();
	}
	std::cout << " OK." << std::endl;
	
	// DEBUG
	//std::cout << "la tabella della probabilità congiunta conterrebbe " << nStatiJoint << " elementi." << std::endl;
	
	//std::cin >> sss;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//*/

	// CREO LA RETE
	std::vector<BayesianNetwork*>* BNSet = new std::vector<BayesianNetwork*>();
	//BNSet->reserve(10);
	//BNSet->push_back(RetiEsempio::createEarthquakeEx());
	//BNSet->push_back(RetiEsempio::createAsiaEx());
	//BNSet->push_back(RetiEsempio::createComaEx());
	//BNSet->push_back(RetiEsempio::createSprinklerEx());
	BNSet->push_back(bn);

	//std::cout << "DOT BN:\n" << net->getDOT() << '\n';
	//std::cin >> sss;

#if USA_CUDA
	//if (Config::useCUDA)
		selectGPU();
#endif

	for (std::vector<BayesianNetwork*>::iterator itNet = BNSet->begin(); itNet != BNSet->end(); itNet++) {
		BayesianNetwork* net = *itNet;
	
		// MORALIZZO
		// DEBUG
		std::cout << "\nMORALIZZAZIONE..." << std::endl;
		//
		net->moralize();
		// DEBUG
		std::cout << "moralizzazione OK." << std::endl;
		//

		// TRIANGOLIZZO E TROVO CRICCHE MASSIMALI
		// DEBUG
		std::cout << "\nTRIANGOLARIZZAZIONE..." << std::endl;
		//
		std::unordered_set<JTClique*>* cli = net->triangolateMaxClique();
		//DEBUG
		std::cout << "triangolarizzazione OK." << std::endl;
		//

		//DEBUG stampo delle statistiche sulle cricche 
		std::size_t nElem = net->getNumeroElementiDelleTabelleProbability();
		std::cout << "\n--------\nIl numero di elementi totali nelle tabelle dei potenziali delle cricche sono: " << nElem << std::endl;
		std::size_t dimD = sizeof(double);
		//std::cout << "un double occupa: " << dimD << " byte." << std::endl;
		std::size_t MbyteOccupati = nElem * dimD / 1000000;
		std::cout << "per un totale di: " << MbyteOccupati << " Mbyte occupati dai soli elementi." << std::endl;
		
		std::cout << "numero di cricche trovate: " << net->getNumeroCricche() << " numero di cricche massimali: " << net->getNumeroCriccheMassimali() << std::endl;
		std::cout << "\n--------" << std::endl;
		//std::cin >> sss;
		//

		// COSTRUZIONE JUNCTION TREE
		// DEBUG
		std::cout << "\nCOSTRUZIONE JUNCTION TREE..."  << std::endl;
		//
		JunctionTree* jt = net->createJunctionTreeMST(cli);
		// DEBUG
		std::cout << "costruzione junction tree OK." << std::endl;
		//

		// controllo se in numero di cricche del jt è uno in più del numero dei link
		/*if (!jt->unicoAlbero()) {
			std::cout << "E' una foresta... NON LO CONSIDERO VALIDO!!!" << std::endl;
			std::cin >> sss;
			return 1;
		}*/
		
		// stampo la rete in formato dot per una questione visiva
		//std::cout << jt->getDOT() << '\n';

		// stampo per vedere il numero di variabili che devono essere minori del numero di cricche
		//std::cout << "nVar BN: " << net->getVariables()->size() << " vs nCli JT: " << jt->getCliques()->size();

		// controllo per ogni cricca se ho calcolato il psi
		/*std::cout << "maximal cliques:\n";
		for (std::unordered_set<JTClique*>::iterator I = cli->begin(); I != cli->end(); ++I) {
		std::cout << (*I)->toString() << '\n';
		if ((*I)->getPsi() == NULL)
		std::cout << "sta merda ha PSI NULLO!!!\n";
		std::cin >> sss;
		}
		*/

#if PRINT_JT_STATISTICS
		// STAMPO LE STATISTICHE DEL JUNCTION TREE
		std::cout << "-----------------------------------------\n";
		jt->getStatistics();
		std::cout << "-----------------------------------------\n";
#endif

		// BELIEF PROPAGATION (finalmente!!!)
		// DEBUG
		std::cout << "\nBELIEF PROPAGATION..." << std::endl;
		//
		BeliefPropagation::BP(jt);
		// DEBUG
		std::cout << "belief propagation OK." << std::endl;
		//
		
		// DEBUG voglio vedere un po' ti tabelle delle cricche che mi da junction tree uguale per ogni posizione
		//for (std::unordered_set<JTClique*>::iterator it = jt->getCliques()->begin(); it != jt->getCliques()->end(); it++) {
		//	std::cout << "psi: " << (*it)->getPsi()->toString();
		//	std::cin >> sss;
		//}
		//

		// METODO PER IL CONTROLLO SE HO PRODOTTO LE TABELLE IN MANIERA ESATTA
		/* NOTA: se le tabelle sono troppo grandi non posso usarlo :( * /
		// CALCOLO LE JOINT PROBABILITY
		// DEBUG
		std::cout << "calcolo joint probability della bayesian network..." << std::endl;
		//
		Probability* jointBN = net->calcolaJointProbability();
		//std::cout << "jointBN: " << jointBN->toString();
		// DEBUG
		std::cout << "calcolo joint probability del junction tree..." << std::endl;
		//
		Probability* jointJT = jt->calcolaJointProbability();
		//std::cout << "jointJT: " << jointJT->toString();

		//std::cout << "BAYESIAN:\n" << jointBN->toString() << '\n';
		//std::cout << "JUNCTION:\n" << jointJT->toString() << '\n';

		// DEBUG
		std::cout << "confronto le tabelle delle due joint probability..." << std::endl;
		//
		bool esito = jointBN->confronta(jointJT);
		if (esito)
			std::cout << "\nOK!!! :)\n";
		else
			std::cout << "\nERROR!!! :(\n";

		/ * */
		

		if (VS)
			std::cin >> sss;
	}

#if USA_CUDA
//	if (Config::useCUDA)
		resetGPU();
#endif	
	//std::cin >> sss;
	
}

