#! /bin/bash

cd /afs/cern.ch/user/h/hroskes/work/public/Run2MC/ZZ4lanalyticalfits

for a in $(ls .. | grep "^VBF\|^HJJ"); do
    files="$(ls ../$a/ | grep '\.root$')"
    if ! [ "$files" ]; then
        continue
    fi
    nfiles=$(echo "$files" | wc -w)

    filename=${a/./}

    g1=$(grep "ghz1 = " ../$a/JHUGen/JHUGenerator/mod_Parameters.F90 | sed "s/.*(//" | sed "s/,.*//" | sed "s/d/e/")
    Re_g4=$(grep "ghz4 = " ../$a/JHUGen/JHUGenerator/mod_Parameters.F90 | sed "s/.*(//" | sed "s/,.*//" | sed "s/d/e/")
    Im_g4=$(grep "ghz4 = " ../$a/JHUGen/JHUGenerator/mod_Parameters.F90 | sed "s/.*,//" | sed "s/).*//" | sed "s/d/e/")

    echo -n '
#include "TROOT.h"
#include "TString.h"
#include "angularDistributions_spin0.C"

'"void $filename()"'
{
    //gROOT->Macro("loadLib.C");'"
    const int nfiles = $nfiles;

    TString files[nfiles] = {" > $filename.C


    comma=
    for file in $files; do
        echo -n "$comma"'
        "'../$a/$file'"' >> $filename.C
        comma=,
    done

    echo '
    };

    angularDistributions_spin0(nfiles, files, "'$a'"'", $g1, $Re_g4, $Im_g4);
}" >> $filename.C


    #if ( echo $a | grep -q "\." ); then
        echo "
            cd /afs/cern.ch/work/h/hroskes/public/Run2MC/CMSSW_7_4_1 && eval "'$(scram ru -sh)'" && cd /afs/cern.ch/user/h/hroskes/work/public/Run2MC/ZZ4lanalyticalfits &&
            echo '
            .x loadLib.C
            .x $filename.C+
        ' | root -l -b" | bsub -q 1nd -J plot$a
    #fi


done
