#include <iostream>
#include <queue>
//#include <list>
#include "BayesianNetwork.h"
#include "Probability.h"
//#include "ProbabilityMD.h"
#include "Separator.h"
#include "States.h"
#include <math.h> // per il ceil
#include "Config.h"


BayesianNetwork::BayesianNetwork(void)
{
	numeroElementiDelleTabelleProbability = 0;
	numeroCricche = 0;
	numeroCriccheMassimali = 0;
	ordinamento = NULL;
}


BayesianNetwork::~BayesianNetwork(void)
{
}

void BayesianNetwork::addVariable(Variable* v)
{
	nodes.insert(v);

	// nuovo 2014-01-24
	//if (ordinamento != NULL && !ordinamento->empty()) // non posso perché l'ordinamento lo decido dopo :(
	findNodes[v->getLabel()] = v;
}

std::unordered_set<Variable*>* BayesianNetwork::getVariables(void)
{
	return &nodes;
}

// FASE 1
void BayesianNetwork::moralize()
{
	// per ogni nodo della rete prendo i padri...
	for (std::unordered_set<Variable*>::iterator node = nodes.begin(); node != nodes.end(); ++node) {
		std::unordered_set<Variable*>* fathers = (*node)->getFathers();
		// ... e li collego tra loro
		for (std::unordered_set<Variable*>::iterator father1 = fathers->begin(); father1 != fathers->end(); ++father1) {
			for (std::unordered_set<Variable*>::iterator father2 = fathers->begin(); father2 != fathers->end(); ++father2) {
				if (father1 != father2) {
					(*father1)->addNeighborIfNecessary(*father2);
				}
			}
		}
	}
}

// FASE 2
std::unordered_set<JTClique*>* BayesianNetwork::triangolateMaxClique()
{
	// siccome nella fase di triangolizzazione, l'ordine con cui scelgo le variabili mi permette di calcolare le cricche massimali
	// mi creo un insieme in cui inserire le cricche massimali che comporranno il junction tree
	//std::unordered_set<JTClique*>* maximalCliqueSet = new std::unordered_set<JTClique*>();
	//maximalCliqueSet->reserve(...); // TODO bisognerebbe scegliere una stima di quante cricche ci sono dati tot nodi e archi...

	//std::set<Variable*> varsCopy = nodes; // dovrebbe copiare se non sbaglio Sì è così :)


	std::unordered_set<JTClique*>* maximalCliqueSet = new std::unordered_set<JTClique*>();

	// insieme dei nodi già elaborati, serve per non inserirli nelle cricche successive
	std::unordered_set<Variable*> done;
	done.reserve(nodes.size());

	Variable* v;


	// coda per la selezione dell'ordine delle variabili da elaborare
	// NOTA: ne userò solo uno dei seguenti, dipende se è stato impostato in input un ordinamento
	//std::priority_queue<Variable*, std::vector<Variable*>, MyComparator> varsQueue;
	std::unordered_set<Variable*> copiaVars;

	if (ordinamento != NULL && !ordinamento->empty()) {
		if (ordinamento->size() != nodes.size()) {
			std::cout << "l'ordinamento non specifica tutte le variabili!" << std::endl;
			std::string sss;
			std::cin >> sss;
		}

		for (int i = 0; i < nodes.size(); i++) {
			std::unordered_map<std::string, Variable*>::iterator iterat = findNodes.find(ordinamento->at(i));
			if (iterat != findNodes.end()) {
				v = iterat->second; //findNodes.find(ordinamento->at(i))->second;
			
				// DEBUG
				//std::cout << "estratto elemento: " << v->toString() << " con indice imposto: " << i << std::endl; //v->getIndiceInOrdinamentoImposto() << '\n';
				//

				triangolarizzaSupport(v, &done, maximalCliqueSet);
			} else {
				// dico che non ho trovato la variabile

				std::cout << "durante l'estrazione della variabile non ho trovato la variabile " << ordinamento->at(i) << std::endl;

				std::cout << "errore a indice " << i << " di " << ordinamento->size() << std::endl;
				if (i == ordinamento->size() - 1)
					std::cout << "TUTTO REGOLARE, c'è solo un \\n alla fine." << std::endl;
				else {
					std::cout << "PROBLEMA..." << std::endl;
					std::string s;
					std::cin >> s;
				}
			}
		}


	} else {
	//if (ordinamento == NULL || ordinamento->empty()) {

		// ORDINAMENTO DINAMICO
		// devo fare un set copia dell'insieme delle variabili
		copiaVars = nodes; // TODO è una copia???
		//
		while (!copiaVars.empty()) {
			// uso il numero di vicini non già selezionati
			v = min(&copiaVars, &done);
			//std::cout << "estratto elemento: " << v->toString() << " con numero di vicini non selezionati: " << v->getNumNeighboursNotSelected() << '\n';
			copiaVars.erase(v); // constant time

			triangolarizzaSupport(v, &done, maximalCliqueSet);
		}
	}

	std::cout << "fine creazione cricche." << std::endl;
	//std::string sss;
	//std::cin >> sss;

	// le cricche massimali sono state create quindi bisogna assegnare la probabilità condizionale
	// di ogni variabile ad una "sola" cricca in modo da calcolare i potenziali
	// NOTA si assegna alla cricca che contiene tutte le variabili che compaiono nella tabella della probabilità condizionale
	//TABELLE
	std::cout << "Costruisco le tabelle dei potenziali delle varie cricche trovate...";
	initPotentialsToMaximalCliques();
	std::cout << " OK." << std::endl;

	return maximalCliqueSet;
}

