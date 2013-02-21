#!/bin/csh -f
set revision=$1
set SVNROOT=~/FDS-SMV
set REMOTESVNROOT=FDS-SMV
set OSXHOST=$2

set BACKGROUNDDIR=$SVNROOT/Utilities/background/intel_osx_32
set SMVDIR=$REMOTESVNROOT/SMV/Build/intel_osx_64
set SMZDIR=$REMOTESVNROOT/Utilities/smokezip/intel_osx_64
set SMDDIR=$REMOTESVNROOT/Utilities/smokediff/intel_osx_64
set FORBUNDLE=$SVNROOT/SMV/for_bundle
set OSXDIR=smv_test\_$revision\_osx64

cd $FORBUNDLE

rm -rf $OSXDIR
mkdir -p $OSXDIR
mkdir -p $OSXDIR/Documentation

cp $FORBUNDLE/objects.svo $OSXDIR/.
cp $FORBUNDLE/*.po $OSXDIR/.
scp $OSXHOST\:$BACKGROUNDDIR/background $OSXDIR/.
scp $OSXHOST\:$SMVDIR/smokeview_osx_test_64 $OSXDIR/.
scp $OSXHOST\:$SMZDIR/smokezip_osx_64 $OSXDIR/.
scp $OSXHOST\:$SMDDIR/smokediff_osx_64 $OSXDIR/.
rm -f $OSXDIR.tar $OSXDIR.tar.gz
tar cvf $OSXDIR.tar $OSXDIR/.
gzip $OSXDIR.tar
