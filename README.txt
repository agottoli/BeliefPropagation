COMPILAZIONE:

Assieme al programma � presente un makefile che permette di compilare facilmente.
Il codice sorgente � unico sia per la versione sequenziale che per la versione parallela,
infatti vengono utilizzate delle direttive di preprocessing per decidere quale versione compilare.
I valori da modificare si trovano nel file Config.h all'interno della directory BeliefPropagationCpp.
Il parametro pi� importante �
- USA_CUDA
	se TRUE  -> abilita la vesione parallela in CUDA
	se FALSE -> abilita la versione sequenziale

- UTILIZZA_CPU_PER_TABELLE_PICCOLE
	ha validit� solo se USA_CUDA = TRUE
	se TRUE -> esegue su cpu le tabelle pi� piccole (dai test fatti si � individuato a 5000 questa dimensione, ma si pu� cambiare modificando un altro parametro)
	se FALSE -> esgue tutta la belief propagation su GPU

Altri parametri che possono interessare sono quelli relativi al modo di prendere i tempi di esecuzione:
- TEMPO_COMPLESSIVO
	se TRUE  -> misura il tempo di esecuzione della belief propagation
	se FALSE -> abilit� la possibilit� di misurare ogni fase dell'algoritmo

- CONSIDERA_TRASFERIMENTI_MEMORIA
	ha validit� solo se TEMPO_COMPLESSIVO = FALSE
	se TRUE  -> misura il tempo di esecuzione di ogni fase COMPRESI i trasferimenti in memoria
	se FALSE -> misura il tempo di esecuzione di ogni fase SENZA i trasferimenti in memoria


ESECUZIONE:

Una volta compilato, si otterr� l'eseguibile BeliefPropagation.
Per eseguirlo si dovr� specificare obbligatoriamente una rete su cui eseguirlo ed un eventuale ordinamento dei nodi (suggerito).
Le reti si trovano nella directory RetiEsempio ed hanno estensione .net, mentre i relativi ordinamenti hanno lo stesso nome ma estensione .num.
In caso di mancato inserimento dell'ordinamento il programma ne sceglie uno utilizzando un'euristica. Tuttavia � consigliabile specificare un ordinamento, per evitare la generazione di tabelle aventi dimensioni ingestibili.
Per evitare di bloccare il computer � stato inserito un limite alla dimensione delle tabelle pari a 100000000, se si supera tale valore si viene avertiti e il programma termina.

Un esempio di configurazione di lancio �:
./BeliefPropagation ./RetiEsempio/Munin1.net ./RetiEsempio/Munin1.num