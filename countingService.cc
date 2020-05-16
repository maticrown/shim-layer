//
// Created by Matanel on 3/25/2020.
//

#include "countingService.h"
#include <vector>
#include <cstdint>
#include <string>
#include <cstdio>
#include <set>
#include <ifaddrs.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "emane/utils/parameterconvert.h"
#include "emane/logserviceprovider.h"
#include "emane/configureexception.h"
#include "emane/controls/serializedcontrolmessage.h"
#include "emane/controls/r2rineighbormetriccontrolmessageformatter.h"
#include "emane/controls/r2riqueuemetriccontrolmessageformatter.h"
#include "emane/controls/r2riselfmetriccontrolmessageformatter.h"



namespace
{
    const char *__MODULE__ = "FirstShim::countingService";

    const char* _PLACE_ = "WAVE_FROM_ROOT";

    const char*__SNMP__ ="SNMP::trap";

    EMANE::StatisticTableLabels STATISTIC_TABLE_LABELS {"Write Error", "Frame Error"};
}

static long      nstAgentSubagentObject = 2;

const EMANE::ConfigParameterMapType DefaultCountingConfiguration =
{
// id, type, value, description
	{"interface",	{"s", "eth0", "the interface to connect"}},
	{"interfaceaddress",{"a","100.10.0.1","interface address"}}
};
// Constructor
    EMANE::CountingService::CountingService(NEMId id, PlatformServiceProvider *pPlatformService,
                                            EMANE::RadioServiceProvider *pRadioService):
            id_{id},
            pPlatformService_{pPlatformService},
            pRadioService_{pRadioService}
{
	countingConfiguration_ = DefaultCountingConfiguration;
}

// Default Destructor
    EMANE::CountingService::~CountingService() = default;

//configuration items
    const EMANE::ConfigParameterMapType & EMANE::CountingService::getConfigItems() const
	{
		return countingConfiguration_;
	}

//function to accept the s object from showIP()

   char* EMANE::CountingService::getS()
   {
	char* addressv_=s.showIP();
	return addressv_;
   }



int
EMANE::CountingService::set_addr(const ConfigurationUpdate &update)
{
   int n;

    struct ifreq ifr;

    for(const auto & item : update)
        {

            if(item.first=="interface")
            {
// I think it does that here where it takes the ip address given in the xml fi$
                interface_ = item.second[0].asString();
                LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                INFO_LEVEL,
                "SHIMI %03hu %s::%s %s = %s",
                id_,
                __MODULE__,
                __func__,
                item.first.c_str(),
                interface_);
            }

    n = socket(AF_INET, SOCK_DGRAM, 0);

    //Type of address to retrieve - IPv4 IP address

    ifr.ifr_addr.sa_family = AF_INET;

    //Copy the interface name in the ifreq structure

   for(int i=0;i<interface_.length();i++){

    strncpy(ifr.ifr_name , interface_.c_str(),IFNAMSIZ - 1 );
}
    ioctl(n, SIOCGIFADDR, &ifr);

    close(n);

    //display result

    //printf("IP Address is %s - %s\n" , array , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );

	LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                INFO_LEVEL,
                "SHIMI %03hu %s::%s %s = %s",
                id_,
                __MODULE__,
                __func__,
                ifr.ifr_name,
		inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));
}
    return 0;
}



// The function configuring the service
    void EMANE::CountingService::configure(const ConfigurationUpdate &update)
    {
// we get this so we can change it later to the ip of the interface

        LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                                DEBUG_LEVEL,
                                "SHIMI %03hu %s::%s", id_,  __MODULE__, __func__);



     const EMANE::ConfigParameterMapType & countingConfiguration{getConfigItems()};

        for(const auto & item : update)
        {

            if(item.first=="address")
            {
// I think it does that here where it takes the ip address given in the xml file 
                address_ = item.second[0].asINETAddr();

                LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                INFO_LEVEL,
                "SHIMI %03hu %s::%s %s = %s",
                id_,
                __MODULE__,
                __func__,
                item.first.c_str(),
                address_.str(false).c_str());
            }

	else if (item.first=="interface")
	{
		//std::string name=set_addr();
		interface_=item.second[0].asString();
	//	addressV_=getS();
		//inet_addr_=s.set_Addr(address_,mask_);
		LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                DEBUG_LEVEL,
                "SHIMI %03hu %s::%s %s=%s %s",
                id_,
                __MODULE__,
                __func__,
                item.first.c_str(),
                interface_.c_str());
