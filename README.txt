Readme

This code generates random caves and dungeons. There are two different styles of generation available:

1. Digger method: A point is chosen at random, and places a 'digger.' The digger selects a random direction and carves out a piece of the cave. There is a variable you can set which adjusts the probability that the digger will continue in the same direction. You can also set the stopping condition, which is the percentage of the total area of the map that has been dug out.

2. Cellular Automata method: Noise is generated over the area of the map, and then cellular automata are applied. A smoothing automaton is also available.

There are examples of how to use both types in cavegen.cpp

cavegen.h contains all of the functions, and it is pretty well commented. In here you can set the size of the maps you want to create.

samples.rtf contains a bunch of sample caves, with the input parameters and seed value needed to reproduce them.

This code was inspired by the TIGSource forum thread "Braving Procedural Generation" 