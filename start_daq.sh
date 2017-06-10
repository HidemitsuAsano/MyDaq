#!/bin/bash
./kill_daq.sh

sleep 1
xterm -T ReaderComp -bg lightblue -geom 100x30+1200+100 -e tail -F /tmp/daqmw/log.NIMEASIROCReaderComp &
xterm -T MonitorComp -bg lightgreen -geom 100x30+1200+600 -e tail -F /tmp/daqmw/log.NIMEASIROCMonitorComp &
pwd
run.py -cl config.xml
