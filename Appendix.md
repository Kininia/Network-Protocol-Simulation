

D0020E
PR-SCTP over IEEE 802.11
Appendix


March 11, 2017
<style>
{list-style-type:decimal;
}
</style>
1. [Introduction](#Introduction)
2. [Installation](#Installation)
    1. Dependency Libraries
    2. NS-3 with DCE
    3. System Settings
    4. Downloading the project
3. Running the simulator
    1. Starting
    2. Plotting
    3. Configuration Simulation Parameters
        1. WiFi Standard
        2. Simulation stop time


## 1. Introduction
## 2. Installation
	Before you can run anything, you need to install the dependencies, NS-3 and DCE.
### 2.1 Dependency Libraries
Use the following command to get all libraries along with NS-3 and DCE has to be installed.
	
### 2.2 NS-3 with DCE
	Run the script below to install NS-3 with DCE.

If there are a problem with installing the dce-linux-1.7.
You could try doing this (as some computers have a problem with the python code in the installation script):

# CARL STUFF PYTHON STUFF
![Screenshot of moving nodes](https://cloud.githubusercontent.com/assets/6905219/23822639/dd899208-0650-11e7-82c1-048080881562.png "Screenshot of moving nodes")

The original installation instructions from the NS-3 webpage.


### 2.3 System settings
Most Linux systems place restrictions on how many user processes can be run at the same time, and how many files each process can open. This project needs to run multiple NS-3 simulation instances in order to generate useful network statistics, which creates a lot of files and processes. Therefore it is necessary to
append the following lines to the end of 

Restart your computer to apply the settings. For more information see the DCE manual
### 2.4 Downloading the project
	Assuming that the NS-3 DCE installation directory is:
	
## 3. Running the simulator
The simulator is built to run in the system terminal. The simulator first runs a simulation for each of the network protocols implemented then it parses the data from the dumped .pcap files into .dat files for the plotting program. Each protocol implemented has both a client and a server class. A diagram of the entire simulator is shown in figure 1.

Running the simulator is done through the terminal.

### 3.1 Starting
To run the simulator, run the following commands	
### 3.2 Plotting
The plotting program is separate from the simulation. To plot the results from a simulation, run:

### 3.3 Configuration Simulation Parameters
There are several premade scripts that are ready to be run. The scripts are named SimRED or SimREG depending on if it uses RED or not. Then they are named after which protocols they are running. For example SimREG_All, runs all of the scripts. While SimRED_SCTP, runs the SCTP protocol with RED. The different scripts that are premade are:
```
SimREG_All
SimREG_DCCP
SimREG_TCP
SimREG_UDP
SimREG_SCTP
SimRED_All
SimRED_DCCP
SimRED_TCP
SimRED_UDP
SimRED_SCTP
```

In order to change the parameters for the simulator the main section of the script has to be edited. 
#### 3.3.1 Wifi standard
The following lines has to be edited in order to change what Wifi standard the simulator uses.

Currently there are five different Wifi standards implemented.

There are also many different data rates implemented. Here are a few examples.


#### 3.3.2 Simulation stop time
	By changing the variable, one changes the max time 
#### 3.3.3 Number of clients
#### 3.3.4 Transfer Bytes
#### 3.3.5 Socket and Streams
#### 3.3.6 SCTP
#### 3.3.7 UDP and DCCP
#### 3.3.8 Cycles
#### 3.3.9 Output
#### 3.3.10 Variable Span
NS-PLOT
Introduction
NS-PLOT is a module within the project PR-SCTP over IEEE 802.11 which is responsible for the visual representation of the simulation result. NS-PLOT is a seperate program, which will create plots from the output of the NS-3 simulation script. NS-PLOT takes one or more .dat files as input and output a single plot (represented as a .png file). 
Use of Gnuplot
NS-PLOT uses Gnuplot for plotting. Gnuplot is a command-line program that can generate two- and three-dimensional plots of functions,data and data fits. Its frequently used for publication-quality graphics as well as education. The program runs on all major computers and operating systems (GNU/Linux, Unix, Microsoft Windows, Mac OS X, and others). IT is a program with a fairly long history, dating back to 1986.

NS-PLOT is handling the destination of input and output files, as well as the desired parameters, labels and type of graph (2D and 3D). This information is then passed to Gnuplot via a system call (system() function) which will generate an output plot.
Input files
The software takes at least one .dat file as input (limited to one file for 3D-plotting). These files contain the simulation summary. Each row in a .dat file stores a specific information about one simulation, for example, by looking at the figure below, we can see that the total amount of frames sent was 8129 it took approximately 5.03 seconds to transfer the total data of 10.73 Megabytes of data, of which 10 megabytes was the raw (actual/useful) data and so on. More information about the parameters can be found in subsection 4.3.



Those simulation files should also be formatted according to a predefined pattern. The parameters should be separated by a single line space, there should be equal amount of parameters in each row, every parameter should either be a real number or be assigned a NaN (Not-a-Number) value.
Functionality
The software supports both 2D- and 3D plotting of simulation data. The user can assign one of 13 available parameters to each axis. The list of parameters is displayed below:

Frames (id= 1) - Number of frames (packets) sent during the transmission.
Transmission time (id = 2) - Total transmission time in seconds (s).
Data with headers (id = 3) - Total amount of data transferred (including header data) in Megabytes (Mb).
Data no headers (id = 4) - Total amount of data transferred (header data not included) in Megabytes (Mb). 
Data expected (id = 5) - Expected amount of raw data to be sent in Megabytes (Mb).
Data percentage (id = 6) - Percentage of raw data transferred.
Data loss percentage (id = 7) - Percentage of data that has been lost during the transmission. 
Transmission speed (id = 8) - Average transmission speed in Megabytes per seconds (Mb/s).
Average frame size (id = 9) - Average frame (packet) size for the transmission in bytes (b).
Data chunks (id = 10) -Number of data chunks transmitted (SCTP only). 
Average chunk size (id = 11) - Average chunk size for the transmission (SCTP only).
Clients (id = 12) - Number of clients which participated in the transmission
Streams/sockets per client (id = 13) - Number of streams/sockets per client. Streams are specific for SCTP protocol.

NS-PLOT supports viewing of multiple simulations on the same data plot. This option is only available for 2D-plotting and can be useful for comparing simulation results to each other. The labels and keys are set automatically by the software, according to the chosen parameters
Usage of NS-PLOT
NS-PLOT  is seperated from the NS-3 DCE simulation script. It is highly recomended that the C++ code is compiled by using the gcc compiler with -std=c++ flag. After the compilation the software can be started from the terminal by changing the directory to the one where the NS-PLOT executable file is stored. A new user of the software may use the command with the -help flag to get some basic information about the program, as described below.
./NSplot -help
Argument format

Creating a plot
Running NS-PLOT within the projekt

