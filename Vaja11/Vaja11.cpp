#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <chrono>
using namespace std;

int number_of_nodes;
string fileName = "graph1.txt";

// Structure to represent a path in the graph
struct Path {
    int from_node = 0, to_node = 0, cost = 0;               // From which node, to which node, and the cost of the path
    bool* visited = new bool[number_of_nodes];  // Array to track visited nodes
    Path* next = nullptr;                                 // Pointer to the next path in the list
};

void menu() {
    cout << endl;
    cout << "Traveling Salesperson Problem - Options:" << endl;
    cout << "1) Read matrix" << endl;
    cout << "2) Solve the Traveling Salesperson Problem" << endl;
    cout << "3) Display generated levels" << endl;
    cout << "4) Reconstruct the path" << endl;
    cout << "5) Exit" << endl;
    cout << endl;
    cout << "Choice: ";
}

// Read the graph matrix from a file
void readMatrix(int* C[]) {
    ifstream file(fileName);

    // Check if the file opened successfully
    if (!file.is_open()) {
        cout << "Error: Could not open the file '" << fileName << "'." << endl;
        return;
    }

    // Try reading the number of nodes from the file
    if (!(file >> number_of_nodes)) {
        cout << "Error: Could not read the number of nodes from the file. Please check the file format." << endl;
        return;
    }

    // Initialize the adjacency matrix
    for (int i = 0; i < number_of_nodes; i++) {
        C[i] = new int[number_of_nodes];
    }

    // Fill the adjacency matrix with costs from the file
    int connection_cost;
    for (int i = 0; i < number_of_nodes; i++) {
        for (int j = 0; j < number_of_nodes; j++) {
            if (!(file >> connection_cost)) { // Check if reading the cost is successful
                cout << "Error: Could not read the connection cost for node (" << i << ", " << j
                    << "). Please check the file format." << endl;
                return;
            }
            C[i][j] = connection_cost;
        }
    }

    // Notify the user that the matrix was read successfully
    cout << "Matrix successfully read from the file '" << fileName << "'." << endl;
}


// Create initial paths starting from the first node
vector<Path*> createInitialPaths(int* C[]) {
    vector<Path*> initial_paths;

    for (int i = 1; i < number_of_nodes; i++) {
        Path* p = new Path();
        p->from_node = i + 1;       // Start from node 1 to all other nodes
        p->to_node = 1;             // End at node 1
        p->cost = C[i][0];          // Cost from the current node to node 1

        // Mark all nodes as unvisited initially
        for (int j = 0; j < number_of_nodes; j++) {
            p->visited[j] = false;
        }

        // Add the new path to the list
        if (!initial_paths.empty()) {
            initial_paths.back()->next = p;
        }

        initial_paths.push_back(p);
    }

    return initial_paths;
}

// Generate the level list of paths for the given node
void generateLevelList(int* C[], int current_node, stack<vector<Path*>>& path_stack, vector<Path*>& level) {
    vector<Path*> previous_level = path_stack.top();

    // Iterate over paths in the previous level
    for (Path* prev_path : previous_level) {
        // Skip paths that originate from the same node
        if (prev_path->from_node == current_node) continue;

        // If the current node is not visited in the path
        if (!prev_path->visited[current_node]) {
            // Create a new path
            Path* p = new Path();
            p->from_node = current_node;
            p->to_node = prev_path->from_node;
            p->cost = C[p->from_node - 1][p->to_node - 1] + prev_path->cost;

            // Copy the visited status and mark the new node as visited
            p->visited = prev_path->visited;
            p->visited[p->to_node - 1] = true;

            level.push_back(p); // Add the new path to the current level
        }
    }

    // Remove redundant paths with higher costs
    for (size_t i = 0; i < level.size(); i++) {
        for (size_t j = i + 1; j < level.size(); j++) {
            if (level[i]->from_node == level[j]->from_node && level[i]->visited == level[j]->visited) {
                if (level[i]->cost > level[j]->cost) {
                    level.erase(level.begin() + i);
                }
                else {
                    level.erase(level.begin() + j);
                }
            }
        }
    }
}

