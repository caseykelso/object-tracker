/**
 * IoU-Based 2D Object Tracker
 * 
 * This implementation tracks 2D objects with dimensions using IoU (Intersection over Union)
 * as the similarity metric between objects in consecutive frames. The Edmonds-Karp algorithm
 * is used to solve the maximum bipartite matching problem.
 */
#include <chrono>
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include <stdint.h>
#include "types.h"
#include "tracker.h"


