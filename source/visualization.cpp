#if 0
#include <vector>
#include <opencv2/opencv.hpp>
#endif
#include <stdint.h>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>
#include "types.h"
#include "visualization.h"


struct Square {
    cv::Point pos;
    int size;
    cv::Scalar color;
};

bool draw_detections(Frame_Track ft)
{
    bool result = true;
    const int width = 1000;
    const int height = 1000;
    cv::Mat image(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
    
    // Define some squares with different positions, sizes and colors
    std::vector<Square> squares = {
        {{100, 100}, 100, {255, 0, 0}},    // Blue square
        {{300, 200}, 150, {0, 255, 0}},    // Green square
        {{500, 300}, 80, {0, 0, 255}},     // Red square
        {{200, 400}, 120, {255, 255, 0}}   // Cyan square
    };
    
    // Draw the squares on the image
//    for (const auto& square : squares) {
        // Draw a filled square
        for (const auto& t : ft.tracks)
        {
            cv::rectangle(
                image, 
                cv::Point(t.x*1000, t.y*1000), 
                cv::Point(t.x*1000 + t.width*1000, t.y*1000 + t.height*1000), 
                {255, 0, 0}, 
                -1  // Filled rectangle
            );
#if 0 
            // Draw a black border around the square
            cv::rectangle(
                image, 
                square.pos, 
                cv::Point(square.pos.x + square.size, square.pos.y + square.size), 
                cv::Scalar(0, 0, 0),
                2  // Border thickness
            );
#endif
        }
    
    // Save the image as PNG
        std::cout << "frame id: " << std::to_string(ft.frame_id) << std::endl;
    cv::imwrite("tracking-frame-"+std::to_string(ft.frame_id)+".png", image);
    
    std::cout << "Image saved as 'squares_on_plane.png'" << std::endl;

    return result;
}
