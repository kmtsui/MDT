#!/bin/bash

drop_from_path()
{
   # Assert that we got enough arguments
   if test $# -ne 2 ; then
      echo "drop_from_path: needs 2 arguments"
      return 1
   fi

   local p=$1
   local drop=$2

   newpath=`echo $p | sed -e "s;:${drop}:;:;g" \
                          -e "s;:${drop}\$;;g"   \
                          -e "s;^${drop}:;;g"   \
                          -e "s;^${drop}\$;;g"`
}

### main ###

## clean old environment
if [ -n "${MDTROOT}" ] ; then
	old_mdtroot=${MDTROOT}

   	if [ -n "${LD_LIBRARY_PATH}" ]; then
		drop_from_path "$LD_LIBRARY_PATH" "${old_mdtroot}/cpp"
		LD_LIBRARY_PATH=$newpath

		drop_from_path "$LD_LIBRARY_PATH" "${WCSIMDIR}"
		LD_LIBRARY_PATH=$newpath

		drop_from_path "$LD_LIBRARY_PATH" "${WCRDROOT}"
		LD_LIBRARY_PATH=$newpath
	fi
fi

if [ "x${BASH_ARGV[0]}" = "x" ]; then
    MDTROOT="$PWD"; export MDTROOT
else
    # get param to "."
    thismdtroot=$(dirname ${BASH_ARGV[0]})
    MDTROOT=$(cd ${thismdtroot} > /dev/null;pwd); export MDTROOT
fi

if [[ -z "${LD_LIBRARY_PATH}" ]]; then
	export LD_LIBRARY_PATH=$MDTROOT/cpp
else
	export LD_LIBRARY_PATH=${MDTROOT}/cpp:${LD_LIBRARY_PATH}
fi

#--- ROOT
if ! [ -x "$(command -v root-config)" ]; then
    echo 'root-config could not be found, consider setup ROOT first'
    return 
else
	ROOTDIR=$(root-config --prefix)
	echo "Using ROOT installed in $ROOTDIR"
fi

#--- WCSim
if [[ -z "${WCSIMDIR}" ]]; then
	echo "WCSIMDIR not set. Exit!"
	return 
else
	echo "Using WCSIM installed in $WCSIMDIR"
	export LD_LIBRARY_PATH=${WCSIMDIR}:$LD_LIBRARY_PATH
fi

#--- MDT's utility
export WCRDROOT=$MDTROOT/app/utilities/WCRootData
export LD_LIBRARY_PATH=${WCRDROOT}:$LD_LIBRARY_PATH

#--- Option to read hybrid WCSIM files
if [[ -z "${HYBRIDWCSIM}" ]]; then
	echo "HYBRIDWCSIM not set. Default to 0"
	export HYBRIDWCSIM=0
elif [[ $HYBRIDWCSIM -eq 0 ]]; then
	echo "HYBRIDWCSIM set to 0"
	export HYBRIDWCSIM=0
else
	echo "HYBRIDWCSIM set to 1"
	export HYBRIDWCSIM=1
fi

#--- Option to read IWCD WCSIM files
if [[ -z "${IWCDWCSIM}" ]]; then
	echo "IWCDWCSIM not set. Default to 0"
	export IWCDWCSIM=0
elif [[ $IWCDWCSIM -eq 0 ]]; then
	echo "IWCDWCSIM set to 0"
	export IWCDWCSIM=0
else
	echo "IWCDWCSIM set to 1"
	export IWCDWCSIM=1
fi

echo "Setup complete!"
echo "MDTROOT=$MDTROOT"
echo "WCRDROOT=$WCRDROOT"

unset old_mdtroot
unset thismdtroot
unset -f drop_from_path