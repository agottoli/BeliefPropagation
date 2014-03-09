#pragma once

#ifndef DISJOINTSET_H
#define DISJOINTSET_H

#include <unordered_map>
#include <unordered_set>

class JTClique;

/**
 * Classe che implementa gli insiemi disgiunti.
 *
 * @author Alessandro Gottoli vr352595
 */
class DisjointSet
{
	std::unordered_map<JTClique*, JTClique*> roots;
	std::unordered_map<JTClique*, int> ranks;

public:
	DisjointSet(std::unordered_set<JTClique*>*);
	~DisjointSet(void);

	void unione(JTClique*, JTClique*);
	JTClique* findSet(JTClique*);

	std::unordered_set<JTClique*>* getRootsDisjoint() {
		std::unordered_set<JTClique*>* rts = new std::unordered_set<JTClique*>();
		for (std::unordered_map<JTClique*, JTClique*>::iterator it = roots.begin(); it != roots.end(); it++) {
			rts->insert(findSet((*it).first)); // se mettessi second dovrebbe essere + veloce
		}
		return rts;
	}
};

#endif /* DISJOINTSET_H */
