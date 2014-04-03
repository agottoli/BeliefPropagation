#include "JunctionTree.h"
#include <string>
#include <sstream>

/**
 * Implementa i metodi per il junction tree.
 *
 * @author Alessandro Gottoli vr352595
 */

//JunctionTree::JunctionTree(std::unordered_set<JTClique*>* c, std::unordered_set<Separator*>* l)
//{
//	cliques = c;
//	links = l;
//	roots = new std::unordered_set<JTClique*>();
//}

JunctionTree::JunctionTree(std::unordered_set<JTClique*>* c, std::unordered_set<Separator*>* l, std::unordered_set<Variable*>* nodesBN, std::unordered_set<JTClique*>* rootsSet)
{
	cliques = c;
	links = l;
	allVariables = nodesBN;
	roots = rootsSet;
}


JunctionTree::~JunctionTree(void)
{
	//cliques->~unordered_set();
	//links->~unordered_set();
	//root->~JTClique();
}

/*
void JunctionTree::calcolaRootMigliore()
{
	// TODO che così è a prima che capita!!!
	root = *cliques->begin();
}

JTClique* JunctionTree::getRoot() 
{
	//if (root == NULL)
	//	calcolaRootMigliore();
	return root;
}
*/

std::unordered_set<JTClique*>* JunctionTree::getRoots() 
{
	return roots;
}

std::string JunctionTree::getDOT()
{
	std::string s = "digraph {\n\tnodesep=\"1.5\"; ranksep=2;\n\tnode [shape=plaintext];\n\tedge [color=gray];\n";
	//JTClique* r = getRoot();
	for (JTClique* r : *roots) {
		// partendo dalla root per ogni link che ha scrive i nodi collegati a lei
		for (std::unordered_map<JTClique*, Separator*>::iterator I = r->getLinks()->begin(); I != r->getLinks()->end(); ++I) {
			s.append("\t\"").append(r->toString()).append("\" -> \"").append((*I).first->toString()).append("\" [label=\"").append((*I).second->getVars()->toString()).append("\"];\n");
			// e chiama ricorsivamente per fare la stessa cosa su ogni figlio (in questo caso devo stare attento a non riscrivere il noo padre)
			s.append(getDOT2((*I).first, r));
		}
	}
	s.append("}\n");

	return s;
}

std::string JunctionTree::getDOT2(JTClique* cricca, JTClique* padre)
{
	std::string s;
	// per ogni collegamento controllo che il figlio della variabile non sia il padre della variabile (altrimenti non finisce più)
	for (std::unordered_map<JTClique*, Separator*>::iterator I = cricca->getLinks()->begin(); I != cricca->getLinks()->end(); ++I) {
		if ((*I).first != padre) {
			s.append("\t\"").append(cricca->toString()).append("\" -> \"").append((*I).first->toString()).append("\" [label=\"").append((*I).second->getVars()->toString()).append("\"];\n");
			s.append(getDOT2((*I).first, cricca));
		}
	}
	return s;
}

