#pragma once
std::vector<Frame> json_to_detections(nlohmann::json j);
nlohmann::json tracks_to_json(std::vector<Frame> f);

