#!/bin/bash

################################################################################
# get needed options from ini file

month_gps_time=`cat write_ifar_scripts.ini | grep 'month_gps_time' | awk '{print $3}'`
month_duration=`cat write_ifar_scripts.ini | grep 'month_duration' | awk '{print $3}'`
cat=`cat write_ifar_scripts.ini | grep 'cat' | awk '{print $3}'`

coire_path=`cat write_ifar_scripts.ini | grep 'coire_path' | awk '{print $3}'`

log_path=`cat write_ifar_scripts.ini | grep 'log_path' | awk '{print $3}'`
condor_priority=`cat write_ifar_scripts.ini | grep 'condor_priority' | awk '{print $3}'`

#Print options out to screen for verification
echo "Options used are:"
echo "  month_gps_time = ${month_gps_time}"
echo "  month_duration = ${month_duration}"
echo "  cat = ${cat}"
echo "  coire_path = ${coire_path}"
echo "  log_path = ${log_path}"
echo "  condor_priority = ${condor_priority}"
echo
################################################################################


#get first_coire zero-lag files
/bin/echo -n "Generating first_coire file list..."
pushd first_coire_files/ > /dev/null
for combo in H1H2L1 H1H2 H1L1 H2L1; do
  for file in ${combo}-COIRE_${cat}_H*xml.gz; do
    echo "first_coire_files/${file}"
  done > ${combo}_first_coire_${cat}.cache
done
popd > /dev/null
echo " done."

#generate zero-lag second_coire dag
/bin/echo -n "Generating zero-lag second_coire.dag and .sub files..."
if [ 1 ]; then
  #write coire jobs for double-in_double files
  for combo in H1H2 H1L1 H2L1; do
    infile="${combo}_first_coire_${cat}.cache"
    parent_file="${combo}-SECOND_COIRE_${cat}_${combo}-${month_gps_time}-${month_duration}.xml.gz"
    summary_file="summ_files_all_data/${combo}-SECOND_COIRE_${cat}_${combo}-${month_gps_time}-${month_duration}.txt"
    echo "JOB $parent_file second_coire.coire.sub"
    echo "RETRY $parent_file 1"
    echo "VARS $parent_file macroinfile=\"$infile\" macrooutfile=\"$parent_file\" macrocombo=\"$combo\" macrosummaryfile=\"$summary_file\""
    echo "CATEGORY $parent_file coire"
    echo "## JOB $parent_file requires input file $infile"
    #write mass coire child jobs
    for mass in mtotal_bin1 mtotal_bin2 mtotal_bin3; do
      if [[ "$mass" == "mtotal_bin1" ]]; then
        low_mass='20.0'
        high_mass='50.0'
      elif [[ "$mass" == "mtotal_bin2" ]]; then
        low_mass='50.0'
        high_mass='85.0'
      else
        low_mass='85.0'
        high_mass='110.0'
      fi
      child_file="$mass/$parent_file"
      echo "JOB $child_file second_coire_mass.coire.sub"
      echo "RETRY $child_file 1"
      echo "VARS $child_file macroinfile=\"$parent_file\" macrooutfile=\"$child_file\" macrolowmass=\"$low_mass\" macrohighmass=\"$high_mass\""
      echo "CATEGORY $child_file coire"
      echo "PARENT $parent_file CHILD $child_file"
      echo "## JOB $child_file requires input file $parent_file"
    done
  done
  #write coire jobs for double-in_triple and triple-in_triple files
  for combo in H1H2L1 H1H2 H1L1 H2L1; do
    infile="H1H2L1_first_coire_${cat}.cache"
    parent_file="${combo}-SECOND_COIRE_${cat}_H1H2L1-${month_gps_time}-${month_duration}.xml.gz"
    summary_file="summ_files_all_data/${combo}-SECOND_COIRE_${cat}_H1H2L1-${month_gps_time}-${month_duration}.txt"
    echo "JOB $parent_file second_coire.coire.sub"
    echo "RETRY $parent_file 1"
    echo "VARS $parent_file macroinfile=\"$infile\" macrooutfile=\"$parent_file\" macrocombo=\"$combo\" macrosummaryfile=\"$summary_file\""
    echo "CATEGORY $parent_file coire"
    echo "## JOB $parent_file requires input file $infile"
    #write mass coire child jobs
    for mass in mtotal_bin1 mtotal_bin2 mtotal_bin3; do
      if [[ "$mass" == "mtotal_bin1" ]]; then
        low_mass='20.0'
        high_mass='50.0'
      elif [[ "$mass" == "mtotal_bin2" ]]; then
        low_mass='50.0'
        high_mass='85.0'
      else
        low_mass='85.0'
        high_mass='110.0'
      fi
      child_file="$mass/$parent_file"
      echo "JOB $child_file second_coire_mass.coire.sub"
      echo "RETRY $child_file 1"
      echo "VARS $child_file macroinfile=\"$parent_file\" macrooutfile=\"$child_file\" macrolowmass=\"$low_mass\" macrohighmass=\"$high_mass\""
      echo "CATEGORY $child_file coire"
      echo "PARENT $parent_file CHILD $child_file"
      echo "## JOB $child_file requires input file $parent_file"
    done
  done
  echo "MAXJOBS coire 20"
