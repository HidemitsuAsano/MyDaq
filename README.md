Hidemitsu Asano


#  How to install

   1. make util
   2. make reader/monitor
   3. cd $HOME/MyDaq
   4. ln -s chikuma_nim_easiroc/reader/ruby1/Gemfile .
   5. su
   6. ./chikuma_nim_easiroc/reader/ruby1/install.sh
   7. exit

# How to start DAQ
  ./start_daq.sh
  or 
  run.py -cl config.xml

  if you want to run with webUI
  run.py -l config.xml

# How to check running component
  pgrep -fl Comp


