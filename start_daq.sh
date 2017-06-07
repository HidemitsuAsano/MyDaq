#!/bin/bash
./kill_daq.sh

sleep 1
xterm -title ReaderComp -bg lightblue -e tail -F /tmp/daqmw/log.NIMEASIROCReaderComp &
xterm -title MonitorComp -bg lightgreen -e tail -F /tmp/daqmw/log.NIMEASIROCMonitorComp &
pwd
run.py -cl config.xml
