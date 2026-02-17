#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

void printHelp() {
    std::cout << "\n=== Camera Viewer Controls ===\n"
              << "  Q / ESC  - Quit\n"
              << "  S        - Save a snapshot (snapshot_<n>.png)\n"
              << "  G        - Toggle grayscale mode\n"
              << "  F        - Toggle horizontal flip\n"
              << "  I        - Print camera info to console\n"
              << "==============================\n\n";
}

int main(int argc, char* argv[]) {
    int cameraIndex = 0;
    if (argc > 1) {
        cameraIndex = std::stoi(argv[1]);
    }

    cv::VideoCapture cap(cameraIndex);

    if (!cap.isOpened()) {
        std::cerr << "[ERROR] Could not open camera index " << cameraIndex << ".\n"
                  << "        Try a different index, e.g.: ./camera_capture 1\n";
        return 1;
    }

    printHelp();

    bool grayscale   = false;
    bool flipped     = false;
    int  snapCounter = 0;

    cv::Mat frame, display;
    const std::string windowName = "Camera Capture  [Q/ESC=quit  S=snap  G=gray  F=flip  I=info]";
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

    while (true) {
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "[WARNING] Empty frame received – camera may have disconnected.\n";
            break;
        }

        display = frame.clone();

        if (flipped) {
            cv::flip(display, display, 1);  
        }

        if (grayscale) {
            cv::cvtColor(display, display, cv::COLOR_BGR2GRAY);
            cv::cvtColor(display, display, cv::COLOR_GRAY2BGR);
        }

        {
            std::string status = std::string(grayscale ? "[GRAY] " : "[COLOR] ")
                               + (flipped ? "[FLIP]" : "");
            cv::putText(display, status,
                        cv::Point(10, 28),
                        cv::FONT_HERSHEY_SIMPLEX, 0.7,
                        cv::Scalar(0, 255, 0), 2);
        }

        cv::imshow(windowName, display);

        int key = cv::waitKey(1) & 0xFF;

        if (key == 'q' || key == 'Q' || key == 27 /* ESC */) {
            std::cout << "Quitting.\n";
            break;
        }
        else if (key == 's' || key == 'S') {
            std::string filename = "snapshot_" + std::to_string(++snapCounter) + ".png";
            cv::imwrite(filename, display);
            std::cout << "Saved: " << filename << "\n";
        }
        else if (key == 'g' || key == 'G') {
            grayscale = !grayscale;
            std::cout << "Grayscale: " << (grayscale ? "ON" : "OFF") << "\n";
        }
        else if (key == 'f' || key == 'F') {
            flipped = !flipped;
            std::cout << "Flip: " << (flipped ? "ON" : "OFF") << "\n";
        }
        else if (key == 'i' || key == 'I') {
            printCameraInfo();
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}