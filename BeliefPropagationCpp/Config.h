#pragma once

#ifndef CONFIG_H
#define CONFIG_H

namespace Config {
    static const bool useMap = true;
	//static const bool useMD = true;

	static bool useIndexingTable = true; // PER CUDA DEVE PER FORZA ESSERE TRUE!!!
	//static bool useIndexingTable = false;
	static bool useCUDA = false; // true
	static bool indexingSumOnRow = false;

	static int numIndexing = 0;
};

#endif /* CONFIG_H */
