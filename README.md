# Naval Battle Simulator

A C-based naval battle simulation developed as a university programming project.

## Project Description

The Naval Battle Simulator simulates a battleship (B) and multiple escort ships (E) moving and attacking within a defined area.

The project is divided into Part 1 and Part 2.

## Part 1

### Part 1-A
- Simulates the basic movement and attacking of the battleship.
- Calculates whether escort ships are within the attacking range.
- Calculates time taken for B's shots to reach E ships.
- Saves initial and final simulation conditions to text files.

### Part 1-B
- Adds gun jamming conditions to the simulation.
- Uses different firing angle restrictions before and after jamming.
- Simulations B1 and B2 are used to compare the jamming conditions.

### Part 1-C
- Extends Part 1-B with cumulative impact on the battleship.
- Simulates the effect of repeated attacks from escort ships.
- Records the final condition of the battleship and escort ships.

## Part 2

### Part 2-A
- Adds the time between consecutive B gun firings (`T_Bq`).
- Uses an attacking strategy to select escort ships.
- Records the attack order of E ships.
- Includes simulations based on Parts 1-A, 1-B and 1-C.

### Part 2-B
- Allows escort ships to fire continuously.
- Each escort ship type has its own firing interval (`T_E`).
- Simulates the effect of continuous E ship attacks.

### Part 2-C
- Extends Part 2-B with changing gun effectiveness.
- Battleship and escort ship impact power can change during the simulation.
- Records the final impact and survival status.

## Program Features

- Main menu for accessing different simulations
- Setup menu for simulation parameters
- Instructions menu
- Simulation statistics menu
- Full program option to run Part 1 and Part 2
- Random seed support
- Detailed simulation results saved as `.txt` files
- Previous simulation results can be viewed through Simulation Statistics

## Output

The terminal displays a brief summary of the simulation.

Detailed results are saved inside the `results` directory.

Each simulation run is stored in a separate run folder.
