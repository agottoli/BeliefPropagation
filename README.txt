COMPILAZIONE:

Assieme al programma c'è un makefile che permette di compilare facilmente.
Il codice sorgente è unico sia per la versione sequenziale che per la versione parallela,
infatti vengono utilizzate delle direttive di preprocessing per decidere quale versione compilare.
I valori da modificare si trovano nel file Config.h all'interno della directory BeliefPropagationCpp.
Il parametro più importante è
- USA_CUDA
	se TRUE  -> abilita la vesione parallela in CUDA
	se FALSE -> abilita la versione sequenziale

Altri parametri che possono interessare sono quelli relativi al modo di prendere i tempi di esecuzione:
- TEMPO_COMPLESSIVO
	se TRUE  -> misura il tempo di esecuzione della belief propagation
	se FALSE -> abilità la possibilità di misurare ogni fase dell'algoritmo

- CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE
	ha validità solo se TEMPO_COMPLESSIVO = FALSE
	se TRUE  -> misura il tempo di esecuzione della fase di marginalizzazione e della fase di scattering divise
	se FALSE -> misura le due fasi assieme, ma solo dei metodi update delle tabelle (senza le chiamate della collect e distribute evidence)

- CONSIDERA_TRASFERIMENTI_MEMORIA
	se TRUE  -> misura il tempo di esecuzione di ogni fase COMPRESI i trasferimenti in memoria
	se FALSE -> misura il tempo di esecuzione di ogni fase SENZA i trasferimenti in memoria


ESECUZIONE:

Una volta compilato, si otterrà l'eseguibile BeliefPropagation.
Per eseguirlo si dovrà specificare obbligatoriamente una rete su cui eseguirlo e un eventuale ordinamento dei nodi (suggerito).
Le reti si trovano nella directory RetiEsempio ed hanno estensione .net, mentre i relativi ordinamenti hanno lo stesso nome ma estensione .num.
In caso di mancato inserimento dell'ordinamento il programma ne sceglie uno utilizzando un'euristica, ma conviene specificarlo perché si rischia di avere tabelle ingestibili.
Per evitare di bloccare il computer è stato inserito un limite alla dimensione delle tabelle a 100000000, se si supera tale valore si viene avertiti e il programma termina.
Un esempio di configurazione di lancio è:
./BeliefPropagation ./RetiEsempio/Munin1.net ./RetiEsempio/Munin1.num