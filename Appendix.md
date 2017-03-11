## 1. Introduction
## 2. Installation
Before you can run anything, you need to install the dependencies, NS-3 and DCE.
### 2.1 Dependency Libraries
Use the following command to get all libraries along with NS-3 and DCE has to be installed.
``` bash
# Dependencies
sudo apt-get install gcc g++ python python-dev qt4-dev-tools libqt4-dev \
                     mercurial bzr cmake libc6-dev g++-multilib gsl-bin \
                     libgsl0-dev libgsl2 flex bison libfl-dev tcpdump sqlite \ 
                     sqlite3 libsqlite3-dev libxml2 libxml2-dev \
                     libgtk2.0-0 libgtk2.0-dev vtun lxc doxygen graphviz \ 
                     imagemagick git python-pygraphviz python-pygoocanvas \
                     libgoocanvas-dev python-setuptools libpcap-dev libdb-dev \ 
                     libssl-dev lksctp-tools libsctp-dev tshark gnuplot cvs \ 
                     unrar p7zip-full autoconf
```
### 2.2 NS-3 with DCE
Run the script below to install NS-3 with DCE.
``` bash
# !/bin/bash

cd ~

# Dependencies
sudo apt-get install gcc g++ python python-dev qt4-dev-tools libqt4-dev \
                     mercurial bzr cmake libc6-dev g++-multilib gsl-bin \
                     libgsl0-dev libgsl2 flex bison libfl-dev tcpdump sqlite \ 
                     sqlite3 libsqlite3-dev libxml2 libxml2-dev \
                     libgtk2.0-0 libgtk2.0-dev vtun lxc doxygen graphviz \ 
                     imagemagick git python-pygraphviz python-pygoocanvas \
                     libgoocanvas-dev python-setuptools libpcap-dev libdb-dev \ 
                     libssl-dev lksctp-tools libsctp-dev tshark gnuplot cvs \ 
                     unrar p7zip-full autoconf

# Installing the bake tool in the directory you're currently in.
hg clone http://code.nsnam.org/bake bake
export BAKE_HOME=`pwd`/bake
export PATH=$PATH:$BAKE_HOME
export PYTHONPATH=$PYTHONPATH:$BAKE_HOME

# Create the directory in which to download and build the DCE variant of NS-3.
# It has only been tried with version 1.7. Try another version at your own risk.
mkdir ns3-dce
cd ns3-dce
bake.py configure -e dce-linux-1.7
bake.py download

sed -i '75s/.*//' source/ns-3-dce/test/test-tsearch.cc

bake.py build
```
If there are a problem with installing the dce-linux-1.7.
You could try doing this (as some computers have a problem with the python code in the installation script):

# CARL STUFF PYTHON STUFF

