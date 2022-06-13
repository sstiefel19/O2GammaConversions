#jq '."internal-dpl-aod-reader"."aod-file" = "abcde"' configTest.json > "$tmp" && mv "$tmp" configTest.json

configFile=$1
inputFile=$2


jq '."internal-dpl-aod-reader"."aod-file" = "'${inputFile}'"' ${configFile} > configMod.json

o2-analysis-em-gammaconversions -b --configuration json://configMod.json

mv AnalysisResults.root AnalysisResults_gammaConv.root