//		addressV_,
		//inet_addr_


	}

//	else if (item.first=="bitrate")
//	{
//		u64BitRate_= item.second[0].asUINT64();

//		LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
  //              DEBUG_LEVEL,
    //            "SHIMI %03hu %s::%s %s=%u kps",
      //          id_,
        //        __MODULE__,
          //      __func__,
            //    item.first.c_str(),
              //  u64BitRate_);
	//}

/*	else if (item.first=="addressRedis")
	{
		addressRedis_= item.second[0].asINETAddr();

		LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
		INFO_LEVEL,
		"SHIMI %03hu %s::%s %s=%s",
		id_,
		__MODULE__,
		__func__,
		item.first.c_str(),
		addressRedis_.str(false).c_str());
	}


	else if (item.first=="kps")
	{
		kps_ = item.second[0].asUINT64();

		LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                        INFO_LEVEL,
                        "SHIMI %03hu %s::%s %s=%u",
                        id_,
                        __MODULE__,
                        __func__,/
                        item.first.c_str(),
			kps_);

	}
*/
            else
             {
                LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                        INFO_LEVEL,
                        "SHIMI %03hu %s::%s %s",
                        id_,
                        __MODULE__,
                        __func__,
                        item.first.c_str());
            }
 
	}
    }
// Starting the emane modem service
    void EMANE::CountingService::start()
    {

	 //pBitPool_->setMaxSize(u64BitRate_);

        LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                                DEBUG_LEVEL,
                                "SHIMI %03hu %s::%s", id_, __MODULE__, __func__);

/*	time_stamp = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());

	global_counter_for_trap_sent = 0;

	std::string str_address = addressRedis_.str(false).c_str();
        std::uint32_t myaddress = inet_addr(str_address.c_str());
	const int NBYTES = 4;
        std::uint8_t octet[NBYTES];
        char ipAddressFinal[16];
        for(int i=0; i<NBYTES; i++) {
            octet[i] = myaddress >> (i * 8);}
	sprintf(ipAddressFinal, "%d.%d.%d.%d", octet[0],octet[1],octet[2],octet[3]);
        const char* hostname = ipAddressFinal;
      // port used to connect to redis
        int port = 6379;
      // timeout is 1.5 seconds
        struct timeval timeout = {1,500000};
        c = redisConnectWithTimeout(hostname, port, timeout);
     // if connection was unsuccessful
	if (c==NULL || c->err)
 	{
		if(c)
		{
			LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
				ERROR_LEVEL,
				"SHIMI %03hu %s::%s Connection error %s",
				id_, __MODULE__,__func__,c->errstr);
			redisFree(c);
		}
		else
		{
			LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                                ERROR_LEVEL,
                                "SHIMI %03hu %s::%s Connection error: can't allocate redis context",
                                id_, __MODULE__,__func__);
		}
	}

 	else
	{
		LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                                DEBUG_LEVEL,
                                "SHIMI %03hu %s::%s Connection Successful",
                                id_, __MODULE__,__func__);
                start_timer = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
	}*/
//  pBitPool_->setMaxSize(u64BitRate_);

   }

   void EMANE::CountingService::postStart()
	{
	int n;

    	struct ifreq ifr;

	char array[]="emane0";

    n = socket(AF_INET, SOCK_DGRAM, 0);

    //Type of address to retrieve - IPv4 IP address

    ifr.ifr_addr.sa_family = AF_INET;

    //Copy the interface name in the ifreq structure


    strncpy(ifr.ifr_name , array ,IFNAMSIZ - 1 );

    ioctl(n, SIOCGIFADDR, &ifr);

    close(n);

    //display result

    //printf("IP Address is %s - %s\n" , array , inet_ntoa(( (struct sockaddr_$

        LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                INFO_LEVEL,
                "SHIMI %03hu %s::%s %s = %s",
                id_,
                __MODULE__,
                __func__,
                ifr.ifr_name,
                inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));

//    return 0;


	}



   void EMANE::CountingService::destroy()
   {
   	LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                                ERROR_LEVEL,
                                "SHIMI %03hu %s::%s",
                                id_, __MODULE__,__func__);
	//redisFree(c);
   }

