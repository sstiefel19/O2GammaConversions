configString="--configuration json://$1"
writerFile="writer-skimmerMc.json"

wholeCommand="o2-analysis-track-propagation -b ${configString}  --aod-writer-json ${writerFile} |  o2-analysis-trackselection -b ${configString} |  o2-analysis-event-selection -b ${configString} | o2-analysis-multiplicity-table -b ${configString} |  o2-analysis-timestamp -b ${configString}  | o2-analysis-lf-lambdakzerobuilder -b ${configString} | o2-analysis-pid-tpc -b ${configString} | o2-analysis-em-skimmergammaconversions -b ${configString}"

cat $1
echo $wholeCommand
eval $wholeCommand
mv AnalysisResults.root AnalysisResults_skimmerGammaConv.root

