
#include <iomanip>
#include <chrono>
#include "gtest/gtest.h"
#include <nlohmann/json.hpp>
#include "types.h"
#include "serialization.h"
#include <queue>
#include <float.h>
#include "tracker.h"

using json = nlohmann::json;

namespace {

TEST(TrackerTests, THREE_OBJECTS_MANY_FRAMES_ONE_DISAPPEAR)
{
    // building json docs is tedius, the parsing is proven, let's just build the objects directly

    IoUTracker tracker;
    std::vector<Frame> frames;
    std::vector<Object2D> detections0 = 
    {
        {0.1, 0.2, 0.3, 0.4, -1},
        {0.4, 0.5, 0.1, 0.2, -1},
        {0.6, 0.0, 0.8, 0.9, -1}
    };
    std::vector<Object2D> detections1 = 
    {
        {0.1, 0.2, 0.3, 0.4, -1},
        {0.6, 0.0, 0.8, 0.9, -1}
    };

    Frame f0(detections0, "2025-03-24T18:00:00Z");
    Frame f1(detections1, "2025-03-24T18:00:02Z");

    auto tracks_detected = tracker.update(f0.detections);
    EXPECT_EQ(3, tracks_detected.size());
    tracks_detected = tracker.update(f0.detections);

    auto tracks_all_active = tracker.getActiveTrackedObjects();
    EXPECT_EQ(3, tracks_all_active.size());

    uint8_t track_id = 0;
    for (auto& t: tracks_all_active)
    {
       EXPECT_EQ(track_id, t.id);
       ++track_id;
    }

    tracks_detected = tracker.update(f1.detections);
    tracks_all_active = tracker.getActiveTrackedObjects();
    EXPECT_EQ(3, tracks_all_active.size());

    // confirm that the track IDs still match, and we haven't generated a new track id
    track_id = 0;
    for (auto& t: tracks_all_active)
    {
       EXPECT_EQ(track_id, t.id);
       ++track_id;
    }

    // confirm that the object persists for 2 more frames
    for (uint8_t i = 0; i < 2; ++i)
    {
        tracks_detected = tracker.update(f1.detections);
        tracks_all_active = tracker.getActiveTrackedObjects();
        EXPECT_EQ(3, tracks_all_active.size());
    }

    // lets see that object again and confirm that it has the same ID
    tracks_detected = tracker.update(f0.detections);
    tracks_all_active = tracker.getActiveTrackedObjects();
    EXPECT_EQ(3, tracks_all_active.size());

    track_id = 0;
    for (auto& t: tracks_all_active)
    {
       EXPECT_EQ(track_id, t.id);
       ++track_id;
    }

    // ok, now let's remove the object for 3 more frames
    for (uint8_t i = 0; i < 3; ++i)
    {
        tracks_detected = tracker.update(f1.detections);
        tracks_all_active = tracker.getActiveTrackedObjects();
        EXPECT_EQ(3, tracks_all_active.size());
    }

    // track is marked as inactive   
    tracks_detected = tracker.update(f1.detections);
    tracks_all_active = tracker.getActiveTrackedObjects();
    EXPECT_EQ(2, tracks_all_active.size());

    // introduce new object and confirm that it receives a new ID
    tracks_detected = tracker.update(f0.detections);
    tracks_all_active = tracker.getActiveTrackedObjects();
    EXPECT_EQ(3, tracks_all_active.size());


    for (auto& f: frames)
    {
        auto tracks = tracker.update(f.detections);

        int last_object_id      = -1;
        uint8_t number_of_tracks = 0;
        Object2D o;

        for (auto& track : tracks)
        {
            last_object_id = track.id;
            o = track;
            ++number_of_tracks;
        }

    }
}




    TEST(TrackerTests, THREE_OBJECTS_TWO_FRAMES_NO_DISAPPEAR)
    {
        std::vector<Object2D> tracks;

        std::string j = R"(
    [
          {
            "frame_id": 123,
            "timestamp": "2025-03-24T18:00:00Z",
            "detections": [
              {
                "x": 0.65,
                "y": 0.42,
                "width": 0.05,
                "height": 0.05
              },
              {
                "x": 0.32,
                "y": 0.78,
                "width": 0.04,
                "height": 0.06
              }
            ]
          },
          {
            "frame_id": 124,
            "timestamp": "2025-03-24T18:00:01Z",
            "detections": [
              {
                "x": 0.66,
                "y": 0.43,
                "width": 0.05,
                "height": 0.05
              },
              {
                "x": 0.33,
                "y": 0.79,
                "width": 0.04,
                "height": 0.06
              }
            ]
          },
          {
            "frame_id": 125,
            "timestamp": "2025-03-24T18:00:02Z",
            "detections": [
              {
                "x": 0.67,
                "y": 0.44,
                "width": 0.05,
                "height": 0.05
              },
              {
                "x": 0.34,
                "y": 0.80,
                "width": 0.04,
                "height": 0.06
              },
              {
                "x": 0.51,
                "y": 0.22,
                "width": 0.03,
                "height": 0.03
              }
            ]
          }
        ]
        )";


