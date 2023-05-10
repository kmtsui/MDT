#!/bin/bash

CUR=`pwd`
TOPDIR=/home/hep/rakutsu/t2k/wcte/mdt_4_wcte
cd $TOPDIR/MDT
source envMDT.sh
cd $CUR

EXE=$MDTROOT/app/application/appWCTESingleEvent

WCSIM_FILE=./wcsim_mPMTmapping_401nm_FileID142_Absff1.000e+11_Rayff1.0e+11_x11.30_y-89.92_z32.92_t0.49_p1.24_R40.00.root
CONFIG_FILE=./MDTParamenter_WCTE.txt
OUT_FILE=out_appWCTESingleEvent.root
SEED=65457869

$EXE -i $WCSIM_FILE\
	 -p $CONFIG_FILE\
	 -o $OUT_FILE\
	 -s $SEED\
	 -n 10
