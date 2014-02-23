///* strtold example */
//#include <stdio.h>      /* printf, NULL */
//#include <stdlib.h>     /* strtold */
//#include <string>
//#include <sstream>
//#include <iostream>
//
//int main ()
//{
//  char szOrbits[] = "90613.305 365.24";
//  char * pEnd;
//  double f1, f2;
//  std::stringstream ss (szOrbits); 
//  ss >> f1;
//  //f1 = std::strtold (szOrbits, &pEnd);
//  //f2 = stold (pEnd, NULL);
//  printf ("Pluto takes %.2Lf years to complete an orbit.\n", f1);
//  std::string sss;
//  std::cin >> sss;
//  return 0;
//}

//#pragma warning(disable:4996)

//#define _SECURE_SCL 0

#include <iostream>
#include <vector>
#include <string>
//#include <boost/lexical_cast.hpp>
#include <iterator>
#include <set>
#include <queue>
//#include <sstream>
//#include "Probability.h"
//#include "ProbabilityMD.h"
#include "Variable.h"
#include "BayesianNetwork.h"
#include "JunctionTree.h"
#include "JTClique.h"
#include "RetiEsempio.cpp"
#include "BeliefPropagation.h"
//#include "huginParser\HuginParser.h"
//#include "adder\Adder.h"
#include "huginParser/JavaCC.h"
#include "huginParser/CharStream.h"
#include "huginParser/HuginParser.h"
#include "huginParser/HuginParserTokenManager.h"
#include <stdlib.h>

// include per leggere un file
#include <fstream>
#include <iomanip>
//#include <iostream>
//#include <string>
//#include <stdlib.h>
// fine

// includo gli header di cuda
//#include "../BeliefPropagationCUDA/sumKernelSmallN.h"
#include "../BeliefPropagationCUDA/myLibKernelBigN.h"
//#include "./CUDA/myLibKernelSmallN.h"


//void provaVarPassaggioPuntatoreOValore();
//void provaBNset();
//void provaJTPotMUL();
//void provaJTPotSUMON();
//void provaPriorityQueueMyComparator();

// metodo preso da un esempio per leggere un file
//std::string ReadFileFully(char *file_name) {
//  std::string s;
//  std::ifstream fp_in;
//  fp_in.open(file_name, std::ios::in);
//  // Very inefficient.
//  while (!fp_in.eof()) {
//   s += fp_in.get();
//  }
//  return s.substr(0, s.length() - 1);
//}

#include <limits.h>

