/*********************************************************************************
* Copyright(C),XXX
* FileName:    ICapture.hpp
* Author:      YYY
* Date:        2024-07-31
* Description: The declaration of ICapture
*
*********************************************************************************/
#ifndef __INTERFACES_ICAPTURE_HPP__
#define __INTERFACES_ICAPTURE_HPP__

/*********************************************************************************
*                      Include                                                   *
**********************************************************************************/
#include <string>

namespace capture {
namespace video {

/*********************************************************************************
*                      Macro                                                     *
**********************************************************************************/


/*********************************************************************************
*                      Enum/Struct                                               *
**********************************************************************************/
enum StreamType
{
    STREAM_TYPE_RTP_TS,
};

struct StreamInfo
{
    StreamType type;
    uint64_t timeMs;
    uint32_t maxCnt;
    uint32_t clockRate;
    uint32_t port;
    std::string ip;
    std::string path;
};


/*********************************************************************************
*                      Variable                                                  *
**********************************************************************************/


/*********************************************************************************
*                      Class                                                     *
**********************************************************************************/

class ICapture
{
public:
    ICapture() = default;
    virtual ~ICapture() = default;

    virtual bool start(const StreamInfo& info) = 0;
    virtual void stop() = 0;
};

} // namespace video
} // namespace capture
#endif // __INTERFACES_ICAPTURE_HPP__
