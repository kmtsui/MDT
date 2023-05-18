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

# Replace _MDTROOT_ with $MDTROOT
# If you do not use the below .txt and .sh files, 
# the following does nothing for you
cd $MDTROOT
TMPFILE=./parameter/MDTParamenter_Hybrid.txt
sed -i --expression "s@_MDTROOT_@$MDTROOT@" $TMPFILE
TMPFILE=./example/run_appHKHybridSingleEvent.sh
sed -i --expression "s@_MDTROOT_@$MDTROOT@" $TMPFILE
