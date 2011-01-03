#!/bin/csh -f
setenv SVNROOT ~/FDS-SMV
set DDIR=$SVNROOT/Verification/Decaying_Isotropic_Turbulence/Current_Results
set WDIR=$SVNROOT/Verification/Decaying_Isotropic_Turbulence/FDS_Output_Files
cp $DDIR/*devc.csv $WDIR
cp $DDIR/*uvw*.csv $WDIR

