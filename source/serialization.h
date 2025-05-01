#pragma once
std::vector<Frame_Detection> json_to_detections(nlohmann::json j);
nlohmann::json tracks_to_json(std::vector<Frame_Track> f);
std::vector<Object2D> detections_to_object2d(Frame_Detection frame);
void print_tracks(const std::vector<Track> &tracks);
Track object2d_to_track(const Object2D &o);
std::string timepoint_to_ISO8601(const std::chrono::system_clock::time_point& tp);
