# BORWin

## About the project

This is the code for the BORWin algorithm developed during Alexandre Heintzmann's thesis: Modèles et algorithmes pour l'optimisation de la production
hydro-électrique (Models and algorithms for the Hydro Unit Commitment problem).

### Prerequisite

Being able to compile C++ code, through a Makefile, or with an IDE.

### Getting started with the test instance

1. Clone the depo: ``` git clone https://github.com/Eegann/BORWin.git ```
2. Compile and run the code:
   - For users compiling C++ code with the Makefile:
	1. Open a terminal and go to the "src" folder of the projet: ``` cd [project_location]/BORWin/src ```
	2. Run the Makefile with parameter "all": ``` make all ```
	3. Run the code "BORWin" executable file generated, with the name of an instance file as a parameter: ``` ./BORWIN data/instance_test.txt ```
   - For users using an IDE, please refer to your IDE manual to compile and run the code.
3. Retrieve the output file at "[project_location]/BORWin/out/instance_test.csv".

## File description

### Instance descrption
Instance files are located in folder "BORWIN/data", and must be defined as follows.
The names that must be replaced by the proper values are between { }.
Comments that must be removed are between [ ].

````
[the Following block is to be repeated for each node]
id {node ID} minResource {minimum resource to access the node} maxResource {maximum resource to access the node} arcs
	to {target node ID} value {value of the arc} resource {resource of the arc} [repeat the line for each arc from the node]
source {source node ID} target {target node ID}
````

### Example of instance file
the following is [this instance file](data/instance_example.txt) corresponding to [this graph](graph_example.pdf) where:
  - The resource window of a vertex is in orange.
  - The value of an arc is in blue.
  - The resource of an arc is in green.

````
id 0 minResource 0 maxResource 0 arcs
	to 1 value 5 resource 10
	to 2 value 1 resource 4
id 1 minResource 5 maxResource 10 arcs
	to 2 value 4 resource 6
	to 3 value 10 resource 2
id 2 minResource 4 maxResource 20 arcs
	to 3 value 8 resource 8
id 3 minResource 12 maxResource 25
source 0 target 3
````

### Output file description

Output file are located in folder "BORWIN/out".
the following is [this output file](out/instance_example.csv) corresponding to the instance file previously mentioned.

````
Objective value:,17
Resource used:,24
Time:,0.002, First phase:,0, Second phase:,0.002
#Iterations:,1
Path:,(0),(1),(2),(3)
````
The first two lines indicate the value and the amount of resource of the optimal solution.
The third line provides the total execution time, as well as the execution time for each phase of the BORWin algorithm.
The fourth line gives the number of iteration for the second phase.
The fifth line is the optimal solution obtained.