int main(int argc, char* argv[]) 
{
	/* * /
	argc = 3;
	char* argv2[3]; 
	argv2[0] = argv[0];
	argv2[1] = "..\\RetiEsempio\\Munin1.net";
	argv2[2] = "..\\RetiEsempio\\Munin1.num";
	argv = argv2;
	/ *  */

	std::string sss;
	
	/// DIM //////////
	//std::ofstream fM("meno1024.txt", std::ios::app); //apre il file in modalità append, lasciando intatto quello che c'è e scrivendo alla fine
 //   if(!fM) {
 //       std::cout << "Errore nell'apertura del file!";
 //       std::string s;
	//	std::cin >> s;
 //   }

 //   fM << argv[1] << std::endl;

 //   fM.close(); //chiudo il file

	//std::ofstream fP("piu1024.txt", std::ios::app); //apre il file in modalità append, lasciando intatto quello che c'è e scrivendo alla fine
 //   if(!fP) {
 //       std::cout << "Errore nell'apertura del file!";
 //       std::string s;
	//	std::cin >> s;
 //   }

 //   fP << argv[1] << std::endl;

 //   fP.close(); //chiudo il file
	///////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///*
	if (argc < 2) {
		std::cout << "Specificare file di input!" << std::endl;
		std::cout << "USAGE: " << argv[0] << " file_input.net [file_ordinamento.num]" << std::endl;
		//std::cin >> sss;
        return 1;
	}
	std::ifstream f(argv[1]);

	std::string input;
    std::string s;

    if(!f) {
        std::cout << "Il file della rete non esiste! (" << argv[1] << ")";
		//std::cin >> sss;
        return 2;
    }

    while(f.good()) //fino a quando c'è qualcosa da leggere ..
    {
        //legge tutta la riga dal file e la mette nella variabile s
        getline(f, s);
        input.append(s).append("\n");
    }

    f.close(); //chiude il file


	//std::cout << "letto file\n";
	hugin_parser::CharStream *stream = new hugin_parser::CharStream(input, input.size() - 1, 1, 1); // stringa, buffer???=fino a che colonna guardare, riga, colonna
	//hugin_parser::CharStream *stream = new hugin_parser::CharStream(s, 1, 1, s.size() - 1);
	hugin_parser::HuginParserTokenManager *scanner = new hugin_parser::HuginParserTokenManager(stream);
	hugin_parser::HuginParser parser(scanner);
	//DEBUG
	std::cout << "Leggo la rete passata in input...";
	//
	parser.Start();
	// DEBUG
	std::cout << " OK." << std::endl;
	//
	
	//std::cout << "file " << input;
	// DEBUG
	//std::cout << "stampo le tabelle delle probabilità della rete appena letta." << std::endl;
	//
	BayesianNetwork* bn = parser.getBayesianNetwork();


	// LEGGO L'ORDINAMENTO
	std::vector<std::string>* ordinamento = new std::vector<std::string>();
	if (argc > 2) {
		
		std::cout << "E' stato specificato un file di ordinamento." << std::endl;

		std::ifstream f2(argv[2]);
	//std::ifstream f2("../RetiEsempio/Barley.num");
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
	//
	//if (argc > 3)
	//	if (strcmp(argv[3], "indexing") == 0)
	//		Config::useIndexingTable = true;

	if (!Config::useIndexingTable) {
		std::cout << "NON ";
	}
	std::cout << "uso le indexing table." << std::endl;
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

	if (Config::useCUDA)
		selectGPU();

	for (std::vector<BayesianNetwork*>::iterator itNet = BNSet->begin(); itNet != BNSet->end(); itNet++) {
		BayesianNetwork* net = *itNet;

		// test delete probability
		//Probability* pppp = (*net->getVariables()->begin())->getConditionalProbability();
		//Probability* copia = pppp->copy();
		//delete pppp;
		//std::cout << copia->toString();

		//std::cin >> sss;
		
		
		//std::cout << "JointBN:\n" << jointBN->toString() << '\n';
		//std::cin >> sss;
		// MORALIZZO
		// DEBUG
		std::cout << "\nMORALIZZAZIONE..." << std::endl;
		//
		net->moralize();
		// DEBUG
		std::cout << "moralizzazione OK." << std::endl;
		//

		// TRIANGOLO E TROVO CRICCHE MASSIMALI
		// DEBUG
		std::cout << "\nTRIANGOLARIZZAZIONE..." << std::endl;
		//
		std::unordered_set<JTClique*>* cli = net->triangolateMaxClique();
		//DEBUG
		std::cout << "triangolarizzazione OK." << std::endl;
		//

		//DEBUG
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
		
		
		//std::cout << jt->getDOT() << '\n';
		//jt->getCliques();
		//std::cout << "nVar BN: " << net->getVariables()->size() << " vs nCli JT: " << jt->getCliques()->size();
		//std::cin >> sss;

		/*std::cout << "maximal cliques:\n";
		for (std::unordered_set<JTClique*>::iterator I = cli->begin(); I != cli->end(); ++I) {
		std::cout << (*I)->toString() << '\n';
		if ((*I)->getPsi() == NULL)
		std::cout << "sta merda ha PSI NULLO!!!\n";
		}
		std::cin >> sss;*/

		// BP!!!
		// DEBUG
		std::cout << "\nBELIEF PROPAGATION..." << std::endl;
		//

		BeliefPropagation::BP(jt);

		std::cout << "belief propagation OK." << std::endl;

		
		// DEBUG voglio vedere un po' ti tabelle delle cricche che mi da junction tree uguale per ogni posizione
		//for (std::unordered_set<JTClique*>::iterator it = jt->getCliques()->begin(); it != jt->getCliques()->end(); it++) {
		//	std::cout << "psi: " << (*it)->getPsi()->toString();
		//	std::cin >> sss;
		//}
		//

		/* se le tabelle sono troppo grandi non posso usarlo :( * /
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
		
		
		std::cin >> sss;
		
		
	}

	if (Config::useCUDA)
		resetGPU();
	
	//std::cin >> sss;
	
}

// NOTA trova l'errore che non compila più :(
//void provaVarPassaggioPuntatoreOValore() 
//{
//	std::string culo = "culo";
//	Variable* vA3;
//	std::vector<std::string>* vec;
//	//std::string a;
//	if (true) {
//		std::string a = "A";
//		//a = "A";
//		std::vector<std::string> statesA;
//		std::string s;
//		for (int i = 0; i < 2; i++) {
//			//s = boost::lexical_cast<std::string>(i);
//			s = "ciao"; // temporaneo...
//			std::cout << "sls : " << s;
//			statesA.push_back(s);
//		}
//
//		//Variable vA (&a, &statesA);
//		//Variable* vA2 = new Variable(&a, &statesA);
//
//		vA3 = new Variable(&a, &statesA);
//
//		//std::vector<int> ve(sizeof(int)*3);
//		vec = new std::vector<std::string>(sizeof(std::string)*20);
//	}
//
//	std::cout << "va3 : " << vA3->toString() << std::endl;
//
//	std::vector<int> statesB;
//	for (int i = 0; i < 2; i++) {
//		//std::string s = boost::lexical_cast<std::string>(i);
//		statesB.push_back(i);
//	}
//
//	//std::vector<std::string> statesC;
//	//for (int i = 0; i < 2; i++) {
//	//	//std::string s = boost::lexical_cast<std::string>(i);
//	//	statesC.push_back("culo");
//	//}
//
//	std::vector<std::string>* sssA = vA3->getStates();
//	for (std::vector<std::string>::iterator I = sssA->begin(), E = sssA->end(); I != E; ++I) {
//		std::cout <<  *I <<"\n";
//	}
//
//	//for (std::vector<std::string>::iterator I = statesC.begin(), E = statesC.end(); I != E; ++I) {
//	//	std::cout << *I << "\t";
//	//}
//
//}
//
//void provaBNset()
//{
//	BayesianNetwork bn;
//
//	//for (int i = 0; i < 3; i++) {
//	//	std::string s;
//	//	std::vector<std::string> v;
//	//	if (i == 0) {
//	//		s = "A";
//	//	} else if (i == 1) {
//	//		s = "B";
//	//	} else {
//	//		s = "C";
//	//	}
//	//	std::cout << "dentro\n";
//
//	//	// mi inserisce un solo elemento!!!
//	//	//Variable var (&s, &v);
//	//	//bn.addNode(&var);
//	//	// USARE NEW VARIABLE!!!
//	//	Variable* var = new Variable(&s, &v);
//	//	bn.addNode(var);
//	//	
//	//	//std::string s= var->toString();
//	//	//std::cout << "variabile : " << s << std::endl;
//	//	
//	//}
//
//	std::string label = "A";
//	std::vector<std::string>* states = new std::vector<std::string>();
//	states->push_back("true");
//	states->push_back("false");
//	Variable* varA = new Variable(&label, states);
//	bn.addVariable(varA);
//
//	label = "B";
//	states = new std::vector<std::string>();
//	states->push_back("culo");
//	states->push_back("ciao");
//	Variable* varB = new Variable(&label, states);
//	bn.addVariable(varB);
//	varB->addNeighborIfNecessary(varA);
//
//
//	//std::set<int>* nodes = bn.getNodesI();
//	std::set<Variable*>* nodes = bn.getVariables();
//
//	for (std::set<Variable*>::iterator I = nodes->begin(), E = nodes->end(); I != E; ++I) {
//		std::cout << "\nel: " <<  (*I)->toString() << "\n\tstati: ";
//		std::vector<std::string>* sssA = (*I)->getStates();
//		for (std::vector<std::string>::iterator II = sssA->begin(), IE = sssA->end(); II != IE; ++II) {
//			std::cout << "." << *II << "\t";
//		}
//		std::cout << "\n\tnei: ";
//		for (std::set<Variable*>::iterator II = (*I)->getNeighbors()->begin(), IE = (*I)->getNeighbors()->end(); II != IE; ++II) {
//			std::cout << "." << (*II)->toString() << "\t";
//		}
//	}
//
//
//	// aggiungiamo le probabilità condizionali
//	std::vector<double>* p = new std::vector<double>();
//	p->push_back(0.1);
//	p->push_back(0.9);
//	ConditionalProbability* cp = new ConditionalProbability(varA, p);
//	varA->setConditionalProbability(cp);
//
//	ConditionalProbability* ppp = varA->getConditionalProbability();
//	std::cout << "\nConditionalProbability:\n" << ppp->toString();
//
//	//BayesianNetwork* bn2 = new BayesianNetwork();
//	bn.addVariable(varA);
//	bn.addVariable(varB);
//
//	bn.triangolateMaxClique();
//
//
//}

//void provaJTPotMUL() 
//{
//	std::string label = "A";
//	std::vector<std::string>* states = new std::vector<std::string>();
//	states->push_back("true");
//	states->push_back("false");
//	Variable* varA = new Variable(&label, states);
//
//
//	label = "B";
//	states = new std::vector<std::string>();
//	states->push_back("culo");
//	states->push_back("ciao");
//	Variable* varB = new Variable(&label, states);
//
//	VecMap* vec1 = new VecMap(2);
//	vec1->push_back(varA);
//	vec1->push_back(varB);
//
//	std::vector<double>* pot1 = new std::vector<double>(4);
//	(*pot1)[0] = 0.3;
//	(*pot1)[1] = 0.4;
//	(*pot1)[2] = 0.1;
//	(*pot1)[3] = 0.2;
//
//	Probability jtp1 (vec1, pot1);
//
//	std::cout << jtp1.toString() << "\n";
//
//	label = "C";
//	states = new std::vector<std::string>();
//	states->push_back("culo");
//	states->push_back("ciao");
//	Variable* varC = new Variable(&label, states);
//	VecMap* vec2 = new VecMap(1);
//	vec2->push_back(varC);
//
//	std::vector<double>* pot2 = new std::vector<double>(2);
//	(*pot2)[0] = 0.3;
//	(*pot2)[1] = 0.7;
//
//	Probability jtp2 (vec2, pot2);
//
//	std::cout << jtp2.toString() << "\n";
//
//	Probability* mul = jtp1.mul(&jtp2);
//
//	std::cout << mul->toString() << "\n";
//}

//void provaJTPotSUMON() 
//{
//	std::string label = "A";
//	std::vector<std::string>* states = new std::vector<std::string>();
//	states->push_back("true");
//	states->push_back("false");
//	Variable* varA = new Variable(&label, states);
//
//
//	label = "B";
//	states = new std::vector<std::string>();
//	states->push_back("culo");
//	states->push_back("ciao");
//	states->push_back("mamma");
//	Variable* varB = new Variable(&label, states);
//
//	label = "C";
//	states = new std::vector<std::string>();
//	states->push_back("culo");
//	states->push_back("ciao");
//	Variable* varC = new Variable(&label, states);
//	std::vector<Variable*>* vec2 = new std::vector<Variable*>(1);
//	(*vec2)[0] = varC;
//
//
//	std::cout << varA->toString() << "\t" << varA->getStates() << "\n";
//	std::cout << varB->toString() << "\t" << varB->getStates() << "\n";
//	std::cout << varC->toString() << "\t" << varC->getStates() << "\n";
//
//	std::vector<Variable*>* vec1 = new std::vector<Variable*>(3);
//	(*vec1)[0] = varA;
//	(*vec1)[1] = varB;
//	(*vec1)[2] = varC;
//
//	std::vector<double>* pot1 = new std::vector<double>(12);
//	(*pot1)[0] = 0.3;
//	(*pot1)[1] = 0.4;
//	(*pot1)[2] = 0.1;
//	(*pot1)[3] = 0.2;
//	(*pot1)[4] = 0.3;
//	(*pot1)[5] = 0.4;
//	(*pot1)[6] = 0.1;
//	(*pot1)[7] = 0.2;
//	(*pot1)[8] = 0.3;
//	(*pot1)[9] = 0.4;
//	(*pot1)[10] = 0.1;
//	(*pot1)[11] = 0.2;
//
//	Probability jtp1 (vec1, pot1);
//
//	//std::cout << jtp1.toString();
//
//	Probability* sumOn = jtp1.sumOn(varB);
//
//	//std::cout << sumOn->toString();
//
//	//std::vector<double>* pot2 = new std::vector<double>(2);
//	//(*pot2)[0] = 0.3;
//	//(*pot2)[1] = 0.7;
//
//	//Probability jtp2 (vec2, pot2);
//
//	//std::cout << jtp2.toString();
//
//	//Probability* mul = jtp1.mul(&jtp2);
//
//	//std::cout << mul->toString();
//}


//void provaPriorityQueueMyComparator() 
//{
//	std::string label = "A";
//	std::vector<std::string>* states = new std::vector<std::string>();
//	states->push_back("true");
//	states->push_back("false");
//	Variable* varA = new Variable(&label, states);
//
//
//	label = "B";
//	states = new std::vector<std::string>();
//	states->push_back("culo");
//	states->push_back("ciao");
//	Variable* varB = new Variable(&label, states);
//	varB->addNeighborIfNecessary(varA);
//
//	VecMap* vec1 = new VecMap(2);
//	vec1->push_back(varA);
//	vec1->push_back(varB);
//
//	label = "C";
//	states = new std::vector<std::string>();
//	states->push_back("culo");
//	states->push_back("ciao");
//	Variable* varC = new Variable(&label, states);
//	VecMap* vec2 = new VecMap(1);
//	vec2->push_back(varC);
//
//	varC->addNeighborIfNecessary(varA);
//	varC->addNeighborIfNecessary(varB);
//
//	//std::priority_queue<Variable*, MyComparator> varsQueue;
//	std::priority_queue<Variable*,std::vector<Variable*>,MyComparator> varsQueue;
//	varsQueue.push(varB);
//	varsQueue.push(varA);
//	varsQueue.push(varC);
//
//	while (!varsQueue.empty()) {
//		Variable* v = varsQueue.top();
//		varsQueue.pop();
//		std::cout << v->toString() << "\n";
//	}
//
//	
//}
