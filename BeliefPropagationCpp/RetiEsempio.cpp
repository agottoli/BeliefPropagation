//#pragma once

//#include <vector>
#include <array>

#include "BayesianNetwork.h"
#include "Probability.h"
//#include "ProbabilityMD.h"
#include "VecMap.h"
#include "States.h"

/**
 * Piccole reti di esempio da tutorial per vedere se i calcoli sono giusti (sono quelle di alcune slide che si fanno anche a mano).
 *
 * @author Alessandro Gottoli vr352595
 */
class RetiEsempio
{

public:

	static BayesianNetwork* createEarthquakeEx() {
		BayesianNetwork* net = new BayesianNetwork();

		States* states = new States(2);
		//std::vector<std::string>* states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* burglary = new Variable("Burglary", states);

		net->addVariable(burglary);

		//net->addVariableEvidence(burglary, "tyyyyfalse");
		//net->addVariableEvidence(burglary, 0);
		//net->addVariableEvidence(burglary, 2);
		//net->addVariableEvidence(burglary, -1);
		//burglary->addEvidence("true");

		states = new States(2);
		//states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* earthquake = new Variable("Earthquake", states);

		net->addVariable(earthquake);

		states = new States(2);
		//states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* alarm = new Variable("Alarm", states);

		net->addVariable(alarm);

		//alarm->addEvidence("true");

		Variable::linkFatherToSon(burglary, alarm);
		//burglary->addSon(alarm);
		//alarm->addFather(burglary);
		Variable::linkFatherToSon(earthquake, alarm);
		//earthquake->addSon(alarm);
		//alarm->addFather(earthquake);

		states = new States(2);
		//states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* johnCalls = new Variable("JohnCalls", states);

		net->addVariable(johnCalls);

		Variable::linkFatherToSon(alarm, johnCalls);
		//alarm->addSon(johnCalls);
		//johnCalls->addFather(alarm);

		states = new States(2);
		//states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* maryCalls = new Variable("MaryCalls", states);

		net->addVariable(maryCalls);

		Variable::linkFatherToSon(alarm, maryCalls);
		//alarm->addSon(maryCalls);
		//maryCalls->addFather(alarm);


		//std::vector<double>* cpTable;
		double* cpTable; // = new double[burglary->getStates()->size()];
		Probability* cp;
		//ProbabilityMD* cp;
		VecMap* vcp;
		
		vcp = new VecMap(1);
		vcp->push_back(burglary);
		// Probability
		cpTable = new double[burglary->getStates()->size()];
		cpTable[0] = 0.001;
		cpTable[1] = 0.999;
		cp = new Probability(vcp, cpTable, 2);
		// ProbabilityMD
		//ProbabilityMD* cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(0.001);
		//cp->push_back(0.999);
		//
		burglary->setConditionalProbability(cp);
		

		vcp = new VecMap(1);
		vcp->push_back(earthquake);
		// Probability
		cpTable = new double[earthquake->getStates()->size()];
		cpTable[0] = 0.002;
		cpTable[1] = 0.998;
		cp = new Probability(vcp, cpTable, 2);
		// ProbabilityMD
		//cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(0.002);
		//cp->push_back(0.998);
		//
		earthquake->setConditionalProbability(cp);
		


		vcp = new VecMap(3);
		vcp->push_back(burglary);
		vcp->push_back(earthquake);
		vcp->push_back(alarm);
		// Probability
		cpTable = new double[burglary->getStates()->size()*earthquake->getStates()->size()*alarm->getStates()->size()];
		cpTable[0] = 0.95;
		cpTable[1] = 0.05;
		cpTable[2] = 0.94;
		cpTable[3] = 0.06;
		cpTable[4] = 0.29;
		cpTable[5] = 0.71;
		cpTable[6] = 0.001;
		cpTable[7] = 0.999;
		cp = new Probability(vcp, cpTable, 8);
		// ProbabilityMD
		//cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(0.95);
		//cp->push_back(0.05);
		//cp->push_back(0.94);
		//cp->push_back(0.06);
		//cp->push_back(0.29);
		//cp->push_back(0.71);
		//cp->push_back(0.001);
		//cp->push_back(0.999);
		//
		alarm->setConditionalProbability(cp);
		


		vcp = new VecMap(2);
		vcp->push_back(alarm);
		vcp->push_back(johnCalls);
		// Probability
		cpTable = new double[alarm->getStates()->size()*johnCalls->getStates()->size()];
		cpTable[0] = 0.9;
		cpTable[1] = 0.1;
		cpTable[2] = 0.05;
		cpTable[3] = 0.95;
		cp = new Probability(vcp, cpTable, 4);
		// ProbabilityMD
		//cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(0.9);
		//cp->push_back(0.1);
		//cp->push_back(0.05);
		//cp->push_back(0.95);
		//
		johnCalls->setConditionalProbability(cp);
		


		vcp = new VecMap(2);
		vcp->push_back(alarm);
		vcp->push_back(maryCalls);
		// Probability
		cpTable = new double[alarm->getStates()->size()*earthquake->getStates()->size()];
		cpTable[0] = 0.7;
		cpTable[1] = 0.3;
		cpTable[2] = 0.01;
		cpTable[3] = 0.99;
		cp = new Probability(vcp, cpTable, 4);
		// ProbabilityMD
		//cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(0.7);
		//cp->push_back(0.3);
		//cp->push_back(0.01);
		//cp->push_back(0.99);
		//
		maryCalls->setConditionalProbability(cp);

		return net;
	}

