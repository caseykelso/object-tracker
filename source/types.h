#pragma once

struct Detection
{
    float x;
    float y;
    float width;
    float height;
};

struct Track
{
    uint32_t id;
    float x;
    float y;
    float width;
    float height;
};

struct Frame_Detection
{
    uint32_t frame_id;
    std::chrono::system_clock::time_point timestamp;
    std::vector<Detection> detections;
};

struct Frame_Track
{
    uint32_t frame_id;
    std::chrono::system_clock::time_point timestamp;
    std::vector<Track> tracks;
};
