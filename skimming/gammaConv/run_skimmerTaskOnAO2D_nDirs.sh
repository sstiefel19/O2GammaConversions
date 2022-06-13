
for serv in 12 13 14
do
	for goX in go3 go4 go5 go6
	do
		suff="serv-${serv}_${goX}"
		echo "starting ${suff}"
		bash run_skimmerTaskOnAO2D_oneDir.sh /misc/alidata141/alice_u/stiefelmaier/work/data/AO2Ds/simChallengeFull/alice-serv${serv}/submitJobs_seq/${goX}/ao2ds 350 ${suff}
	done
done