// The function counting packets being transmitted downward
    bool EMANE::CountingService::countDataMessages(DownstreamPacket &pkt)
    {
        // get the vector of the packet
        auto vIO = pkt.getVectorIO();
        auto v = vIO[0];

        //get the IPv4 destination address of the packet
        std::uint32_t addrV = ((Utils::Ip4Header*) ((Utils::EtherHeader*) v.iov_base+1))->u32Ipv4dst;

        std::string str_address = address_.str(false);
        std::uint32_t myaddress = inet_addr(str_address.c_str());

        const int NBYTES = 4;
        std::uint8_t octet[NBYTES];
        char ipAddressFinal[16];
        for(int i=0; i<NBYTES; i++) {
            octet[i] = addrV >> (i * 8);
        }
        sprintf(ipAddressFinal, "%d.%d.%d.%d", octet[3], octet[2], octet[1], octet[0]);

        LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                                DEBUG_LEVEL,
                                "SHIMI %03hu %s::%s packetAddr = %u  myaddress = %u ipAddressFinal = %s",
                                id_, __MODULE__, __func__, addrV, myaddress, ipAddressFinal);
        if(addrV==myaddress)
            {
                LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                        DEBUG_LEVEL,
                        "SHIMI %03hu %s::%s checked if equal, if it is drop it since it's occupied",
                         id_, __MODULE__, __func__);

                return false;
            }
        else
        {

         counter++;

         LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                        DEBUG_LEVEL,
                        "SHIMI %03hu %s::%s checked if equal, if not, add one more to counter. The number of passed packets untill now is: %d",
                         id_, __MODULE__, __func__,counter);
            return true;
        }

    }

void EMANE::CountingService::bitrate(DownstreamPacket &pkt)

{
	const TimePoint beginTime{Clock::now()};

	std::uint64_t size = pkt.length();

	auto micro = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - beginTime);

	std::chrono::duration<long, std::micro> int_usec = micro;

	auto rate = micro/size;

	LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                        DEBUG_LEVEL,
                        "SHIMI %03hu %s::%s, bitrate is %u, size %u",
                         id_, __MODULE__, __func__,rate,size);

}


// check parameters
  bool EMANE::CountingService::send_trap(DownstreamPacket & pkt)
	{
	std::string aredis=address_.str(false);

        std::uint32_t addre = inet_addr(aredis.c_str());

        const int NBYTES = 4;
        std::uint8_t octet[NBYTES];
	char ipAddr[16];
        for(int i=0;i<NBYTES;i++)
        {
                octet[i]=addre>>(i*8);
        }

	sprintf(ipAddr, "%d. %d. %d. %d",octet[0],octet[1],octet[2],octet[3]);

	int port =6379;

	const char* hostname=ipAddr;


	global_counter_for_trap_sent=0;

      // timeout is 1.5 seconds
        struct timeval timeout = {1,500000};
        c = redisConnectWithTimeout(hostname, port, timeout);


		   LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                        DEBUG_LEVEL,
                        "SHIMI %03hu %s::%s starting trap function",
                         id_, __MODULE__, __func__);

		redisReply *reply;

// !!!!!!!!!!here make if statement of if over 100kps then send the trap instead of proprtion bigger than upperbound
	// getting the ip address where the trap should be sent
		std::string mibpktq = std::string(_PLACE_) + "TRAP_IP";
		std::string strkeypktq = std::to_string(id_).c_str()+ mibpktq;
		const char* pktqkey = strkeypktq.c_str();
		reply = static_cast<redisReply*>(redisCommand(c, "GET %s", pktqkey));
		std::string TRAP_IP = reply->str;
		freeReplyObject(reply);

		// get the ip port where the trap should be sent to
		mibpktq = std::string(_PLACE_) + "TRAP_PORT";
		strkeypktq = std::to_string(id_).c_str() +mibpktq;
		pktqkey = strkeypktq.c_str();
		reply = static_cast<redisReply*>(redisCommand(c, "GET %s", pktqkey));
		std::string TRAP_PORT = reply->str;
		freeReplyObject(reply);

		++global_counter_for_trap_sent;

		std::stringstream ss;
		ss << "snmptrap -v2c -c public"
		<< TRAP_IP << ":" << TRAP_PORT << "1.3.6.1.6.3.1.1.5 1.3.6.1.2.1.1.3.0 t" << time_stamp <<"1.3.6.1.6.3.1.1.4.1.0 oid 1.3.6.1.4.1.1111.1.18.1.0.1.0" << "1.3.6.1.4.1.1111.1.18.1.0.2.0 i" << global_counter_for_trap_sent;
		std::string str = ss.str();
		const char* command = str.c_str();
		system(command);

		const char *strvalpktq="1";

	        LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                        DEBUG_LEVEL,
                        "SHIMI %03hu %s::%s sending trap: %s to %s:%s, the trap is: %s",
                         id_, __MODULE__, __func__, strvalpktq, TRAP_IP.c_str(), TRAP_PORT.c_str(), command);

			return true;

		

	}