Probability* JunctionTree::calcolaJointProbability()
{
	//// calcolo moltiplicando tra loro tutti i potenziali delle cricche e dei separatori
	//// e poi divido i primi per i secondi
	//std::unordered_set<JTClique*>::iterator clique = cliques->begin(); // ce n'è almeno una per forza
	//ProbabilityMD* jointClique = (*clique)->getPsi();
	//if (++clique == cliques->end())
	//	jointClique = jointClique->copy();
	//else {
	//	jointClique = jointClique->mul((*clique)->getPsi());
	//	ProbabilityMD* temp;
	//	while (++clique != cliques->end()) {
	//		temp = jointClique;
	//		jointClique = jointClique->mul((*clique)->getPsi()); //&Probability(vars, (*variable)->getConditionalProbability()->getProbability()));
	//		// elimino il risultato parziale
	//		//temp->~Probability();
	//		delete temp;

	//	}
	//}

	//if (links->empty()) {
	//	// c'è una sola cricca allora restituisco la probabilità direttamente :)
	//	return jointClique;
	//}
	//// altrimenti c'è almeno un separatore
	//std::unordered_set<Separator*>::iterator separator = links->begin(); // ce n'è almeno uno per forza
	//ProbabilityMD* jointSeparator =  (*separator)->getFi();
	//if (++separator == links->end())
	//	jointSeparator = jointSeparator->copy();
	//else {
	//	jointSeparator = jointSeparator->mul((*separator)->getFi());
	//	ProbabilityMD* temp;
	//	while (++separator != links->end()) {
	//		temp = jointSeparator;
	//		jointSeparator = jointSeparator->mul((*separator)->getFi()); //&Probability(vars, (*variable)->getConditionalProbability()->getProbability()));
	//		// elimino il risultato parziale
	//		//temp->~Probability();
	//		delete temp;
	//	}
	//}

	//ProbabilityMD* joint = jointClique->div(jointSeparator);

	//delete jointClique;
	//delete jointSeparator;

	//if (!joint->isNormalized()) {
	//	//std::cout << "jointP di BN NON è normalizzata!\n";
	//	joint->normalizza();
	//} 
	////else {
	////	std::cout << "jointP di BN è normalizzata!\n";
	////}

	//return joint;

	VecMap* vars = new VecMap(allVariables->size());
	for (std::unordered_set<Variable*>::iterator var = allVariables->begin(); var != allVariables->end(); var++) {
		vars->push_back(*var);
	}

	Probability* jointCricche = new Probability(vars, 1);
	for (std::unordered_set<JTClique*>::iterator clique = cliques->begin(); clique != cliques->end(); clique++) {
		jointCricche->mul((*clique)->getPsi());
	}

	Probability jointSeparatori (vars, 1);
	for (std::unordered_set<Separator*>::iterator sep = links->begin(); sep != links->end(); sep++) {
		jointSeparatori.mul((*sep)->getFi());
	}

	jointCricche->divOrdinato(&jointSeparatori);

	if (!jointCricche->isNormalized())
		jointCricche->normalizza();

	return jointCricche;
}

std::string toString()
{
	std::string s;
	// TODO
	return s;
}

void JunctionTree::getStatistics() 
{
	std::cout << "Numero di cricche: " << cliques->size() << "\n";
	std::size_t minCliqueSize = SIZE_MAX;
	std::size_t maxCliqueSize = 0;
	std::size_t sumCliqueSize = 0;
	for (std::unordered_set<JTClique*>::iterator clique = cliques->begin(); clique != cliques->end(); clique++) {
		if (maxCliqueSize < (*clique)->getPsi()->getTableSize())
			maxCliqueSize = (*clique)->getPsi()->getTableSize();

		if (minCliqueSize > (*clique)->getPsi()->getTableSize())
			minCliqueSize = (*clique)->getPsi()->getTableSize();

		sumCliqueSize += (*clique)->getPsi()->getTableSize();
	}
	std::cout << "Dimensione totale cricche: " << sumCliqueSize << "\n";
	std::cout << "Massima dimensione cricca: " << maxCliqueSize << "\n";
	std::cout << "Media dimensione cricca:   " << sumCliqueSize / cliques->size() << "\n";
	std::cout << "Minima dimensione cricca:  " << minCliqueSize << "\n";

	std::cout << "Numero di separatori: " << links->size() << "\n";
	std::size_t minSepSize = SIZE_MAX;
	std::size_t maxSepSize = 0;
	std::size_t sumSepSize = 0;
	for (std::unordered_set<Separator*>::iterator sep = links->begin(); sep != links->end(); sep++) {
		if (maxSepSize < (*sep)->getFi()->getTableSize())
			maxSepSize = (*sep)->getFi()->getTableSize();

		if (minSepSize > (*sep)->getFi()->getTableSize())
			minSepSize = (*sep)->getFi()->getTableSize();

		sumSepSize += (*sep)->getFi()->getTableSize();
	}
	std::cout << "Massima dimensione separatori: " << maxSepSize << "\n";
	std::cout << "Media dimensione separatori:   " << sumSepSize / links->size() << "\n";
	std::cout << "Minima dimensione separatori:  " << minSepSize << "\n";

}


