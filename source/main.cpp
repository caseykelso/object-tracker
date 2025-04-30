#include "docopt/docopt.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>
#include "types.h"
#include "serialization.h"
#include <tracker.h>

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

    return 0;
}
