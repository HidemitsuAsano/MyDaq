#ifndef NIMEASIROCDATA_H
#define NIMEASIROCDATA_H

const int ONE_EVENT_SIZE = 8;

struct NIMEASIROCData {
  unsigned char magic;
  unsigned char format_ver;
  unsigned char module_num;
  unsigned char reserved;
  unsigned int  data;
};

#endif
