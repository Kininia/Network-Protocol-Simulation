#!/bin/bash

cd ~

sudo apt-get install gcc g++ python python-dev qt4-dev-tools libqt4-dev mercurial bzr cmake libc6-dev g++-multilib gsl-bin libgsl0-dev libgsl2 flex bison libfl-dev tcpdump sqlite sqlite3 libsqlite3-dev libxml2 libxml2-dev libgtk2.0-0 libgtk2.0-dev vtun lxc doxygen graphviz imagemagick git python-pygraphviz python-pygoocanvas libgoocanvas-dev python-setuptools libpcap-dev libdb-dev libssl-dev lksctp-tools libsctp-dev tshark gnuplot cvs unrar p7zip-full autoconf

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