	static BayesianNetwork* createAsiaEx()
	{
		BayesianNetwork* net = new BayesianNetwork();

		// VARIABLE VisitAsia
		States* states = new States(2);
		//std::vector<std::string>* states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* visitAsia = new Variable("VisitAsia", states);

		net->addVariable(visitAsia);

		//std::vector<double>* cpTable;
		double* cpTable; // = new double[burglary->getStates()->size()];
		Probability* cp;
		//ProbabilityMD* cp;
		VecMap* vcp;
		

		vcp = new VecMap(1);
		vcp->push_back(visitAsia);
		// P
		cpTable = new double[visitAsia->getStates()->size()];
		//std::vector<double>* cpTable = new std::vector<double>();
		//cpTable->reserve(visitAsia->getStates()->size());
		cpTable[0] = 0.01;
		cpTable[1] = 0.99;
		cp = new Probability(vcp, cpTable, 2);
		// PMD
		//cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(0.01);
		//cp->push_back(0.99);
		//
		visitAsia->setConditionalProbability(cp);		

		// VARIABLE Tuberculosis
		states = new States(2);
		//states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* tuberculosis = new Variable("Tuberculosis", states);

		net->addVariable(tuberculosis);

		Variable::linkFatherToSon(visitAsia, tuberculosis);

		vcp = new VecMap(2);
		vcp->push_back(visitAsia);
		vcp->push_back(tuberculosis);
		// P
		cpTable = new double[visitAsia->getStates()->size()*tuberculosis->getStates()->size()];
		//cpTable = new std::vector<double>();
		//cpTable->reserve(visitAsia->getStates()->size()*tuberculosis->getStates()->size());
		cpTable[0] = 0.05;
		cpTable[1] = 0.95;
		cpTable[2] = 0.01;
		cpTable[3] = 0.99;
		cp = new Probability(vcp, cpTable, 4);
		// PMD
		//cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(0.05);
		//cp->push_back(0.95);
		//cp->push_back(0.01);
		//cp->push_back(0.99);
		//
		tuberculosis->setConditionalProbability(cp);
	
		// VARIABLE Smoking
		states = new States(2);
		//states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* smoking = new Variable("Smoking", states);

		net->addVariable(smoking);

		vcp = new VecMap(1);
		vcp->push_back(smoking);
		// P
		cpTable = new double[smoking->getStates()->size()];
		//cpTable = new std::vector<double>();
		//cpTable->reserve(smoking->getStates()->size());
		cpTable[0] = 0.5;
		cpTable[1] = 0.5;
		cp = new Probability(vcp, cpTable, 2);
		// PMD
		//cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(0.5);
		//cp->push_back(0.5);
		//
		smoking->setConditionalProbability(cp);

	
		// VARIABLE LungCancer
		states = new States(2);
		//states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* lungCancer = new Variable("LungCancer", states);

		net->addVariable(lungCancer);

		Variable::linkFatherToSon(smoking, lungCancer);
		
		vcp = new VecMap(2);
		vcp->push_back(smoking);
		vcp->push_back(lungCancer);
		// P
		cpTable = new double[smoking->getStates()->size()*lungCancer->getStates()->size()];
		//cpTable = new std::vector<double>();
		//cpTable->reserve(smoking->getStates()->size()*lungCancer->getStates()->size());
		cpTable[0] = 0.1;
		cpTable[1] = 0.9;
		cpTable[2] = 0.01;
		cpTable[3] = 0.99;
		cp = new Probability(vcp, cpTable, 4);
		// PMD
		//cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(0.1);
		//cp->push_back(0.9);
		//cp->push_back(0.01);
		//cp->push_back(0.99);
		//
		lungCancer->setConditionalProbability(cp);


		// VARIABLE Bronchitis
		states = new States(2);
		//states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* bronchitis = new Variable("Bronchitis", states);

		net->addVariable(bronchitis);

		Variable::linkFatherToSon(smoking, bronchitis);
		
		vcp = new VecMap(2);
		vcp->push_back(smoking);
		vcp->push_back(bronchitis);
		// P
		cpTable = new double[smoking->getStates()->size()*bronchitis->getStates()->size()];
		//cpTable = new std::vector<double>();
		//cpTable->reserve(smoking->getStates()->size()*bronchitis->getStates()->size());
		cpTable[0] = 0.6;
		cpTable[1] = 0.4;
		cpTable[2] = 0.3;
		cpTable[3] = 0.7;
		cp = new Probability(vcp, cpTable, 4);
		// PMD
		//cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(0.6);
		//cp->push_back(0.4);
		//cp->push_back(0.3);
		//cp->push_back(0.7);
		//
		bronchitis->setConditionalProbability(cp);


		// VARIABLE EitherTubLungCancer
		states = new States(2);
		//states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* eitherTubLungCancer = new Variable("EitherTubLungCancer", states);

		net->addVariable(eitherTubLungCancer);

		Variable::linkFatherToSon(lungCancer, eitherTubLungCancer);
		Variable::linkFatherToSon(tuberculosis, eitherTubLungCancer);
		
		vcp = new VecMap(3);
		vcp->push_back(lungCancer);
		vcp->push_back(tuberculosis);
		vcp->push_back(eitherTubLungCancer);
		// P
		cpTable = new double[lungCancer->getStates()->size()*tuberculosis->getStates()->size()*eitherTubLungCancer->getStates()->size()];
		//cpTable = new std::vector<double>();
		//cpTable->reserve(lungCancer->getStates()->size()*tuberculosis->getStates()->size()*eitherTubLungCancer->getStates()->size());
		cpTable[0] = 1.0;
		cpTable[1] = 0.0;
		cpTable[2] = 1.0;
		cpTable[3] = 0.0;
		cpTable[4] = 1.0;
		cpTable[5] = 0.0;
		cpTable[6] = 0.0;
		cpTable[7] = 1.0;
		cp = new Probability(vcp, cpTable, 8);
		// PMD
		//cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(1.0);
		//cp->push_back(0.0);
		//cp->push_back(1.0);
		//cp->push_back(0.0);
		//cp->push_back(1.0);
		//cp->push_back(0.0);
		//cp->push_back(0.0);
		//cp->push_back(1.0);
		//
		eitherTubLungCancer->setConditionalProbability(cp);

		
		// VARIABLE PositiveXRay
		states = new States(2);
		//states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* positiveXRay = new Variable("PositiveXRay", states);

		net->addVariable(positiveXRay);

		Variable::linkFatherToSon(eitherTubLungCancer, positiveXRay);

		vcp = new VecMap(2);
		vcp->push_back(eitherTubLungCancer);
		vcp->push_back(positiveXRay);
		// P
		cpTable = new double[eitherTubLungCancer->getStates()->size()*positiveXRay->getStates()->size()];
		//cpTable = new std::vector<double>();
		//cpTable->reserve(eitherTubLungCancer->getStates()->size()*positiveXRay->getStates()->size());
		cpTable[0] = 0.98;
		cpTable[1] = 0.02;
		cpTable[2] = 0.05;
		cpTable[3] = 0.95;
		cp = new Probability(vcp, cpTable, 4);
		// PMD
		//cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(0.98);
		//cp->push_back(0.02);
		//cp->push_back(0.05);
		//cp->push_back(0.95);
		//
		positiveXRay->setConditionalProbability(cp);


		// VARIABLE Dyspnoea
		states = new States(2);
		//states = new std::vector<std::string>();
		//states->reserve(2);
		states->push_back("true");
		states->push_back("false");
		Variable* dyspnoea = new Variable("Dyspnoea", states);

		net->addVariable(dyspnoea);

		Variable::linkFatherToSon(eitherTubLungCancer, dyspnoea);
		Variable::linkFatherToSon(bronchitis, dyspnoea);
		
		vcp = new VecMap(3);
		vcp->push_back(eitherTubLungCancer);
		vcp->push_back(bronchitis);
		vcp->push_back(dyspnoea);
		// P
		cpTable = new double[eitherTubLungCancer->getStates()->size()*bronchitis->getStates()->size()*dyspnoea->getStates()->size()];
		//cpTable = new std::vector<double>();
		//cpTable->reserve(eitherTubLungCancer->getStates()->size()*bronchitis->getStates()->size()*dyspnoea->getStates()->size());
		cpTable[0] = 0.9;
		cpTable[1] = 0.1;
		cpTable[2] = 0.7;
		cpTable[3] = 0.3;
		cpTable[4] = 0.8;
		cpTable[5] = 0.2;
		cpTable[6] = 0.1;
		cpTable[7] = 0.9;
		cp = new Probability(vcp, cpTable, 8);
		// PMD
		//cp = new ProbabilityMD(vcp); //, cpTable);
		//cp->push_back(0.9);
		//cp->push_back(0.1);
		//cp->push_back(0.7);
		//cp->push_back(0.3);
		//cp->push_back(0.8);
		//cp->push_back(0.2);
		//cp->push_back(0.1);
		//cp->push_back(0.9);
		//
		dyspnoea->setConditionalProbability(cp);


		// esperimento VecMap
		//vcp->statistiche();

		//VecMap* nuovo = new VecMap(vcp->size() + 3);
		//nuovo->statistiche();

		//*nuovo = *vcp; // capacity rimane quella di vcp
		//
		//nuovo->statistiche();

		//std::string s;
		//std::cin >> s;

		return net;
		
	}

