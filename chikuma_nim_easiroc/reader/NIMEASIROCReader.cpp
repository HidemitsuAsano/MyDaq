// -*- C++ -*-
/*!
 * @file NIMEASIROCReader.cpp
 * @brief  This code is for NIM-EASICROC with Chikuma-san's firmware.
 *         Ishijima-san's firmware is not compatible with this code.
 * @date   Jan. 20th, 2017
 * @author Hidemitsu Asano
 *
 */

#include <cstdlib>
#include <string>


#include "SiTcpRbcp.hh"
#include "getaddr.h"
#include "HexDump.hh"
#include "NIMEASIROCReader.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1;
using DAQMW::FatalType::USER_DEFINED_ERROR2;

// Module specification
// Change following items to suit your component's spec.
static const char* nimeasiroceader_spec[] =
{
    "implementation_id", "NIMEASIROCReader",
    "type_name",         "NIMEASIROCReader",
    "description",       "NIMEASIROCReader component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};



NIMEASIROCReader::NIMEASIROCReader(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_OutPort("out0", m_out_data),
      m_sock(NULL),
      m_data(),
      m_header(),
      m_recv_byte_size(0),
      m_out_status(BUF_SUCCESS),
      m_rbcp(NULL),
      m_isSendADC(true),
      m_isSendTDC(true),
      m_isSendScaler(false),//DO NOT turn on
      
      m_debug(true)
{
    // Registration: InPort/OutPort/Service

    // Set OutPort buffers
    registerOutPort("nimeasiroceader_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("NIMEASIROCREADER");
}

NIMEASIROCReader::~NIMEASIROCReader()
{
}

RTC::ReturnCode_t NIMEASIROCReader::onInitialize()
{
    if (m_debug) {
        std::cerr << "NIMEASIROCReader::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t NIMEASIROCReader::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int NIMEASIROCReader::daq_dummy()
{
    return 0;
}

int NIMEASIROCReader::daq_configure()
{
    std::cerr << "*** NIMEASIROCReader::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);
    
    std::string rubydir = "chikuma_nim_easiroc/reader/ruby/Controller.rb";
    std::string execmd  = rubydir + " " + m_srcAddr; 
    //int sysout1 = std::system("cd ruby");
    std::cout << std::endl;
    std::cout << "Initializing NIM-EASIROC......... "  << std::endl;
    std::cout << std::endl;
    int sysout = std::system(execmd.c_str());
    //int sysout = std::system("./Controller.rb");
    std::cout << std::endl;
    std::cout << __FILE__  << " L." << __LINE__ << " system command " <<  std::endl;
    std::cout << execmd.c_str() << std::endl;
    std::cout << std::endl;

    //register configuration via SiTCP RBCP
    if (!m_rbcp) {
      std::cout << std::endl;
      std::cout << "register Remote Bus Control Porotocol (RBCP) : port " << SiTcpRbcp::kDefaultPort << std::endl;
      m_rbcp = new SiTcpRbcp(m_srcAddr, SiTcpRbcp::kDefaultPort);
    }

    // 
    //MonitorMode();
    
    return 0;
}

int NIMEASIROCReader::parse_params(::NVList* list)
{
    bool srcAddrSpecified = false;
    bool srcPortSpecified = false;

    std::cerr << "param list length:" << (*list).length() << std::endl;

    int len = (*list).length();
    for (int i = 0; i < len; i+=2) {
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i+1].value;

        std::cerr << "sname: " << sname << "  ";
        std::cerr << "value: " << svalue << std::endl;

        if ( sname == "srcAddr1" ) {
            srcAddrSpecified = true;
            if (m_debug) {
                std::cerr << "source addr: " << svalue << std::endl;
            }
            m_srcAddr = svalue;
        }
        if ( sname == "srcPort1" ) {
            srcPortSpecified = true;
            if (m_debug) {
                std::cerr << "source port: " << svalue << std::endl;
            }
            char* offset;
            m_srcPort = (int)strtol(svalue.c_str(), &offset, 10);
        }
    }

    if (!srcAddrSpecified) {
        std::cerr << "### ERROR:data source address not specified\n";
        fatal_error_report(USER_DEFINED_ERROR1, "NO SRC ADDRESS");
    }
    if (!srcPortSpecified) {
        std::cerr << "### ERROR:data source port not specified\n";
        fatal_error_report(USER_DEFINED_ERROR2, "NO SRC PORT");
    }
    
    return 0;
}

int NIMEASIROCReader::daq_unconfigure()
{
    std::cerr << "*** NIMEASIROCReader::unconfigure" << std::endl;

    return 0;
}

int NIMEASIROCReader::daq_start()
{
    try {
        // Create socket and connect to data server.
        m_sock = new DAQMW::Sock();
        m_sock->connect(m_srcAddr, m_srcPort);
    } catch (DAQMW::SockException& e) {
        std::cerr << "Sock Fatal Error : " << e.what() << std::endl;
        fatal_error_report(USER_DEFINED_ERROR1, "SOCKET FATAL ERROR");
    } catch (...) {
        std::cerr << "Sock Fatal Error : Unknown" << std::endl;
        fatal_error_report(USER_DEFINED_ERROR1, "SOCKET FATAL ERROR");
    }
    m_sock->setOptRecvTimeOut(1);
    // Check data port connections
    bool outport_conn = check_dataPort_connections( m_OutPort );
    if (!outport_conn) {
        std::cerr << "### NO Connection" << std::endl;
        fatal_error_report(DATAPATH_DISCONNECTED);
    }
     
    //readAndThrowPreviousData ?
    size_t thrownSize=0;
    int status = 0;
    unsigned char rs[1]={0};
    while(status != DAQMW::Sock::ERROR_TIMEOUT){
      status = m_sock->read(rs,1);
      thrownSize++;
    }
    std::cout << "ThrowPreviousDataSize: " << thrownSize << std::endl;

    //Go to DAQ mode from monitor mode
    std::cerr << __FILE__ << " l." << __LINE__ << " Enter DAQ mode"  << std::endl;
    unsigned char data =0;
    //enable DAQ mode
    data |= NIMEASIROC::daqModeBit;
    if(m_isSendADC){
      //enable ADC info. 
      data |= NIMEASIROC::sendAdcBit;
    }
    if(m_isSendTDC){
      //enable TDC info.
      data |= NIMEASIROC::sendTdcBit;
    }
    if(m_isSendScaler){
      //enable scaler info.
      //should be disable 
      data |= NIMEASIROC::sendScalerBit;
    }
    
    if(m_debug){
      std::cout << __FILE__ << " l." << __LINE__ << " write status register " <<  std::endl;
      std::cout << "Address: " << NIMEASIROC::statusRegisterAddress  << " val.: " <<  (int)data << std::endl;
    }
    int datalength = 1;//byte
    m_rbcp->write(NIMEASIROC::statusRegisterAddress,&data,datalength);
    std::cerr << "*** NIMEASIROCReader::start" << std::endl;

    m_out_status = BUF_SUCCESS;



    return 0;
}

int NIMEASIROCReader::daq_stop()
{
    std::cerr << "*** NIMEASIROCReader::stop" << std::endl;
    
    //Go to monitor mode from DAQ mode
    std::cerr << __FILE__ << "l. " << __LINE__ << "exit DAQ mode"  << std::endl;
    
    //disable daq mode
    unsigned char data =0;
    if(m_isSendADC){
      //enable ADC info. 
      data |= NIMEASIROC::sendAdcBit;
    }
    if(m_isSendTDC){
      //enable TDC info.
      data |= NIMEASIROC::sendTdcBit;
    }
    if(m_isSendScaler){
      //enable scaler info.
      //should be disable 
      data |= NIMEASIROC::sendScalerBit;
    }
    
    if(m_debug){
      std::cout << __FILE__ << "l. " << __LINE__ << " write status register " <<  std::endl;
      std::cout << "Address: " << NIMEASIROC::statusRegisterAddress  << "val. " <<  (int)data << std::endl;
    }
    int datalength = 1;//byte
    m_rbcp->write(NIMEASIROC::statusRegisterAddress,&data,datalength);

    if (m_sock) {
        m_sock->disconnect();
        delete m_sock;
        m_sock = NULL;
    }
    
    // Finalize EASIROC
    // TODO : implement this function
    //finalize_device(); 

    return 0;
}

int NIMEASIROCReader::daq_pause()
{
    std::cerr << "*** NIMEASIROCReader::pause" << std::endl;

    return 0;
}

int NIMEASIROCReader::daq_resume()
{
    std::cerr << "*** NIMEASIROCReader::resume" << std::endl;

    return 0;
}

int NIMEASIROCReader::read_data_from_detectors()
{
    int received_data_size = 0;
     
    //How to read NIM-EASIROC data
    //1. receive Header (always 4 bytes)
    //2. get data size from header (changes event by event)
    //3. receive Data
    
    memset(m_header,0,sizeof(m_header));
    
    int status = m_sock->readAll(m_header,NIMEASIROC::headersize);

    if (status == DAQMW::Sock::ERROR_FATAL) {
        std::cerr << "### ERROR: m_sock->readAll() header" << std::endl;
        fatal_error_report(USER_DEFINED_ERROR1, "SOCKET FATAL ERROR");
    }
    else if (status == DAQMW::Sock::ERROR_TIMEOUT) {
        std::cerr << "### Timeout: m_sock->readAll() header" << std::endl;
        fatal_error_report(USER_DEFINED_ERROR2, "SOCKET TIMEOUT");
    }
    else {
        received_data_size = NIMEASIROC::headersize;
    }


    //check header fomat and get data size
    unsigned int normalFrame = 0x80000000;
    unsigned int header32 = unpackBigEndian32(m_header);
    unsigned int frame = header32 & 0x80808080;
    if(frame != normalFrame){
      std::cerr << __FILE__ << " L. " << __LINE__ << " Frame Error! " << std::endl;
      return received_data_size;
    }
    
    size_t dataSize = header32 & 0x0fff; 
    m_data.resize(dataSize);

    status = m_sock->readAll(&(m_data[0]), dataSize);
    if (status == DAQMW::Sock::ERROR_FATAL) {
        std::cerr << "### ERROR: m_sock->readAll() body" << std::endl;
        fatal_error_report(USER_DEFINED_ERROR1, "SOCKET FATAL ERROR");
    }
    else if (status == DAQMW::Sock::ERROR_TIMEOUT) {
        std::cerr << "### Timeout: m_sock->readAll() body" << std::endl;
        fatal_error_report(USER_DEFINED_ERROR2, "SOCKET TIMEOUT");
    }
    else {
        received_data_size += dataSize;
    }

    return received_data_size;
}

//set data going to monitorComp
int NIMEASIROCReader::set_data(unsigned int data_byte_size)
{
    unsigned char header[8];
    unsigned char footer[8];

    set_header(&header[0], data_byte_size);
    set_footer(&footer[0]);

    ///set OutPort buffer length
    m_out_data.data.length(data_byte_size + HEADER_BYTE_SIZE + FOOTER_BYTE_SIZE);
    memcpy(&(m_out_data.data[0]), &header[0], HEADER_BYTE_SIZE);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), &m_data[0], data_byte_size);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE + data_byte_size]), &footer[0],
           FOOTER_BYTE_SIZE);

    return 0;
}

