sourceFile=$1
echo $sourceFile

writerFile="writer-skimmerTruthMc.json"

wholeCommand="o2-analysis-em-skimmergammaconversionstruthonlymc -b --aod-file ${sourceFile} --aod-writer-json ${writerFile}" 

if [ -f "$sourceFile" ]; then
	echo "exists"
	echo $wholeCommand
	eval $wholeCommand
fi  

mv AnalysisResults.root AnalysisResults_skimmerTruthOnly.root

