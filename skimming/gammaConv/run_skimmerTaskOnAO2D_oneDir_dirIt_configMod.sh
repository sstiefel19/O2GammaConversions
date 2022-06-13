motherDir=$1
nInputFiles=$2
configFile=$3
writerFile=$4
suffixOutput=$5

filenameBase=AO2D
targetDir="output_${suffixOutput}"

mkdir $targetDir
for ((i=1; i<=nInputFiles; i++))
do
	echo "i = $i"
        if (( i < 10 )); then
    	    pref=00
        fi
        if (( i > 9 )); then
	    pref=0
        fi
        iDir=${pref}${i}
	sourceFile="${motherDir}/${iDir}/AO2D.root"

	if [ -f "$sourceFile" ]; then
		echo "${sourceFile} exists"

		# modify configFile
		jq '."internal-dpl-aod-reader"."aod-file" = "'${sourceFile}'"' ${configFile} > configMod.json

		bash run_skimmerTaskOnAO2D_single.sh configMod.json ${writerFile}
		echo "finished file number ${i}."
		mv dpl-config.json                        ${targetDir}/dpl-config_${i}.json
		mv AnalysisResults_skimmerGammaConv.root  ${targetDir}/AnalysisResults_skimmerGammaConv_${i}.root
		mv V0tables.root                          ${targetDir}/V0tables_${i}.root
	fi
done