int NIMEASIROCReader::write_OutPort()
{
    ////////////////// send data from OutPort  //////////////////
    //memo by H.Asano
    //The monitor module receives data from this OutPort.
    //The name of this OutPort defined in config.xml  must be same as the name of inPort for monitorComp
    bool ret = m_OutPort.write();

    //////////////////// check write status /////////////////////
    if (ret == false) {  // TIMEOUT or FATAL
        m_out_status  = check_outPort_status(m_OutPort);
        if (m_out_status == BUF_FATAL) {   // Fatal error
            fatal_error_report(OUTPORT_ERROR);
        }
        if (m_out_status == BUF_TIMEOUT) { // Timeout
            return -1;
        }
    }
    else {
        m_out_status = BUF_SUCCESS; // successfully done
    }

    return 0;
}

int NIMEASIROCReader::daq_run()
{
    if (m_debug) {
        std::cerr << "*** NIMEASIROCReader::run" << std::endl;
    }

    if (check_trans_lock()) {  // check if stop command has come
        set_trans_unlock();    // transit to CONFIGURED state
        return 0;
    }

    if (m_out_status == BUF_SUCCESS) {   // previous OutPort.write() successfully done
        int ret = read_data_from_detectors();
        if (ret > 0) {
            m_recv_byte_size = ret;
            set_data(m_recv_byte_size); // set data to OutPort Buffer
        }
    }

    if (write_OutPort() < 0) {
        ;     // Timeout. do nothing.
    }
    else {    // OutPort write successfully done
        inc_sequence_num();                     // increase sequence num.
        inc_total_data_size(m_recv_byte_size);  // increase total data byte size
    }

    return 0;
}