bool JunctionTree::checkRunningIntersectionProperty()
{
	std::string debug_print = "";
	bool result = true;
	JTClique** cliques_array = new JTClique*[cliques->size()];
	std::size_t index = 0;
	for (std::unordered_set<JTClique*>::iterator clique_it = cliques->begin(); clique_it != cliques->end(); clique_it++) {
		cliques_array[index++] = *clique_it;
	}

	// per ogni coppia di nodi
	// devo trovare il path e controllare che le variabili che hanno in comune
	// siano presenti in ogni separatore del path
	for (std::size_t i = 0; i < cliques->size(); i++) {
		for (std::size_t j = i + 1; j < cliques->size(); j++) {

			debug_print.append("\n\nAnalizzo le cricche: ").append(cliques_array[i]->toString()).append(" e ").append(cliques_array[j]->toString()).append(":\n");

			// calcolo le variabili condivise tra le cricche
			std::unordered_set<Variable*>* i_vars = cliques_array[i]->getVars();
			std::unordered_set<Variable*>* j_vars = cliques_array[j]->getVars();

			std::unordered_set<Variable*>* common_vars = new std::unordered_set<Variable*>();
			
			for (std::unordered_set<Variable*>::iterator i_var_it = i_vars->begin(); i_var_it != i_vars->end(); i_var_it++) {
				// controllo quali variabili della cricca i sono in comune con la cricca j
				if (j_vars->find(*i_var_it) != j_vars->end()) {
					// la variabile è in comune
					common_vars->insert(*i_var_it);
				}
			}

			std::stringstream sizeSS;
			sizeSS << common_vars->size();
			debug_print.append("Ci sono ").append(sizeSS.str()).append(" variabili in comune.\n");

			// bene, adesso ho l'insieme delle variabili condivise
			// se ce ne sono calcolo i path che devono esserci per forza, poi controllo la running intersection property
			

			if (common_vars->size() > 0) {

				debug_print.append("Le variabili condivise sono: \n");
				for (std::unordered_set<Variable*>::iterator common_var_it = common_vars->begin(); common_var_it != common_vars->end(); common_var_it++) {
					debug_print.append((*common_var_it)->toString()).append("\n");
				}


				std::vector<Separator*>* path = findPath(cliques_array[i], cliques_array[j], cliques->size());
				//std::cout << "Path tra " << cliques_array[i]->toString() << " e " << cliques_array[j]->toString() << ":" << std::endl;
				debug_print.append("il path è composto da ");
				int nEl = 0;
				for (std::size_t i = 0; i < path->size(); i++) {
					//std::cout << path->at(i)->toString() << std::endl;
					nEl++;
				}
				sizeSS.clear();
				sizeSS << nEl;
				debug_print.append(sizeSS.str()).append(" hop.\n");

				// bene, qua ho il path dei separatori che vanno da un nodo all'altro
				
				// mi basta controllare che le variabili in comune tra i nodi
				// siano tutti presenti nei vari separatori
			
				for (std::vector<Separator*>::iterator sep_it = path->begin(); sep_it != path->end(); sep_it++) {
					// per ogni separatore...
					debug_print.append("analizzo separatore ").append((*sep_it)->toString()).append(":\n");
					for (std::unordered_set<Variable*>::iterator var_it = common_vars->begin(); var_it != common_vars->end(); var_it++) {
						// controllo che ogni variabile in comune sia presente anche nei separatori del path
						if (!(*sep_it)->getVars()->exists(*var_it)) {
							// la variabile NON esiste quindi è cannato
							result = false;
							debug_print.append((*var_it)->toString()).append(" MANCANTE\n");
							std::cout << "Running intersection property non soddisfatta!!!\n";
							//std::string ss;
							//std::cin >> ss;
						} else {
							// la variabile è presente
							debug_print.append((*var_it)->toString()).append(" PRESENTE\n");
						}
					}
				}


				delete(path);
			}

			delete(common_vars);
		}
	}

	//std::cout << debug_print;

	return result;
}

