#include <memory>
#include <thread>
#include <RTPCapture.hpp>
#include <Log.hpp>

using namespace capture::video;

int main() {
    StreamInfo info {
        STREAM_TYPE_RTP_TS,
        5000,
        5,
        90000,
        5003,
        "239.1.1.1",
        "test",
    };
    std::unique_ptr<ICapture> capture = std::make_unique<RTPCapture>();

    if (!capture->start(info)) {
        LOG_ERR("start failed");
        return 0;
    }

    std::string cmd;
    while (true) {
        std::cin >> cmd;
        if (cmd.find("stop") != std::string::npos) {
            capture->stop();
        } else if (cmd.find("start") != std::string::npos) {
            capture->start(info);
        } else if (cmd.find("exit") != std::string::npos) {
            break;
        }
    }
    return 0;
}