void BayesianNetwork::triangolarizzaSupport(Variable* v, std::unordered_set<Variable*>* done, std::unordered_set<JTClique*>* maximalCliqueSet)
{
		// creo l'insieme di variabili che varanno parte della cricca della variabile estratta
		std::unordered_set<Variable*>* nodiCricca = new std::unordered_set<Variable*>();
		nodiCricca->reserve(v->getNumNeighboursNotSelected()); // TODO dare una dimensione alla cricca potrebbe essere in numero di vicini di un nodo (alcuni saranno stati cancellati ma chissene, oppure il parametro per comparare!!!)

		nodiCricca->insert(v);

		std::vector<Variable*> varsToLink;

		// elaboro tutti i vicini della variabile per aggiungerli nella cricca
		for (std::unordered_set<Variable*>::iterator neighbor = v->getNeighbors()->begin(); neighbor != v->getNeighbors()->end(); ++neighbor) {

			// se un nodo non è stato già elaborato lo aggiungo alla nuova cricca
			if (done->find(*neighbor) == done->end()) {
				nodiCricca->insert(*neighbor);
				// siccome devo collegare tra loro tutti i vicini per creare effettivamente la cricca
				varsToLink.push_back(*neighbor);
			}

			(*neighbor)->decrementNumNeighboursNotSelected(); // non si può modificare dinamicamente :( // 2014-01-08 ho implementato la finziona che cerca il minimo quindi lo posso reinserire :)
		}

		// collego effettivamente tra loro i vicini
		for (unsigned i = 0; i < varsToLink.size(); i++) {
			for (unsigned j = i + 1; j < varsToLink.size(); j++) {
				varsToLink.at(i)->addNeighborIfNecessary(varsToLink.at(j));
				varsToLink.at(j)->addNeighborIfNecessary(varsToLink.at(i));
			}
		}

		// creo la nuova cricca se vedo che è massimale

		// TODO commenta in caso di test su efficienza ordinamento
		createCliqueAndAddToSetIfMaximal(maximalCliqueSet, nodiCricca);

		// aggiungo il nodo appena elaborato all'insieme dei nodi fatti per indicare che lo elimino
		done->insert(v);
		
}


