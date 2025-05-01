#pragma once

// Structure to represent a 2D object with dimensions
struct Object2D {
    float x;         // Center x-coordinate
    float y;         // Center y-coordinate
    float width;     // Object width
    float height;    // Object height
    int id;          // Unique identifier for tracking
    uint8_t frames_missing; // the number of frames where the object is missing after first detection

    Object2D(float x_val = 0, float y_val = 0, float w = 0, float h = 0, int id_val = -1) 
        : x(x_val), y(y_val), width(w), height(h), id(id_val), frames_missing(0) {}
};

// Edge in the flow network
struct Edge {
    int to;        // Destination vertex
    int capacity;  // Edge capacity
    int flow;      // Current flow
    int rev;       // Index of the reverse edge in the adjacency list of 'to'

    Edge(int t, int c, int f, int r) : to(t), capacity(c), flow(f), rev(r) {}
};

struct Frame
{
    std::vector<Object2D> detections;
    Frame(std::vector<Object2D> d) : detections(d) {}
};


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
