#!/bin/bash
pgrep -fl Comp
killall -KILL DaqOperatorComp
killall -KILL DispatcherComp
killall -KILL NIMEASIROCReaderComp
killall -KILL NIMEASIROCMonitorComp
killall -KILL NIMEASIROCLoggerComp
killall -KILL SampleReaderComp
killall -KILL SampleMonitorComp
killall -KILL SampleLoggerComp
#pkill -f Comp

