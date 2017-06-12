#!/bin/bash
pgrep -fl Comp
killall -KILL -q xterm
killall -KILL -q DaqOperatorComp
killall -KILL -q DispatcherComp
killall -KILL -q NIMEASIROCReaderComp
killall -KILL -q NIMEASIROCMonitorComp
killall -KILL -q NIMEASIROCLoggerComp
killall -KILL -q SampleReaderComp
killall -KILL -q SampleMonitorComp
killall -KILL -q SampleLoggerComp
#pkill -f Comp

