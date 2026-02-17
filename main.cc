#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

const std::string HAAR_BASE = "/opt/homebrew/Cellar/opencv/4.13.0_3/share/opencv4/haarcascades/";

void printHelp() {
    std::cout << "\n=== Camera Viewer Controls ===\n"
              << "  Q / ESC  - Quit\n"
              << "  I        - Print camera info to console\n"
              << "  E        - Toggle eye detection on/off\n"
              << "==============================\n\n";
}

int main(int argc, char* argv[]) {
    int cameraIndex = 0;
    if (argc > 1) {
        cameraIndex = std::stoi(argv[1]);
    }

    // ── Load Haar cascades ────────────────────────────────────────────────────
    cv::CascadeClassifier faceCascade, eyeCascade;

    if (!faceCascade.load(HAAR_BASE + "haarcascade_frontalface_default.xml")) {
        std::cerr << "[ERROR] Could not load face cascade.\n";
        return 1;
    }
    if (!eyeCascade.load(HAAR_BASE + "haarcascade_eye.xml")) {
        std::cerr << "[ERROR] Could not load eye cascade.\n";
        return 1;
    }

    std::cout << "Cascades loaded OK.\n";

    // ── Open camera ───────────────────────────────────────────────────────────
    cv::VideoCapture cap(cameraIndex, cv::CAP_AVFOUNDATION);
    if (!cap.isOpened()) {
        std::cerr << "[ERROR] Could not open camera index " << cameraIndex << ".\n";
        return 1;
    }
    cap.set(cv::CAP_PROP_FPS, 30);

    printHelp();

    auto printCameraInfo = [&]() {
        std::cout << "Camera #" << cameraIndex << " info:\n"
                  << "  Resolution : "
                  << (int)cap.get(cv::CAP_PROP_FRAME_WIDTH) << " x "
                  << (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT) << "\n"
                  << "  FPS        : " << cap.get(cv::CAP_PROP_FPS) << "\n";
    };
    printCameraInfo();

    bool eyeDetection = true;

    cv::Mat frame, gray, display;
    const std::string windowName = "Eye Tracker  [Q=quit  E=toggle detection  I=info]";
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "[WARNING] Empty frame.\n";
            break;
        }

        display = frame.clone();

        if (eyeDetection) {
            // Work on a grayscale + equalised copy for better detection
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            cv::equalizeHist(gray, gray);

            // ── Step 1: detect faces ──────────────────────────────────────────
            std::vector<cv::Rect> faces;
            faceCascade.detectMultiScale(
                gray,
                faces,
                1.1,   // scale factor
                5,     // min neighbours (higher = fewer false positives)
                0,
                cv::Size(80, 80)  // minimum face size
            );

            for (const auto& face : faces) {
                // Draw face rectangle in blue
                cv::rectangle(display, face, cv::Scalar(255, 100, 0), 2);

                // Label
                cv::putText(display, "Face",
                            cv::Point(face.x, face.y - 8),
                            cv::FONT_HERSHEY_SIMPLEX, 0.6,
                            cv::Scalar(255, 100, 0), 2);

                // ── Step 2: detect eyes within the face region ────────────────
                // Only look in the top 60% of the face (avoids mouth/nose hits)
                cv::Rect upperFace = face;
                upperFace.height = (int)(face.height * 0.6);

                cv::Mat faceROI = gray(upperFace);

                std::vector<cv::Rect> eyes;
                eyeCascade.detectMultiScale(
                    faceROI,
                    eyes,
                    1.1,
                    10,   // high min-neighbours = only confident detections
                    0,
                    cv::Size(20, 20)
                );

                for (const auto& eye : eyes) {
                    // Convert eye coords back to full-frame coords
                    cv::Rect eyeGlobal(
                        upperFace.x + eye.x,
                        upperFace.y + eye.y,
                        eye.width,
                        eye.height
                    );

                    // Draw eye rectangle in green
                    cv::rectangle(display, eyeGlobal, cv::Scalar(0, 220, 0), 2);

                    // Draw a circle at the centre of the eye
                    cv::Point eyeCenter(
                        eyeGlobal.x + eyeGlobal.width  / 2,
                        eyeGlobal.y + eyeGlobal.height / 2
                    );
                    cv::circle(display, eyeCenter, 3, cv::Scalar(0, 255, 0), -1);

                    cv::putText(display, "Eye",
                                cv::Point(eyeGlobal.x, eyeGlobal.y - 5),
                                cv::FONT_HERSHEY_SIMPLEX, 0.5,
                                cv::Scalar(0, 220, 0), 1);
                }
            }

            // Status overlay
            std::string status = "Detection: ON  |  Faces: " + std::to_string(faces.size());
            cv::putText(display, status,
                        cv::Point(10, 28),
                        cv::FONT_HERSHEY_SIMPLEX, 0.7,
                        cv::Scalar(0, 255, 0), 2);
        } else {
            cv::putText(display, "Detection: OFF",
                        cv::Point(10, 28),
                        cv::FONT_HERSHEY_SIMPLEX, 0.7,
                        cv::Scalar(0, 100, 255), 2);
        }

        cv::imshow(windowName, display);

        int key = cv::waitKey(1) & 0xFF;
        if (key == 'q' || key == 'Q' || key == 27) {
            std::cout << "Quitting.\n";
            break;
        } else if (key == 'e' || key == 'E') {
            eyeDetection = !eyeDetection;
            std::cout << "Eye detection: " << (eyeDetection ? "ON" : "OFF") << "\n";
        } else if (key == 'i' || key == 'I') {
            printCameraInfo();
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