Variable* BayesianNetwork::min(std::unordered_set<Variable*>* cliques, std::unordered_set<Variable*>* done) {
	std::unordered_set<Variable*>::iterator it = cliques->begin();

	bool metodoA = true; //false;
	if (metodoA) {

	//std::vector<Variable*>* mins = new std::vector<Variable*>;
	//mins->reserve(ceil(cliques->size() / 2));
	std::size_t dim = cliques->size() / 2 + cliques->size() % 2;
	Variable **mins = new Variable*[dim];
	Variable *a, *b;
	std::size_t j = 0;
	for (std::size_t i = 1; i < cliques->size(); i += 2, j++) {
		a = *(it); // TODO controlla che voglio l'elemento puntato dal puntatore prima dell'incremento
		it++;
		b = *(it);
		it++;
		if (a->getNumNeighboursNotSelected() <= b->getNumNeighboursNotSelected()) {
			//mins->push_back(a);
			mins[j] = a;
		} else {
			//mins->push_back(b);
			mins[j] = b;
		}
	}
	if (cliques->size() % 2 != 0) {
		//mins->push_back(*it);
		mins[j] = *it;
	}

	std::size_t i = 0;
	//std::size_t neiI, neiJ;


	while (dim > 1) {
		i = dim % 2;
		j = i + dim / 2;

		// prova unrolling
		//for (; j + 3 < dim; i = i + 4, j = j + 4) {
		//	if (mins[i]->getNumNeighboursNotSelected() > mins[j]->getNumNeighboursNotSelected()) {
		//		mins[i] = mins[j];
		//	}
		//	if (mins[i+1]->getNumNeighboursNotSelected() > mins[j+1]->getNumNeighboursNotSelected()) {
		//		mins[i+1] = mins[j+1];
		//	}
		//	if (mins[i+2]->getNumNeighboursNotSelected() > mins[j+2]->getNumNeighboursNotSelected()) {
		//		mins[i+2] = mins[j+2];
		//	}
		//	if (mins[i+3]->getNumNeighboursNotSelected() > mins[j+3]->getNumNeighboursNotSelected()) {
		//		mins[i+3] = mins[j+3];
		//	}
		//}

		for (; j < dim; i++, j++) {

			if (mins[i]->getNumNeighboursNotSelected() > mins[j]->getNumNeighboursNotSelected()) {
				mins[i] = mins[j];
			}
		}

		dim = dim / 2 + dim % 2;

	}
	a = mins[0];

	// ...così cancello il vector
	delete mins;

	//if (a == b) {
	//	std::cout << "a = b" << std::endl;
	//	std::string sss;
	//	std::cin >> sss;
	//}

	return a;

	}
	else {
		// prendo il primo e scorro tutto
		Variable* min = *it;
		it++;
		while (it != cliques->end()) {
			if (min->getNumNeighboursNotSelected() > (*it)->getNumNeighboursNotSelected()) {
				min = *it;
			}
			it++;
		}
		return min;
	}
}
	
//Variable* BayesianNetwork::minSupport(Variable** minsPartenza, std::size_t dimMP) {
//	//std::size_t dimMP = minsPartenza->size();
//	if (dimMP > 1) {
//		//std::vector<Variable*>* mins = new std::vector<Variable*>;
//		//mins->reserve(ceil(dimMP / 2.0));
//		std::size_t dim = ceil(dimMP / 2.0);
//		Variable** mins = new Variable*[dim];
//		Variable *a, *b;
//		std::size_t j = 0;
//		for (std::size_t i = 1; i < dimMP; i += 2, j++) {
//			a = minsPartenza[i - 1];
//			b = minsPartenza[i];
//			if (a->getNumNeighboursNotSelected() <= b->getNumNeighboursNotSelected())
//				mins[j] = a;
//			else
//				mins[j] = b;
//		}
//		if (dimMP % 2 != 0)
//			mins[j] = minsPartenza[dimMP - 1];
//	
//		// salvo il risultato in a...
//		a = minSupport(mins, dim);
//		// ...così cancello il vector
//		delete mins;
//
//		return a;
//	}
//
//	return minsPartenza[0];
//}

