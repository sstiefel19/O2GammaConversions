
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
echo $SCRIPT_DIR


root -l -x ${SCRIPT_DIR}/produceAfterburnerPlots.C+\(\"$1\"\,\"$2\"\)
