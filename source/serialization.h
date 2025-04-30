#pragma once
std::vector<Frame_Detection> json_to_detections(nlohmann::json j);
nlohmann::json tracks_to_json(std::vector<Frame_Track> f);
std::vector<Object2D> detections_to_object2d(Frame_Detection frame);

