#ifndef NIMEASIROCDATA_H
#define NIMEASIROCDATA_H

const int ONE_EVENT_SIZE = 4;//byte

struct NIMEASIROCData {
  unsigned int  header;
  //unsigned int  data;
  int ch;
  int Adchigh;
  int Adclow;
  int TdcLeading;
  int TdcTrailing;
  int Scaler;
};

#endif