	//static BayesianNetwork* createComaEx()
	//{
	//	BayesianNetwork* net = new BayesianNetwork();

	//	// VARIABLE metastaticCancer
	//	States* states = new States(2);
	//	//std::vector<std::string>* states = new std::vector<std::string>();
	//	//states->reserve(2);
	//	states->push_back("true");
	//	states->push_back("false");
	//	Variable* metastaticCancer = new Variable("metastaticCancer", states);

	//	net->addVariable(metastaticCancer);

	//	//std::vector<double>* cpTable = new std::vector<double>();
	//	//cpTable->reserve(metastaticCancer->getStates()->size());
	//	//cpTable->push_back(0.9);
	//	//cpTable->push_back(0.1);
	//	VecMap* vcp = new VecMap(1);
	//	vcp->push_back(metastaticCancer);
	//	ProbabilityMD* cp = new ProbabilityMD(vcp); //, cpTable);
	//	cp->push_back(0.9);
	//	cp->push_back(0.1);
	//	metastaticCancer->setConditionalProbability(cp);	

	//			//Variable metastaticCancer = new Variable("MetastaticCancer", new Object[] { true, false});
	//			//net.addVariable(metastaticCancer);
	//			//
	//			//double[] pMetastaticCancerTable = new double[metastaticCancer.states.size()];
	//			//pMetastaticCancerTable[0] = 0.9;
	//			//pMetastaticCancerTable[1] = 0.1;
	//			//ConditionalProbability pMetastaticCancer = new ConditionalProbability(metastaticCancer, pMetastaticCancerTable);
	//			//metastaticCancer.setConditionalProbability(pMetastaticCancer);
	//			
	//	// VARIABLE increasedCalcium
	//	states = new States(2);
	//	//states = new std::vector<std::string>();
	//	//states->reserve(2);
	//	states->push_back("true");
	//	states->push_back("false");
	//	Variable* increasedCalcium = new Variable("increasedCalcium", states);

