# FleetTracker
Dugan's Dogs capstone project! Solar-powered vehicle tracking with real-time map visualization.

Setup instructions:
A and B necessary to complete setup

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

TeraTerm download and Tips:
Download latest version of TeraTerm [here](https://ttssh2.osdn.jp/index.html.en).
To use TeraTerm with the firmware:
1. Open Tera Term and select Serial. Select the appropriate USB Serial Port for the port. Click OK.
2. If the device does not show up as a serial port, the proper drivers may not be installed and can be found [here](https://www.ftdichip.com/FTDrivers.htm).
3. Go to Setup > Serial Port. In the dropdown menu for Speed, select the appropriate baudrate. Click OK.
4. Go to Setup > Terminal and select Local echo. In the Coding-receive and Coding-transmit dropdown menus, select "UTF-8". Click OK.
5. TeraTerm is ready to be used

UI Setup
1. If using Windows, we recommend using Anaconda to install all Python packages, found here: https://www.anaconda.com/distribution/#windows
2. The following packages are required for our UI to display correctly:
* serial:      conda install -c anaconda pyserial
* folium:      conda install -c conda-forge folium
* matplotlib:  conda install -c conda-forge matplotlib
* selenium:    conda install -c conda-forge selenium
* pyautogui:   conda install -c conda-forge pyautogui
3. To run the program:
* open an Anaconda terminal
* cd into the project directory
* run in the terminal:
	* python main.py <COM#>
