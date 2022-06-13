configFile=${1}

#writerFile=/misc/alidata141/alice_u/stiefelmaier/work/repos/projects/2022/2022-03-03_project_o2/writer-skimmer.json
writerFile=$2
echo ${writerFile}

configString="--configuration json://config-skimmerMc.json"

wholeCommand="o2-analysis-trackselection -b ${configString} --aod-writer-json ${writerFile} | o2-analysis-event-selection -b ${configString} | o2-analysis-multiplicity-table -b ${configString} |  o2-analysis-timestamp -b ${configString} | o2-analysis-trackextension -b ${configString} | o2-analysis-lf-lambdakzerobuilder -b ${configString} | o2-analysis-em-skimmergammaconversionsmc -b ${configString}" 

echo $wholeCommand
eval $wholeCommand


