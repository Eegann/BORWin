# BORWin

## About the project
Code for the BORWin algorithm developed during Alexandre Heintzmann's thesis: Modèles et algorithmes pour l'optimisation de la production
hydro-électrique (Models and algorithms for the Hydro Unit Commitment problem)

## Prerequisite

C++ compiler g++

## Getting started

1. Clone the depo ``` git clone https://github.com/Eegann/BORWin.git ```
2. Open a terminal and go to the "src" folder of the projet ``` cd [project_location]/BORWin/src ```
3. Run the Makefile with parameter "all" ``` make all ```
4. Go to the root node of the project ``` cd ../ ```
5. Run the code with an instance as a parameter ``` ./BORWIN data/[instance_file] ```
6. Retrieve the output file at "out/[instance_file]"

## Instance descrption
An instance file is as follows.
The names that must be replaced by the proper values are between { }
Comments that must be removed are between [ ]

````
[the Following block is to be repeated for each node]
id {node ID} minResource {minimum resource to access the node} maxResource {maximum resource to access the node} arcs
	to {target node ID} value {value of the arc} resource {resource of the arc} [repeat the line for each arc from the node]
source {source node ID} target {target node ID}
````

### Example of instance
the following is [this instance file](data/instance_example.txt) corresponding to [this graph](graph_example.pdf) where:
  - The resource window of a vertex is in orange
  - The value of an arc is in blue
  - The resource of an arc is in green  

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
