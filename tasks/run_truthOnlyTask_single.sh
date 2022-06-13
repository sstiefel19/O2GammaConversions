echo "running file $1"
o2-analysis-em-gammaconversionstruthonlymc -b --aod-file $1  

mv AnalysisResults.root AnalysisResults_truthOnly.root

