configFile=$1

o2-analysis-em-gammaconversions -b --configuration json://${configFile}

mv AnalysisResults.root AnalysisResults_gammaConv.root

