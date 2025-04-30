#pragma once
std::vector<Frame_Detection> json_to_detections(nlohmann::json j);
nlohmann::json tracks_to_json(std::vector<Frame_Track> f);
