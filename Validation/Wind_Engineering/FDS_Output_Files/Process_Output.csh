#!/bin/csh -f
setenv SVNROOT ~/FDS-SMV
set DDIR=$SVNROOT/Validation/Wind_Engineering/Current_Results
set WDIR=$SVNROOT/Validation/Wind_Engineering/FDS_Output_Files

cp $DDIR/*line.csv $WDIR
cp $DDIR/*svn.txt $WDIR