	//	net->addVariable(increasedCalcium);

	//	Variable::linkFatherToSon(metastaticCancer, increasedCalcium);
	//	
	//	//cpTable = new std::vector<double>();
	//	//cpTable->reserve(metastaticCancer->getStates()->size()*increasedCalcium->getStates()->size());
	//	//cpTable->push_back(0.2);
	//	//cpTable->push_back(0.8);
	//	//cpTable->push_back(0.05);
	//	//cpTable->push_back(0.95);
	//	vcp = new VecMap(2);
	//	vcp->push_back(metastaticCancer);
	//	vcp->push_back(increasedCalcium);
	//	cp = new ProbabilityMD(vcp); //, cpTable);
	//	cp->push_back(0.2);
	//	cp->push_back(0.8);
	//	cp->push_back(0.05);
	//	cp->push_back(0.95);
	//	increasedCalcium->setConditionalProbability(cp);


	//			//Variable increasedCalcium = new Variable("IncreasedCalcium", new Object[] { true, false});
	//			//net.addVariable(increasedCalcium);
	//			//Variable.linkFatherSon(metastaticCancer, increasedCalcium);
	//			//
	//			//double[] pIncreasedCalciumTable = 
	//			//		new double[metastaticCancer.states.size()*increasedCalcium.states.size()];
	//			//pIncreasedCalciumTable[0] = 0.20;
	//			//pIncreasedCalciumTable[1] = 0.80;
	//			//pIncreasedCalciumTable[2] = 0.05;
	//			//pIncreasedCalciumTable[3] = 0.95;
	//			//ConditionalProbability pIncreasedCalcium = new ConditionalProbability(increasedCalcium, new Variable[] {metastaticCancer}, pIncreasedCalciumTable);
	//			//increasedCalcium.setConditionalProbability(pIncreasedCalcium);
	//			
	//	// VARIABLE brainTumor
	//	states = new States(2);
	//	//states = new std::vector<std::string>();
	//	//states->reserve(2);
	//	states->push_back("true");
	//	states->push_back("false");
	//	Variable* brainTumor = new Variable("brainTumor", states);

