# Belief Propagation 
===========================================================================

Implementazione dell'algoritmo di belief propagation in versione sequenziale (C++) e in versione parallela (CUDA) per i corsi di Intelligenza Artificiale (Prof. Alessandro Farinelli) e Architetture Avanzate (Prof. Nicola Bombieri) partendo da una rete bayesiana in formato Hugin e un ordinamento per le variabili (opzionale) utilizzato per la ricerca delle cricche massimali.


Il programma prevede una fase di preprocessing dove si ha:

- trasformazione da rete bayesiana a junction tree, visto che la belief propagation si applica su questi ultimi

- costruzione delle index mapping table per velocizzare la ricerca degli elementi durante gli aggiornamenti delle tabelle

Le reti di benchmark utilizzate possono essere trovate [qui]:http://bndg.cs.aau.dk/html/bayesian_networks.html.
 

## COMPILAZIONE:

Assieme al programma è presente un makefile che permette di compilare facilmente con il segunte comando:

    make

Il codice sorgente è unico sia per la versione sequenziale che per la versione parallela,
infatti vengono utilizzate delle direttive di preprocessing per decidere quale versione compilare.
I valori da modificare si trovano nel file Config.h all'interno della directory BeliefPropagationCpp.
I parametri più importanti sono:

- USA_CUDA
>	se TRUE  -> abilita la vesione parallela in CUDA
>
>	se FALSE -> abilita la versione sequenziale

- UTILIZZA_CPU_PER_TABELLE_PICCOLE
>	ha validità solo se USA_CUDA = TRUE
>
>	se TRUE -> esegue su cpu le tabelle più piccole (dai test fatti si è individuato a 5000 questa dimensione, ma si può cambiare modificando un altro parametro)
>
>	se FALSE -> esgue tutta la belief propagation su GPU

Altri parametri che possono interessare sono quelli relativi al modo di prendere i tempi di esecuzione:

- TEMPO_COMPLESSIVO
>	se TRUE  -> misura il tempo di esecuzione della belief propagation
>
>	se FALSE -> abilità la possibilità di misurare ogni fase dell'algoritmo

- CONSIDERA_TRASFERIMENTI_MEMORIA
>	ha validità solo se TEMPO_COMPLESSIVO = FALSE
>
>	se TRUE  -> misura il tempo di esecuzione di ogni fase COMPRESI i trasferimenti in memoria
>
>	se FALSE -> misura il tempo di esecuzione di ogni fase SENZA i trasferimenti in memoria


## ESECUZIONE:

Una volta compilato, si otterrà l'eseguibile BeliefPropagation.
Per eseguirlo si dovrà specificare obbligatoriamente una rete su cui eseguirlo ed un eventuale ordinamento dei nodi (suggerito).
Le reti si trovano nella directory RetiEsempio ed hanno estensione .net, mentre i relativi ordinamenti hanno lo stesso nome ma estensione .num.
In caso di mancato inserimento dell'ordinamento il programma ne sceglie uno utilizzando un'euristica. Tuttavia è consigliabile specificare un ordinamento, per evitare la generazione di tabelle aventi dimensioni ingestibili.
Per evitare di bloccare il computer è stato inserito un limite alla dimensione delle tabelle pari a 100000000, se si supera tale valore si viene avvertiti e il programma termina.

Un esempio di configurazione di lancio è:

	./BeliefPropagation ./RetiEsempio/Munin1.net ./RetiEsempio/Munin1.num
	
> # ENJOY