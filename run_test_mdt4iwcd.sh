#!/bin/bash


EXE=$MDTROOT/app/application/appWCTESingleEvent

WCSIM_FILE=/disk1/disk01/usr5/rakutsu/iwcd/neutron/beam/MassProNov2020_sukap/files/wcsim_root/NuMode/2p39/iwcd_p320ka_w750m_1e17pot_2p39_wcsim.00000.root
CONFIG_FILE=$MDTROOT/parameter/MDTParamenter_IWCD.txt
OUT_FILE=out_appWCTESingleEvent_iwcd.root
SEED=65457869

$EXE -i $WCSIM_FILE\
	 -p $CONFIG_FILE\
	 -o $OUT_FILE\
	 -s $SEED\
	 -n -1 # to run all events