	//	net->addVariable(brainTumor);

	//	Variable::linkFatherToSon(metastaticCancer, brainTumor);
	//	
	//	//cpTable = new std::vector<double>();
	//	//cpTable->reserve(metastaticCancer->getStates()->size()*brainTumor->getStates()->size());
	//	//cpTable->push_back(0.8);
	//	//cpTable->push_back(0.2);
	//	//cpTable->push_back(0.2);
	//	//cpTable->push_back(0.8);
	//	vcp = new VecMap(2);
	//	vcp->push_back(metastaticCancer);
	//	vcp->push_back(brainTumor);
	//	cp = new ProbabilityMD(vcp); //, cpTable);
	//	cp->push_back(0.8);
	//	cp->push_back(0.2);
	//	cp->push_back(0.2);
	//	cp->push_back(0.8);
	//	brainTumor->setConditionalProbability(cp);

	//			//Variable brainTumor = new Variable("BrainTumor", new Object[] { true, false});
	//			//net.addVariable(brainTumor);
	//			//Variable.linkFatherSon(metastaticCancer, brainTumor);
	//			//
	//			//double[] pBrainTumorTable = 
	//			//		new double[metastaticCancer.states.size()*brainTumor.states.size()];
	//			//pBrainTumorTable[0] = 0.80;
	//			//pBrainTumorTable[1] = 0.20;
	//			//pBrainTumorTable[2] = 0.20;
	//			//pBrainTumorTable[3] = 0.80;
	//			//ConditionalProbability pBrainTumor = new ConditionalProbability(brainTumor, new Variable[] {metastaticCancer}, pBrainTumorTable);
	//			//brainTumor.setConditionalProbability(pBrainTumor);
	//			
	//	// VARIABLE coma
	//	states = new States(2);
	//	//states = new std::vector<std::string>();
	//	//states->reserve(2);
	//	states->push_back("true");
	//	states->push_back("false");
	//	Variable* coma = new Variable("coma", states);

