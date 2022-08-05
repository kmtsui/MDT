#!/bin/bash

# This script runs appHKHybridSingleEvent
# True hits saved in your input file will be digitized,
# and the resultant digitized hits are saved in an output root
# file with the same format as the input file

# Set **PATH_TO_MDT** in $MDTROOT/MDTParamenter_Hybrid.txt 
# to the full path to your MDT properly

# Similarly, set the below PATH_TO_MDT properly

CURDIR=`pwd`
MDTDIR=_MDTROOT_
cd $MDTDIR
source envMDT.sh

cd $CURDIR

EXE=$MDTROOT/app/application/appHKHybridSingleEvent

# Running configuratino (e.g. TTS, dark rate, threshold on NDigiHits algorithm, etc)
PARFILE=$MDTROOT/MDTParamenter_Hybrid.txt

# Input file shold contains 
#	- wcsimrootgeom
#	- wcsimrootevent
#	- wcsimrootevent2
INFILE=OUTPUT_OF_WCSIM

# Output file name. Should end with .root
OUTFILE=OUTPUT_OF_THISAPP

# Seed to initialize random number generators used in MDT
SEED=65834

$EXE -i $INFILE\
	 -o $OUTFILE\
	 -p $PARFILE\
	 -s $SEED