//Variable* BayesianNetwork::minSupport(std::vector<Variable*>* minsPartenza) {
//	std::size_t dimMP = minsPartenza->size();
//	if (dimMP > 1) {
//		std::vector<Variable*>* mins = new std::vector<Variable*>;
//		mins->reserve(ceil(dimMP / 2.0));
//		Variable *a, *b;
//		for (std::size_t i = 1, j = 0; i < dimMP; i += 2, j++) {
//			a = minsPartenza->at(i - 1);
//			b = minsPartenza->at(i);
//			if (a->getNumNeighboursNotSelected() <= b->getNumNeighboursNotSelected())
//				mins->push_back(a);
//			else
//				mins->push_back(b);
//		}
//		if (dimMP % 2 != 0)
//			mins->push_back(minsPartenza->at(dimMP - 1));
//	
//		// salvo il risultato in a...
//		a = minSupport(mins);
//		// ...così cancello il vector
//		delete mins;
//
//		return a;
//	}
//
//	return minsPartenza->at(0);
//}

void BayesianNetwork::createCliqueAndAddToSetIfMaximal(std::unordered_set<JTClique*>* maximalCliqueSet, std::unordered_set<Variable*>* nodiCricca)
{
	// DEBUG
	numeroCricche++;
	//
	
	if (isMaximalCliqueInSetOfMaximalClique(maximalCliqueSet, nodiCricca)) {
		// DEBUG
		//std::cout << "creo una nuova cricca massimale, la numero " << numeroCriccheMassimali << std::endl;
		numeroCriccheMassimali++;
		//
		
		JTClique* c = new JTClique(nodiCricca);

		// metto un potenziale con vars e table vuote alla cricca appena creata così evito le paranoie del == NULL
		// aggiunta 2013-12-19 ma setto lo stesso le variabili di vecmap
		VecMap* vm = new VecMap(nodiCricca->size());
		//unsigned int dimTabella = 1;
		for (std::unordered_set<Variable*>::iterator I = nodiCricca->begin(); I != nodiCricca->end(); ++I) { // TODO INCORPORARE NEL FOR SOTTO :)
			vm->push_back(*I);
		}

//TABELLE			
//std::cout << "la tabella ha variabili: " << vm->toString() << std::endl;
std::size_t elementiInTabella = vm->numeroElementiDellaRelativaTabella();
//std::size_t kbytes = elementiInTabella * sizeof(double) / 1000;
//std::cout << "devo creare una PSI di dimensione: " << elementiInTabella << " elementi circa " << kbytes << "kbyte." << std::endl;
numeroElementiDelleTabelleProbability += elementiInTabella;


//if (!Config::tabellaScazza)
	c->setPsi(new Probability(vm, 1));

		// specifico alla variabile che fa parte della cricca, mi serve per analizzare solo le cricche a cui appartiene
		// quando devo assegnare la sua probabilità ad una cricca
		for (std::unordered_set<Variable*>::iterator I = nodiCricca->begin(); I != nodiCricca->end(); ++I) {
				(*I)->addClique(c);
		}

		//std::cout << "cricca appena creata: " << c->toString() << '\n';

		// la inserisco nell'insieme delle cricche massimali
		maximalCliqueSet->insert(c);
	}
}

bool BayesianNetwork::isMaximalCliqueInSetOfMaximalClique(std::unordered_set<JTClique*>* maximalCliqueSet, std::unordered_set<Variable*>* nodiCricca)
{
	for (std::unordered_set<JTClique*>::iterator I = maximalCliqueSet->begin(); I != maximalCliqueSet->end(); ++I) {
		if ((*I)->containsAll(nodiCricca)) {
			return false; // è sottoinsieme
		}
	}
	return true; // non è sottoinsieme
}