	//	net->addVariable(coma);

	//	Variable::linkFatherToSon(increasedCalcium, coma);
	//	Variable::linkFatherToSon(brainTumor, coma);
	//	
	//	//cpTable = new std::vector<double>();
	//	//cpTable->reserve(increasedCalcium->getStates()->size()*brainTumor->getStates()->size()*coma->getStates()->size());
	//	//cpTable->push_back(0.8);
	//	//cpTable->push_back(0.2);
	//	//cpTable->push_back(0.8);
	//	//cpTable->push_back(0.2);
	//	//cpTable->push_back(0.8);
	//	//cpTable->push_back(0.2);
	//	//cpTable->push_back(0.05);
	//	//cpTable->push_back(0.95);
	//	vcp = new VecMap(3);
	//	vcp->push_back(increasedCalcium);
	//	vcp->push_back(brainTumor);
	//	vcp->push_back(coma);
	//	cp = new ProbabilityMD(vcp); //, cpTable);
	//	cp->push_back(0.8);
	//	cp->push_back(0.2);
	//	cp->push_back(0.8);
	//	cp->push_back(0.2);
	//	cp->push_back(0.8);
	//	cp->push_back(0.2);
	//	cp->push_back(0.05);
	//	cp->push_back(0.95);
	//	coma->setConditionalProbability(cp);

	//			//Variable coma = new Variable("Coma", new Object[] { true, false});
	//			//net.addVariable(coma);
	//			//Variable.linkFatherSon(increasedCalcium, coma);
	//			//Variable.linkFatherSon(brainTumor, coma);
	//			//
	//			//double[] pComaTable = 
	//			//		new double[increasedCalcium.states.size()*brainTumor.states.size()*coma.states.size()];
	//			//pComaTable[0] = 0.80;
	//			//pComaTable[1] = 0.20;
	//			//pComaTable[2] = 0.80;
	//			//pComaTable[3] = 0.20;
	//			//pComaTable[4] = 0.80;
	//			//pComaTable[5] = 0.20;
	//			//pComaTable[6] = 0.05;
	//			//pComaTable[7] = 0.95;
	//			//ConditionalProbability pComa = new ConditionalProbability(coma, new Variable[] {increasedCalcium, brainTumor}, pComaTable);
	//			//coma.setConditionalProbability(pComa);
	//			

	//	// VARIABLE severeHeadache
	//	states = new States(2);
	//	//states = new std::vector<std::string>();
	//	//states->reserve(2);
	//	states->push_back("true");
	//	states->push_back("false");
	//	Variable* severeHeadache = new Variable("severeHeadache", states);

	//	net->addVariable(severeHeadache);

	//	Variable::linkFatherToSon(brainTumor, severeHeadache);
	//	
	//	//cpTable = new std::vector<double>();
	//	//cpTable->reserve(brainTumor->getStates()->size()*severeHeadache->getStates()->size());
	//	//cpTable->push_back(0.9);
	//	//cpTable->push_back(0.1);
	//	//cpTable->push_back(0.7);
	//	//cpTable->push_back(0.3);
	//	vcp = new VecMap(2);
	//	vcp->push_back(brainTumor);
	//	vcp->push_back(severeHeadache);
	//	cp = new ProbabilityMD(vcp); //, cpTable);
	//	cp->push_back(0.9);
	//	cp->push_back(0.1);
	//	cp->push_back(0.7);
	//	cp->push_back(0.3);
	//	severeHeadache->setConditionalProbability(cp);

	//			//Variable severeHeadache = new Variable("SevereHeadache", new Object[] { true, false});
	//			//net.addVariable(severeHeadache);
	//			//Variable.linkFatherSon(brainTumor, severeHeadache);
	//			//
	//			//double[] pSevereHeadacheTable = 
	//			//		new double[metastaticCancer.states.size()*brainTumor.states.size()];
	//			//pSevereHeadacheTable[0] = 0.80;
	//			//pSevereHeadacheTable[1] = 0.20;
	//			//pSevereHeadacheTable[2] = 0.60;
	//			//pSevereHeadacheTable[3] = 0.40;
	//			//ConditionalProbability pSevereHeadache = new ConditionalProbability(severeHeadache, new Variable[] {brainTumor}, pSevereHeadacheTable);
	//			//severeHeadache.setConditionalProbability(pSevereHeadache);


