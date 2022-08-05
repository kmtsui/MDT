#!/bin/bash

export MDTROOT=`pwd`
if [[ -z "${LD_LIBRARY_PATH}" ]]; then
	export LD_LIBRARY_PATH=$MDTROOT/cpp
else
	export LD_LIBRARY_PATH=${MDTROOT}/cpp:${LD_LIBRARY_PATH}
fi

#--- ROOT
ROOTDIR=/usr/local/sklib_gcc8/root_v6.22.06_python3.8/
source $ROOTDIR/bin/thisroot.sh

#--- WCSim
export WCSIMDIR=/home/rakutsu/disk2/iwcd/wcsim/NewComputer/WCSim-build/ROOT6/nuPRISM/develop/
export LD_LIBRARY_PATH=${WCSIMDIR}:$LD_LIBRARY_PATH

#--- MDT's utility
export WCRDROOT=$MDTROOT/app/utilities/WCRootData
export LD_LIBRARY_PATH=${WCRDROOT}:$LD_LIBRARY_PATH
