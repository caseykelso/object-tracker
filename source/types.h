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

struct Frame
{
    uint32_t frame_id;
    std::chrono::system_clock::time_point timestamp;
    union {
        std::vector<Detection> detections;
        std::vector<Track> tracks;
    };
};


