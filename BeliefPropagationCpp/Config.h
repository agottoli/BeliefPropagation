#pragma once

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


/////////////////////////////////////////////////////////////////
#ifndef USA_CUDA
// questo è il parametro più importante in cui scegliere se si vuole la versione sequenziale o quella parallela
// se impostato a TRUE  -> si compilerà la versione parallela
// se impostato a FALSE -> si compilerà la versione sequenziale
#define USA_CUDA TRUE
#endif
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
#ifndef TEMPO_COMPLESSIVO // da girare 
// permette di prendere i tempi relativi alle sole operazioni sulle tabelle
#define TEMPO_COMPLESSIVO FALSE
// se impostato a TRUE  -> misura tutto il metodo della belief propagation dall'inizio alla fine contando anche le chiamate ricorsive dell'esplorazione dell'albero
// se impostato a FALSE -> attiva la possibilità di misurare le varie fasi dell'algoritmo (con o senza trasferimenti...)
#endif

#ifndef CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
// pemette di dividere la misurazione del tempo per entrambe le fasi di aggiornamento
#define CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE TRUE //FALSE 
// se impostato a TRUE  -> prende i tempi della fase di marginalizzazione e scattering separatamente 
// se impostato a FALSE -> prende i tempi della fase di update delle tabelle tutto insieme
#endif

#ifndef CONSIDERA_TRASFERIMENTI_MEMORIA 
// Permette di prendere i tempi senza le operazioni di trasferimento dei dati tra ram e gpu
// influisce solo sui tempi di cuda e con CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE = TRUE, altrimenti sono sempre compresi i tempi dovuti ai trasferimenti
#define CONSIDERA_TRASFERIMENTI_MEMORIA FALSE //TRUE
// se impostato a TRUE  -> prende i tempi della fase di marginalizzazione e scattering COMPRESO IL TEMPO PER LE CUDAMEMCPY
// se impostato a FALSE -> prende i tempi della fase di marginalizzazione e scattering SOLO DEI KERNEL
#endif
/////////////////////////////////////////////////////////////////

#ifndef SIZE_MAX_SMALL_N
// questo parametro permette di specificare la dimensione oltre la quale utiliziamo il kernel BigN per fare la marginalizzazione
// NON IMPOSTARE OLTRE I 512
#define SIZE_MAX_SMALL_N 256
#endif

#ifndef UTILIZZA_CPU_PER_TABELLE_PICCOLE
// imposta se attivare la modaltà di processare con la cpu le tabelle che hanno dimensione minore o uguale al valore del parametro scritto sotto
#define UTILIZZA_CPU_PER_TABELLE_PICCOLE TRUE
// se impostato a TRUE   -> esegue su GPU solo le tabelle con dimensione maggiore del parametro sotto SIZE_MAX_CPU
// se impostato a FALSE  -> esegue tutto su GPU
#endif

// TODO si potrebbe passare da input!!!
#ifndef SIZE_MAX_CPU
// questo valore rappresenta la dimensione massima della tabella dei potenziali 
// che viene processata direttamente dalla cpu
// invece di essere trasferita alla gpu
#define SIZE_MAX_CPU 150 
#endif

#ifndef PRINT_EXECUTION_BP
// questo parametro da la possibilità di attivare la visualizzazione delle chiamate dell'algoritmo di belief propagation
// MESSO A FALSE PER PERFORMANCE
#define PRINT_EXECUTION_BP FALSE
#endif

///////////////////////////////////////////////////////////////
// NON MODIFICARE I VALORI QUA SOTTO

#ifndef CONTROLLA_UPDATE
#define CONTROLLA_UPDATE TRUE //FALSE
// se impostato a TRUE   -> controlla se ogni tabella è stata calcolata correttamente (serve solo per DEBUG)
// se impostato a FALSE  -> assume che siano corrette

// NOTA: utilizza un margine di errore impostato in EPSILON_SOGLIA per lasciare un po' di margine per gli errori di arrotondamento
#endif

#ifndef EPSILON_SOGLIA
// in fase di debug è utilizzato come valore massimo di errore consentito
// per il confronto dei valori delle probabilità, 
// in quanto può esserci una leggera differenza per arrotondamenti vari
#define EPSILON_SOGLIA 0.0000000001 // 10^-10 
#endif

#ifndef SUM_ON_ROW
// imposta le diverse configurazioni in cui si posso dsporre gli indici nelle tabelle di indicizzazione
// nello specifico indica se la disposizione degli elementi che vanno sommati assieme in fase di marginalizzazione
// sono vicini (quindi sulla stessa riga) oppure sono alternati (quindi scritti per colonne)
// in cuda sono per colonne, quindi va impostato a false
// per il sequenziale è implementato in entrambi i versi
#define SUM_ON_ROW FALSE 
#endif

#ifndef NORMALIZZA_AD_OGNI_PASSO
// indica se si ha intenzione di normalizzare le tabelle appena vengono aggiornate oppure no
#define NORMALIZZA_AD_OGNI_PASSO TRUE //FALSE // sulla GPU lo fa in ogni caso perché non costa nulla farlo con le cublas!!!
#endif


#ifndef ZERO_DIVISIONE
#define ZERO_DIVISIONE 0.0 // per gestire il fatto della divisione 0/0 che da 0 invece di NaN
#endif

#ifndef USE_MAP_IN_VECMAP
// indica se utilizzare la struttura dati map per la ricerca più efficiente delle variabili in una cricca
// LASCIARE A TRUE PER LE PERFORMANCE
#define USE_MAP_IN_VECMAP TRUE
#endif

#ifndef USE_INDEXING_TABLE
// indica se utilizzare degli array di supporto per ogni separatore in cui indico dove si trova nella tabella dei potenziali della cricca corrispondente
// l'elemento interessato, si consuma più ram ma una volta calcolate le tabelle di indicizzazione non c'è bisogno di andare a ricercarsi un dato elemento in tabella
// LASCIARE A TRUE PER PERFORMANCE (CUDA SI BASA SU QUESTO)
#define USE_INDEXING_TABLE TRUE // obbligatorio per cuda
#endif

#ifndef PRINT_JT_STATISTICS
// abilita la visualizzazione dei parametri della rete passata in input
#define PRINT_JT_STATISTICS TRUE
#endif


#ifndef CONFIG_H
#define CONFIG_H

namespace Config {
    //static const bool useMap = true;
	//static const bool useMD = true;

	//static bool useIndexingTable = true; // PER CUDA DEVE PER FORZA ESSERE TRUE!!!
	//static bool useCUDA = true;
	//static bool indexingSumOnRow = false;

	static int numIndexing = 0;
};

#endif 