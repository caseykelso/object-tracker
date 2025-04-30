#include <algorithm>
#include <caca.h>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#define STANDALONE_TEST 1
// Structure to represent a 3D object with dimensions
struct Object3D
{
    double x, y, z; // Position in 3D space (z=0 for plane)
    double width;   // Width of object
    double height;  // Height of object
    int id;         // Tracker ID

    Object3D(double x, double y, double z, double width, double height, int id)
        : x(x), y(y), z(z), width(width), height(height), id(id)
    {
    }
};

class Visualizer3D
{
  private:
    caca_canvas_t *cv;
    caca_display_t *dp;

    int canvasWidth;
    int canvasHeight;

    double worldMinX, worldMaxX;
    double worldMinY, worldMaxY;
    double worldMinZ, worldMaxZ;

    std::vector<Object3D> objects;
    std::mutex objectsMutex;

    bool running;
    std::thread renderThread;

    // Convert world coordinates to canvas coordinates
    int worldToCanvasX(double x)
    {
        return static_cast<int>((x - worldMinX) / (worldMaxX - worldMinX) * canvasWidth);
    }

    int worldToCanvasY(double y)
    {
        return static_cast<int>((y - worldMinY) / (worldMaxY - worldMinY) * canvasHeight);
    }

    // Draw a filled rectangle with a character and color
    void drawFilledRect(int x, int y, int w, int h, char ch, uint32_t fg, uint32_t bg)
    {
        for (int i = x; i < x + w && i < canvasWidth; i++)
        {
            for (int j = y; j < y + h && j < canvasHeight; j++)
            {
                if (i >= 0 && j >= 0)
                {
                    caca_put_char(cv, i, j, ch);
                    caca_set_color_argb(cv, fg, bg);
                }
            }
        }
    }

    // Draw a text with a specified color
    void drawText(int x, int y, const std::string &text, uint32_t fg, uint32_t bg)
    {
        if (x < 0 || y < 0 || x >= canvasWidth || y >= canvasHeight)
        {
            return;
        }

        caca_set_color_argb(cv, fg, bg);
        caca_put_str(cv, x, y, text.c_str());
    }

    // Draw coordinate axes
    void drawAxes()
    {
        // X-axis
        for (int i = 0; i < canvasWidth; i++)
        {
            caca_put_char(cv, i, canvasHeight / 2, '-');
            caca_set_color_argb(cv, 0xFFFF0000, 0xFF000000); // Red
        }

        // Y-axis
        for (int j = 0; j < canvasHeight; j++)
        {
            caca_put_char(cv, canvasWidth / 2, j, '|');
            caca_set_color_argb(cv, 0xFF00FF00, 0xFF000000); // Green
        }

        // Origin
        caca_put_char(cv, canvasWidth / 2, canvasHeight / 2, '+');
        caca_set_color_argb(cv, 0xFFFFFFFF, 0xFF000000); // White

        // Labels
        drawText(canvasWidth - 5, canvasHeight / 2 - 1, "X", 0xFFFF0000, 0xFF000000);
        drawText(canvasWidth / 2 + 1, 0, "Y", 0xFF00FF00, 0xFF000000);
        drawText(canvasWidth / 2 - 2, canvasHeight / 2 - 1, "O", 0xFFFFFFFF, 0xFF000000);
    }

    // Generate different colors based on ID
    uint32_t getColorForId(int id)
    {
        // Simple hash function to generate colors
        const uint32_t colors[] = {
            0xFFFF0000, // Red
            0xFF00FF00, // Green
            0xFF0000FF, // Blue
            0xFFFFFF00, // Yellow
            0xFF00FFFF, // Cyan
            0xFFFF00FF, // Magenta
            0xFFFF8000, // Orange
            0xFF8000FF, // Purple
            0xFF0080FF, // Light blue
            0xFF80FF00  // Lime
        };

        return colors[id % 10];
    }