![The original installation instructions from the NS-3 webpage.](https://www.nsnam.org/docs/dce/manual/html/getting-started.html#building-dce-basic-mode)


### 2.3 System settings
Most Linux systems place restrictions on how many user processes can be run at the same time, and how many files each process can open. This project needs to run multiple NS-3 simulation instances in order to generate useful network statistics, which creates a lot of files and processes. Therefore it is necessary to append the following lines to the end of 
``` bash
/etc/security/limits.conf
```

```
*         hard    nproc       65536
*         soft    nproc       65536
*         hard    nofile      65536
*         soft    nofile      65536
```
Restart your computer to apply the settings. For more information see the [DCE Manual](https://www.nsnam.org/docs/dce/release/1.4/manual/singlehtml/index.html#processes-limit-resource-temporarily-unavailable).
### 2.4 Downloading the project
Assuming that the NS-3 DCE installation directory is:
``` bash
export NS3_HOME="$HOME/dce"
```

``` bash
cd $NS3_HOME/source/ns-3-dce
git init
git remote add origin https://github.com/Kininia/Network-Protocol-Simulation.git
git fetch
git checkout -t origin/master
```
	
## 3. Running the simulator
The simulator is built to run in the system terminal. The simulator first runs a simulation for each of the network protocols implemented then it parses the data from the dumped .pcap files into .dat files for the plotting program. Each protocol implemented has both a client and a server class. A diagram of the entire simulator is shown in figure 1.
![Simulator class diagram](https://cloud.githubusercontent.com/assets/6905219/23823844/438f9308-066b-11e7-8e4f-9a186e3cd4b7.PNG)
Running the simulator is done through the terminal.

### 3.1 Starting
To run the simulator, run the following commands
``` bash
./waf configure --with-ns3=$NS3_HOME/build --prefix=$NS3_HOME/build \
                --enable-kernel-stack=$NS3_HOME/source/net-next-sim-2.6.36/arch
./waf build
./waf --run my-simulator

```	
### 3.2 Plotting
The plotting program is separate from the simulation. To plot the results from a simulation, run:
``` bash
cd $NS3_HOME/source/ns-3-dce
build/myscripts/my-simulator/bin/./NSplot output.png my-simulator-output/*.dat -2d
```
### 3.3 Configuration Simulation Parameters
There are several premade scripts that are ready to be run. The scripts are named *SimRED* or *SimREG* depending on if it uses *RED* or not. Then they are named after which protocols they are running. For example *SimREG_All*, runs all of the scripts. While *SimRED_SCTP*, runs the *SCTP* protocol with *RED*. The different scripts that are premade are:
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
```
sim settings.wifi std = WIFI PHY STANDARD 80211a;
sim settings.wifi mode = "OfdmRate54Mbps";
```
Currently there are five different Wifi standards implemented.
```
WIFI PHY STANDARD 80211a
WIFI PHY STANDARD 80211b
WIFI PHY STANDARD 80211g
WIFI PHY STANDARD 80211n 2 4 GHZ
WIFI PHY STANDARD 80211ac
```
There are also many different data rates implemented. Here are a few examples.
```
"OfdmRate6Mbps"
"OfdmRate9Mbps"
"OfdmRate12Mbps"
"OfdmRate18Mbps"
"OfdmRate24Mbps"
"OfdmRate36Mbps"
"OfdmRate48Mbps"
"OfdmRate54Mbps"
```

#### 3.3.2 Simulation stop time
By changing the ```sim_settings.sim_stop_time_sec``` variable, one changes the max time the simulator is going to run. It is currently set to 10000 seconds. 
#### 3.3.3 Number of clients
Adding more clients to the simulator is done through changing the ```sim_settings.number_of_clients``` variable. The simulator will always have one server node in addition to the clients.
#### 3.3.4 Transfer Bytes
Currently the amount of bytes to transfer from each client is 1 megabyte. This can be increased or decreased by changing the ```sim_settings.transfer_data_bytes``` variable.
#### 3.3.5 Socket and Streams
The number of SCTP streams and the number of TCP & DCCP connections aswell as the number of UDP sockets are all configured by the ```sim_settings.num_sockets_streams``` variable.
#### 3.3.6 SCTP
For SCTP both the time-to-live and unordered packet option can be configured. Changing the ```sim_settings.sctp_ttl_ms``` variable configures the time-to-live in milliseconds with 0 such that time-to-live will be disabled. Setting ```sim_settings.sctp_unordered``` variable to 0 sends the data in-order and setting it to 1 sends the data unordered.
#### 3.3.7 UDP and DCCP
Both UDP and DCCP send rates can be configured with the 
```
sim_settings.udp_send_rate_kbytes_sec
sim_settings.dccp_send_rate_kbytes_sec
```
variables. Both are measured in kilobytes/second.
#### 3.3.8 Cycles
Configuring cycles changes if the simulator should run on/off sources or a single continuous stream. The sim settings.num cycles variable changes how many cycles is going to be run. Giving a total data sent per client = transfer data bytes * num cycles. The time between each cycle is configured by the ```sim_settings.time_between_cycles_usec``` variable and is measured in microseconds.
#### 3.3.9 Output
The output directory for the simulator can be configured by the ```sim_settings.output_dir``` variable and is currently set to ```”/my-simulator-output/”```.
#### 3.3.10 Variable Span
In order to change what variable span to iterate over the int *var variable has to be altered. It takes the memory address of the variable of iteration. One also has to change the ```int min, max, inc``` variables to fit the new iteration variable and if necessary change the loop itself.