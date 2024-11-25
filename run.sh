#!/bin/bash

LOG_DIR="/workspace/verifier/data/blindw_wh_10000"
VERIFIER="vbox"
TIMING_ENABLED="true"
COMPACTION_ENABLED="true"
MERGING_ENABLED="true"
PRUNING_STRATEGY="prune_opt"
TC_CONSTRUCTION="purdom_opt"
TC_UPDATE="italino_opt"
SAT_STRATEGY="vboxsat"


if [ $# -gt 0 ]; then
    LOG_DIR="$1"
    VERIFIER="$2"
    TIMING_ENABLED="$3"
    COMPACTION_ENABLED="$4"
    MERGING_ENABLED="$5"
    PRUNING_STRATEGY="$6"
    TC_CONSTRUCTION="$7"
    TC_UPDATE="$8"
    SAT_STRATEGY="$9"
fi

./build/SerVerifier "$LOG_DIR" "$VERIFIER" "$TIMING_ENABLED" "$COMPACTION_ENABLED" "$MERGING_ENABLED" "$PRUNING_STRATEGY" "$TC_CONSTRUCTION" "$TC_UPDATE" "$SAT_STRATEGY"

if [ $? -eq 0 ]; then
    echo "Verification completed successfully."
else
    echo "Verification encountered an error."
fi