void BayesianNetwork::initPotentialsToMaximalCliques()
{
	// DEBUG
	//std::cout << "initPotentialsToMaximalCliques start" << std::endl;
	//std::size_t varNum = 0;
	//
	
	// prendo la probabilità condizionale di ogni variabile...
	for (std::unordered_set<Variable*>::iterator I = nodes.begin(); I != nodes.end(); ++I) {
		Probability* cp = (*I)->getConditionalProbability();
		
		// DEBUG
		//std::cout << "elaboro la prob condizionale della variabile " << varNum++ << ": " << (*I)->toString() << std::endl;
		//std::cout << "prob: " << cp->toString();
		//std::string sss;
		//std::cin >> sss;
		//

		// ... e controllo tra tutte le cricche a cui appartiene la variabile...
		std::unordered_set<JTClique*>* cliques =  (*I)->getCliques();
		for (std::unordered_set<JTClique*>::iterator IJTC = cliques->begin(); IJTC != cliques->end(); ++IJTC) {
			// ... qual è quella che contiene tutte le variabili (padre) = quelle che compaiono nella probabilità condizionale della variabile
			if ((*IJTC)->containsAll(cp->getVariables())) {
				//vars->push_back(*I); // ultimo elemento (se copio direttamente le variabili c'è anche l'ultima)

				// DEBUG
				//std::cout << "la CP di " << (*I)->getLabel() << " la assegno alla cricca " << (*IJTC)->toString() << std::endl;
				//
			
				// evidenza TODO
				//if ((*I)->getEvidence() != SIZE_MAX) {
				//	//std::cout << "faccio slice su " << (*I)->toString() <<":\n";

				//	//std::cout << cp->toString() << '\n';

				//	ProbabilityMD* temp = cp->sliceOn(*I); //, (*I)->getEvidence());

				//	//std::cout << temp->toString() << '\n';

				//	if (!temp->isNormalized())
				//		temp->normalizza();

				//	//std::cout << temp->toString() << '\n';

				//	//(*IJTC)->initPotenziale(temp);
				//	ProbabilityMD* psi = (*IJTC)->getPsi();
				//	if (psi == NULL) {
				//		// non era stato inizializzato il psi quindi gli assegno direttamente il temp
				//		(*IJTC)->setPsi(temp);
				//	} else {
				//		// altrimenti faccio la moltiplicazione e cancello i parziali
				//		(*IJTC)->setPsi(psi->mul(temp));
				//		delete psi;
				//		delete temp;
				//	}

				//	// TODO dovrei uccidere la temp
				//	//temp...muori();
				//} else {
				//// fine TODO
				//	//(*IJTC)->initPotenziale(cp);
				//	ProbabilityMD* psi = (*IJTC)->getPsi();
				//	if (psi == NULL) {
				//		// non era stato inizializzato il psi quindi gli assegno direttamente il temp
				//		(*IJTC)->setPsi(cp->copy());
				//	} else {
				//		// altrimenti faccio la moltiplicazione e cancello i parziali
				//		(*IJTC)->setPsi(psi->mul(cp));
				//		delete psi;
				//	}

				// 
				//}
				//
				(*IJTC)->getPsi()->mul(cp);

				break;
			}
		}
	}
}


/*
	 *  FASE 3: implementare il MST
	 *          calcolo tutti i separatori tra le cricche che uso come peso per gli archi
	 *          applico kruskal per trovare il maximum spanning tree (nota: kruskal usa DisjointSet implementato in Utils
	 */
