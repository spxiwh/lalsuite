#!/bin/bash

## allow 'make test' to work from builddir != srcdir
if [ -n "${srcdir}" ]; then
    builddir="./";
else
    srcdir=.
fi

testDIR="./mfd_TEST"

oldcode="${builddir}lalapps_makefakedata_test"
newcodeDEFAULT="${builddir}lalapps_Makefakedata_v4"
compCode="${builddir}lalapps_compareSFTs"

if [ -z "$LAL_DATA_PATH" ]; then
    if [ -n "$LALPULSAR_PREFIX" ]; then
	LAL_DATA_PATH=".:${LALPULSAR_PREFIX}/share/lalpulsar";
    else
	echo
	echo "Need environment-variable LALPULSAR_PREFIX, or LAL_DATA_PATH to be set"
	echo "to your ephemeris-directory (e.g. /usr/local/share/lalpulsar)"
	echo "This might indicate an incomplete LAL+LALPULSAR installation"
	echo
	exit 1
    fi
fi
export LAL_DATA_PATH

if [ -z "$1" ]; then
    newcode=${newcodeDEFAULT}
else
    newcode="$1"
    shift 1
    echo "'$newcode'"
fi

#prepare test subdirectory
if [ ! -d "$testDIR" ]; then
    mkdir $testDIR
else
## cleanup: remove previous output-SFTs
    rm -f $testDIR/*SFT* || true
fi

tol="1e-4";	## tolerance on relative difference between SFTs in comparison
# input parameters
## FIXED
#ephemdir=$LALPULSAR_PREFIX/share/lalpulsar
# determine ephemdir from LAL_DATA_PATH
SAVEIFS="$IFS"
foundEphem=no
IFS=:
for ephemdir in $LAL_DATA_PATH; do test -r $ephemdir/earth00-04.dat && foundEphem=yes && break; done
IFS="$SAVEIFS"

if [ "$foundEphem" = "no" ]; then
    echo "Failed to located ephemeris files."
    echo "Need environment-variable LAL_DATA_PATH to be set"
    echo "to your ephemeris-directory (e.g. /usr/local/share/lalpulsar)"
    echo "This might indicate an incomplete LAL+LALPULSAR installation"
    echo
    exit 1
fi

Tsft=1800
nTsft=20
timestamps="$srcdir/testT8_1800"
refTime=701210229
##refTime=714180733
fmin=300.0
Band=10.0

## VARY
IFO=LLO
aPlus=1.5
aCross=0.7
psi=0.5
phi0=0.9
f0=300.2
alpha=1.7
delta=0.9
noiseDir="../"
noiseSFTs="$noiseDir/SFT.0000[0-9]"
f1dot="-1.e-9"
f2dot="1e-14"

dataTMP=In.data-test
oldCL="-i $dataTMP  -I $IFO -E $ephemdir -S $refTime" ## -D $noiseDir"
newCL="-E $ephemdir --Tsft=$Tsft --fmin=$fmin --Band=$Band --aPlus=$aPlus --aCross=$aCross --psi=$psi --phi0=$phi0 --f0=$f0 --longitude=$alpha --latitude=$delta --detector=$IFO --timestampsFile=$timestamps --refTime=$refTime --f1dot=$f1dot --f2dot=$f2dot $@" ## -D$noiseSFTs -v1"
newCL2="-E $ephemdir --Tsft=$Tsft --fmin=$fmin --Band=$Band --aPlus=$aPlus --aCross=$aCross --psi=$psi --phi0=$phi0 --Freq=$f0  --Alpha=$alpha --Delta=$delta --IFO=$IFO --timestampsFile=$timestamps --refTime=$refTime --f1dot=$f1dot --f2dot=$f2dot $@" ## -D$noiseSFTs -v1"


## produce In.data file for makefakedata_v2
echo "$Tsft	%Tsft_in_sec
$nTsft	%nTsft
$fmin   %first_SFT_frequency_in_Hz
$Band	%SFT_freq_band_in_Hz
0.0	%sigma_(std_of_noise.When=0_only_signal_present)
$aPlus	%Aplus
$aCross	%Across
$psi	%psi
$phi0	%phi0
$f0	%f0
$delta	%latitude_in_radians
$alpha	%longitude_in_radians
2	%max_spin-down_param_order
$f1dot  %value of first spindown
$f2dot  %value of second spindown
$timestamps 	%name_of_time-stamps_file
" > In.data-test

echo "1) Testing isolated pulsar-signal without noise"
echo
echo "Running 'reference-code':"
thisCL="$oldCL  -n ${testDIR}/mfdv2_SFTv1"
echo "$oldcode $thisCL"
$oldcode $thisCL

echo
echo "Running makefakedata_v4, writing v1-SFTs"
thisCL="$newCL --outSFTv1 --outSFTbname=${testDIR}/mfdv4_SFTv1"
echo "$newcode $thisCL"
$newcode $thisCL

echo
echo "Running makefakedata_v4, writing v2-SFTs"
thisCL="$newCL2 --outSFTbname=${testDIR}/"
echo "$newcode $thisCL"
$newcode $thisCL


echo
echo "comparison of resulting SFTs:"

cmdline="$compCode -e $tol -1 '${testDIR}/mfdv2_SFTv1.*' -2 '${testDIR}/mfdv4_SFTv1.*'"
echo ${cmdline}
if ! eval $cmdline; then
    echo "OUCH... SFTs differ by more than $tol. Something might be wrong..."
    exit 2
else
    echo "OK."
fi


echo
cmdline="$compCode -e $tol -1 '${testDIR}/mfdv4_SFTv1.*' -2 '${testDIR}/*_L1_*.sft'"
echo ${cmdline}
if ! eval $cmdline; then
    echo "OUCH... SFTs differ by more than $tol. Something might be wrong..."
    exit 2
else
    echo "OK."
fi


## clean up files [allow turning off via 'NOCLEANUP' environment variable
if [ -z "$NOCLEANUP" ]; then
    rm -rf $testDIR In.data-test
fi

