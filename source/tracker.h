#pragma once

struct Object2D
{
    cv::Point2d centroid;
    double width;
    double height;
    uint32_t disappeared;
};


std::map<int, Object2D> update(const std::vector<std::tuple<cv::Point2d, double, double>>& detections);
