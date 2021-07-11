#include <thread>
#include <chrono>
#include <iostream>
#include <string>

#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sstream>
#include <algorithm>

#include <ctype.h>
#include <malloc.h>
#include <sys/types.h>
#include <linux/if.h>

#define NTP_PORT               123               /*NTP专用端口号字符串*/
#define TIME_PORT              37               /* TIME/UDP端口号 */
#define NTP_SERVICE_IP       "61.135.250.78"  /*国家授时中心IP*/
#define NTP_SERVICE_PORT        "123"          /*NTP专用端口号字符串*/
#define NTPV1                "NTP/V1"      /*协议及其版本号*/
#define NTPV2                "NTP/V2"
#define NTPV3                "NTP/V3"
#define NTPV4                "NTP/V4"
#define TIME                "TIME/UDP"
#define EIGHT_HOUR_TIMEZONE_OFFSET   28800

#define NTP_PCK_LEN 48
#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4 
#define PREC -6

#define JAN_1970  0x83aa7e80  /* 1900年～1970年之间的时间秒数 */
#define NTPFRAC(x)   (4294 * (x) + ((1981 * (x)) >> 11))
#define USEC(x)         (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))

class FetchNTPTimeThread
{
public:
    FetchNTPTimeThread();
    ~FetchNTPTimeThread();
    void run();

private:
    int create_ntp_packet(char *ntp_packer);
    int send_udp_request();
    int receive_ntp_network_time(std::string &current_network_time);
    int get_socket_keyword();
    int get_local_address_message();

    int GetNTPTime(void);
    unsigned long GetTickCount(void);

private:
	static int sc_nDiffSec;
    static int socket_key;
    static struct addrinfo *t_service_address;
};

int FetchNTPTimeThread::socket_key = -1;
struct addrinfo *FetchNTPTimeThread::t_service_address = NULL;
int FetchNTPTimeThread::sc_nDiffSec = 0;

FetchNTPTimeThread::FetchNTPTimeThread()
{}

FetchNTPTimeThread::~FetchNTPTimeThread()
{}

