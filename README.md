# README #


Questa repository contiene una modifica del seguente progetto [MCA-RMCA](https://github.com/nobodyczcz/MCA-RMCA).
Per ulteriori informazioni e librerie necessarie consultare il progetto originale.


## Compilazione


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
Per farlo viene utilizzato un algoritmo che tramite una matrice registra la posizione degli agenti nel tempo prevedendo e risolvendo eventuali conflitti cambiando, all'occorrenza, il percorso degli agenti interessati.

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
**Azioni Agente**
Per ogni agente specifica le azioni che deve compiere che possono essere di tipo START, PICK UP, DROP OFF e DOCKER.
Ogni azione è formato dalla seguente tupla (ideal_action_time, posizione, delay, tipo, tempo di rilascio).
**Percorsi Agenti**
Per ogni agente specifica il percorso senza conflitti calcolato dal programma. Il percorso presenta il timestamp e la relativa posizione dell'agente in quell'istante. 

### Vedi anche 
[
ai-lab-1-progetto](https://github.com/evolutionapp/ai-lab-1-progetto)









