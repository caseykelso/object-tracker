#pragma once
//#define DEBUG 1

// Maximum bipartite matching solver using Edmonds-Karp algorithm
class MaximumBipartiteMatching {
private:
    std::vector<std::vector<Edge>> graph;  // Adjacency list representation
    int source;                            // Source vertex
    int sink;                              // Sink vertex
    int vertices;                          // Number of vertices in the graph

    // BFS to find augmenting path
    bool bfs(std::vector<int>& parent) {
        std::fill(parent.begin(), parent.end(), -1);
        parent[source] = -2;  // Mark source as visited
        
        std::queue<int> queue;
        queue.push(source);
        
        while (!queue.empty()) {
            int current = queue.front();
            queue.pop();
            
            for (int i = 0; i < graph[current].size(); i++) {
                Edge& edge = graph[current][i];
                int next = edge.to;
                
                // If not visited and has capacity
                if (parent[next] == -1 && edge.capacity - edge.flow > 0) {
                    parent[next] = current;
                    parent[vertices + next] = i;  // Store the edge index
                    
                    if (next == sink) {
                        return true;  // Path to sink found
                    }
                    
                    queue.push(next);
                }
            }
        }
        
        return false;  // No augmenting path found
    }

public:
    MaximumBipartiteMatching(int num_vertices, int s, int t) : vertices(num_vertices), source(s), sink(t) {
        graph.resize(vertices);
    }
    
    // Add an edge to the graph (and its reverse)
    void addEdge(int from, int to, int capacity) {
        // Forward edge
        graph[from].push_back(Edge(to, capacity, 0, graph[to].size()));
        // Reverse edge with 0 capacity (for residual network)
        graph[to].push_back(Edge(from, 0, 0, graph[from].size() - 1));
    }
    
    // Find maximum matching
    int maxFlow() {
        int total_flow = 0;
        std::vector<int> parent(2 * vertices, -1);  // Parent array for BFS
        
        while (bfs(parent)) {
            // Find minimum residual capacity along the augmenting path
            int path_flow = std::numeric_limits<int>::max();
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                int edge_idx = parent[vertices + v];
                path_flow = std::min(path_flow, graph[u][edge_idx].capacity - graph[u][edge_idx].flow);
            }
            
            // Update residual capacities and flows
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                int edge_idx = parent[vertices + v];
                
                graph[u][edge_idx].flow += path_flow;  // Forward edge
                graph[v][graph[u][edge_idx].rev].flow -= path_flow;  // Reverse edge
            }
            
            total_flow += path_flow;
        }
        
        return total_flow;
    }
    
    // Retrieve matching pairs
    std::vector<std::pair<int, int>> getMatching(int num_left, int num_right) {
        std::vector<std::pair<int, int>> matches;
        
        // Check edges from left partition (excluding source and sink)
        for (int left = 1; left <= num_left; left++) {
            for (const Edge& edge : graph[left]) {
                // Only consider edges to right partition with flow
                if (edge.flow > 0 && edge.to > num_left && edge.to <= num_left + num_right) {
                    matches.push_back(std::make_pair(left - 1, edge.to - num_left - 1));  // Adjust indices
                }
            }
        }
        
        return matches;
    }
};

// IoU-based 2D object tracker with object persistence
class IoUTracker {
private:
    // Structure to track object state across frames
    struct TrackedObject {
        Object2D object;              // The object data
        int frames_since_last_match;  // Counter for frames without a match
        bool is_active;               // Whether the object is currently active
        
        TrackedObject(const Object2D& obj) 
            : object(obj), frames_since_last_match(0), is_active(true) {}
    };
    
    std::vector<TrackedObject> tracked_objects;  // All tracked objects (active and inactive)
    int next_id;
    double min_iou_threshold;    // Minimum IoU to consider a match
    int max_frames_to_keep;      // Maximum frames to keep without a match
    
