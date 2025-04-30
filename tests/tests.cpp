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
                         "timestamp": "2025-03-24T18:00:00Z",
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

        //TODO: there must be a way to directly construct std::chrono::system_clock::time_point directly from a YYY-MM-DDTHH:MM:SSZ" string
        EXPECT_EQ("2025-03-24T18:00:00Z", j_object["timestamp"]);
        std::string s(j_object["timestamp"]);
        std::istringstream ss(s);

        std::chrono::from_stream(ss, timestamp);

        auto date = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(timestamp)};
        std::cout << date.day() << std::endl;
        //EXPECT_EQ(static_cast<unsigned int>(date.year()), 2025);
        EXPECT_EQ(static_cast<unsigned int>(date.day()), 25);

//        auto hours = std::chrono::duration_cast<std::chrono::hours>(timestamp);

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

