#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Open the default camera
    cv::VideoCapture cap(0);
    
    // Check if the camera opened successfully
    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to open camera" << std::endl;
        return -1;
    }

    // Create a window to display the camera feed
    cv::namedWindow("Camera Feed", cv::WINDOW_NORMAL);
    
    // Capture a frame from the camera
    cv::Mat frame;
    cap >> frame;
    
    // Check if the frame is empty
    if (frame.empty()) {
        std::cerr << "Error: Unable to capture frame" << std::endl;
        return -1;
    }

    // Save the captured frame as an image file in the current directory
    std::string filename = "captured_image.jpg";
    cv::imwrite(filename, frame);
    std::cout << "Image saved as: " << filename << std::endl;

    // Display the captured frame
    cv::imshow("Camera Feed", frame);

    // Wait for a key press
    cv::waitKey(0);

    // Release the camera and close all OpenCV windows
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