        json j_object = json::parse(j);
        auto detections = json_to_detections(j_object);

        uint8_t i = 0;

        for (const auto& detection : detections)
        {
            IoUTracker tracker;
            auto objects    = detections_to_object2d(detection);
            tracks = tracker.update(objects);

            int last_object_id      = -1;
            uint8_t number_of_tracks = 0;
            Object2D o;

            for (auto& track : tracks)
            {
                last_object_id = track.id;
                o = track;
                ++number_of_tracks;
            }

            if (0 == i) // first frame
            {
                EXPECT_EQ(2, number_of_tracks); 
            }
            else if (1 == i) // second frame
            {
                EXPECT_EQ(2, number_of_tracks); 
            }
            else // third frame
            {
                EXPECT_EQ(3, number_of_tracks);
            }

            ++i;
        }
    }


    TEST(TrackerTests, THREE_OBJECTS_ONE_FRAME)
    {
        std::vector<Object2D> tracks;
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
                             "x": 0.65,
                             "y": 0.42,
                             "width": 0.05,
                             "height": 0.10
                         }
                        ]
                       })";

        json j_object = json::parse(j);

        auto detections = json_to_detections(j_object);

        uint8_t i = 0;

        for (const auto& detection : detections)
        {
            IoUTracker tracker;
            auto objects    = detections_to_object2d(detection);
            tracks          = tracker.update(objects);

            int last_object_id      = -1;
            uint8_t number_of_tracks = 0;
            Object2D o;

            for (auto& track : tracks)
            {
                last_object_id = track.id;
                o = track;
                ++number_of_tracks;
            }
             EXPECT_EQ(3, number_of_tracks); 
        }
    }


    TEST(TrackerTests, SINGLE_OBJECT)
    {
        std::vector<Object2D> tracks;

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

        auto detections = json_to_detections(j_object);

        for (const auto& detection : detections)
        {
            Object2D o;
            IoUTracker tracker;
            auto objects             = detections_to_object2d(detection);
            tracks                   = tracker.update(objects);
            int first_object_id      = -1;
            uint8_t number_of_tracks = 0;

            for (auto& track : tracks)
            {
                first_object_id = track.id;
                o = track;
                ++number_of_tracks;
            }

           EXPECT_EQ(0, first_object_id);
           EXPECT_EQ(1, number_of_tracks); 
           EXPECT_FLOAT_EQ(0.05, o.width);
           EXPECT_FLOAT_EQ(0.1, o.height);
           EXPECT_FLOAT_EQ(0.65, o.x);
           EXPECT_FLOAT_EQ(0.42, o.y);
        }
    }

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

    TEST(TrackerTests, EMPTY_DETECTIONS)
    {
        IoUTracker tracker(0.3);

        std::vector<Object2D> objects;
        std::vector<Object2D> tracks = tracker.update(objects);

        int last_object_id      = -1;
        uint8_t number_of_tracks = 0;
        Object2D o;

        for (auto& track : tracks)
        {
            last_object_id = track.id;
            ++number_of_tracks;
        }
        EXPECT_EQ(0, number_of_tracks); 
    }

    TEST(TrackerTests, OBJECT2D_TO_TRACK)
    {
        Object2D o;

        o.width      = 0.1;
        o.height     = 0.2;
        o.x          = 0.05;
        o.y          = 0.07;

        Track t = object2d_to_track(o);

        EXPECT_EQ(-1, t.id);
        EXPECT_NEAR(0.1, t.width, FLT_EPSILON);
        EXPECT_NEAR(0.2, t.height, FLT_EPSILON);
        EXPECT_NEAR(0.05, o.x, FLT_EPSILON);
        EXPECT_NEAR(0.07, o.y, FLT_EPSILON);
    }
};

int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

