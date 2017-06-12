#!/bin/bash
./kill_daq.sh

sleep 1
xterm -T ReaderComp -bg lightblue -geom 100x20+1200+100 -e tail -F /tmp/daqmw/log.NIMEASIROCReaderComp &
xterm -T MonitorComp -bg lightgreen -geom 100x20+1200+400 -e tail -F /tmp/daqmw/log.NIMEASIROCMonitorComp &
xterm -T LoggerComp -bg lightyellow -geom 100x20+1200+700 -e tail -F /tmp/daqmw/log.NIMEASIROCLoggerComp &
pwd
run.py -cl config.xml