    // Calculate IoU (Intersection over Union) between two objects
    double calculateIoU(const Object2D& obj1, const Object2D& obj2) {
        // Calculate the boundaries of each box
        float left1 = obj1.x - obj1.width / 2.0f;
        float right1 = obj1.x + obj1.width / 2.0f;
        float top1 = obj1.y - obj1.height / 2.0f;
        float bottom1 = obj1.y + obj1.height / 2.0f;
        
        float left2 = obj2.x - obj2.width / 2.0f;
        float right2 = obj2.x + obj2.width / 2.0f;
        float top2 = obj2.y - obj2.height / 2.0f;
        float bottom2 = obj2.y + obj2.height / 2.0f;
        
        // Calculate intersection area
        float left_intersection = std::max(left1, left2);
        float right_intersection = std::min(right1, right2);
        float top_intersection = std::max(top1, top2);
        float bottom_intersection = std::min(bottom1, bottom2);
        
        float intersection_width = std::max(0.0f, right_intersection - left_intersection);
        float intersection_height = std::max(0.0f, bottom_intersection - top_intersection);
        float intersection_area = intersection_width * intersection_height;
        
        // Calculate union area
        float area1 = obj1.width * obj1.height;
        float area2 = obj2.width * obj2.height;
        float union_area = area1 + area2 - intersection_area;
        
        // Return IoU
        if (union_area > 0) {
            return intersection_area / union_area;
        }
        return 0.0;  // No overlap
    }
    
    // Convert IoU to an integer capacity (higher IoU = higher capacity)
    int iouToCapacity(double iou) {
        if (iou < min_iou_threshold) {
            return 0;  // No edge if IoU too small
        }
        
        // Scale to integer (higher IoU = higher capacity)
        int capacity = static_cast<int>(iou * 10000);
        return std::max(capacity, 1);  // Ensure at least capacity 1 if above threshold
    }

public:
    // Constructor with IoU threshold and frames to keep parameters
    IoUTracker(double iou_threshold = 0.3, int frames_to_keep = 10) 
        : next_id(0), min_iou_threshold(iou_threshold), max_frames_to_keep(frames_to_keep) {}

