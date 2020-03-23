# MoveIt
A delivery logistics system built in C++. Given OSM (OpenStreetMap) street map data, this system finds an efficient route to one or more delivery points specified by the user. MoveIt utilizes methods such as the heuristic A* search algorithm, simulated annealing, and more to accomplish this task. 
Currently, the program will take these files and output to the console a turn-by-turn set of directions from a starting point to each delivery location and back to the starting point. These directions include the distances in miles for each direction and the total miles traveled for all deliveries.

# How to use
The program takes in two text (.txt) files, one of street segments (OSM data) and one of deliveries (locations and items). 
