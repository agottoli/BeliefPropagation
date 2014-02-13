#include "JunctionTree.h"


JunctionTree::JunctionTree(std::unordered_set<JTClique*>* c, std::unordered_set<Separator*>* l)
{
	cliques = c;
	links = l;
	root = NULL;
}

JunctionTree::JunctionTree(std::unordered_set<JTClique*>* c, std::unordered_set<Separator*>* l, std::unordered_set<Variable*>* nodesBN)
{
	cliques = c;
	links = l;
	allVariables = nodesBN;
	root = NULL;
}


JunctionTree::~JunctionTree(void)
{
	//cliques->~unordered_set();
	//links->~unordered_set();
	//root->~JTClique();
}

void JunctionTree::calcolaRootMigliore()
{
	// TODO che cos� � a prima che capita!!!
	root = *cliques->begin();
}

JTClique* JunctionTree::getRoot() 
{
	//if (root == NULL)
	//	calcolaRootMigliore();
	return root;
}

std::string JunctionTree::getDOT()
{
	std::string s = "digraph {\n\tnodesep=\"1.5\"; ranksep=2;\n\tnode [shape=plaintext];\n\tedge [color=gray];\n";
	JTClique* r = getRoot();
	// partendo dalla root per ogni link che ha scrive i nodi collegati a lei
	for (std::unordered_map<JTClique*, Separator*>::iterator I = r->getLinks()->begin(); I != r->getLinks()->end(); ++I) {
		s.append("\t\"").append(r->toString()).append("\" -> \"").append((*I).first->toString()).append("\";\n");
		// e chiama ricorsivamente per fare la stessa cosa su ogni figlio (in questo caso devo stare attento a non riscrivere il noo padre)
		s.append(getDOT2((*I).first, r));
	}
	s.append("}\n");

	return s;
}

std::string JunctionTree::getDOT2(JTClique* cricca, JTClique* padre)
{
	std::string s;
	// per ogni collegamento controllo che il figlio della variabile non sia il padre della variabile (altrimenti non finisce pi�)
	for (std::unordered_map<JTClique*, Separator*>::iterator I = cricca->getLinks()->begin(); I != cricca->getLinks()->end(); ++I) {
		if ((*I).first != padre) {
			s.append("\t\"").append(cricca->toString()).append("\" -> \"").append((*I).first->toString()).append("\";\n");
			s.append(getDOT2((*I).first, cricca));
		}
	}
	return s;
}

Probability* JunctionTree::calcolaJointProbability()
{
	//// calcolo moltiplicando tra loro tutti i potenziali delle cricche e dei separatori
	//// e poi divido i primi per i secondi
	//std::unordered_set<JTClique*>::iterator clique = cliques->begin(); // ce n'� almeno una per forza
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
	//	// c'� una sola cricca allora restituisco la probabilit� direttamente :)
	//	return jointClique;
	//}
	//// altrimenti c'� almeno un separatore
	//std::unordered_set<Separator*>::iterator separator = links->begin(); // ce n'� almeno uno per forza
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
	//	//std::cout << "jointP di BN NON � normalizzata!\n";
	//	joint->normalizza();
	//} 
	////else {
	////	std::cout << "jointP di BN � normalizzata!\n";
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
