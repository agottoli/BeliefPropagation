#pragma once

#ifndef CONFIG_H
#define CONFIG_H

namespace Config {
    static const bool useMap = true;
	//static const bool useMD = true;

	static bool useIndexingTable = true;
	//static bool useIndexingTable = false;
	static bool useCUDA = true; //false;

	static int numIndexing = 0;
};

#endif /* CONFIG_H */
