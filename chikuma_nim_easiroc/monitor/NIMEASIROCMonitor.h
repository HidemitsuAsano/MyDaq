// -*- C++ -*-
/*!
 * @file  NIMEASIROCMonitor.h
 * @brief
 * @date Jan. 20th, 2017
 * @author Hidemitsu Asano
 *
 */

#ifndef NIMEASIROCMONITOR_H
#define NIMEASIROCMONITOR_H

#include "DaqComponentBase.h"

#include <arpa/inet.h> // for ntohl()

////////// ROOT Include files //////////
#include "TH1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TApplication.h"

#include "NIMEASIROCData.h"

using namespace RTC;

class NIMEASIROCMonitor
    : public DAQMW::DaqComponentBase
{
public:
    NIMEASIROCMonitor(RTC::Manager* manager);
    ~NIMEASIROCMonitor();

    // The initialize action (on CREATED->ALIVE transition)
    // former rtc_init_entry()
    virtual RTC::ReturnCode_t onInitialize();

    // The execution action that is invoked periodically
    // former rtc_active_do()
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
    TimedOctetSeq          m_in_data;
    InPort<TimedOctetSeq>  m_InPort;

private:
    int daq_dummy();
    int daq_configure();
    int daq_unconfigure();
    int daq_start();
    int daq_run();
    int daq_stop();
    int daq_pause();
    int daq_resume();

    int parse_params(::NVList* list);
    int reset_InPort();

    unsigned int read_InPort();
    //int online_analyze();
    int decode_data(const unsigned char* mydata);
    int fill_data(const unsigned char* mydata, const int size);

    BufferStatus m_in_status;

    ////////// ROOT Histogram //////////
    TCanvas *m_canvas;
    TH1F    *m_hist;
    int      m_bin;
    double   m_min;
    double   m_max;
    int      m_monitor_update_rate;
    unsigned char m_recv_data[4096];
    unsigned int  m_event_byte_size;
    struct NIMEASIROCData m_NIMEASIROCData;

    bool m_debug;
};


extern "C"
{
    void NIMEASIROCMonitorInit(RTC::Manager* manager);
};

#endif // NIMEASIROCMONITOR_H