void FetchNTPTimeThread::run()
{
    while(true)
    {
        if (0 == GetNTPTime())
        {
            return;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

int FetchNTPTimeThread::GetNTPTime()
{
    std::string network_time = "";

    if (FetchNTPTimeThread::socket_key < 0)
    {
        get_local_address_message();
        get_socket_keyword();
    }

    if (send_udp_request() || receive_ntp_network_time(network_time))
    {
        std::cout << "get network time send or receive failure" << std::endl;
        network_time.clear();
        return 1;
    }

    std::cout << ".Diff time= " << sc_nDiffSec << std::endl;
    std::cout << ".NTP time = " << network_time << std::endl;

    return 0;
}

int FetchNTPTimeThread::get_local_address_message()
{
    int ret;
    struct addrinfo client_address;

    memset(&client_address , 0 , sizeof(struct addrinfo));

    client_address.ai_family = AF_UNSPEC;
    client_address.ai_socktype = SOCK_DGRAM;
    client_address.ai_protocol = IPPROTO_UDP;

    ret = getaddrinfo(NTP_SERVICE_IP , NTP_SERVICE_PORT , &client_address , &FetchNTPTimeThread::t_service_address);
    if (ret)
    {
        std::cout << "get local address faiulre" << std::endl;
        return 1;
    }

    return 0;
}


int FetchNTPTimeThread::create_ntp_packet(char *ntp_packer)
{
    int             version = 3;
    time_t          timer;
    //long          ntp_head;
    long int    ntp_head;

    std::cout << "create ntp packet" << std::endl;

    ntp_head = htonl((LI << 30) | (version << 27) | (MODE << 24) | (STRATUM << 16) | (POLL << 8) | (PREC & 0xff));

    memcpy(ntp_packer , &ntp_head , sizeof(ntp_head));

    ntp_head = htonl(1 << 16);

    memcpy(&ntp_packer[4], &ntp_head , sizeof(ntp_head));
    memcpy(&ntp_packer[8], &ntp_head , sizeof(ntp_head));

    time(&timer);

    ntp_head = htonl(JAN_1970 + (unsigned int)timer);
    memcpy(&ntp_packer[40] , &ntp_head , sizeof(ntp_head));

    ntp_head = htonl((unsigned int)NTPFRAC(timer));
    memcpy(&ntp_packer[44] , &ntp_head , sizeof(ntp_head));

    return 0;
}


int FetchNTPTimeThread::send_udp_request()
{
    char    head_info[NTP_PCK_LEN];
    int     result = 1;
    ssize_t ret_socket = -1;
    int     address_length = t_service_address->ai_addrlen;

    memset(head_info , '\0' , sizeof(head_info));
    create_ntp_packet(head_info);

    if (((ret_socket = sendto(FetchNTPTimeThread::socket_key , head_info , NTP_PCK_LEN , 0 , (struct sockaddr * ) t_service_address->ai_addr , address_length)) < 0))
    {
        std::cout << "sendto by failure" << std::endl;
        return result;
    }

    return result = 0;
}


int FetchNTPTimeThread::receive_ntp_network_time(std::string &current_network_time)
{
    fd_set                  pending_data;
    struct timeval          block_time;
//    char                  head_info[NTP_PCK_LEN];
    char                    recv_packet[NTP_PCK_LEN * 8] = {'\0'};
    ssize_t                 count;
    int                     select_status;
    unsigned int            network_time;
    time_t                  service_time;
    char                    time_buffer[128];
    socklen_t               address_length = t_service_address->ai_addrlen;

    FD_ZERO(&pending_data);
    FD_SET(FetchNTPTimeThread::socket_key, &pending_data);

    block_time.tv_sec = 0;
    block_time.tv_usec = 100000;
    //create_ntp_packet(head_info);

    unsigned long ulBefore = GetTickCount();

    if ((select_status = select(FetchNTPTimeThread::socket_key + 1 , &pending_data , NULL , NULL , &block_time)) > 0)
    {
        unsigned long ulAfter = GetTickCount();

        std::cout << "Diff milisec is : " << ulAfter - ulBefore << std::endl;

        count = recvfrom(FetchNTPTimeThread::socket_key , recv_packet , NTP_PCK_LEN * 8 , 0 , (struct sockaddr *)t_service_address->ai_addr , &address_length);
        if (count < 0)
        {
            return 1;
        }
    }
    else
    {
        std::cout << "select sleep failure" << std::endl;
        return 1;
    }

    network_time = ntohl(*(int *) & (recv_packet[40]));
    network_time -= JAN_1970;
    service_time = network_time + EIGHT_HOUR_TIMEZONE_OFFSET;

    time_t tt_temp = time(NULL);

    sc_nDiffSec = static_cast<int>(service_time - tt_temp);

    tm *t = localtime(&service_time);
    tm _tm;
    memset(&_tm, 0x00, sizeof(tm));
    if (t)
    {
        _tm.tm_year = t->tm_year + 1900;
        _tm.tm_mon = t->tm_mon + 1;
        _tm.tm_mday = t->tm_mday;
        _tm.tm_hour = t->tm_hour;
        _tm.tm_min = t->tm_min;
        _tm.tm_sec = t->tm_sec;
    }
    else
    {
        std::cout << "localtime failed and the memory maybe leaked" << std::endl;
    }

    struct timeval _us;
    gettimeofday(&_us, NULL);

    memset(time_buffer, 0x00, sizeof(time_buffer));
    sprintf(time_buffer, "%04d-%02d-%02d %02d:%02d:%02d,%03ld", _tm.tm_year, _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec, _us.tv_usec / 1000);
    current_network_time = time_buffer;

    return 0;
}

int FetchNTPTimeThread::get_socket_keyword()
{
    int     result = 0;

    FetchNTPTimeThread::socket_key = socket(t_service_address->ai_family, t_service_address->ai_socktype, t_service_address->ai_protocol);

    if (FetchNTPTimeThread::socket_key < 0 )
    {
        std::cout << "get socket key failure" << std::endl;
        return 1;
    }

    return result;
}

unsigned long FetchNTPTimeThread::GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

int main()
{
    FetchNTPTimeThread obj;
    obj.run();
    return 1;
}
