#include <chrono>
#include <stdint.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "munkres.h"
#include "types.h"
#include "tracker.h"


std::map<int, Object2D> objects;
uint32_t next_object_id;
const double max_distance = 0.6;
const double max_disappeared = 20;

void tracker_clear() //TODO: this is janky, create a class to with constructors/destructors and properly manage memory lifecycle
{
    objects.clear();
    next_object_id = 0;
}

void register_object(const cv::Point2d& centroid, double width, double height)
{
    Object2D object = {centroid, width, height, 0};
    objects[next_object_id] = object;
    ++next_object_id;
}

void deregister_object(uint32_t object_id)
{
    objects.erase(object_id);
}

double distance(const cv::Point2d& p1, const cv::Point2d& p2)
{
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

std::map<int, Object2D> update(const std::vector<Object2D>& detections)
{
    // If we have no detections, increment disappearance counters
    if (detections.empty()) 
    {
        std::cout << "**********EMPTY " << std::endl;
        for (auto it = objects.begin(); it != objects.end();) 
        {
            it->second.disappeared++;
            
            if (it->second.disappeared > max_disappeared) 
            {
                it = objects.erase(it);
            } else 
            {
                ++it;
            }
        }
        return objects;
    }

    // If we're not tracking any objects, register all detections
    if (objects.empty()) 
    {
        std::cout << "*********REGISTER ALL" << std::endl;
        for (const auto& detection : detections) 
        {
            register_object(detection.centroid, detection.width, detection.height);
        }
        return objects;
    }

    // Extract existing object centroids and IDs
    std::vector<int> object_ids;
    std::vector<cv::Point2d> object_centroids;
    
    for (const auto& pair : objects) 
    {
        object_ids.push_back(pair.first);
        object_centroids.push_back(pair.second.centroid);
    }

    // Extract input centroids from detections
    std::vector<cv::Point2d> input_centroids;
    std::vector<double> input_widths;
    std::vector<double> input_heights;
    
    for (const auto& detection : detections) 
    {
        input_centroids.push_back(detection.centroid);
        input_widths.push_back(detection.width);
        input_heights.push_back(detection.height);
    }

    // Compute the cost matrix for the Hungarian algorithm
    cv::Mat_<int> cost_matrix(object_centroids.size(), input_centroids.size(), CV_64F);
    
    for (size_t i = 0; i < object_centroids.size(); i++) 
    {
        for (size_t j = 0; j < input_centroids.size(); j++) 
        {
            // Cost is the Euclidean distance between centroids (in XY plane only)
            cost_matrix.at<double>(i, j) = distance(object_centroids[i], input_centroids[j]);
        }
    }

    // Solve the assignment problem using the Hungarian algorithm
    std::vector<int> assignment;
    assignment.resize(cost_matrix.rows);
    std::cout << "cost_matrix_rows: " << cost_matrix.rows << std::endl;
   
    Munkres m;  
    m.solve(cost_matrix); 

    // Process assignments
    std::vector<bool> used_rows(object_centroids.size(), false);
    std::vector<bool> used_columns(input_centroids.size(), false);

    
    for (size_t i = 0; i < assignment.size(); i++) 
    {
        std::cout << "----------------------" << std::endl;
        // If the cost is greater than the maximum distance, don't consider it a match
        std::cout << "assignment: " << assignment[i] << std::endl;
        if (cost_matrix.at<double>(i, assignment[i]) <= DBL_MAX) //max_distance) 
        {
            std::cout <<"******MATCH: " << cost_matrix.at<double>(i, assignment[i]) << "," << max_distance << std::endl;
            used_rows[i] = true;
            used_columns[assignment[i]] = true;
            
            // Update the existing object with new data
            int object_id = object_ids[i];
            objects[object_id].centroid    = input_centroids[assignment[i]];
            objects[object_id].width       = input_widths[assignment[i]];
            objects[object_id].height      = input_heights[assignment[i]];
            objects[object_id].disappeared = 0;

        }
        else
        {
            assignment[i] = -1;
            std::cout <<"******MISMATCH: " << cost_matrix.at<double>(i, assignment[i]) << "," << max_distance << std::endl;
            used_rows[i] = false;
        }
    }

    // Check for unused rows (objects that weren't matched)
    for (size_t i = 0; i < used_rows.size(); i++) 
    {
        if (!used_rows[i]) 
        {
            int object_id = object_ids[i];
            objects[object_id].disappeared++;
            
            if (objects[object_id].disappeared > max_disappeared) 
            {
                deregister_object(object_id);
            }
        }
    }

    // Check for unused columns (new detections that weren't matched)
    for (size_t j = 0; j < used_columns.size(); j++) 
    {
        if (!used_columns[j]) 
        {
            register_object(input_centroids[j], input_widths[j], input_heights[j]);
        }
    }

    return objects;
}

