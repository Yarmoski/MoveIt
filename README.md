# MoveIt
A delivery logistics system built in C++. Given OSM (OpenStreetMap) street map data, this system finds an efficient route to one or more delivery points specified by the user. MoveIt utilizes methods such as the heuristic A* search algorithm, simulated annealing, and more to accomplish this task.

The program will take these files and output to the console a turn-by-turn set of directions from a starting point to each delivery location and back to the starting point. These directions include the distances in miles for each direction and the total miles traveled for all deliveries.

# How to use
The program takes in two text (.txt) files, one of street segments (OSM data) and one of deliveries (locations and items). Once these are supplied in the correct formats (detailed in the formatting section), a path will be provided if it exists.

## Data File (.txt) formatting

Sample data files are provided in this repository. These files contain street and delivery data from the UCLA area.

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

# Sample Output
This output uses the two sample data files.

```
Proceed northwest on South Barrington Avenue for 0.44 miles
Turn left on Santa Monica Boulevard
Proceed southwest on Santa Monica Boulevard for 0.05 miles
Turn right on Stoner Avenue
Proceed northwest on Stoner Avenue for 0.04 miles
Turn left on Santa Monica Boulevard
Proceed southwest on Santa Monica Boulevard for 0.40 miles
Turn right on South Bundy Drive
Proceed northwest on South Bundy Drive for 0.04 miles
Turn left on Rochester Avenue
Proceed southwest on Rochester Avenue for 0.05 miles
Turn right on South Bundy Drive
Proceed northwest on South Bundy Drive for 0.04 miles
Turn left on Texas Avenue
Proceed southwest on Texas Avenue for 0.10 miles
Turn right on South Bundy Drive
Proceed northwest on South Bundy Drive for 0.11 miles
DELIVER Pabst Blue Ribbon beer (Beta Theta Pi)
Proceed northeast on Wilshire Boulevard for 1.65 miles
Turn left on Gayley Avenue
Proceed north on Gayley Avenue for 0.07 miles
Turn right on Lindbrook Drive
Proceed east on Lindbrook Drive for 0.23 miles
Turn left on Hilgard Avenue
Proceed northeast on Hilgard Avenue for 0.67 miles
Turn right on Westholme Avenue
Proceed east on Westholme Avenue for 0.04 miles
Turn left on Thayer Avenue
Proceed northeast on Thayer Avenue for 0.13 miles
Turn left on Loring Avenue
Proceed east on Loring Avenue for 0.19 miles
DELIVER Chicken tenders (Sproul Landing)
Proceed southwest on Loring Avenue for 0.07 miles
Turn left on Warner Avenue
Proceed southeast on Warner Avenue for 0.34 miles
Turn right on Lindbrook Drive
Proceed southwest on Lindbrook Drive for 0.19 miles
Turn left on Westholme Avenue
Proceed southeast on Westholme Avenue for 0.61 miles
Turn right on Eastborne Avenue
Proceed southwest on Eastborne Avenue for 0.14 miles
Turn left on Manning Avenue
Proceed southeast on Manning Avenue for 0.23 miles
Turn right on Santa Monica Boulevard
Proceed southwest on Santa Monica Boulevard for 0.23 miles
Turn left on Selby Avenue
Proceed southeast on Selby Avenue for 0.08 miles
Turn right on Missouri Avenue
Proceed southwest on Missouri Avenue for 0.12 miles
Turn left on Glendon Avenue
Proceed southeast on Glendon Avenue for 0.13 miles
Turn right on La Grange Avenue
Proceed southwest on La Grange Avenue for 0.06 miles
Turn left on Westwood Boulevard
Proceed southeast on Westwood Boulevard for 0.18 miles
DELIVER B-Plate salmon (Eng IV)
Proceed northwest on Midvale Avenue for 0.39 miles
Proceed northwest on Kelton Avenue for 0.03 miles
Turn left on Santa Monica Boulevard
Proceed west on Santa Monica Boulevard for 0.51 miles
Turn left on Beloit Avenue
Proceed southeast on Beloit Avenue for 0.08 miles
Turn right on Iowa Avenue
Proceed west on Iowa Avenue for 0.46 miles
Proceed southwest on Nebraska Avenue for 0.22 miles
You are back at the depot and your deliveries are done!
8.23 miles travelled for all deliveries.
```