EMANE::INETAddr & EMANE::CountingService::get_addr()
{
	return Addr_;
}

EMANE::INETAddr & EMANE::CountingService::get_mask()
{
	return Mask_;
}

int EMANE::CountingService::open(const char *sDevicePath, const char *sDeviceName)
{
  int result;

  // open tuntap device
  if((Handle_ = ::open(sDevicePath, O_RDWR)) == -1)
    {
      LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                              ABORT_LEVEL,
                              "FirstShim::%s:open:error %s",
                              __func__,
                              strerror(errno));

      // fail
      return -1;
    }

  // interface info
  struct ifreq ifr;

  // clear ifr
  memset(&ifr, 0, sizeof(ifr));

  // copy dev name
  strncpy(ifr.ifr_name, sDeviceName, sizeof(ifr.ifr_name));

  // set flags no proto info and tap mode
  ifr.ifr_flags = IFF_NO_PI | IFF_TAP;

  // set tun flags
  if(ioctl(Handle_, TUNSETIFF, &ifr) < 0)
    {
      LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                              ABORT_LEVEL,
                              "FirstShim::%s:ioctl:error %s",
                              __func__,
                              strerror(errno));

      // fail
      return -1;
    }

  // clear ifr
  memset(&ifr, 0, sizeof(ifr));

  // copy dev name
  strncpy(ifr.ifr_name, sDeviceName, sizeof(ifr.ifr_name));

  int ctrlsock = socket(AF_INET,SOCK_DGRAM,0);

  // get iff index
  if(ioctl(ctrlsock, SIOCGIFINDEX, &ifr) < 0)
    {
      LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                              ABORT_LEVEL,
                              "FirstShim::%s:getindex:error %s",
                              __func__,
                              strerror(errno));

      // fail
      return -1;
    }

  // save the dev path, name, guid and index
  Path_  = sDevicePath;
  Name_  = sDeviceName;
  Guid_  = "n/a";
  Index_ = ifr.ifr_ifindex;

  // close control socket
  ::close(ctrlsock);

  // success
  result = 0;


  LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                          DEBUG_LEVEL,
                          "FirstShim::%s, path %s, name %s, guid %s, index %d",
                          __func__,
                          Path_.c_str(),
                          Name_.c_str(),
                          Guid_.c_str(),
                          Index_);

  // return result
  return result;
}

/*int EMANE::CountingService::close()
{
 ::close(Handle_);
 return 0;
}
*/
int EMANE::CountingService::get_handle()
{
 return Handle_;
}

int EMANE::CountingService::writev(const struct iovec *iov, size_t iov_len)
{
 int result;

  result = ::write(Handle_, iov, iov_len);

  // check result
  if(result < 0)
    {
      LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                              ABORT_LEVEL,
                              "FirstShim::%s:write:error %s",
                              __func__,
                              strerror(errno));
    }

  // return result
  return result;
}

int EMANE::CountingService::readv(struct iovec *iov, size_t iov_len)
{
  int result;

  result = ::read(Handle_, iov, iov_len);

  // check result
  if(result < 0)
    {
      LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                              ABORT_LEVEL,
                              "FirstShim::%s:read:error %s",
                              __func__,
                              strerror(errno));
    }

  // return result
  return result;
}

