#include <vector>
#include <chrono>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "types.h"
#include "serialization.h"

std::vector<Frame_Detection> json_to_detections(nlohmann::json j)
{
    std::vector<Frame_Detection> result;
    Frame_Detection f;

    std::string s(j["timestamp"]);
    std::istringstream ss(s);

    if (std::chrono::from_stream (ss, "%Y-%m-%dT%H:%M:%SZ", f.timestamp)) 
    {
        std::chrono::zoned_time zoned (std::chrono::current_zone (), f.timestamp);
    }
    else
    {
        throw std::runtime_error("ERROR: failed to parse malformed json date from input file");
    }


    auto detections_array = j.at("detections");
    for (const auto& jd : detections_array)
    {
       Detection d;

       d.x      = jd["x"];
       d.y      = jd["y"];
       d.width  = jd["width"];
       d.height = jd["height"];

       //ASSUMPTION: frame IDs are sorted, in order, and frame-ids increment as the timestamp increments, all in ascending order
       //TODO add error checking for out of order frames
       f.detections.push_back(d);
    }

    result.push_back(f);

    return result;
}

nlohmann::json tracks_to_json(std::vector<Frame_Track> f)
{
    nlohmann::json result;
    return result;
}

