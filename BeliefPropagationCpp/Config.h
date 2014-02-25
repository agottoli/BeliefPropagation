#pragma once
/*
#ifndef CONFIG_H
#define CONFIG_H

namespace Config {
    static const bool useMap = true;
	//static const bool useMD = true;

	static bool useIndexingTable = true; // PER CUDA DEVE PER FORZA ESSERE TRUE!!!
	//static bool useIndexingTable = false;
	static bool useCUDA = true;
	static bool indexingSumOnRow = false;

	static int numIndexing = 0;
};

#endif 
*/

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef EPSILON_SOGLIA
#define EPSILON_SOGLIA 0.0000000001 //L // 10^-9 per il confronto dei valori delle probabilità, può esserci una leggera differenza per arrotondamenti vari
#endif

#ifndef ZERO_DIVISIONE
#define ZERO_DIVISIONE 0.0 //L // per gestire il fatto della divisione 0/0 che da 0 invece di NaN
#endif

#ifndef USE_MAP_IN_VECMAP
#define USE_MAP_IN_VECMAP TRUE
#endif

#ifndef USE_INDEXING_TABLE
#define USE_INDEXING_TABLE TRUE // obbligatorio per cuda
#endif

#ifndef USE_CUDA
#define USE_CUDA TRUE
#endif

#ifndef SUM_ON_ROW
// la disposizione degli elementi che vanno sommati assieme in fase di marginalizzazione
// in cuda sono per colonne, quindi va false
// per il sequnziale è implementato in entrambi i versi
#define SUM_ON_ROW FALSE 
#endif

#ifndef NORMALIZZA_AD_OGNI_PASSO
// indica se normalizzo ogni tabella oppure no dopo averla aggiornata
#define NORMALIZZA_AD_OGNI_PASSO FALSE
#endif

#ifndef CONTROLLA_UPDATE
#define CONTROLLA_UPDATE TRUE
#endif

#ifndef PRINT_EXECUTION_BP
#define PRINT_EXECUTION_BP FALSE
#endif

#ifndef TIMER_DETTAGLIATO
#define TIMER_DETTAGLIATO TRUE
#endif

#ifndef TIMER_CON_TRASFERIMENTI_MEMORIA
#define TIMER_CON_TRASFERIMENTI_MEMORIA FALSE //TRUE
#endif

#ifndef LIMITE_SEPARATORE_MARGINALIZZAZIONE
#define LIMITE_SEPARATORE_MARGINALIZZAZIONE 256
#endif