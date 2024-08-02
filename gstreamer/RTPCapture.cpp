/*********************************************************************************
* Copyright(C),XXX
* FileName:    RTPCapture.cpp
* Author:      YYY
* Date:        2024-07-31
* Description: The implementation of RTPCapture
*
*********************************************************************************/

/*********************************************************************************
*                      Include                                                   *
**********************************************************************************/
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <gst/gst.h>
#include <RTPCapture.hpp>
#include <Log.hpp>
#include <FileUtils.hpp>

namespace capture {
namespace video {

/*********************************************************************************
*                      Macro                                                     *
**********************************************************************************/


/*********************************************************************************
*                      Enum/Struct                                               *
**********************************************************************************/
class RTPCapture::Impl
{
public:
    Impl() = default;
    ~Impl() = default;

    bool initialize(const StreamInfo& info);
    void destroy();
    bool start();
    void stop();

private:
    enum MatchType
    {
        MATCH_TYPE_PAD_TYPE,
        MATCH_TYPE_PAD_NAME
    };

    struct RequestPad
    {
        GstPad* self;
        GstElement* el;

        RequestPad(GstPad *pad, GstElement *el)
            : self(pad), el(el)
        {}
    };

    static void parserPadAddedHandler(GstElement* src, GstPad* pad, RTPCapture::Impl* data);
    static void queuePadAddedHandler(GstElement* src, GstPad* pad, RTPCapture::Impl* data);
    void insidePadAddedHandler(GstElement* src, GstPad* pad, const std::vector<const char*>& srcPadList,
                               GstElement* dst, const std::vector<const char*>& dstPadList, MatchType type);
    const char* getEncodingName(StreamType type);
    void run();

    GstElement* mSource = nullptr;
    GstElement* mJitter = nullptr;
    GstElement* mDepay = nullptr;
    GstElement* mParser = nullptr;
    GstElement* mQueue = nullptr;
    GstElement* mSink = nullptr;
    GstElement* mPipeline = nullptr;
    GstState mState = GST_STATE_NULL;

