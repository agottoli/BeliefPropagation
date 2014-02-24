#include "States.h"
#include "Config.h"
#include <algorithm>    // std::find


States::States(int reserve)
{
	siza = 0;
	states = new std::vector<std::string>();
	states->reserve(reserve);
	// MEMORIA 2014-01-14
	//useMap = false; //true;
#if USE_MAP_IN_VECMAP
//	if (Config::useMap)
		statesMap = new std::unordered_map<std::string,int>();
#endif
	//
}


States::~States(void)
{
}

std::vector<std::string>::reference States::get(std::size_t index) { // std::vector<Variable*>::size_type index) {
	return states->at(index);
}

void States::push_back(std::string var) {
	states->push_back(var);
	// MEMORIA 2014-01-14 
#if USE_MAP_IN_VECMAP
//	if (Config::useMap)
		(*statesMap)[var] = siza;
#endif
	//
	siza++;
}

std::size_t States::indexOf(std::string var) {
	// MEMORIA 2014-01-14 
#if USE_MAP_IN_VECMAP
//	if (Config::useMap) {
		std::unordered_map<std::string,int>::iterator temp = statesMap->find(var);
		if (temp == statesMap->end())
			return SIZE_MAX;
		return temp->second;
#else
//	} else {
		// sostituito da
		std::vector<std::string>::iterator temp = std::find(states->begin(), states->end(), var);
		if (temp == states->end())
			return SIZE_MAX; // non può esserci un indice di dimensione max perché la length sarebbe max + 1
		return temp - states->begin();
//	}
#endif
	//
}

std::vector<std::string>::iterator States::begin() {
	return states->begin();
}

std::vector<std::string>::iterator States::end() {
	return states->end();
}

std::vector<std::string>::size_type States::size() {
	return siza;
}

std::string States::toString() {
	std::string s = "{";
	//std::cout << "size: " << siza;
	for (std::vector<std::string>::iterator I = states->begin(); I != states->end(); ++I) {
		s.append(*I).append(",\t");
		//std::cout << ".";
	}
	s.append("}");

	return s;
}
