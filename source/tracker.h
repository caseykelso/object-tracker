#pragma once

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

// IoU-based 2D object tracker
class IoUTracker 
{
private:
    std::vector<Object2D> previous_objects;
    int next_id;
    double min_iou_threshold;    // Minimum IoU to consider a match
    
    // Calculate IoU (Intersection over Union) between two objects
    double calculateIoU(const Object2D& obj1, const Object2D& obj2) 
    {
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
        if (union_area > 0) 
        {
            return intersection_area / union_area;
        }
        return 0.0;  // No overlap
    }
    
    // Convert IoU to an integer capacity (higher IoU = higher capacity)
    int iouToCapacity(double iou) 
    {
        if (iou < min_iou_threshold) 
        {
            return 0;  // No edge if IoU too small
        }
        
        // Scale to integer (higher IoU = higher capacity)
        int capacity = static_cast<int>(iou * 10000);
        return std::max(capacity, 1);  // Ensure at least capacity 1 if above threshold
    }

public:
    // Constructor with IoU threshold parameter
    IoUTracker(double iou_threshold = 0.3, uint8_t max_missing_frames = 10) 
        : next_id(0), min_iou_threshold(iou_threshold) {}

    // Process new detections and match with existing tracks using IoU
    std::vector<Object2D> update(const std::vector<Object2D>& detections) 
    {
        std::vector<Object2D> current_objects = detections;
        
        // First frame or no previous objects
        if (previous_objects.empty()) 
        {
            // Assign new IDs to all detections
            for (auto& obj : current_objects) 
            {
                obj.id = next_id++;
            }

            previous_objects = current_objects;
            return current_objects;
        }

        int n_prev = previous_objects.size();
        int n_curr = current_objects.size();
        
        // Create a flow network for bipartite matching
        // Vertices: source(0) + previous objects(1 to n_prev) + current objects(n_prev+1 to n_prev+n_curr) + sink(n_prev+n_curr+1)
        int source = 0;
        int sink = n_prev + n_curr + 1;
        int total_vertices = sink + 1;
        
        MaximumBipartiteMatching network(total_vertices, source, sink);
        
        // Connect source to all previous objects with capacity 1
        for (int i = 1; i <= n_prev; i++) 
        {
            network.addEdge(source, i, 1);
        }
        
        // Connect current objects to sink with capacity 1
        for (int j = 1; j <= n_curr; j++) 
        {
            network.addEdge(n_prev + j, sink, 1);
        }
        
        // Connect previous objects to current objects with capacity based on IoU
        for (int i = 0; i < n_prev; i++) 
        {
            for (int j = 0; j < n_curr; j++) 
            {
                double iou = calculateIoU(previous_objects[i], current_objects[j]);
                int capacity = iouToCapacity(iou);
                
                if (capacity > 0) 
                {
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
        
        // Assign IDs based on matching
        for (const auto& match : matches) 
        {
            int prev_idx = match.first;
            int curr_idx = match.second;
            
            current_objects[curr_idx].id = previous_objects[prev_idx].id;
            assigned_curr[curr_idx] = true;
        }
        
        // Assign new IDs to unmatched detections
        for (int i = 0; i < n_curr; i++) 
        {
            if (!assigned_curr[i]) 
            {
                current_objects[i].id = next_id++;
            }
        }
        
        // Update previous objects for next frame
        previous_objects = current_objects;
        
        return current_objects;
    }
    
    // Reset the tracker
    void reset() 
    {
        previous_objects.clear();
        next_id = 0;
    }
};


