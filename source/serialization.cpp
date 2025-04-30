#include <stdint.h>
#include <chrono>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <vector>
#include <opencv2/opencv.hpp>
#include "types.h"
#include "serialization.h"

std::vector<Frame_Detection> json_to_detections(nlohmann::json j)
{
    std::vector<Frame_Detection> result;
    Frame_Detection f;

    std::string s(j["timestamp"]);
    std::istringstream ss(s);

    if (std::chrono::from_stream(ss, "%Y-%m-%dT%H:%M:%SZ", f.timestamp))
    {
        std::chrono::zoned_time zoned(std::chrono::current_zone(), f.timestamp);
    }
    else
    {
        throw std::runtime_error("ERROR: failed to parse malformed json date from input file");
    }

    auto detections_array = j.at("detections");
    for (const auto &jd : detections_array)
    {
        Detection d;

        d.x = jd["x"];
        d.y = jd["y"];
        d.width = jd["width"];
        d.height = jd["height"];

        // ASSUMPTION: timestamps are ascending 
        f.detections.push_back(d);
    }

    result.push_back(f);

    return result;
}

std::vector<Object2D> detections_to_object2d(Frame_Detection frame)
{
    std::vector<Object2D> objects;
    for (const auto& detection : frame.detections)
    {
        Object2D o = {cv::Point2d(detection.x, detection.y), detection.width, detection.height, 0};
        objects.push_back(o);
    }
    return objects;
}


nlohmann::json tracks_to_json(std::vector<Frame_Track> f)
{
    nlohmann::json result;
    return result;
}