bool JunctionTree::findPathSup(JTClique* father, JTClique* node, JTClique* dst, std::vector<Separator*>* path) {

	for (std::unordered_map<JTClique*, Separator*>::iterator links_it = node->getLinks()->begin(); links_it != node->getLinks()->end(); links_it++) {
		// se ho il link che mi porta indietro lo salto
		if (father != (*links_it).first) {
			path->push_back((*links_it).second);

			if ((*links_it).first == dst) {
				// ho trovato il path completo
				return true;
			}

			if (findPathSup(node, (*links_it).first, dst, path))
				return true;

			path->pop_back(); // così cancello la scelta visto che non va a buon fine
		}
	}

	return false;
}

std::vector<Separator*>* JunctionTree::findPath(JTClique* src, JTClique* dst, std::size_t nCliques)
{
	std::vector<Separator*>* path = new std::vector<Separator*>();
	// il primo approccio che mi viene in mente è il backtracking
	for (std::unordered_map<JTClique*, Separator*>::iterator links_it = src->getLinks()->begin(); links_it != src->getLinks()->end(); links_it++) {
		path->push_back((*links_it).second);

		if ((*links_it).first == dst) {
			// ho trovato il path completo
			return path;
		}

		if (findPathSup(src, (*links_it).first, dst, path))
			return path;

		path->pop_back(); // così cancello la scelta visto che non va a buon fine
	}

	// se arrivo qua significa che non c'è un path, quindi non deve accadere
	return path; // restituisco lo stesso path ma sarà vuoto!!! controllo con size = 0

}
bool JunctionTree::isATreeWithDFSSupport(JTClique* padre, JTClique* node) {
	node->setVisited();

	bool result = true;
	for (std::unordered_map<JTClique*, Separator*>::iterator links_it = node->getLinks()->begin(); links_it != node->getLinks()->end(); links_it++) {
		if ((*links_it).first != padre) {
			if ((*links_it).first->isVisited()) {
				//result = false;
				return false;
			}

			if (!isATreeWithDFSSupport(node, (*links_it).first)) {
				//result = false;
				return false;
			}
		}
	}
	return result;
}

bool JunctionTree::isATreeWithDFS() {
	bool result = true;
	
	// metto tutti i nodi come non visitati
	for (JTClique* node : *cliques) {
		node->setNotVisited();
	}

	for (JTClique* r : *roots) {
		r->setVisited();
		for (std::unordered_map<JTClique*, Separator*>::iterator links_it = r->getLinks()->begin(); links_it != r->getLinks()->end(); links_it++) {
			if ((*links_it).first->isVisited()) {
				//result = false;
				return false;
			}

			if (!isATreeWithDFSSupport(r, (*links_it).first)) {
				//result = false;
				return false;
			}
		}
	}

	return result;
}

/*
void JunctionTree::aggiungiUnArcoACaso() {
	int i = 0;

	JTClique* c1;
	JTClique* c2;
	for (std::unordered_set<JTClique*>::iterator it = cliques->begin(); it != cliques->end(); it++, i++) {
		if (i == 20)
			c1 = *it;
		
		if (i == 29)
			c2 = *it;
	}

	Separator* sep = new Separator(c1, c2, new VecMap(0));

	// devo aggiungerlo a links
	links->insert(sep);
	// e chiamare sul separatore il metodo sceltoPerMST
	sep->sceltoPerMST();
}
*/