    // Process new detections and match with existing tracks using IoU
    std::vector<Object2D> update(const std::vector<Object2D>& detections) 
    {
        std::vector<Object2D> current_detections = detections;
#ifdef DEBUG
        std::cout << "UPDATE" << std::endl;
#endif //DEBUG
        
        // If this is the first frame, initialize all objects with new IDs
        if (tracked_objects.empty()) 
        {
#ifdef DEBUG
            std::cout << "EMPTY" << std::endl;
#endif //DEBUG
            for (auto& detection : current_detections) {
                std::cout << "!" << std::endl;
                detection.id = next_id++;
                tracked_objects.push_back(TrackedObject(detection));
            }
            return current_detections;
        }
        
        // Get only active objects for matching
        std::vector<Object2D> active_objects;
        std::vector<int> active_indices;
        
        for (size_t i = 0; i < tracked_objects.size(); i++) {
            if (tracked_objects[i].is_active) {
                active_objects.push_back(tracked_objects[i].object);
                active_indices.push_back(i);
#ifdef DEBUG
                std::cout << "ACTIVE" << std::endl;
#endif //DEBUG
            }
        }
        
        int n_prev = active_objects.size();
        int n_curr = current_detections.size();
        
        // If no active objects or no current detections, handle accordingly
        if (n_prev == 0) {
            // All new detections get new IDs
            for (auto& detection : current_detections) {
                detection.id = next_id++;
                tracked_objects.push_back(TrackedObject(detection));
            }
            return current_detections;
        }
        
        if (n_curr == 0) {
            // Increment counters for all active objects and mark as inactive if needed
            for (auto& tracked_obj : tracked_objects) {
                if (tracked_obj.is_active) {
                    tracked_obj.frames_since_last_match++;
                    if (tracked_obj.frames_since_last_match > max_frames_to_keep) {
                        tracked_obj.is_active = false;
#ifdef DEBUG
                        std::cout << "INACTIVE" << std::endl;
#endif // DEBUG
                    }
                }
            }
            return {};  // Return empty vector as no current detections
        }
        
        // Create a flow network for bipartite matching
        // Vertices: source(0) + active objects(1 to n_prev) + current detections(n_prev+1 to n_prev+n_curr) + sink(n_prev+n_curr+1)
        int source = 0;
        int sink = n_prev + n_curr + 1;
        int total_vertices = sink + 1;
        
        MaximumBipartiteMatching network(total_vertices, source, sink);
        
        // Connect source to all active objects with capacity 1
        for (int i = 1; i <= n_prev; i++) {
            network.addEdge(source, i, 1);
        }
        
        // Connect current detections to sink with capacity 1
        for (int j = 1; j <= n_curr; j++) {
            network.addEdge(n_prev + j, sink, 1);
        }
        
        // Connect active objects to current detections with capacity based on IoU
        for (int i = 0; i < n_prev; i++) {
            for (int j = 0; j < n_curr; j++) {
                double iou = calculateIoU(active_objects[i], current_detections[j]);
                int capacity = iouToCapacity(iou);
                
                if (capacity > 0) {
                    network.addEdge(i + 1, n_prev + j + 1, capacity);
                }
            }
        }
        
        // Compute maximum flow (maximum IoU matching)
        network.maxFlow();
        
        // Retrieve matching pairs
        std::vector<std::pair<int, int>> matches = network.getMatching(n_prev, n_curr);
        
        // Mark all current detections as unassigned initially
        std::vector<bool> assigned_curr(n_curr, false);
       
        // assume non-match, increment all frame counters now, easier to reset them upon match 
        for (auto& tracked_obj : tracked_objects) {
            if (tracked_obj.is_active) {
                tracked_obj.frames_since_last_match++;
            }
        }
        
        // update ids and reset counters for matched objects
        for (const auto& match : matches) {
            int active_idx = match.first;
            int curr_idx = match.second;
            
            // Get the actual index in tracked_objects
            int tracked_idx = active_indices[active_idx];
            
            // Update current detection with matched ID
            current_detections[curr_idx].id = tracked_objects[tracked_idx].object.id;
            
            // Reset counter for this object
            tracked_objects[tracked_idx].frames_since_last_match = 0;
            
            // Update object position and dimensions
            tracked_objects[tracked_idx].object = current_detections[curr_idx];
            
            // Mark as assigned
            assigned_curr[curr_idx] = true;
        }
        
        // Check for objects to deactivate (exceeded max frames without match)
        for (auto& tracked_obj : tracked_objects) 
        {
            if (tracked_obj.is_active && tracked_obj.frames_since_last_match > max_frames_to_keep) 
            {
                tracked_obj.is_active = false;
#ifdef DEBUG
                std::cout << "*********************" << std::endl;
#endif //DEBUG
            }
        }
        
        // Assign new IDs to unmatched detections and add to tracked objects
        for (int i = 0; i < n_curr; i++) 
        {
            if (!assigned_curr[i]) 
            {
                current_detections[i].id = next_id++;
                std::cout << "#" << std::endl;
                tracked_objects.push_back(TrackedObject(current_detections[i]));
            }
        }
        
        // Return all current detections with assigned IDs
        return current_detections;
    }
    
    // Get all currently tracked objects (including inactive ones that are kept for persistence)
    std::vector<Object2D> getAllTrackedObjects() const {
        std::vector<Object2D> result;
        for (const auto& tracked_obj : tracked_objects) {
            result.push_back(tracked_obj.object);
        }
        return result;
    }
    
    // Get only active tracked objects
    std::vector<Track> getActiveTrackedObjects() const {
        std::vector<Track> result;
        for (const auto& tracked_obj : tracked_objects) {
            if (tracked_obj.is_active) {
                result.push_back(object2d_to_track(tracked_obj.object));
            }
        }
        return result;
    }
    
    // Reset the tracker
    void reset() {
        tracked_objects.clear();
        next_id = 0;
    }
};