    // Render loop
    void renderLoop()
    {
        while (running)
        {
            // Clear canvas
            caca_clear_canvas(cv);

            // Draw axes
            drawAxes();

            // Draw objects
            {
                std::lock_guard<std::mutex> lock(objectsMutex);
                for (const auto &obj : objects)
                {
                    // Convert world coordinates to canvas coordinates
                    int cx = worldToCanvasX(obj.x);
                    int cy = worldToCanvasY(obj.y);

                    // Calculate rectangle size (scaled from world size)
                    int width = std::max(1, static_cast<int>((obj.width / (worldMaxX - worldMinX)) * canvasWidth));
                    int height = std::max(1, static_cast<int>((obj.height / (worldMaxY - worldMinY)) * canvasHeight));

                    // Get color based on ID
                    uint32_t color = getColorForId(obj.id);

                    // Draw filled rectangle for the object
                    drawFilledRect(cx - width / 2, cy - height / 2, width, height, '#', color, 0xFF000000);

                    // Create ID text
                    std::stringstream ss;
                    ss << obj.id;
                    std::string idText = ss.str();

                    // Draw ID text
                    drawText(cx - idText.length() / 2, cy, idText, 0xFFFFFFFF, color);

                    // Draw position and size info
                    std::stringstream infoSS;
                    infoSS << std::fixed << std::setprecision(1) << "(" << obj.x << "," << obj.y << ") " << obj.width
                           << "x" << obj.height;
                    std::string infoText = infoSS.str();

                    drawText(cx - infoText.length() / 2, cy + 1, infoText, 0xFFFFFFFF, 0xFF000000);
                }
            }

            // Display information
            drawText(0, 0, "3D Object Tracker Visualization", 0xFFFFFFFF, 0xFF000000);
            drawText(0, 1, "Objects are displayed on XY plane (Z=0)", 0xFFFFFFFF, 0xFF000000);

            std::stringstream ss;
            ss << "Active objects: " << objects.size();
            drawText(0, 2, ss.str(), 0xFFFFFFFF, 0xFF000000);

            drawText(0, canvasHeight - 1, "Press 'q' to quit", 0xFFFFFFFF, 0xFF000000);

            // Refresh display
            caca_refresh_display(dp);
#if 0 
            // Check for keyboard input (non-blocking)
            const caca_event_t &event = caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, 0);
            if (event && caca_get_event_key_ch(event) == 'q') {
                running = false;
            }
#endif
            // Sleep to limit refresh rate
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

  public:
    Visualizer3D(int width = 80, int height = 40, double minX = -10.0, double maxX = 10.0, double minY = -10.0,
                 double maxY = 10.0, double minZ = -10.0, double maxZ = 10.0)
        : canvasWidth(width), canvasHeight(height), worldMinX(minX), worldMaxX(maxX), worldMinY(minY), worldMaxY(maxY),
          worldMinZ(minZ), worldMaxZ(maxZ), running(false)
    {

        // Initialize libcaca
        cv = caca_create_canvas(width, height);
        if (!cv)
        {
            throw std::runtime_error("Failed to create canvas");
        }

        dp = caca_create_display(cv);
        if (!dp)
        {
            caca_free_canvas(cv);
            throw std::runtime_error("Failed to create display");
        }

        caca_set_display_title(dp, "3D Object Tracker");
        caca_set_color_ansi(cv, CACA_WHITE, CACA_BLACK);
    }

    ~Visualizer3D()
    {
        stop();
        caca_free_display(dp);
        caca_free_canvas(cv);
    }

    // Start the visualization thread
    void start()
    {
        if (!running)
        {
            running = true;
            renderThread = std::thread(&Visualizer3D::renderLoop, this);
        }
    }

    // Stop the visualization thread
    void stop()
    {
        if (running)
        {
            running = false;
            if (renderThread.joinable())
            {
                renderThread.join();
            }
        }
    }

    // Update objects to be visualized
    void updateObjects(const std::vector<Object3D> &newObjects)
    {
        std::lock_guard<std::mutex> lock(objectsMutex);
        objects = newObjects;
    }

    // Add a single object
    void addObject(const Object3D &obj)
    {
        std::lock_guard<std::mutex> lock(objectsMutex);

        // Check if object with this ID already exists
        auto it = std::find_if(objects.begin(), objects.end(), [&obj](const Object3D &o) { return o.id == obj.id; });

        if (it != objects.end())
        {
            // Update existing object
            *it = obj;
        }
        else
        {
            // Add new object
            objects.push_back(obj);
        }
    }

    // Remove an object by ID
    void removeObject(int id)
    {
        std::lock_guard<std::mutex> lock(objectsMutex);
        objects.erase(std::remove_if(objects.begin(), objects.end(), [id](const Object3D &o) { return o.id == id; }),
                      objects.end());
    }

    // Clear all objects
    void clearObjects()
    {
        std::lock_guard<std::mutex> lock(objectsMutex);
        objects.clear();
    }

    // Check if visualization is running
    bool isRunning() const
    {
        return running;
    }
};

// Example usage and integration with the tracker
#ifdef STANDALONE_TEST

int main()
{
    try
    {
        // Create visualizer with custom world bounds
        Visualizer3D viz(100, 40, -5.0, 5.0, -3.0, 3.0);

        // Start visualization thread
        viz.start();

        // Add some example objects
        viz.addObject(Object3D(-2.0, 1.0, 0.0, 1.0, 0.5, 1));
        viz.addObject(Object3D(0.0, 0.0, 0.0, 0.8, 0.8, 2));
        viz.addObject(Object3D(3.0, -1.5, 0.0, 1.2, 1.0, 3));

        // Main loop
        int frame = 0;
        while (viz.isRunning() && frame < 1000)
        {
            // Simulate object movement
            viz.updateObjects({Object3D(-2.0 + sin(frame * 0.02), 1.0 + cos(frame * 0.01), 0.0, 1.0, 0.5, 1),
                               Object3D(sin(frame * 0.03), cos(frame * 0.03), 0.0, 0.8, 0.8, 2),
                               Object3D(3.0 - sin(frame * 0.01), -1.5 + sin(frame * 0.02), 0.0, 1.2, 1.0, 3)});

            // Add/remove objects occasionally
            if (frame % 100 == 0)
            {
                viz.addObject(Object3D(-4.0 + static_cast<double>(rand()) / RAND_MAX * 8.0,
                                       -2.0 + static_cast<double>(rand()) / RAND_MAX * 4.0, 0.0,
                                       0.5 + static_cast<double>(rand()) / RAND_MAX,
                                       0.5 + static_cast<double>(rand()) / RAND_MAX, frame / 100 + 4));
            }

            // Wait a bit
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
            frame++;
        }

        // Cleanup is handled by destructor
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

#endif // STANDALONE_TEST

// Integration function to use with the MunkresTracker3D
#ifndef STANDALONE_TEST

// Forward declaration of Object3D structure from tracker
struct TrackerObject3D
{
    cv::Point3d centroid;
    double width;
    double height;
    int disappeared;
};

// Create a visualization window for the tracker
Visualizer3D *createVisualization(double minX = -5.0, double maxX = 5.0, double minY = -5.0, double maxY = 5.0)
{
    Visualizer3D *viz = new Visualizer3D(100, 40, minX, maxX, minY, maxY);
    viz->start();
    return viz;
}

// Update visualization with tracker data
void updateVisualization(Visualizer3D *viz, const std::map<int, TrackerObject3D> &trackedObjects)
{
    std::vector<Object3D> vizObjects;

    for (const auto &pair : trackedObjects)
    {
        int id = pair.first;
        const TrackerObject3D &obj = pair.second;

        vizObjects.emplace_back(obj.centroid.x, obj.centroid.y, obj.centroid.z, obj.width, obj.height, id);
    }

    viz->updateObjects(vizObjects);
}

// Cleanup visualization
void destroyVisualization(Visualizer3D *viz)
{
    if (viz)
    {
        viz->stop();
        delete viz;
    }
}

#endif // STANDALONE_TEST