JunctionTree* BayesianNetwork::createJunctionTreeMST(std::unordered_set<JTClique*>* cliques) {
	std::priority_queue<Separator*, std::vector<Separator*>, MyComparatorSeparator>* coda = new std::priority_queue<Separator*, std::vector<Separator*>, MyComparatorSeparator>(); // TODO
	// TODO c'è un metodo per riservare lo spazio per tutte le cricche senza rischiare di dover riallocare mille volte visto che so già la dimensione??? 
	
	VecMap* separator;

	// calcolo i vari separatori tra le varie cricche
	std::cout << "Calcolo tutti i separatori tra le cricche...";
	// quindi per ogni cricca...
	for (std::unordered_set<JTClique*>::iterator cricca = cliques->begin(); cricca != cliques->end(); ++cricca) {
		// ... controllo per le cricche successive...
		for (std::unordered_set<JTClique*>::iterator criccaSucc = cricca; ++criccaSucc != cliques->end(); ) {
			
			// ottimizzazione itero sulle variabili della cricca più piccola
			// ... le variabili in comune per inserirle nelle variabili del separatore
			if ((*cricca)->getVars()->size() < (*criccaSucc)->getVars()->size()) {
				separator = new VecMap((*cricca)->getVars()->size());
				for (std::unordered_set<Variable*>::iterator III = (*cricca)->getVars()->begin(); III != (*cricca)->getVars()->end(); ++III) {
					if ((*criccaSucc)->getVars()->find(*III) != (*criccaSucc)->getVars()->end()) {
						separator->push_back(*III);
					}
				}
			} else {
				separator = new VecMap((*criccaSucc)->getVars()->size());
				for (std::unordered_set<Variable*>::iterator III = (*criccaSucc)->getVars()->begin(); III != (*criccaSucc)->getVars()->end(); ++III) {
					if ((*cricca)->getVars()->find(*III) != (*cricca)->getVars()->end()) {
						separator->push_back(*III);
					}
				}
			}

			// se le variabili del separatore non sono vuote allora creo e aggiungo il separatore (insieme di link)
			if (separator->size() != 0) {
				Separator* s = new Separator(*cricca, *criccaSucc, separator);
				coda->push(s);
				// ai separatori setto il Fi con una probabilità con vars e table vuote per evitare le paranoie del == NULL

// E' INUTILE CHE COSTRUISCO PER TUTTI I SEPARATORI ANCHE QUELLI CHE NON SCEGLIERO'!!! 
////TABELLE
//if (!Config::tabellaScazza) {
//
//	std::cout << "costruisco il fi!!!" << std::endl;
//	s->setFi(new Probability(separator, 1));
//	
//	if (Config::useIndexingTable)
//		s->createIndexingTable();
//	//else {
//	//	double* tttSogg = new double[s->getSoggetto()->getPsi()->getTableSize()];
//	//	double* tttOgg = new double[s->getOggetto()->getPsi()->getTableSize()];
//	//}
//}
//std::size_t elementiInTabella = separator->memoriaOccupataDallaRElativaTabella();
////std::cout << "devo creare una FI di dimensione: " << elementiInTabella << std::endl;
//numeroElementiDelleTabelleProbability += elementiInTabella;
////

			}
		}
	}

	/*
	// ADESSO DEVO TROVARE IL MST per linkare le cricche
	// uso Kruskal
	std::unordered_set<Separator*>* links = kruskal(coda, cliques); 

	// adesso ho tutte le cricche e i separatori scelti da kruskal, quindi posso creare il junction tree
	//return new JunctionTree(cliques, links);
	return new JunctionTree(cliques, links, &nodes);
	*/

	std::cout << " OK." << std::endl;

	return kruskal(coda, cliques);
}


