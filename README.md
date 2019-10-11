# FleetTracker
Dugan's Dogs capstone project! Solar-powered vehicle tracking with real-time map visualization.

Setup instructions:
A and B necessary to complete set up

A. CCS Setup
1. Create new CCS project
2. In New CCS Project window, select MSP430F5529 for target and choose to create an empty project (without main.c)
3. In Project Explorer, right click on project, go to Add Files
4. Select relevant files and choose Link to Files (ensures compatibility with Git)

B. Adding "driverlib" folder
1. In Project Explorer, right click on project, go to Import > Import
2. In Import window, go to General > File System, then Next
3. Browse and select the directory (DirX) one step above driverlib folder, then OK
4. Dropdown DirX and check driverlib (not DirX), then Finish

C. Creating new files
1. Create file in local repo, but don't create it using CCS (i.e. use file explorer, terminal, etc)
2. Link to file in CCS (see A. 3-4)
