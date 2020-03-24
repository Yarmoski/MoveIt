# MoveIt
A delivery logistics system built in C++. Given OSM (OpenStreetMap) street map data, this system finds an efficient route to one or more delivery points specified by the user. MoveIt utilizes methods such as the heuristic A* search algorithm, simulated annealing, and more to accomplish this task.

The program will take these files and output to the console a turn-by-turn set of directions from a starting point to each delivery location and back to the starting point. These directions include the distances in miles for each direction and the total miles traveled for all deliveries.

# How to use
The program takes in two text (.txt) files, one of street segments (OSM data) and one of deliveries (locations and items). Once these are supplied in the correct formats (detailed in the formatting section), a path will be provided if it exists.

## Data File (.txt) formatting

Sample data files are provided in this repository.
mapdata.txt is a street map data file and deliveries.txt is a deliveries data file. 

### Street Map Data File
- Name of Street we’re about to provide geolocation data for 
- A count C of how many segments there are for this particular street. 
- Next C lines hold the starting and ending geo-coordinates of the street segments in latitude, longitude format as four numbers separated by whitespace: start_latitude start_longitude end_latitude end_longitude.

This pattern is continued throughout the file.

### Deliveries Data File
- Start_Latitude Start_Longitude  
- Delivery1_Latitude Delivery1_Longitude: item to be delivered 
- Delivery2_Latitude Delivery2_Longitude: item to be delivered
- ...
- DeliveryN_Latitude DeliveryN_Longitude: item to be delivered 

