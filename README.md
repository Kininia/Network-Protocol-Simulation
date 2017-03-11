## Welcome to the Pr-SCTP Simulation Project

### About
[Link to RED DOCUMENT](http://www.diva-portal.org/smash/get/diva2:831714/FULLTEXT01.pdf)

### Installation script for NS-3 environment
Using Ubuntu 16.04, run this script to install and build NS-3 DCE.
``` bash
# !/bin/bash

cd ~

# Dependencies
sudo apt-get install gcc g++ python python-dev qt4-dev-tools libqt4-dev \
                     mercurial bzr cmake libc6-dev g++-multilib gsl-bin \
                     libgsl0-dev libgsl2 flex bison libfl-dev tcpdump sqlite \ sqlite3 libsqlite3-dev libxml2 libxml2-dev \
                     libgtk2.0-0 libgtk2.0-dev vtun lxc doxygen graphviz \ imagemagick git python-pygraphviz python-pygoocanvas \
                     libgoocanvas-dev python-setuptools libpcap-dev libdb-dev \ libssl-dev lksctp-tools libsctp-dev tshark gnuplot cvs \ unrar p7zip-full autoconf

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
[Link to installation instructions on NS3 webpage](https://www.nsnam.org/docs/dce/manual/html/getting-started.html#building-dce-basic-mode)

### Required system settings
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

### Downloading and installation of RED
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

### Running
``` bash
./waf configure --with-ns3=$NS3_HOME/build --prefix=$NS3_HOME/build \
                --enable-kernel-stack=$NS3_HOME/source/net-next-sim-2.6.36/arch
./waf build
./waf --run my-simulator

```

### Plotting
The plotting program is separate from the simulation. To plot the results from a simulation, run:
``` bash
cd $NS3_HOME/source/ns-3-dce
build/myscripts/my-simulator/bin/./NSplot output.png my-simulator-output/*.dat -2d
```

### Useful commands
Output from the ``` DceApplicationHelper ``` processes, i.e. the SCTP server and client programs, are stored in the folders ```$NS3_HOME/source/ns-3-dce/files-*``` where the star is the NS-3 ```NodeContainer``` id number.

To print what was written to standard output with e.g. printf("debug output") run
``` bash
cd $NS3_HOME/source/ns-3-dce
find files-* -name 'stdout' -exec cat {} \;
```
Searching for all lines from standard output which contain the word "debug" can be done with
``` bash
find files-* -name 'stdout' -exec grep 'debug' {} \;
```

### Screenshots
The simulation interface is text based, but the program supports generation of NetAnim trace files in order to visualize node mobility. Below is a screenshot showing a network client sending data over a Wi-Fi network. 
![Screenshot of moving nodes](https://cloud.githubusercontent.com/assets/6905219/23822639/dd899208-0650-11e7-82c1-048080881562.png "Screenshot of moving nodes")