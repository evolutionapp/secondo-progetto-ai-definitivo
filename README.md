# README #


Questa repository contiene una modifica del seguente progetto [MCA-RMCA](https://github.com/nobodyczcz/MCA-RMCA).
Per ulteriori informazioni e librerie necessarie consultare il progetto originale.


## Compilazione

### Dipendenze

Per installare le librerie necessarie alla compilazione del codice è necessario eseguire i seguenti comandi:

```
$ sudo apt-get update
$ sudo apt-get install sparsehash
$ sudo apt-get install libboost-all-dev
```

Clona questa repository.

```
$ mkdir build
$ cd build
$ cmake ../
$ make
```

## Premessa 

Il progetto MCA-RMCA offre una soluzione alternativa al problema Path-Finding e Task Assignment eseguendo le operazioni contemporaneamente. Come si può vedere dai risultati mostrati dal paper di riferimento [Integrated Task Assignment and Path Planning for Capacitated Multi-Agent Pickup and Delivery](https://arxiv.org/abs/2110.14891), questa soluzione migliora le performance rispetto gli algoritmi classici. Il nostro progetto ha come obiettivo l'analisi dei singoli problemi (Path-Finding e Task Assignment).  

## Descrizione del progetto
Il progetto prende in input:
1. una mappa contenente informazioni riguardanti gli agenti, gli ostacoli e le stazioni
2. un insieme di task (start, goal)
3. un assegnamento dei task ai vari agenti


Restituendo in output i percorsi di ogni agente privi di conflitti.
Per farlo viene utilizzato un algoritmo chiamato CBS (Conflict-based search) che tramite una matrice registra la posizione degli agenti nel tempo prevedendo e risolvendo eventuali conflitti cambiando, all'occorrenza, il percorso degli agenti interessati.
Di seguito una breve descrizione del funzionamento:

-Crea una struttura dati contenente le informazioni sulla posizione occupata dai robot nel tempo durante lo svolgimento dei task.

-Calcola il percorso migliore attraverso un algoritmo di path-finding considerando le posizioni occupate dagli altri robot come ostacoli. Ovviamente l'algoritmo è più elaborato in quanto prende in considerazione la variabile tempo, essendo gli "ostacoli" mobili.

Per maggiori dettagli consultare il seguente documento [Conflict-based search for optimal multi-agent pathfinding](https://www.sciencedirect.com/science/article/pii/S0004370214001386#fg0050).
L' utilizzo di questo algoritmo nel paper di [riferimento](https://github.com/nobodyczcz/MCA-RMCA) si trova nell' Algoritmo 2.

## Esempi d'uso e Argomenti
Mostriamo il comando di esecuzione:
```
./MAPD 
-m kiva-20-500-5.map 
-a kiva-20-500-5.map 
-A assegnamento.txt
-t 0.task 
--capacity 1 
-o output_PP_2.txt 
```

**-m [path]** indica la mappa da caricare 

**-a [path]** indica gli agenti da caricare

**-A [path]** indica l'assegnamento dei task agli agenti da caricare

**-t [path]** indica i task da caricare

**--capacity** capacità massima degli agenti

**-o** specifica il file di output

N.B. il file di assegnamento deve avere il seguente formato:
```
20  //numero di agenti
100 //numero di task
0 15 31 37 41 70 79 98 -1   //assegnamento dei task all'agente 0
1 42 50 71 88 94 99 -1  
2 14 23 40 47 58 -1
3 10 61 83 91 -1
4 7 25 48 80 89 -1
5 67 69 78 81 -1
```
Dove i primi parametri rappresentano il numero di agenti e il numero di task. 
Mentre i valori successivi mostrano l'assegnamento dei task all'agente. 
Il -1 rappresenta la fine della riga di assegnamenti ad un agente.
Può essere prodotto dal programma [ai-lab-progetto](https://github.com/evolutionapp/ai-lab-1-progetto).

## Output 
L' output generato si presenterà nella seguente forma:
```
AZIONI AGENTI : 
Agent: 0, Cost: 154, Capacity: 1, Task amount: 7, Actions:
<0,(4,31),delay0, START ,r0>
<1,(4,30),delay0, PICK UP ,r0>
<2,(5,30),delay1, DROP OFF ,r0>
...
PERCORSI AGENTI : 
Agent: 0, Delay: 308 , Cost: 154, plan: 
		0 : (4,31)->
		1 : (4,30)->
		2 : (5,30)->
		3 : (4,30)->
		4 : (3,30)->
		5 : (2,30)->
		6 : (2,29)->
```
L'output è diviso in due sezioni:

**-Azioni Agente**
Per ogni agente specifica le azioni che deve compiere che possono essere di tipo START, PICK UP, DROP OFF e DOCKER.
Ogni azione è formato dalla seguente tupla (ideal_action_time, posizione, delay, tipo, tempo di rilascio).

**-Percorsi Agenti**
Per ogni agente specifica il percorso senza conflitti calcolato dal programma. Il percorso presenta il timestamp e la relativa posizione dell'agente in quell'istante. 

## Esempio di esecuzione e risultati
Di seguito presentiamo un esempio di esecuzione del programma in cui viene mostrato il calcolo del percorso e la risoluzione dei conflitti.
Nella prima parte viene presentato il task da svolgere con i relativi parametri:

**-0** indica il tempo al quale viene generato il task

**-61** indica la coordinata di start del task 

**-62** indica la coordinata di goal del task

**-Altri parametri** non necessari nell'esempio proposto

#### Task
```
0	61	62	0	0
0	62	61	0	0
```
Di seguito presentiamo gli assegnamenti dei task sopra riportati agli agenti. 

Il task 0 <0, 61, 62, 0, 0> è assegnato all'agente 1.

Il task 1 <0, 62, 61, 0, 0> è assegnato all'agente 1.

```
20 //numero di agenti
2 //numero di task
//idAgente idTasks(-1 numero di terminazione riga)
0  -1
1 0 -1
2 1 -1
3 -1
4 -1
5 -1
6 -1
7 -1
8 -1
9 -1
10 -1
11 -1
12 -1
13 -1
14 -1
15 -1
16 -1
17 -1
18 -1
19 -1
```
#### Mappa
Presentiamo la mappa di riferimento interpretata nel seguente modo:

**e** indica una stazione di start o goal per un task

**.** indica lo spazio percorribile

**@** indica un ostacolo

**r** indica un agente

A1 e A2 rappresentano gli agenti a cui sono stati assegnati i task e T1 e T2 i relativi task da svolgere. 

A1 porterà T1 in posizione T2

A2 porterà T2 in posizione T1

In questo scenario per portare a termine il loro compito i due agenti dovrebbero scontrarsi ovvero creare un conflitto.

```
...................................
.ee..e.eeeeeeeeee.eeeeeeeeee.e..ee.
.e..ee.@@@@@@@@@@.@@@@@@@@@@.ee.ee.
.ee.ee.eeeeeeeeee.eeeeeeeeee.er.ee.
..e.T1A1.......................T2A2.e..
.rr.e..eeeeeeeeee.eeeeeeeeee.er.e..
.ee.ee.@@@@@@@@@@.@@@@@@@@@@.ee..e.
.ee..e.eeeeeeeeee.eeeeeeeeee..e.re.
.e..re.......................ee.re.
....ee.eeeeeeeeee.eeeeeeeeee.e..rr.
.ee..e.@@@@@@@@@@.@@@@@@@@@@.e..e..
.ee.ee.eeeeeeeeee.eeeeeeeeee.er.ee.
.ee.ee........................e.re.
.er.ee.eeeeeeeeee.eeeeeeeeee.e..re.
..e.ee.@@@@@@@@@@.@@@@@@@@@@.ee.e..
.ee.e..eeeeeeeeee.eeeeeeeeee..r..r.
.ee..e.......................ee.er.
..e.ee.eeeeeeeeee.eeeeeeeeee.ee.er.
.ee.ee.@@@@@@@@@@.@@@@@@@@@@.ee.re.
..e..e.eeeeeeeeee.eeeeeeeeee.ee....
...................................
```
#### Percorso
Come mostrato nella seguente porzione di risultato il programma riconosce e risolve i conflitti tra i percorsi. Infatti i due agenti dovrebbero incorntrarsi al tempo 14 ma per evitare collisioni all'agente 2 viene assegnato un percorso diverso che lo fa deviare verso (4,18).   
```
Agent: 1			Agente: 2
10 : (5,14)->			10 : (5,21)->
11 : (5,15)->			11 : (5,20)->
12 : (5,16)->			12 : (5,19)->
13 : (5,17)->			13 : (5,18)->
14 : (5,18)->			14 : (4,18)->
15 : (5,19)->			15 : (4,17)->
16 : (5,20)->			16 : (4,16)->

```
I percorsi calcolati dal programma minimizzano il total-travel time e sono privi di conflitti.

### Vedi anche 
[
ai-lab-1-progetto](https://github.com/evolutionapp/ai-lab-1-progetto)









