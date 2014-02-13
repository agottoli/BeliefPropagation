#pragma once

#ifndef DISJOINTSET_H
#define DISJOINTSET_H

#include <unordered_map>
#include <unordered_set>

class JTClique;

class DisjointSet
{
	std::unordered_map<JTClique*, JTClique*> roots;
	std::unordered_map<JTClique*, int> ranks;

public:
	DisjointSet(std::unordered_set<JTClique*>*);
	~DisjointSet(void);

	void unione(JTClique*, JTClique*);
	JTClique* findSet(JTClique*);
};

#endif /* DISJOINTSET_H */