void NIMEASIROCReader::MonitorMode()
{

  return;
}

void NIMEASIROCReader::DaqMode()
{

  return;
}

unsigned int NIMEASIROCReader::unpackBigEndian32(const unsigned char* array4byte)
{
    if(sizeof(array4byte) !=4){
      std::cerr << __FILE__ << "  " << __FUNCTION__  << " size of input is not 4 bytes! "<< std::endl;
      return 0;
    }

    return ((array4byte[0] << 24) & 0xff000000) |
           ((array4byte[1] << 16) & 0x00ff0000) |
           ((array4byte[2] <<  8) & 0x0000ff00) |
           ((array4byte[3] <<  0) & 0x000000ff);
}


bool NIMEASIROCReader::isAdcHg(unsigned int data)
{
  return (data & 0x00680000) == 0x00000000;
}


bool NIMEASIROCReader::isAdcLg(unsigned int data)
{
  return (data & 0x00680000) == 0x00080000;
}


bool NIMEASIROCReader::isTdcLeading(unsigned int data)
{
  return (data & 0x00601000) == 0x00201000;
}


bool NIMEASIROCReader::isTdcTrailing(unsigned int data)
{
  return (data & 0x00601000) == 0x00200000;
}


bool NIMEASIROCReader::isScaler(unsigned int data)
{
  return (data & 0x00600000) == 0x00400000;
}


extern "C"
{
    void NIMEASIROCReaderInit(RTC::Manager* manager)
    {
        RTC::Properties profile(nimeasiroceader_spec);
        manager->registerFactory(profile,
                    RTC::Create<NIMEASIROCReader>,
                    RTC::Delete<NIMEASIROCReader>);
    }
};
