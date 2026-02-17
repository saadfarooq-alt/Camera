#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    std::cout << "Scanning camera indices 0-5...\n\n";

    for (int i = 0; i < 6; i++) {
        cv::VideoCapture cap(i, cv::CAP_AVFOUNDATION);
        if (cap.isOpened()) {
            std::cout << "  Index " << i << ": FOUND"
                      << "  (" << (int)cap.get(cv::CAP_PROP_FRAME_WIDTH)
                      << "x"   << (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT)
                      << " @ " << cap.get(cv::CAP_PROP_FPS) << "fps)\n";

            // Try to actually grab a frame
            cv::Mat frame;
            cap >> frame;
            if (!frame.empty()) {
                std::cout << "             ^ Successfully grabbed a frame!\n";
            } else {
                std::cout << "             ^ Opens but can't grab frames (likely Continuity Camera)\n";
            }
            cap.release();
        } else {
            std::cout << "  Index " << i << ": not available\n";
        }
    }

    std::cout << "\nDone. Use the index marked 'grabbed a frame' in camera_capture.\n";
    return 0;
}