fi > second_coire.dag

if [ 1 ]; then
  echo "universe = standard"
  echo "executable = ${coire_path}"
  echo "arguments = --input first_coire_files/\$(macroinfile) --output second_coire_files/\$(macrooutfile) --data-type all_data --coinc-stat effective_snrsq --coinc-cut \$(macrocombo) --cluster-time 10000 --summary-file second_coire_files/\$(macrosummaryfile) --eff-snr-denom-fac 50"
  echo "log = " `mktemp -p ${log_path}`
  echo "error = logs/coire-\$(cluster)-\$(process).err"
  echo "output = logs/coire-\$(cluster)-\$(process).out"
  echo "notification = never"
  echo "priority = ${condor_priority}"
  echo "queue 1"
fi > second_coire.coire.sub

if [ 1 ]; then
  echo "universe = standard"
  echo "executable = ${coire_path}"
  echo "arguments = --glob second_coire_files/\$(macroinfile) --output second_coire_files/\$(macrooutfile) --data-type all_data --coinc-stat effective_snrsq --cluster-time 10000 --mass-cut mtotal --mass-range-low \$(macrolowmass) --mass-range-high \$(macrohighmass) --eff-snr-denom-fac 50"
  echo "log = " `mktemp -p ${log_path}`
  echo "error = logs/coire-\$(cluster)-\$(process).err"
  echo "output = logs/coire-\$(cluster)-\$(process).out"
  echo "notification = never"
  echo "priority = ${condor_priority}"
  echo "queue 1"
fi > second_coire_mass.coire.sub

echo " done."

#get first_coire time-slide files
/bin/echo -n "Generating first_coire_slide file list..."
pushd first_coire_files/ > /dev/null
for combo in H1H2L1 H1H2 H1L1 H2L1; do
  for file in ${combo}-COIRE_SLIDE_${cat}_H*xml.gz; do
    echo "first_coire_files/${file}"
  done > ${combo}_first_coire_slide_${cat}.cache
done
popd > /dev/null
echo " done."

