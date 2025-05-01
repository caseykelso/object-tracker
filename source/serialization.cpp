#include <stdint.h>
#include <chrono>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <vector>
#include <opencv2/opencv.hpp>
#include "types.h"
#include "serialization.h"
void print_track(const Track &t)
{
    std::cout << "-----------------------" << std::endl;
    std::cout << "id: " << t.id << std::endl;
    std::cout << "x : " << t.x  << std::endl;
    std::cout << "y : " << t.y << std::endl;
    std::cout << "width: " << t.width << std::endl;
    std::cout << "height: " << t.height << std::endl;
}

void print_tracks(const std::vector<Track> &tracks)
{
    for (const auto &t : tracks)
    {
        print_track(t);
    }
}

Track object2d_to_track(const Object2D &o)
{
   Track result;
   result.id     = o.id;
   result.x      = o.x;
   result.y      = o.y;
   result.width  = o.width;
   result.height = o.height;

   return result;
}

Frame_Detection json_to_detection(nlohmann::json j) // parse a single frame
{
    Frame_Detection f;

    f.frame_id = j["frame_id"];
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

    return f;
}

std::vector<Frame_Detection> json_to_detections(nlohmann::json j)
{
    std::vector<Frame_Detection> result;

    if (j.is_array())
    {

       for (const auto &jf : j)
       {
           Frame_Detection f;
           f = json_to_detection(jf);
           result.push_back(f);
       }
    }
    else
    {
       Frame_Detection f = json_to_detection(j); 
       result.push_back(f);
    }

    return result;
}

std::vector<Object2D> detections_to_object2d(Frame_Detection frame)
{
    std::vector<Object2D> objects;
    for (const auto& detection : frame.detections)
    {
        Object2D o = {detection.x, detection.y, detection.width, detection.height, 0};
        objects.push_back(o);
    }
    return objects;
}


nlohmann::json tracks_to_json(std::vector<Frame_Track> f)
{
    nlohmann::json result;
    return result;
}

std::string timepoint_to_ISO8601(const std::chrono::system_clock::time_point& tp) 
{
    auto time_t_point = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(gmtime(&time_t_point), "%FT%TZ");
    
    return ss.str();
}
