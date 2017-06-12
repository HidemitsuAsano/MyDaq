#!/bin/bash
./kill_daq.sh

sleep 1
xterm -T ReaderComp -bg lightblue -geom 100x20+1200+100 -e tailf  /tmp/daqmw/log.NIMEASIROCReaderComp &
xterm -T MonitorComp -bg lightgreen -geom 100x20+1200+400 -e tailf /tmp/daqmw/log.NIMEASIROCMonitorComp &
xterm -T LoggerComp -bg yellow -geom 100x20+1200+700 -e tailf /tmp/daqmw/log.NIMEASIROCLoggerComp &
xterm -T Operator  -geom 100x20 -e 'run.py -cl config.xml' &
