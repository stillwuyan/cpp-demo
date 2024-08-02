/*********************************************************************************
* Copyright(C),XXX
* FileName:    RTPCapture.hpp
* Author:      YYY
* Date:        2024-07-31
* Description: The declaration of RTPCapture
*
*********************************************************************************/
#ifndef __CONTROLLER_RTPCAPTURE_HPP__
#define __CONTROLLER_RTPCAPTURE_HPP__

/*********************************************************************************
*                      Include                                                   *
**********************************************************************************/
#include <memory>
#include <ICapture.hpp>

namespace capture {
namespace video {

/*********************************************************************************
*                      Macro                                                     *
**********************************************************************************/


/*********************************************************************************
*                      Enum/Struct                                               *
**********************************************************************************/
enum CaptureState
{
    CAPTURE_STATE_IDLE,
    CAPTURE_STATE_STARTED,
};


/*********************************************************************************
*                      Variable                                                  *
**********************************************************************************/


/*********************************************************************************
*                      Class                                                     *
**********************************************************************************/
class RTPCapture
    : public ICapture
{
public:
    RTPCapture();
    virtual ~RTPCapture();

    bool start(const StreamInfo& info) override;
    void stop() override;

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
    CaptureState mState;
};

} // namespace video
} // namespace capture
#endif // __CONTROLLER_RTPCAPTURE_HPP__