	//	return net;
	//}


	///*
	// * The Sprinkler Network (slide http://www-anw.cs.umass.edu/~cs691t/SS02/lectures/week7.PDF)
	// */
	//static BayesianNetwork* createSprinklerEx() {
	//	BayesianNetwork* net = new BayesianNetwork();
	//	
	//	//Variable Cloudy = new Variable("Cloudy", new Object[] {true, false});
	//	//net.addVariable(Cloudy);
	//	//
	//	//double[] pCloudyTable = new double[Cloudy.states.size()];
	//	//pCloudyTable[0] = 0.5;
	//	//pCloudyTable[1] = 0.5;
	//	//ConditionalProbability pCloudy = new ConditionalProbability(Cloudy, pCloudyTable);
	//	//Cloudy.setConditionalProbability(pCloudy);
	//	
	//	// VARIABLE Cloudy
	//	States* states = new States(2);
	//	//std::vector<std::string>* states = new std::vector<std::string>();
	//	//states->reserve(2);
	//	states->push_back("true");
	//	states->push_back("false");
	//	Variable* Cloudy = new Variable("Cloudy", states);

	//	net->addVariable(Cloudy);

	//	//std::vector<double>* cpTable = new std::vector<double>();
	//	//cpTable->reserve(Cloudy->getStates()->size());
	//	//cpTable->push_back(0.5);
	//	//cpTable->push_back(0.5);
	//	VecMap* vcp = new VecMap(1);
	//	vcp->push_back(Cloudy);
	//	ProbabilityMD* cp = new ProbabilityMD(vcp); //, cpTable);
	//	cp->push_back(0.5);
	//	cp->push_back(0.5);
	//	Cloudy->setConditionalProbability(cp);	

	//	//
	//	//Variable Sprinkler = new Variable("Sprinkler", new Object[] {true, false});
	//	//net.addVariable(Sprinkler);
	//	//Variable.linkFatherSon(Cloudy, Sprinkler);
	//	//
	//	//double[] pSprinklerTable = new double[Cloudy.states.size()*Sprinkler.states.size()];
	//	//pSprinklerTable[0] = 0.1;
	//	//pSprinklerTable[1] = 0.9;
	//	//pSprinklerTable[2] = 0.5;
	//	//pSprinklerTable[3] = 0.5;
	//	//ConditionalProbability pSprinkler = new ConditionalProbability(Sprinkler, pSprinklerTable);
	//	//Sprinkler.setConditionalProbability(pSprinkler);
	//	
	//	// VARIABLE Sprinkler
	//	states = new States(2);
	//	//states = new std::vector<std::string>();
	//	//states->reserve(2);
	//	states->push_back("true");
	//	states->push_back("false");
	//	Variable* Sprinkler = new Variable("Sprinkler", states);

	//	net->addVariable(Sprinkler);

	//	Variable::linkFatherToSon(Cloudy, Sprinkler);
	//	
	//	//cpTable = new std::vector<double>();
	//	//cpTable->reserve(Cloudy->getStates()->size()*Sprinkler->getStates()->size());
	//	//cpTable->push_back(0.1);
	//	//cpTable->push_back(0.9);
	//	//cpTable->push_back(0.5);
	//	//cpTable->push_back(0.5);
	//	vcp = new VecMap(2);
	//	vcp->push_back(Cloudy);
	//	vcp->push_back(Sprinkler);
	//	cp = new ProbabilityMD(vcp); //, cpTable);
	//	cp->push_back(0.1);
	//	cp->push_back(0.9);
	//	cp->push_back(0.5);
	//	cp->push_back(0.5);
	//	Sprinkler->setConditionalProbability(cp);




