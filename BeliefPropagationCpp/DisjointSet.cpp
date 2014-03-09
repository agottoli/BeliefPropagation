#include "DisjointSet.h"

/**
 * Implementa i metodi per gli insiemi disgiunti.
 *
 * @author Alessandro Gottoli vr352595
 */
DisjointSet::DisjointSet(std::unordered_set<JTClique*>* elements)
{
	for (std::unordered_set<JTClique*>::iterator I = elements->begin(); I != elements->end(); ++I) {
		roots[*I] = *I;
		ranks[*I] = 0;
	}

}


DisjointSet::~DisjointSet(void)
{
}


void DisjointSet::unione(JTClique* a, JTClique* b)
{
	if (ranks[a] > ranks[b]) {
		roots[b] = a;
	} else {
		roots[a] = b;
		if (ranks[a] == ranks[b]) {
			ranks[b] += 1;
		}
	}
}

JTClique* DisjointSet::findSet(JTClique* i)
{
	if (roots[i] != i)
		return roots[i] = findSet(roots[i]);
	return i;
}
