#include <sstream>
#include <iomanip>
#include <chrono>
#include "gtest/gtest.h"
#include <nlohmann/json.hpp>
#include "types.h"
#include "serialization.h"

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

      
        // parse the json doc and do a quick sanity check 
        json j_object = json::parse(j);
        EXPECT_EQ(123, j_object["frame_id"]);
        EXPECT_EQ("2025-03-24T18:33:22Z", j_object["timestamp"]);

        // use the serialization function to extract the data
        std::vector<Frame_Detection> frames = json_to_detections(j_object); 

        uint32_t detection_iterator = 0;

        // iterate over all of the frames
        for (const auto& f : frames)
        {
            auto time_of_day = std::chrono::hh_mm_ss{std::chrono::duration_cast<std::chrono::milliseconds>(f.timestamp - std::chrono::floor<std::chrono::days>(f.timestamp))};
            EXPECT_EQ(123, j_object["frame_id"]);
            EXPECT_EQ(18, time_of_day.hours().count());
            EXPECT_EQ(33, time_of_day.minutes().count());
            EXPECT_EQ(22, time_of_day.seconds().count());
            EXPECT_EQ(0.65, j_object["detections"][detection_iterator]["x"]);
            EXPECT_EQ(0.42, j_object["detections"][detection_iterator]["y"]);
            EXPECT_EQ(0.05, j_object["detections"][detection_iterator]["width"]);
            EXPECT_EQ(0.10, j_object["detections"][detection_iterator]["height"]);
        }
    }
}

    TEST(TrackerTests, DESERIALIZE_JSON_MULTIPLE_DETECTIONS)
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
                         },
                         {
                             "x": 0.11,
                             "y": 0.22,
                             "width": 0.33,
                             "height": 0.44
                         },
                         {
                             "x": 0.99,
                             "y": 0.88,
                             "width": 0.77,
                             "height": 0.66
                         }
                        ]
                       })";

      
        // parse the json doc and do a quick sanity check 
        json j_object = json::parse(j);
        EXPECT_EQ(123, j_object["frame_id"]);
        EXPECT_EQ("2025-03-24T18:33:22Z", j_object["timestamp"]);

        // use the serialization function to extract the data
        std::vector<Frame_Detection> frames = json_to_detections(j_object); 


        // iterate over all of the frames
        for (const auto& f : frames)
        {
            auto time_of_day = std::chrono::hh_mm_ss{std::chrono::duration_cast<std::chrono::milliseconds>(f.timestamp - std::chrono::floor<std::chrono::days>(f.timestamp))};
            EXPECT_EQ(123, j_object["frame_id"]);
            EXPECT_EQ(18, time_of_day.hours().count());
            EXPECT_EQ(33, time_of_day.minutes().count());
            EXPECT_EQ(22, time_of_day.seconds().count());

            uint32_t detection_iterator = 0;
            for (const auto& d : f.detections)
            {
                if (0 == detection_iterator)
                {
                    EXPECT_EQ(0.65, j_object["detections"][detection_iterator]["x"]);
                    EXPECT_EQ(0.42, j_object["detections"][detection_iterator]["y"]);
                    EXPECT_EQ(0.05, j_object["detections"][detection_iterator]["width"]);
                    EXPECT_EQ(0.10, j_object["detections"][detection_iterator]["height"]);
                }
                else if (1 == detection_iterator)
                {
                    EXPECT_EQ(0.11, j_object["detections"][detection_iterator]["x"]);
                    EXPECT_EQ(0.22, j_object["detections"][detection_iterator]["y"]);
                    EXPECT_EQ(0.33, j_object["detections"][detection_iterator]["width"]);
                    EXPECT_EQ(0.44, j_object["detections"][detection_iterator]["height"]);
                }
                else if (2 == detection_iterator)
                {
                    EXPECT_EQ(0.99, j_object["detections"][detection_iterator]["x"]);
                    EXPECT_EQ(0.88, j_object["detections"][detection_iterator]["y"]);
                    EXPECT_EQ(0.77, j_object["detections"][detection_iterator]["width"]);
                    EXPECT_EQ(0.66, j_object["detections"][detection_iterator]["height"]);
                }
                else
                {
                    EXPECT_EQ(true,false); // we should never this case
                }
                ++detection_iterator;
            }
        }
    }

int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

