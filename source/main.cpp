#include "docopt/docopt.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <fstream>
#include <string>
#include "types.h"
#include "serialization.h"
#include "tracker.h"
#include "visualization.h"

using json = nlohmann::json;

static const char USAGE[] =
    R"(RedBarnRobotics

    Usage:
      tracker_rbr --input=<filename> --output=<filename>
      tracker_rbr (-h | --help)
      tracker_rbr --version

    Options:
      -h --help             Show this help message
      --version             Show version
      --frames=<filename>   Frame data input path
      --tracks=<filename>   Tracks output path
)";

int main(int argc, char **argv)
{
    std::map<std::string, docopt::value> args;

    args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "Red Barn Robotics Cabbage Tracker 0.1");

    std::string frames_path = args["--input"].asString();
    std::string tracks_path = args["--output"].asString();

    std::cout << "input file: " << frames_path << std::endl;
    std::cout << "output file: " << tracks_path << std::endl;

    std::ifstream json_file(frames_path);

    if(json_file)
    {
        auto j = json::parse(json_file); 
        auto frames = json_to_detections(j);
        std::cout << "frames read: " << frames.size() << std::endl;

        IoUTracker tracker;
        std::vector<Frame_Track> track_frames;

        for (auto& frame : frames)
        {
           std::string timestamp = timepoint_to_ISO8601(frame.timestamp);
           uint32_t frame_id = frame.frame_id;
           auto objects = detections_to_object2d(frame); // extract the detections from the frame
           std::cout << "object detections in this frame: " << objects.size() << std::endl;
           tracker.update(objects);
           auto tracks = tracker.getActiveTrackedObjects();
           Frame_Track ft(frame_id, timestamp, tracks);
           track_frames.push_back(ft);
           draw_detections_and_tracks(ft, objects);
        }

        //TODO: sort out floating point precision lost on round-trip of json
        std::ofstream tracks_file(tracks_path);
        json track_json;
        json json_track_frames = track_frames;
        tracks_file << json_track_frames.dump(4) << std::endl; // format with 4 space indentation
        tracks_file.close();

    }
    else
    {
        std::cerr << "ERROR: could not read json input file: " << frames_path << std::endl;
    }
    return 0;
}