//std::unordered_set<Separator*>* BayesianNetwork::kruskal(std::priority_queue<Separator*, std::vector<Separator*>, MyComparatorSeparator>* coda, std::unordered_set<JTClique*>* maximalCliques)
JunctionTree* BayesianNetwork::kruskal(std::priority_queue<Separator*, std::vector<Separator*>, MyComparatorSeparator>* coda, std::unordered_set<JTClique*>* maximalCliques)
{

	std::cout << "Kruskal (scelgo i separatori e creo le tabelle comprese le indexing se ho scelto di utilizzarle)...";

	// insieme di tutti i separatori scelti nell'albero
	std::unordered_set<Separator*>* scelti = new std::unordered_set<Separator*>(); // TODO
	// TODO c'è un metodo per riservare uno spazio al set??? so che sono (nCliques - 1)

	// creo una struttura per rappresentare gli insiemi disgiunti
	DisjointSet ds (maximalCliques);

	Separator* elemento;
	JTClique* findSetSogg; 
	JTClique* findSetOgg;
	// estraggo dalla coda i separatori (= link) in ordine per creare l'albero... 
	while (!coda->empty()) {
		elemento = coda->top();
		coda->pop();

		//std::cout << "estratto separatore: " << elemento->toString() << '\n';

		// ... e controllo se collegano già elementi nello stesso insieme disgiunto (scarto) o di insiemi diversi (scelgo)
		if ((findSetSogg = ds.findSet(elemento->getSoggetto())) != (findSetOgg = ds.findSet(elemento->getOggetto()))) {
			ds.unione(findSetSogg, findSetOgg);
			elemento->sceltoPerMST();
			scelti->insert(elemento);

			// QUA COSTRUISCO SOLO PER QUELLI SCELTI
			//TABELLE
//if (!Config::tabellaScazza) {

	// DEBUG
	//std::cout << "costruisco il fi!!!" << std::endl;
	//

	elemento->setFi(new Probability(elemento->getVars(), 1));
	
	if (Config::useIndexingTable) {
		if (!Config::useCUDA)
			elemento->createIndexingTable();
		else
			elemento->createIndexingTableCUDA();
	}
	//else {
	//	double* tttSogg = new double[s->getSoggetto()->getPsi()->getTableSize()];
	//	double* tttOgg = new double[s->getOggetto()->getPsi()->getTableSize()];
	//}
}
		// DEBUG
//std::size_t elementiInTabella = elemento->getVars()->numeroElementiDellaRelativaTabella();
//numeroElementiDelleTabelleProbability += elementiInTabella;
//
		}
//	}

	/*
	return scelti;
	*/
	// trovo quali sono i rappresentanti che fanno da root agli alberi (se ce ne sono di più)
	//ds.getRootsDijoint();

	std::cout << " OK." << std::endl;

	return new JunctionTree(maximalCliques, scelti, &nodes, ds.getRootsDisjoint());
}


Probability* BayesianNetwork::calcolaJointProbability()
{
	// calcolo moltiplicando tra loro tutte le probabilità condizionate della rete bayesiana iniziale
	
	// creo una tabella che conterrà la probabilità congiunta
	VecMap* tutte = new VecMap(nodes.size());
	for (std::unordered_set<Variable*>::iterator var = nodes.begin(); var != nodes.end(); var++) {
		tutte->push_back(*var);
	}
	Probability* joint = new Probability(tutte, 1);


	// evidenza
	//if ((*variable)->isEvidence())
	//	joint = (*variable)->getConditionalProbability()->sliceOn(*variable);
	//else
	//	joint = (*variable)->getConditionalProbability()->copy();
	// fine copia probabilità
	
	// per ogni variabile moltiplico tutte le probabilità condizionali
	for (std::unordered_set<Variable*>::iterator variable = nodes.begin(); variable != nodes.end(); variable++) {

		// TODO
		//temp = joint;
		//
		//if ((*variable)->isEvidence()) {
		//	evidTemp = (*variable)->getConditionalProbability()->sliceOn(*variable);

		//	joint = joint->mul(evidTemp);

		//	// TODO dovrei uccidere evidTemp;
		//} else {
		// fine TODO
		//	joint = joint->mul((*variable)->getConditionalProbability()); //&Probability(vars, (*variable)->getConditionalProbability()->getProbability()));
		//}
		//delete temp;
		// fine TODO
		
		joint->mul((*variable)->getConditionalProbability());

	}

	// alla fine normalizzo che se c'è un evidenza è necessario
	if (!joint->isNormalized()) {
		//std::cout << "jointP di BN NON è normalizzata!\n";
		joint->normalizza();
	} 
	//else {
	//	std::cout << "jointP di BN è normalizzata!\n";
	//}

	return joint;

}


std::string BayesianNetwork::getDOT()
{
	std::string s = "digraph {\n\tnodesep=\"1.5\"; ranksep=2;\n\tnode [shape=plaintext];\n\tedge [color=gray];\n";
	// stampo ogni variabile con il collegamento ai suoi figli
	for (std::unordered_set<Variable*>::iterator var = nodes.begin(); var != nodes.end(); ++var) {
		for (std::unordered_set<Variable*>::iterator son = (*var)->getSons()->begin(); son != (*var)->getSons()->end(); ++son) {
			s.append("\t\"").append((*var)->getLabel()).append("\" -> \"").append((*son)->getLabel()).append("\";\n");
		}
	}
	s.append("}\n");

	return s;

}
