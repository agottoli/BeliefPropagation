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
#define EPSILON_SOGLIA 0.0000000001 //L // 10^-10 per il confronto dei valori delle probabilit�, pu� esserci una leggera differenza per arrotondamenti vari
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

/////////////////////////////////////////////////////////////////
#ifndef USE_CUDA
#define USE_CUDA TRUE
#endif
/////////////////////////////////////////////////////////////////

#ifndef SUM_ON_ROW
// la disposizione degli elementi che vanno sommati assieme in fase di marginalizzazione
// in cuda sono per colonne, quindi va false
// per il sequnziale � implementato in entrambi i versi
#define SUM_ON_ROW FALSE 
#endif

#ifndef NORMALIZZA_AD_OGNI_PASSO
// indica se normalizzo ogni tabella oppure no dopo averla aggiornata
#define NORMALIZZA_AD_OGNI_PASSO TRUE //FALSE // sulla GPU lo fa in ogni caso perch� non costa nulla farlo con le cublas!!!
#endif

#ifndef CONTROLLA_UPDATE
#define CONTROLLA_UPDATE TRUE //FALSE
// se impostato a TRUE   -> controlla se ogni tabella � stata calcolata correttamente (serve solo per DEBUG)
// se impostato a FALSE  -> assume che siano corrette
#endif

#ifndef PRINT_EXECUTION_BP
#define PRINT_EXECUTION_BP FALSE
#endif

/////////////////////////////////////////////////////////////////
#ifndef TIMER_DETTAGLIATO 
#define TIMER_DETTAGLIATO TRUE
// se impostato a TRUE   -> attiva la possibilit� di misurare le varie fasi dell'algoritmo
// se impostato a FALSE -> misura tutto il metodo della belief propagation dall'inizio alla fine contando anche le chiamate ricorsive dell'esplorazione dell'albero
#endif

#ifndef TIMER_MARG_SCATT_DIVISI
#define TIMER_MARG_SCATT_DIVISI TRUE //FALSE 
// se mpostato a TRUE   -> prende i tempi della fase di marginalizzazione e scattering separatamente 
// se impostato a FALSE -> prende i tempi della fase di update delle tabelle tutto insieme
#endif

#ifndef TIMER_CON_TRASFERIMENTI_MEMORIA // influisce solo sui tempi di cuda e con TIMER_MARG_SCATT_DIVISI = TRUE
#define TIMER_CON_TRASFERIMENTI_MEMORIA FALSE //TRUE
// se impostato a TRUE   -> prende i tempi della fase di marginalizzazione e scattering COMPRESO IL TEMPO PER LE CUDAMEMCPY
// se impostato a FALSE -> prende i tempi della fase di marginalizzazione e scattering SOLO DEI KERNEL
#endif
/////////////////////////////////////////////////////////////////

#ifndef LIMITE_SEPARATORE_MARGINALIZZAZIONE
#define LIMITE_SEPARATORE_MARGINALIZZAZIONE 256
#endif

#ifndef IBRIDO_GPU_CPU
#define IBRIDO_GPU_CPU TRUE
#endif

#ifndef LIMITE_CRICCHE_GPU
#define LIMITE_CRICCHE_GPU 1000
#endif

#ifndef PRINT_JT_STATISTICS
#define PRINT_JT_STATISTICS TRUE
#endif