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

bool draw_detections_and_tracks(Frame_Track ft, std::vector<Object2D> detections)
{
    bool result = true;
    const int width = 1000;
    const int height = 1000;
    cv::Mat image(height, width, CV_8UC3, cv::Scalar(255, 255, 255));


    // draw the detections in this frame
    for (const auto& d : detections)
    {
            cv::rectangle(
                image, 
                cv::Point(d.x*1000, d.y*1000), 
                cv::Point(d.x*1000 + d.width*1000, d.y*1000 + d.height*1000), 
                {255, 0, 0}, 
                -1  // Filled rectangle
            );
    }

        for (const auto& t : ft.tracks)
        {
            // draw a red border if an object is being tracked / even if it isn't detected
            cv::rectangle(
                image, 
                cv::Point(t.x*1000, t.y*1000), 
                cv::Point(t.x*1000 + t.width*1000, t.y*1000 + t.height*1000),
                cv::Scalar(0, 0, 255),
                4  // Border thickness
            );

            cv::putText(
                    image,
                    std::to_string(t.id),
                    cv::Point(t.x*1000-10, t.y*1000-5),
                    cv::FONT_HERSHEY_SIMPLEX,
                    1,
                    cv::Scalar(255, 0, 0)
                    );
        }
    
    // Save the image as PNG
    cv::imwrite("tracking-frame-"+std::to_string(ft.frame_id)+".png", image);
    
    std::cout << "Image saved as 'tracking-frame-"+std::to_string(ft.frame_id)+".png'" << std::endl;

    return result;
}