/*void EMANE::CountingService::init_nstAgentSubagentObject(void)
{
    static oid      nstAgentSubagentObject_oid[] =
        { 1, 3, 6, 1, 4, 1, 8072, 2, 4, 1, 1, 2, 0 };


  LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                         DEBUG_LEVEL,
                "nstAgentSubagentObject",
                "Initializing the nstAgentSubagentObject module\n", id_,__SNMP__,__func__);


     LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                         DEBUG_LEVEL,
                "nstAgentSubagentObject",
                "Initalizing nstAgentSubagentObject scalar integer.  Default value = %d\n",
                nstAgentSubagentObject,id_,__SNMP__,__func__);

//    netsnmp_register_long_instance("nstAgentSubagentObject",
  //                                nstAgentSubagentObject_oid,
    //                              OID_LENGTH(nstAgentSubagentObject_oid),
      //                            &nstAgentSubagentObject, NULL);

     LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                         DEBUG_LEVEL,
                "nstAgentSubagentObject",
                "Done initalizing nstAgentSubagentObject module\n",id_,__SNMP__,__func__);

}
*/
/*void EMANE::CountingService::init_notification(void)
{
    LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                            DEBUG_LEVEL,
                             __SNMP__, __func__,
		"example_notification",
                "initializing (setting callback alarm)\n");
    snmp_alarm_register(30,     /* seconds */
/*                        SA_REPEAT,      /* repeat (every 30 seconds). */
  //                      send_example_notification,      /* our callback */
    //                    NULL    /* no callback data needed */
      //  );
//}

/*void send_example_notification(unsigned int clientreg, void *clientarg)
{
    /*
     * define the OID for the notification we're going to send
     * NET-SNMP-EXAMPLES-MIB::netSnmpExampleHeartbeatNotification 
     */
  /*  oid             notification_oid[] =
        { 1, 3, 6, 1, 4, 1, 8072, 2, 3, 0, 1 };
    size_t          notification_oid_len = OID_LENGTH(notification_oid);
    static u_long count = 0;

    /*
     * In the notification, we have to assign our notification OID to
     * the snmpTrapOID.0 object. Here is it's definition. 
     */
    /*oid             objid_snmptrap[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };
    size_t          objid_snmptrap_len = OID_LENGTH(objid_snmptrap);

    oid      hbeat_rate_oid[]   = { 1, 3, 6, 1, 4, 1, 8072, 2, 3, 2, 1, 0 };
    size_t   hbeat_rate_oid_len = OID_LENGTH(hbeat_rate_oid);
    oid      hbeat_name_oid[]   = { 1, 3, 6, 1, 4, 1, 8072, 2, 3, 2, 2, 0 };
    size_t   hbeat_name_oid_len = OID_LENGTH(hbeat_name_oid);

    /*
     * here is where we store the variables to be sent in the trap 
     */
    /*netsnmp_variable_list *notification_vars = NULL;
    const char *heartbeat_name = "A girl named Maria";
#ifdef  RANDOM_HEARTBEAT
    int  heartbeat_rate = rand() % 60;
#else
    int  heartbeat_rate = 30;
#endif

			printf((
			"example_notification", "defining the trap\n"));

    /*
     * add in the trap definition object 
     */
    /*snmp_varlist_add_variable(&notification_vars,
                              objid_snmptrap, objid_snmptrap_len,
                              /*
                               * value type is an OID 
                               */
      //                        ASN_OBJECT_ID,
                              /*
                               * value contents is our notification OID 
                               */
        /*                      (u_char *) notification_oid,
                              notification_oid_len * sizeof(oid));

    /*
     * add in the additional objects defined as part of the trap
     */

    /*snmp_varlist_add_variable(&notification_vars,
                               hbeat_rate_oid, hbeat_rate_oid_len,
                               ASN_INTEGER,
                              (u_char *)&heartbeat_rate,
                                  sizeof(heartbeat_rate));

    /*
     * if we want to insert additional objects, we do it here 
     */
    /*if (heartbeat_rate < 30 ) {
        snmp_varlist_add_variable(&notification_vars,
                               hbeat_name_oid, hbeat_name_oid_len,
                               ASN_OCTET_STR,
                            heartbeat_name, strlen(heartbeat_name));
    }

    /*
     * send the trap out.  This will send it to all registered
     * receivers (see the "SETTING UP TRAP AND/OR INFORM DESTINATIONS"
     * section of the snmpd.conf manual page. 
     */
   /* ++count;
			printf(
			 "sending trap %ld\n",count);
			send_v2trap(notification_vars);    
/*
     * free the created notification variable list 
     */
//			printf((
//			"example_notification", "cleaning up\n"));
  //  snmp_free_varbind(notification_vars);
//}



