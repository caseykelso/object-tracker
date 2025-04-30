#include <sstream>
#include <iomanip>
#include <chrono>
#include "gtest/gtest.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {

    TEST(TrackerTests, DESERIALIZE_JSON)
    {

        std::string j = R"({
                         "frame_id": 123,
                         "timestamp": "2025-03-24T18:33:22Z",
                         "detections": [
                         {
                             "x": 0.65,
                             "y": 0.42,
                             "width": 0.05,
                             "height": 0.10
                         }
                        ]
                       })";

        json j_object = json::parse(j);          
        EXPECT_EQ(123, j_object["frame_id"]);
        std::chrono::system_clock::time_point timestamp;

        EXPECT_EQ("2025-03-24T18:33:22Z", j_object["timestamp"]);
        std::string s(j_object["timestamp"]);
        std::istringstream ss(s);

        if (std::chrono::from_stream (ss, "%Y-%m-%dT%H:%M:%SZ", timestamp)) 
        {
  	    std::chrono::zoned_time zoned (std::chrono::current_zone (), timestamp);
	    //std::cout << std::format ("{:%d/%b/%Y %H:%M:%S}", zoned);
        }
        else
        {
            std::cerr << "failed to parse time" << std::endl;
        }

        auto time_of_day = std::chrono::hh_mm_ss{std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - std::chrono::floor<std::chrono::days>(timestamp))};

        EXPECT_EQ(18, time_of_day.hours().count());
        EXPECT_EQ(33, time_of_day.minutes().count());
        EXPECT_EQ(22, time_of_day.seconds().count());
        EXPECT_EQ(0.65, j_object["detections"][0]["x"]);
        EXPECT_EQ(0.42, j_object["detections"][0]["y"]);
        EXPECT_EQ(0.05, j_object["detections"][0]["width"]);
        EXPECT_EQ(0.10, j_object["detections"][0]["height"]);

    }
}

int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

