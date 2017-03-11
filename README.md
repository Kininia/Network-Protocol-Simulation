## Welcome to the Pr-SCTP Simulation Project

###About

###Dependencies

###Installation script for environment
~~~~ bash
#!/bin/bash

cd ~

sudo apt-get install gcc g++ python python-dev qt4-dev-tools libqt4-dev mercurial bzr cmake libc6-dev g++-multilib\
gsl-bin libgsl0-dev libgsl2 flex bison libfl-dev tcpdump sqlite sqlite3 libsqlite3-dev libxml2 libxml2-dev\
libgtk2.0-0 libgtk2.0-dev vtun lxc doxygen graphviz imagemagick git python-pygraphviz python-pygoocanvas \
libgoocanvas-dev python-setuptools libpcap-dev libdb-dev libssl-dev lksctp-tools libsctp-dev tshark gnuplot cvs\

unrar p7zip-full autoconf

hg clone http://code.nsnam.org/bake bake
export BAKE_HOME=`pwd`/bake
export PATH=$PATH:$BAKE_HOME
export PYTHONPATH=$PYTHONPATH:$BAKE_HOME

mkdir ns3-dce
cd ns3-dce
bake.py configure -e dce-linux-1.7
bake.py download

sed -i '75s/.*//' source/ns-3-dce/test/test-tsearch.cc

bake.py build
~~~~
[Link to installation script on NS3 webpage](https://www.nsnam.org/docs/dce/manual/html/getting-started.html#building-dce-basic-mode)

###Installation of RED
~~~~
cd $NS3_HOME/source/ns-3-dce
git init
git remote add origin https://github.com/Kininia/Network-Protocol-Simulation.git
git fetch
git checkout -t origin/master
~~~~


### Project Link

[Link to Project Material](https://drive.google.com/drive/u/2/folders/0B5gIZlC2RN2oTGNRRjVTQ3BGSGc)

#### TODO list
[Link to backlog 1](https://docs.google.com/spreadsheets/d/1MV2RN98cWmCtaJ1_0CGGi7_4Nv7sKSASS3jTShc4Nj8/)
[Link to backlog 2](https://docs.google.com/spreadsheets/d/1n70baUivFsSjJZWA9vRQaaQj7RgWKoa_FzVCKOycr-w/)

##### RED document
[Link to RED DOCUMENT 1](http://www.diva-portal.org/smash/get/diva2:831714/FULLTEXT01.pdf)

[Link to RED DOCUMENT 2](http://www.icir.org/floyd/papers/early.pdf)
