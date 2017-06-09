#!/bin/bash

pgrep -fl Comp
killall -KILL DaqOperatorComp
killall -KILL NIMEASIROCReaderComp
killall -KILL NIMEASIROCMonitorComp
killall -KILL SampleReaderComp
killall -KILL SampleMonitorComp
#pkill -f Comp

