#!/bin/bash

source envMDT.sh

cd ./cpp
make clean
make all
cd ../app/utilities/WCRootData
make clean
make all
cd $MDTROOT
echo `pwd`
cd ./app/application
make appHKHybridSingleEvent

# Required only if you want python module
#cd pywrap
#make clean
#make all
#cd ..
#`ls -d ./pywrap/* | grep .so | xargs -l ln -s`