    std::vector<RequestPad> mRequestPads;
    std::thread mThread;
    std::mutex mMutex;
    bool mStopFlag = false;
};

/*********************************************************************************
*                      Variable                                                  *
**********************************************************************************/


/*********************************************************************************
*                      Function                                                  *
**********************************************************************************/
RTPCapture::RTPCapture()
    : mImpl(std::make_unique<Impl>())
    , mState(CAPTURE_STATE_IDLE)
{}

RTPCapture::~RTPCapture()
{
    stop();
}

bool RTPCapture::start(const StreamInfo& info)
{
    if (mState != CAPTURE_STATE_IDLE) {
        LOG_WARN("Already started, nothing to do!");
        return true;
    }

    if (!mImpl->initialize(info)) {
        LOG_ERR("Failed to initialize RTP stream!");
        mImpl->destroy();
        return false;
    }

    if (mImpl->start()) {
        mState = CAPTURE_STATE_STARTED;
        return true;
    } else {
        mImpl->destroy();
        LOG_ERR("Failed to start RTP stream!");
        return false;
    }
}

void RTPCapture::stop()
{
    if (mState == CAPTURE_STATE_STARTED) {
        mImpl->stop();
        mImpl->destroy();
        mState = CAPTURE_STATE_IDLE;
    } else {
        LOG_WARN("Already stopped, nothing to do!");
    }
}

bool RTPCapture::Impl::initialize(const StreamInfo& info)
{
    gst_init(nullptr, nullptr);

    // Create the elements
    mSource = gst_element_factory_make("udpsrc", "source");
    mJitter = gst_element_factory_make("rtpjitterbuffer", "jitter");
    if (info.type == STREAM_TYPE_RTP_TS) {
        mDepay = gst_element_factory_make("rtpmp2tdepay", "depay");
    } else {
        LOG_ERR("Unsupported RTP stream type!")
        return false;
    }
    mParser = gst_element_factory_make("parsebin", "parser");
    mQueue = gst_element_factory_make("multiqueue", "queue");
    mSink = gst_element_factory_make("splitmuxsink", "sink");

    // Create the empty pipeline
    mPipeline = gst_pipeline_new("rtp-pipeline");
    if (!mSource || !mJitter || !mDepay ||
        !mParser || !mQueue || !mSink ||
        !mPipeline) {
        LOG_ERR("Failed to create GStreamer elements!");
        return false;
     }

    // Build the pipeline
    gst_bin_add_many(GST_BIN(mPipeline),
                     mSource, mJitter, mDepay,
                     mParser, mQueue, mSink,
                     nullptr);
    if (!gst_element_link_many(mSource, mJitter, mDepay, mParser, nullptr)) {
        LOG_ERR("Failed to link source elements!");
        return false;
    }

    // Setup the source element
    uint32_t index = 0;
    if (FileUtils::createDir(info.path)) {
        char name[256] = {0};
        for (; index < info.maxCnt; index++) {
            snprintf(name, sizeof(name), "%s/video%02d.mp4", info.path.c_str(), index);
            if (!FileUtils::isFileExist(name)) {
                break;
            }
        }
        index = (index == info.maxCnt ? 0 : index); // TODO: need check file timestamp
        LOG_INFO("Start index %d", index);
    } else {
        LOG_ERR("Failed to create path %s", info.path.c_str());
        return false;
    }

    GstCaps* caps = gst_caps_new_simple("application/x-rtp",
                                        "media", G_TYPE_STRING, "video",
                                        "clock-rate", G_TYPE_INT, info.clockRate,
                                        "encoding-name", G_TYPE_STRING, getEncodingName(info.type),
                                        nullptr);
    g_object_set(mSource, "address", info.ip.c_str(), nullptr);
    g_object_set(mSource, "port", info.port, nullptr);
    g_object_set(mSource, "caps", caps, nullptr);
    g_object_set(mQueue, "max-size-time", 0, nullptr);
    g_object_set(mSink, "location", (info.path + "/video%02d.mp4").c_str(), nullptr);
    g_object_set(mSink, "max-size-time", info.timeMs * 1000000, nullptr);
    g_object_set(mSink, "start-index", index, nullptr);
    g_object_set(mSink, "max-files", info.maxCnt, nullptr);
    gst_caps_unref(caps);

    // Dynamic link sink by the pad type
    g_signal_connect(mParser, "pad-added", G_CALLBACK(parserPadAddedHandler), this);
    g_signal_connect(mQueue, "pad-added", G_CALLBACK(queuePadAddedHandler), this);

    mState = GST_STATE_NULL;
    return true;
}

void RTPCapture::Impl::destroy()
{
    gst_element_set_state(mPipeline, GST_STATE_NULL);
    if (mPipeline) {
        gst_object_unref(mPipeline);
        mPipeline = nullptr;
        mSource = nullptr;
        mJitter = nullptr;
        mDepay = nullptr;
        mParser = nullptr;
        mQueue = nullptr;
        mSink = nullptr;
    }
}

bool RTPCapture::Impl::start()
{
    GstStateChangeReturn ret = gst_element_set_state(mPipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        LOG_ERR("Failed to start pipline!");
        return false;
    }

    mStopFlag = false;
    mThread = std::thread(&RTPCapture::Impl::run, this);
    return true;
}

void RTPCapture::Impl::stop()
{
    if (mState == GST_STATE_PLAYING) {
        gst_element_send_event(mPipeline, gst_event_new_eos());
        LOG_INFO("Send EOS to stop RTP stream!");
    } else {
        mStopFlag = true;
        LOG_INFO("Stop RTP stream directly!");
    }
    mThread.join();
}

void RTPCapture::Impl::parserPadAddedHandler(GstElement* src, GstPad* pad, RTPCapture::Impl* data)
{
    std::vector<const char*> srcPadNameList {"src_0", "src_1"};
    std::vector<const char*> dstPadNameList {"sink_%u", "sink_%u"};
    data->insidePadAddedHandler(src, pad, srcPadNameList, data->mQueue, dstPadNameList, MATCH_TYPE_PAD_NAME);
}

void RTPCapture::Impl::queuePadAddedHandler(GstElement* src, GstPad* pad, RTPCapture::Impl* data)
{
    std::vector<const char*> srcPadNameList {"src_0", "src_1"};
    std::vector<const char*> dstPadNameList {"video", "audio_%u"};
    data->insidePadAddedHandler(src, pad, srcPadNameList, data->mSink, dstPadNameList, MATCH_TYPE_PAD_NAME);
}

void RTPCapture::Impl::insidePadAddedHandler(GstElement* src, GstPad* pad, const std::vector<const char*>& srcPadList,
                                             GstElement* dst, const std::vector<const char*>& dstPadList, MatchType type)
{
    LOG_INFO("Received new pad '%s' from '%s'", GST_PAD_NAME(pad), GST_ELEMENT_NAME(src));

    GstPadLinkReturn ret;
    const gchar* srcName = nullptr;

    if (type == MATCH_TYPE_PAD_TYPE) {
        // Check the new pad's type
        if (!gst_pad_has_current_caps(pad)) {
            LOG_ERR("No current caps on the %s.%s!", GST_PAD_NAME(pad), GST_ELEMENT_NAME(src));
            return;
        }
        GstCaps* padCaps = gst_pad_get_current_caps(pad);
        GstStructure* padStruct = gst_caps_get_structure(padCaps, 0);
        srcName = gst_structure_get_name(padStruct);
        gst_caps_unref(padCaps);
    } else if (type == MATCH_TYPE_PAD_NAME) {
        // Check the new pad's name
        srcName = GST_PAD_NAME(pad);
    } else {
        LOG_ERR("Unsupport match type! %d", type);
        return;
    }

    for (size_t i = 0; i < srcPadList.size(); ++i) {
        if (g_str_has_prefix(srcName, srcPadList[i])) {
            GstPad* dstPad = gst_element_request_pad_simple(dst, dstPadList[i]);
            if (!dstPad) {
                LOG_ERR("Pad name %s was not found in %s", dstPadList[i], GST_ELEMENT_NAME(dst));
                return;
            }
            if (gst_pad_is_linked(dstPad)) {
                LOG_ERR("Pad %s.%s is already linked", GST_ELEMENT_NAME(dst), GST_PAD_NAME(dstPad))
                gst_object_unref(dstPad);
                return;
            }
            ret = gst_pad_link(pad, dstPad);
            if (GST_PAD_LINK_FAILED(ret)) {
                LOG_ERR("Link failed! %s.%s -> %s.%s", GST_ELEMENT_NAME(src), GST_PAD_NAME(pad), GST_ELEMENT_NAME(dst), GST_PAD_NAME(dstPad));
            } else {
                LOG_INFO("Link success! %s.%s -> %s.%s", GST_ELEMENT_NAME(src), GST_PAD_NAME(pad), GST_ELEMENT_NAME(dst), GST_PAD_NAME(dstPad));
            }
            std::lock_guard<std::mutex> lock(mMutex);
            mRequestPads.emplace_back(dstPad, dst);
            return;
        }
    }
}

const char* RTPCapture::Impl::getEncodingName(StreamType type)
{
    switch (type) {
    case STREAM_TYPE_RTP_TS:
        return "MP2T";
    default:
        LOG_WARN("Unsupported RTP stream type! Use MP2T as default");
        return "MP2T";
    }
}

void RTPCapture::Impl::run()
{
    GstBus* bus = gst_element_get_bus(mPipeline);
    GstMessage* msg = nullptr;
    do {
        msg = gst_bus_timed_pop_filtered(bus, GST_MSECOND * 10,
                                         static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_STATE_CHANGED));
        // Process timeout
        if (!msg && mStopFlag && mState == GST_STATE_PLAYING) {
            gst_element_send_event(mPipeline, gst_event_new_eos());
            mStopFlag = false;
            LOG_INFO("Already playing, send EOS to stop RTP stream!");
            continue;
        } else if (!msg) {
            continue;
        }

        // Process the message
        GError* err = nullptr;
        gchar *debugInfo = nullptr;
        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debugInfo);
            LOG_ERR("Error received from element %s: %s", GST_OBJECT_NAME(msg->src), err->message);
            LOG_ERR("Debugging information: %s", debugInfo ? debugInfo : "none");
            g_clear_error(&err);
            g_free(debugInfo);
            mStopFlag = true;
            break;
        case GST_MESSAGE_EOS:
            LOG_INFO("End-Of-Stream reached.");
            mStopFlag = true;
            break;
        case GST_MESSAGE_STATE_CHANGED:
            // We are only interested in state-changed messages from the pipeline
            if (GST_MESSAGE_SRC(msg) == GST_OBJECT(mPipeline)) {
                GstState oldState, newState, pendingState;
                gst_message_parse_state_changed(msg, &oldState, &newState, &pendingState);
                mState = newState;
                LOG_INFO("Pipeline state changed from %s to %s", gst_element_state_get_name(oldState), gst_element_state_get_name(newState));
            }
            break;
        default:
            LOG_WARN("Unexpected message received.");
            break;
        }
        gst_message_unref(msg);
    } while (!mStopFlag);

    std::lock_guard<std::mutex> lock(mMutex);
    for (const auto& pad : mRequestPads) {
        gst_element_release_request_pad(pad.el, pad.self);
        gst_object_unref(pad.self);
    }
    mRequestPads.clear();
    gst_object_unref(bus);
}

} // namespace video
} // namespace capture
