sourceDir=$1
nInputFiles=$2
suffixOutput=$3

filenameBase=AO2D
targetDir="output_${suffixOutput}"

mkdir $targetDir
for ((i=1; i<=nInputFiles; i++))
do
	echo "starting file $i"
        sourceFile="${sourceDir}/AO2D_${i}.root"
	echo $sourceFile
        bash  run_skimmerTruthOnlyTaskOnAO2D_single.sh $sourceFile writer-skimmerTruthMc.json
	targetName="${targetDir}/AnalysisResults_${i}.root"
        echo "finished file number ${i}.n moving to ${targetName}"
        mv AnalysisResults.root $targetName
	mv McGammatables.root ${targetDir}/McGammatables_${i}.root
	mv dpl-config.json ${targetDir}/dpl-config_${i}.json
done

#echo "starting to merge outputfiles"
#hadd AnalysisResults_merged_${suffixOutput}.root $targetDir/*