	//	//Variable Rain = new Variable("Rain", new Object[] {true, false});
	//	//net.addVariable(Rain);
	//	//Variable.linkFatherSon(Cloudy, Rain);
	//	//
	//	//double[] pRainTable = new double[Cloudy.states.size()*Rain.states.size()];
	//	//pRainTable[0] = 0.8;
	//	//pRainTable[1] = 0.2;
	//	//pRainTable[2] = 0.2;
	//	//pRainTable[3] = 0.8;
	//	//ConditionalProbability pRain = new ConditionalProbability(Rain, pRainTable);
	//	//Rain.setConditionalProbability(pRain);
	//	
	//	// VARIABLE Rain
	//	states = new States(2);
	//	//states = new std::vector<std::string>();
	//	//states->reserve(2);
	//	states->push_back("true");
	//	states->push_back("false");
	//	Variable* Rain = new Variable("Rain", states);

	//	net->addVariable(Rain);

	//	Variable::linkFatherToSon(Cloudy, Rain);
	//	
	//	//cpTable = new std::vector<double>();
	//	//cpTable->reserve(Cloudy->getStates()->size()*Rain->getStates()->size());
	//	//cpTable->push_back(0.8);
	//	//cpTable->push_back(0.2);
	//	//cpTable->push_back(0.2);
	//	//cpTable->push_back(0.8);
	//	vcp = new VecMap(2);
	//	vcp->push_back(Cloudy);
	//	vcp->push_back(Rain);
	//	cp = new ProbabilityMD(vcp); //, cpTable);
	//	cp->push_back(0.8);
	//	cp->push_back(0.2);
	//	cp->push_back(0.2);
	//	cp->push_back(0.8);
	//	Rain->setConditionalProbability(cp);



	//	//Variable WetGrass = new Variable("WetGrass", new Object[] {true, false});
	//	//net.addVariable(WetGrass);
	//	//Variable.linkFatherSon(Sprinkler, WetGrass);
	//	//Variable.linkFatherSon(Rain, WetGrass);
	//	//
	//	//double[] pWetGrassTable = new double[Sprinkler.states.size()*Rain.states.size()*WetGrass.states.size()];
	//	//pWetGrassTable[0] = 0.99;
	//	//pWetGrassTable[1] = 0.01;
	//	//pWetGrassTable[2] = 0.9;
	//	//pWetGrassTable[3] = 0.1;
	//	//pWetGrassTable[4] = 0.9;
	//	//pWetGrassTable[5] = 0.1;
	//	//pWetGrassTable[6] = 0.0;
	//	//pWetGrassTable[7] = 1.0;
	//	//ConditionalProbability pWetGrass = new ConditionalProbability(WetGrass, pWetGrassTable);
	//	//WetGrass.setConditionalProbability(pWetGrass);

	//	// VARIABLE WetGrass 
	//	states = new States(2);
	//	//states = new std::vector<std::string>();
	//	//states->reserve(2);
	//	states->push_back("true");
	//	states->push_back("false");
	//	Variable* WetGrass = new Variable("WetGrass", states);

	//	net->addVariable(WetGrass);

	//	Variable::linkFatherToSon(Sprinkler, WetGrass);
	//	Variable::linkFatherToSon(Rain, WetGrass);
	//	
	//	//cpTable = new std::vector<double>();
	//	//cpTable->reserve(Sprinkler->getStates()->size()*Rain->getStates()->size()*WetGrass->getStates()->size());
	//	//cpTable->push_back(0.99);
	//	//cpTable->push_back(0.01);
	//	//cpTable->push_back(0.9);
	//	//cpTable->push_back(0.1);
	//	//cpTable->push_back(0.9);
	//	//cpTable->push_back(0.1);
	//	//cpTable->push_back(0.0);
	//	//cpTable->push_back(1.0);
	//	vcp = new VecMap(3);
	//	vcp->push_back(Sprinkler);
	//	vcp->push_back(Rain);
	//	vcp->push_back(WetGrass);
	//	cp = new ProbabilityMD(vcp); //, cpTable);
	//	cp->push_back(0.99);
	//	cp->push_back(0.01);
	//	cp->push_back(0.9);
	//	cp->push_back(0.1);
	//	cp->push_back(0.9);
	//	cp->push_back(0.1);
	//	cp->push_back(0.0);
	//	cp->push_back(1.0);
	//	WetGrass->setConditionalProbability(cp);
	//	
	//	return net;
	//}
};
