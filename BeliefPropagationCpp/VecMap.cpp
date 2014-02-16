#include "VecMap.h"
#include "Variable.h"
#include "Config.h"
#include <algorithm>    // std::find

#include <iostream>

VecMap::VecMap(std::size_t reserve)
{
	siza = 0;
	variables = new std::vector<Variable*>();
	variables->reserve(reserve);
	// MEMORIA 2014-01-14
	//useMap = false; //true;
	if (Config::useMap)
		variablesMap = new std::unordered_map<Variable*,std::size_t>();
	//
	//variablesMap // TODO c'è un modo per riservare lo spazio anche per le map???
}

VecMap::VecMap(std::vector<Variable*>* v, std::unordered_map<Variable*,std::size_t>* vm)
{
	siza = v->size();
	variables = new std::vector<Variable*>(*v);
	// MEMORIA 2014-01-14
	//useMap = false;
	if (Config::useMap)
		variablesMap = new std::unordered_map<Variable*,std::size_t>(*vm);
	//
}

VecMap::~VecMap(void)
{
	delete variables;
	// MEMORIA 2014-01-14
	if (Config::useMap)
		delete variablesMap;
	//
	//std::cout << "distruggo un VecMap";
	//variables->~vector(); non posso distruggere il vettore altrimenti mi distrugge anche gli elementi che ci sono dentro :(
	//variablesMap->~unordered_map();
}

std::vector<Variable*>::reference VecMap::get(std::size_t index) { // std::vector<Variable*>::size_type index) {
	return variables->at(index);
}

//int VecMap::indexOf(Variable* var) {
//	std::unordered_map<Variable*,int>::iterator temp = variablesMap->find(var);
//	if (temp == variablesMap->end())
//		return -1;
//	return temp->second;
//}

std::size_t VecMap::indexOf(Variable* var) {
	// MEMORIA 2014-01-14
	if (Config::useMap) {
		std::unordered_map<Variable*,std::size_t>::iterator temp = variablesMap->find(var);
		if (temp == variablesMap->end())
			return SIZE_MAX; // non può esserci un indice di dimensione max perché la length sarebbe max + 1
		return temp->second;
	} else {
		// sostituito da
		std::vector<Variable*>::iterator temp = std::find(variables->begin(), variables->end(), var);
		if (temp == variables->end())
			return SIZE_MAX; // non può esserci un indice di dimensione max perché la length sarebbe max + 1
		return temp - variables->begin();
	}
	//

	
}

bool VecMap::exists(Variable* var) {
	// MEMORIA 2014-01-14
	if (Config::useMap) 
		return (variablesMap->find(var) != variablesMap->end()) ;
	
	// sostituito da
	return std::find(variables->begin(), variables->end(), var) != variables->end();
	//
}

std::vector<Variable*>::iterator VecMap::begin() {
	return variables->begin();
}

std::vector<Variable*>::iterator VecMap::end() {
	return variables->end();
}

void VecMap::push_back(Variable* var) {
	variables->push_back(var);
	// MEMORIA 2014-01-14
	if (Config::useMap)
		(*variablesMap)[var] = siza;
	//

	siza++;
}

std::vector<Variable*>::reference VecMap::back() {
	return variables->back();
}

std::vector<Variable*>::size_type VecMap::size() {
	return siza;
}

VecMap* VecMap::copy()
{
	return new VecMap(variables, variablesMap);
}


std::string VecMap::toString() {
	std::string s = "{";
	//std::cout << "size: " << siza;
	for (std::vector<Variable*>::iterator I = variables->begin(); I != variables->end(); ++I) {
		s.append((*I)->toString()).append(",\t");
		//std::cout << ".";
	}
	s.append("}");

	return s;
}

	std::size_t VecMap::numeroElementiDellaRelativaTabella() {
		std::size_t numEl = 1;
		for (std::vector<Variable*>::iterator it = variables->begin(); it != variables->end(); it++) {
			numEl *= (*it)->getNStates();
		}
		return numEl; // * sizeof(double) / 1000; // 1000 per kbyte, 1000000 per mbyte.
	}