#generate time-slide second_coire dag
/bin/echo -n "Generating time-slide second_coire .dag and .sub files..."
if [ 1 ]; then
  #write coire_slide jobs for double-in_double files
  for combo in H1H2 H1L1 H2L1; do
    infile="${combo}_first_coire_slide_${cat}.cache"
    parent_file="${combo}-SECOND_COIRE_SLIDE_${cat}_${combo}-${month_gps_time}-${month_duration}.xml.gz"
    echo "JOB $parent_file second_coire_slide.coire.sub"
    echo "RETRY $parent_file 1"
    echo "VARS $parent_file macroinfile=\"$infile\" macrooutfile=\"$parent_file\" macrocombo=\"$combo\""
    echo "CATEGORY $parent_file coire"
    echo "## JOB $parent_file requires input file $infile"
    #write mass coire child jobs
    for mass in mtotal_bin1 mtotal_bin2 mtotal_bin3; do
      if [[ "$mass" == "mtotal_bin1" ]]; then
        low_mass='20.0'
        high_mass='50.0'
      elif [[ "$mass" == "mtotal_bin2" ]]; then
        low_mass='50.0'
        high_mass='85.0'
      else
        low_mass='85.0'
        high_mass='110.0'
      fi
      child_file="$mass/$parent_file"
      echo "JOB $child_file second_coire_slide_mass.coire.sub"
      echo "RETRY $child_file 1"
      echo "VARS $child_file macroinfile=\"$parent_file\" macrooutfile=\"$child_file\" macrolowmass=\"$low_mass\" macrohighmass=\"$high_mass\""
      echo "CATEGORY $child_file coire_slide"
      echo "PARENT $parent_file CHILD $child_file"
      echo "## JOB $child_file requires input file $parent_file"
    done
  done
  #write coire_slide jobs for double-in_triple and triple-in_triple files
  for combo in H1H2L1 H1H2 H1L1 H2L1; do
    infile="H1H2L1_first_coire_slide_${cat}.cache"
    parent_file="${combo}-SECOND_COIRE_SLIDE_${cat}_H1H2L1-${month_gps_time}-${month_duration}.xml.gz"
    echo "JOB $parent_file second_coire_slide.coire.sub"
    echo "RETRY $parent_file 1"
    echo "VARS $parent_file macroinfile=\"$infile\" macrooutfile=\"$parent_file\" macrocombo=\"$combo\""
    echo "CATEGORY $parent_file coire_slide"
    echo "## JOB $parent_file requires input file $infile"
    #write mass coire child jobs
    for mass in mtotal_bin1 mtotal_bin2 mtotal_bin3; do
      if [[ "$mass" == "mtotal_bin1" ]]; then
        low_mass='20.0'
        high_mass='50.0'
      elif [[ "$mass" == "mtotal_bin2" ]]; then
        low_mass='50.0'
        high_mass='85.0'
      else
        low_mass='85.0'
        high_mass='110.0'
      fi
      child_file="$mass/$parent_file"
      echo "JOB $child_file second_coire_slide_mass.coire.sub"
      echo "RETRY $child_file 1"
      echo "VARS $child_file macroinfile=\"$parent_file\" macrooutfile=\"$child_file\" macrolowmass=\"$low_mass\" macrohighmass=\"$high_mass\""
      echo "CATEGORY $child_file coire_slide"
      echo "PARENT $parent_file CHILD $child_file"
      echo "## JOB $child_file requires input file $parent_file"
    done
  done
  echo "MAXJOBS coire_slide 20"
fi > second_coire_slide.dag

if [ 1 ]; then
  echo "universe = standard"
  echo "executable = ${coire_path}"
  echo "arguments = --input first_coire_files/\$(macroinfile) --output second_coire_files/\$(macrooutfile) --data-type all_data --coinc-stat effective_snrsq --cluster-time 10000 --coinc-cut \$(macrocombo) --num-slides 50 --eff-snr-denom-fac 50"
  echo "log = " `mktemp -p ${log_path}`
  echo "error = logs/coire_slide-\$(cluster)-\$(process).err"
  echo "output = logs/coire_slide-\$(cluster)-\$(process).out"
  echo "notification = never"
  echo "priority = ${condor_priority}"
  echo "queue 1"
fi > second_coire_slide.coire.sub

if [ 1 ]; then
  echo "universe = standard"
  echo "executable = ${coire_path}"
  echo "arguments = --glob second_coire_files/\$(macroinfile) --output second_coire_files/\$(macrooutfile) --data-type all_data --coinc-stat effective_snrsq --cluster-time 10000 --num-slides 50 --mass-cut mtotal --mass-range-low \$(macrolowmass) --mass-range-high \$(macrohighmass) --eff-snr-denom-fac 50"
  echo "log = " `mktemp -p ${log_path}`
  echo "error = logs/coire_slide-\$(cluster)-\$(process).err"
  echo "output = logs/coire_slide-\$(cluster)-\$(process).out"
  echo "notification = never"
  echo "priority = ${condor_priority}"
  echo "queue 1"
fi > second_coire_slide_mass.coire.sub

echo " done."

#set up directory structure
/bin/echo -n "Creating second_coire_files directory structure..."
if [ ! -d second_coire_files ] ; then
  mkdir second_coire_files
fi
if [ ! -d second_coire_files/summ_files_all_data ] ; then
  mkdir second_coire_files/summ_files_all_data
fi
for mass in mtotal_bin1 mtotal_bin2 mtotal_bin3; do
  if [ ! -d second_coire_files/${mass} ] ; then
    mkdir second_coire_files/${mass}
  fi
done
echo " done."
echo "******************************************************"
echo "  Now run: condor_submit_dag second_coire.dag"
echo "      and: condor_submit_dag second_coire_slide.dag"
echo "  These dags can be run simutaneously."
echo "******************************************************"