// Solve the Traveling Salesperson Problem
stack<vector<Path*>> solveTSP(int* C[]) {
    auto start = chrono::steady_clock::now();

    stack<vector<Path*>> path_stack;

    // Generate the first level of paths and add it to the stack
    vector<Path*> first_level = createInitialPaths(C);
    path_stack.push(first_level);

    // Generate all levels except the last
    for (int i = 2; i < number_of_nodes - 1; i++) {
        vector<Path*> level;                                // Create an empty level
        for (int node = 2; node < number_of_nodes; node++) {
            generateLevelList(C, node, path_stack, level);  // Populate the level
        }
        path_stack.push(level);                             // Add the level to the stack
    }

    // Generate the final level of paths
    vector<Path*> last_level;
    generateLevelList(C, 1, path_stack, last_level); // Connect back to the starting node
    path_stack.push(last_level);

    auto end = chrono::steady_clock::now();
    cout << "Algorithm execution time: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << "μs." << endl;

    return path_stack;
}

// Display all levels of paths
void displayLevels(stack<vector<Path*>> path_stack) {
    vector<Path*> level;
    int level_counter = 0;

    while (!path_stack.empty()) {
        level = path_stack.top();

        cout << "LEVEL " << path_stack.size() - level_counter << ":" << endl;
        for (Path* p : level) {
            cout << "From: " << p->from_node << ", To: " << p->to_node << ", Cost: " << p->cost << endl;
        }
        cout << endl;

        path_stack.pop();
    }
}

// Reconstruct the optimal path for the Traveling Salesman Problem
void reconstructPath(stack<vector<Path*>> levels) {
    if (levels.empty()) {
        cout << "Error: No levels to reconstruct the path." << endl;
        return;
    }

    // Get the last level from the stack
    vector<Path*> lastLevel = levels.top();
    levels.pop();

    // Find the path with the minimum cost in the last level
    Path* optimalPath = nullptr;
    int minimumCost = INT_MAX;

    for (Path* path : lastLevel) {
        if (path->cost < minimumCost) {
            minimumCost = path->cost;
            optimalPath = path;
        }
    }

    if (!optimalPath) {
        cout << "Error: No valid path found in the last level." << endl;
        return;
    }

    // Trace back the path from the optimal path to the start
    vector<int> reconstructedPath;
    int currentNode = optimalPath->from_node;
    reconstructedPath.push_back(currentNode);

    while (!levels.empty()) {
        vector<Path*> currentLevel = levels.top();
        levels.pop();

        for (Path* path : currentLevel) {
            if (path->to_node == currentNode) {
                currentNode = path->from_node;
                reconstructedPath.push_back(currentNode);
                break;
            }
        }
    }

    // Add the start node to complete the cycle
    reconstructedPath.push_back(1);

    // Reverse the path to display it in the correct order
    reverse(reconstructedPath.begin(), reconstructedPath.end());

    // Output the reconstructed path
    cout << "Reconstructed Path: ";
    for (int node : reconstructedPath) {
        cout << node << " ";
    }
    cout << endl;

    cout << "Total Path Cost: " << optimalPath->cost << endl;
}


int main() {
    int selection;
    bool running = true;

    // Adjacency matrix for the graph
    int** C = new int* [number_of_nodes];
    stack<vector<Path*>> path_stack;

    do {
        menu();
        cin >> selection;
        system("cls");
        switch (selection) {
        case 1:
            readMatrix(C);
            cout << endl;
            break;
        case 2:
            path_stack = solveTSP(C);
            cout << endl;
            break;
        case 3:
            cout << endl;
            displayLevels(path_stack);
            cout << endl;
            break;
        case 4:
            if (path_stack.empty()) {
                cout << "Error: No levels to reconstruct the path. Solve the problem first (Case 2)." << endl;
            }
            else {
                reconstructPath(path_stack);
            }
            break;
            break;
        case 5:
            running = false;
            break;
        default:
            cout << "ERROR: Invalid selection." << endl;
            break;
        }
    } while (running);

    return 0;
}