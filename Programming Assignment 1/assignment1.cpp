#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cmath>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>

using namespace std;

string movePrint(int n) {
    switch(n) {
        case 0: return "Starting state";
        case 1: return "Put one chicken in the boat";
        case 2: return "Put two chickens in the boat";
        case 3: return "Put one wolf in the boat";
        case 4: return "Put one wolf and one chicken in the boat";
        case 5: return "Put two wolves in the boat";
        default: exit(1);
    }
}

struct state {
    int leftChickens;
    int leftWolves;
    int leftBoat;
    int rightChickens;
    int rightWolves;
    int rightBoat;
    int index;
    int parentIndex;
    int move;
    int depth;
    double heuristic;
};

// Used to create a state
state newState(int leftChickens, int leftWolves, int leftBoat, int rightChickens, int rightWolves, int rightBoat, int index, int parentIndex, int move, int depth, double heuristic = -1) {
    state retState;
    retState.leftChickens = leftChickens;
    retState.leftWolves = leftWolves;
    retState.leftBoat = leftBoat;
    retState.rightChickens = rightChickens;
    retState.rightWolves = rightWolves;
    retState.rightBoat = rightBoat;
    retState.index = index;
    retState.parentIndex = parentIndex;
    retState.move = move;
    retState.depth = depth;
    retState.heuristic = heuristic;
    return retState;
}

// Prints a state
void printState(const state s) {
    cout << "Left Bank: " << s.leftChickens << " chicken(s), " << s.leftWolves << " wolf(s), " << s.leftBoat << " boat\n";
    cout << "Right Bank: " << s.rightChickens << " chicken(s), " << s.rightWolves << " wolf(s), " << s.rightBoat << " boat\n";
}

// Tests equality between two states
bool statesEqual(const state s1, const state s2) {
    return (s1.leftChickens == s2.leftChickens) && (s1.leftWolves == s2.leftWolves) && (s1.leftBoat == s2.leftBoat)
                && (s1.rightChickens == s2.rightChickens) && (s1.rightWolves == s2.rightWolves) && (s1.rightBoat == s2.rightBoat);
}

// Used to check if a state is valid
bool validState(const state s) {
    bool leftValid = true;
    bool rightValid = true;

    if(s.leftChickens > 0) {
        if(s.leftChickens < s.leftWolves) {
            leftValid = false;
        }
    }

    if(s.rightChickens > 0) {
        if(s.rightChickens < s.rightWolves) {
            rightValid = false;
        }
    }

    if(leftValid && rightValid) {
        return true;
    }

    return false;
}

int printSolutionRec(vector<state>& states, int index) {
    if(states[index].parentIndex != -1) {
        int length = printSolutionRec(states, states[index].parentIndex);

        cout << movePrint(states[index].move);
    
        if(states[index].leftBoat) {
            cout << " going to the left\n";
        } else {
            cout << " going to the right\n";
        }

        return length+1;
    } else {
        return 0;
    }
}

// Prints a solution
void printSolution(vector<state>& states, int index) {
    int length = printSolutionRec(states, index);
    cout << "\nTotal length: " << length << "\n";
}

// Hash function used in hash table
struct hashFunc {
    std::size_t operator() (const state& s) const {
        return (11 * s.leftChickens) + (13 * s.leftWolves) + (17 * s.leftBoat) + (19 * s.rightChickens) + (23 * s.rightWolves) + (29 * s.rightBoat);
    }
};

// Used for hash table.
struct keyEqual {
    bool operator()(state const& s1, state const& s2) const {
        return (s1.leftChickens == s2.leftChickens) && (s1.leftWolves == s2.leftWolves) && (s1.leftBoat == s2.leftBoat)
                && (s1.rightChickens == s2.rightChickens) && (s1.rightWolves == s2.rightWolves) && (s1.rightBoat == s2.rightBoat);
    }
};

// Comparator for priority_queue
struct compareStates {
    bool operator()(state const& s1, state const& s2) const {
        return (s1.depth + s1.heuristic > s2.depth + s2.heuristic);
    }
};

// Heuristic used in A*
double heuristic(const state s, const state goal) {
    // return (s.rightWolves + s.rightChickens)/2.0;
    return ((abs(goal.rightWolves - s.rightWolves) + abs(goal.rightChickens - s.rightChickens)
            + abs(goal.leftWolves - s.leftWolves) + abs(goal.leftChickens - s.leftChickens)))/4;
}

void dfs(state initialState, state goalState) {
    int expandedCounter = 0;

    // Stores all seen states. Used at end to trace solution.
    vector<state> states;
    states.push_back(initialState);

    // Stack used for DFS frontier. Stores indices in states
    stack<int> frontier;
    frontier.push(0);

    // Hash table used for lookup. Maps state -> depth
    unordered_map<state, int, hashFunc, keyEqual> explored;

    while(true) {
        if(frontier.empty()) {
            cout << "DFS failed: No solution\n";
            return;
        }

        // Get current state from frontier
        state s = states[frontier.top()];
        frontier.pop();

        // Check if already explored. If so, check if the current depth is shallower than the recorded depth. If it is shallower, don't continue and expand it
        if(explored[s]) {
            if(explored[s] <= s.depth) {
                continue;
            }
        }

        // Check if goal state
        if(statesEqual(s, goalState)) {
            cout << "DFS Solution found:\n\n";

            cout << "Starting state:\n";
            printState(initialState);
            cout << "\n";

            printSolution(states, s.index);

            cout << "\n";
            cout << "Final state:\n";
            printState(s);
            cout << "\n";

            cout << "Nodes expanded: " << expandedCounter << "\n";
            return;
        }

        // Add current state to explored states
        explored[s] = s.depth;
        expandedCounter++;

        // Expand current state

        // If boat on left side
        if(s.leftBoat) {
            // Try putting one chicken in the boat
            if(s.leftChickens >= 1) {
                state nextState = newState(s.leftChickens - 1, s.leftWolves, 0, s.rightChickens + 1, s.rightWolves, 1, 0, s.index, 1, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting two chickens in the boat
            if(s.leftChickens >= 2) {
                state nextState = newState(s.leftChickens - 2, s.leftWolves, 0, s.rightChickens + 2, s.rightWolves, 1, 0, s.index, 2, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting one wolf in the boat
            if(s.leftWolves >= 1) {
                state nextState = newState(s.leftChickens, s.leftWolves - 1, 0, s.rightChickens, s.rightWolves + 1, 1, 0, s.index, 3, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting one wolf and one chicken in the boat
            if(s.leftWolves >= 1 && s.leftChickens >= 1) {
                state nextState = newState(s.leftChickens - 1, s.leftWolves - 1, 0, s.rightChickens + 1, s.rightWolves + 1, 1, 0, s.index, 4, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting two wolves in the boat
            if(s.leftWolves >= 2) {
                state nextState = newState(s.leftChickens, s.leftWolves - 2, 0, s.rightChickens, s.rightWolves + 2, 1, 0, s.index, 5, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }
        }

        // If boat on right side
        if(s.rightBoat) {
            // Try putting one chicken in the boat
            if(s.rightChickens >= 1) {
                state nextState = newState(s.leftChickens + 1, s.leftWolves, 1, s.rightChickens - 1, s.rightWolves, 0, 0, s.index, 1, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting two chickens in the boat
            if(s.rightChickens >= 2) {
                state nextState = newState(s.leftChickens + 2, s.leftWolves, 1, s.rightChickens - 2, s.rightWolves, 0, 0, s.index, 2, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting one wolf in the boat
            if(s.rightWolves >= 1) {
                state nextState = newState(s.leftChickens, s.leftWolves + 1, 1, s.rightChickens, s.rightWolves - 1, 0, 0, s.index, 3, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting one wolf and one chicken in the boat
            if(s.rightWolves >= 1 && s.rightChickens >= 1) {
                state nextState = newState(s.leftChickens + 1, s.leftWolves + 1, 1, s.rightChickens - 1, s.rightWolves - 1, 0, 0, s.index, 4, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting two wolves in the boat
            if(s.rightWolves >= 2) {
                state nextState = newState(s.leftChickens, s.leftWolves + 2, 1, s.rightChickens, s.rightWolves - 2, 0, 0, s.index, 5, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }
        }
    }
}

void bfs(state initialState, state goalState) {
    int expandedCounter = 0;

    // Stores all seen states. Used at end to trace solution.
    vector<state> states;
    states.push_back(initialState);

    // Stack used for DFS frontier. Stores indices in states
    queue<int> frontier;
    frontier.push(0);

    // Hash table used for lookup. Maps state -> depth
    unordered_map<state, int, hashFunc, keyEqual> explored;

    while(true) {
        if(frontier.empty()) {
            cout << "BFS failed: No solution\n";
            return;
        }

        // Get current state from frontier
        state s = states[frontier.front()];
        frontier.pop();

        // Check if already explored. If so, check if the current depth is shallower than the recorded depth. If it is shallower, don't continue and expand it
        if(explored[s]) {
            if(explored[s] <= s.depth) {
                continue;
            }
        }

        // Check if goal state
        if(statesEqual(s, goalState)) {
            cout << "BFS Solution found:\n\n";

            cout << "Starting state:\n";
            printState(initialState);
            cout << "\n";

            printSolution(states, s.index);

            cout << "\n";
            cout << "Final state:\n";
            printState(s);
            cout << "\n";
            
            cout << "Nodes expanded: " << expandedCounter << "\n";
            return;
        }

        // Add current state to explored states
        explored[s] = s.depth;
        expandedCounter++;

        // Expand current state

        // If boat on left side
        if(s.leftBoat) {
            // Try putting one chicken in the boat
            if(s.leftChickens >= 1) {
                state nextState = newState(s.leftChickens - 1, s.leftWolves, 0, s.rightChickens + 1, s.rightWolves, 1, 0, s.index, 1, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting two chickens in the boat
            if(s.leftChickens >= 2) {
                state nextState = newState(s.leftChickens - 2, s.leftWolves, 0, s.rightChickens + 2, s.rightWolves, 1, 0, s.index, 2, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting one wolf in the boat
            if(s.leftWolves >= 1) {
                state nextState = newState(s.leftChickens, s.leftWolves - 1, 0, s.rightChickens, s.rightWolves + 1, 1, 0, s.index, 3, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting one wolf and one chicken in the boat
            if(s.leftWolves >= 1 && s.leftChickens >= 1) {
                state nextState = newState(s.leftChickens - 1, s.leftWolves - 1, 0, s.rightChickens + 1, s.rightWolves + 1, 1, 0, s.index, 4, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting two wolves in the boat
            if(s.leftWolves >= 2) {
                state nextState = newState(s.leftChickens, s.leftWolves - 2, 0, s.rightChickens, s.rightWolves + 2, 1, 0, s.index, 5, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }
        }

        // If boat on right side
        if(s.rightBoat) {
            // Try putting one chicken in the boat
            if(s.rightChickens >= 1) {
                state nextState = newState(s.leftChickens + 1, s.leftWolves, 1, s.rightChickens - 1, s.rightWolves, 0, 0, s.index, 1, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting two chickens in the boat
            if(s.rightChickens >= 2) {
                state nextState = newState(s.leftChickens + 2, s.leftWolves, 1, s.rightChickens - 2, s.rightWolves, 0, 0, s.index, 2, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting one wolf in the boat
            if(s.rightWolves >= 1) {
                state nextState = newState(s.leftChickens, s.leftWolves + 1, 1, s.rightChickens, s.rightWolves - 1, 0, 0, s.index, 3, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting one wolf and one chicken in the boat
            if(s.rightWolves >= 1 && s.rightChickens >= 1) {
                state nextState = newState(s.leftChickens + 1, s.leftWolves + 1, 1, s.rightChickens - 1, s.rightWolves - 1, 0, 0, s.index, 4, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }

            // Try putting two wolves in the boat
            if(s.rightWolves >= 2) {
                state nextState = newState(s.leftChickens, s.leftWolves + 2, 1, s.rightChickens, s.rightWolves - 2, 0, 0, s.index, 5, s.depth + 1);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(states.size()-1);
                }
            }
        }
    }
}

void iddfs(state initialState, state goalState) {
    int expandedCounter = 0;

    int depthLimit = -1;
    bool newStateFound = true;
    while(newStateFound) {
        depthLimit++;
        newStateFound = false;

        // Stores all seen states. Used at end to trace solution.
        vector<state> states;
        states.push_back(initialState);

        // Stack used for DFS frontier. Stores indices in states
        stack<int> frontier;
        frontier.push(0);

        // Hash table used for lookup. Maps state -> depth
        unordered_map<state, int, hashFunc, keyEqual> explored;

        while(true) {
            if(frontier.empty()) {
                break;
            }

            // Get current state from frontier
            state s = states[frontier.top()];
            frontier.pop();

            // Check if already explored. If so, check if the current depth is shallower than the recorded depth. If it is shallower, don't continue and expand it
            if(explored[s]) {
                if(explored[s] <= s.depth) {
                    continue;
                }
            }

            // If a non-explored state of depth past the limit is found. If so, mark newStateFound true so that the next iteration starts
            if(s.depth > depthLimit) {
                newStateFound = true;
                continue;
            }

            // Check if goal state
            if(statesEqual(s, goalState)) {
                cout << "IDDFS Solution found:\n\n";

                cout << "Starting state:\n";
                printState(initialState);
                cout << "\n";

                printSolution(states, s.index);

                cout << "\n";
                cout << "Final state:\n";
                printState(s);
                cout << "\n";

                cout << "Nodes expanded (all loops): " << expandedCounter << "\n";
                return;
            }

            // Add current state to explored states
            explored[s] = s.depth;
            expandedCounter++;

            // Expand current state

            // If boat on left side
            if(s.leftBoat) {
                // Try putting one chicken in the boat
                if(s.leftChickens >= 1) {
                    state nextState = newState(s.leftChickens - 1, s.leftWolves, 0, s.rightChickens + 1, s.rightWolves, 1, 0, s.index, 1, s.depth + 1);
                    if(validState(nextState)) {
                        // Check if already explored first
                        nextState.index = states.size();
                        states.push_back(nextState);
                        frontier.push(states.size()-1);
                    }
                }

                // Try putting two chickens in the boat
                if(s.leftChickens >= 2) {
                    state nextState = newState(s.leftChickens - 2, s.leftWolves, 0, s.rightChickens + 2, s.rightWolves, 1, 0, s.index, 2, s.depth + 1);
                    if(validState(nextState)) {
                        // Check if already explored first
                        nextState.index = states.size();
                        states.push_back(nextState);
                        frontier.push(states.size()-1);
                    }
                }

                // Try putting one wolf in the boat
                if(s.leftWolves >= 1) {
                    state nextState = newState(s.leftChickens, s.leftWolves - 1, 0, s.rightChickens, s.rightWolves + 1, 1, 0, s.index, 3, s.depth + 1);
                    if(validState(nextState)) {
                        // Check if already explored first
                        nextState.index = states.size();
                        states.push_back(nextState);
                        frontier.push(states.size()-1);
                    }
                }

                // Try putting one wolf and one chicken in the boat
                if(s.leftWolves >= 1 && s.leftChickens >= 1) {
                    state nextState = newState(s.leftChickens - 1, s.leftWolves - 1, 0, s.rightChickens + 1, s.rightWolves + 1, 1, 0, s.index, 4, s.depth + 1);
                    if(validState(nextState)) {
                        // Check if already explored first
                        nextState.index = states.size();
                        states.push_back(nextState);
                        frontier.push(states.size()-1);
                    }
                }

                // Try putting two wolves in the boat
                if(s.leftWolves >= 2) {
                    state nextState = newState(s.leftChickens, s.leftWolves - 2, 0, s.rightChickens, s.rightWolves + 2, 1, 0, s.index, 5, s.depth + 1);
                    if(validState(nextState)) {
                        // Check if already explored first
                        nextState.index = states.size();
                        states.push_back(nextState);
                        frontier.push(states.size()-1);
                    }
                }
            }

            // If boat on right side
            if(s.rightBoat) {
                // Try putting one chicken in the boat
                if(s.rightChickens >= 1) {
                    state nextState = newState(s.leftChickens + 1, s.leftWolves, 1, s.rightChickens - 1, s.rightWolves, 0, 0, s.index, 1, s.depth + 1);
                    if(validState(nextState)) {
                        // Check if already explored first
                        nextState.index = states.size();
                        states.push_back(nextState);
                        frontier.push(states.size()-1);
                    }
                }

                // Try putting two chickens in the boat
                if(s.rightChickens >= 2) {
                    state nextState = newState(s.leftChickens + 2, s.leftWolves, 1, s.rightChickens - 2, s.rightWolves, 0, 0, s.index, 2, s.depth + 1);
                    if(validState(nextState)) {
                        // Check if already explored first
                        nextState.index = states.size();
                        states.push_back(nextState);
                        frontier.push(states.size()-1);
                    }
                }

                // Try putting one wolf in the boat
                if(s.rightWolves >= 1) {
                    state nextState = newState(s.leftChickens, s.leftWolves + 1, 1, s.rightChickens, s.rightWolves - 1, 0, 0, s.index, 3, s.depth + 1);
                    if(validState(nextState)) {
                        // Check if already explored first
                        nextState.index = states.size();
                        states.push_back(nextState);
                        frontier.push(states.size()-1);
                    }
                }

                // Try putting one wolf and one chicken in the boat
                if(s.rightWolves >= 1 && s.rightChickens >= 1) {
                    state nextState = newState(s.leftChickens + 1, s.leftWolves + 1, 1, s.rightChickens - 1, s.rightWolves - 1, 0, 0, s.index, 4, s.depth + 1);
                    if(validState(nextState)) {
                        // Check if already explored first
                        nextState.index = states.size();
                        states.push_back(nextState);
                        frontier.push(states.size()-1);
                    }
                }

                // Try putting two wolves in the boat
                if(s.rightWolves >= 2) {
                    state nextState = newState(s.leftChickens, s.leftWolves + 2, 1, s.rightChickens, s.rightWolves - 2, 0, 0, s.index, 5, s.depth + 1);
                    if(validState(nextState)) {
                        // Check if already explored first
                        nextState.index = states.size();
                        states.push_back(nextState);
                        frontier.push(states.size()-1);
                    }
                }
            }
        }
    }
}

void astar(state initialState, state goalState) {
    int expandedCounter = 0;

    // Stores all seen states. Used at end to trace solution.
    vector<state> states;
    states.push_back(initialState);

    // Stack used for DFS frontier. Stores indices in states
    priority_queue<state, vector<state>, compareStates> frontier;
    frontier.push(initialState);

    // Hash table used for lookup. Maps state -> depth
    unordered_map<state, int, hashFunc, keyEqual> explored;

    while(true) {
        if(frontier.empty()) {
            cout << "A* failed: No solution\n";
            return;
        }

        // Get current state from frontier
        state s = frontier.top();
        frontier.pop();

        // Check if already explored. If so, check if the current depth is shallower than the recorded depth. If it is shallower, don't continue and expand it
        if(explored[s]) {
            if(explored[s] <= s.depth) {
                continue;
            }
        }

        // printState(s);
        // cout << s.depth << " " << s.heuristic << endl << endl;

        // Check if goal state
        if(statesEqual(s, goalState)) {
            cout << "A* Solution found:\n\n";

            cout << "Starting state:\n";
            printState(initialState);
            cout << "\n";

            printSolution(states, s.index);

            cout << "\n";
            cout << "Final state:\n";
            printState(s);
            cout << "\n";
            
            cout << "Nodes expanded: " << expandedCounter << "\n";
            return;
        }

        // Add current state to explored states
        explored[s] = s.depth;
        expandedCounter++;

        // Expand current state

        // If boat on left side
        if(s.leftBoat) {
            // Try putting one chicken in the boat
            if(s.leftChickens >= 1) {
                state nextState = newState(s.leftChickens - 1, s.leftWolves, 0, s.rightChickens + 1, s.rightWolves, 1, 0, s.index, 1, s.depth + 1);
                nextState.heuristic = heuristic(nextState, goalState);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(nextState);
                }
            }

            // Try putting two chickens in the boat
            if(s.leftChickens >= 2) {
                state nextState = newState(s.leftChickens - 2, s.leftWolves, 0, s.rightChickens + 2, s.rightWolves, 1, 0, s.index, 2, s.depth + 1);
                nextState.heuristic = heuristic(nextState, goalState);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(nextState);
                }
            }

            // Try putting one wolf in the boat
            if(s.leftWolves >= 1) {
                state nextState = newState(s.leftChickens, s.leftWolves - 1, 0, s.rightChickens, s.rightWolves + 1, 1, 0, s.index, 3, s.depth + 1);
                nextState.heuristic = heuristic(nextState, goalState);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(nextState);
                }
            }

            // Try putting one wolf and one chicken in the boat
            if(s.leftWolves >= 1 && s.leftChickens >= 1) {
                state nextState = newState(s.leftChickens - 1, s.leftWolves - 1, 0, s.rightChickens + 1, s.rightWolves + 1, 1, 0, s.index, 4, s.depth + 1);
                nextState.heuristic = heuristic(nextState, goalState);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(nextState);
                }
            }

            // Try putting two wolves in the boat
            if(s.leftWolves >= 2) {
                state nextState = newState(s.leftChickens, s.leftWolves - 2, 0, s.rightChickens, s.rightWolves + 2, 1, 0, s.index, 5, s.depth + 1);
                nextState.heuristic = heuristic(nextState, goalState);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(nextState);
                }
            }
        }

        // If boat on right side
        if(s.rightBoat) {
            // Try putting one chicken in the boat
            if(s.rightChickens >= 1) {
                state nextState = newState(s.leftChickens + 1, s.leftWolves, 1, s.rightChickens - 1, s.rightWolves, 0, 0, s.index, 1, s.depth + 1);
                nextState.heuristic = heuristic(nextState, goalState);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(nextState);
                }
            }

            // Try putting two chickens in the boat
            if(s.rightChickens >= 2) {
                state nextState = newState(s.leftChickens + 2, s.leftWolves, 1, s.rightChickens - 2, s.rightWolves, 0, 0, s.index, 2, s.depth + 1);
                nextState.heuristic = heuristic(nextState, goalState);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(nextState);
                }
            }

            // Try putting one wolf in the boat
            if(s.rightWolves >= 1) {
                state nextState = newState(s.leftChickens, s.leftWolves + 1, 1, s.rightChickens, s.rightWolves - 1, 0, 0, s.index, 3, s.depth + 1);
                nextState.heuristic = heuristic(nextState, goalState);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(nextState);
                }
            }

            // Try putting one wolf and one chicken in the boat
            if(s.rightWolves >= 1 && s.rightChickens >= 1) {
                state nextState = newState(s.leftChickens + 1, s.leftWolves + 1, 1, s.rightChickens - 1, s.rightWolves - 1, 0, 0, s.index, 4, s.depth + 1);
                nextState.heuristic = heuristic(nextState, goalState);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(nextState);
                }
            }

            // Try putting two wolves in the boat
            if(s.rightWolves >= 2) {
                state nextState = newState(s.leftChickens, s.leftWolves + 2, 1, s.rightChickens, s.rightWolves - 2, 0, 0, s.index, 5, s.depth + 1);
                nextState.heuristic = heuristic(nextState, goalState);
                if(validState(nextState)) {
                    // Check if already explored first
                    nextState.index = states.size();
                    states.push_back(nextState);
                    frontier.push(nextState);
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if(argc < 5) {
        cout << "Incorrect number of arguments\n";
        return 1;
    }

    // Start state
    ifstream startFile;
    startFile.open(argv[1]);

    string leftBank;
    string rightBank;

    if(startFile.is_open()) {
        startFile >> leftBank;
        startFile >> rightBank;
    } else {
        cout << "Failed to open start file\n";
        return 1;
    }

    state initialState;

    int index = leftBank.find(",", 0);
    initialState.leftChickens = stoi(leftBank.substr(0, index));
    leftBank.erase(0, index+1);

    index = leftBank.find(",", 0);
    initialState.leftWolves = stoi(leftBank.substr(0, index));
    leftBank.erase(0, index+1);

    initialState.leftBoat = stoi(leftBank);

    index = rightBank.find(",", 0);
    initialState.rightChickens = stoi(rightBank.substr(0, index));
    rightBank.erase(0, index+1);

    index = rightBank.find(",", 0);
    initialState.rightWolves = stoi(rightBank.substr(0, index));
    rightBank.erase(0, index+1);

    initialState.rightBoat = stoi(rightBank);

    initialState.index = 0;

    initialState.parentIndex = -1;

    initialState.depth = 1;

    initialState.move = 0;

    // Goal state
    ifstream goalFile;
    goalFile.open(argv[2]);

    if(goalFile.is_open()) {
        goalFile >> leftBank;
        goalFile >> rightBank;
    } else {
        cout << "Failed to open goal file\n";
        return 1;
    }

    state goalState;

    index = leftBank.find(",", 0);
    goalState.leftChickens = stoi(leftBank.substr(0, index));
    leftBank.erase(0, index+1);

    index = leftBank.find(",", 0);
    goalState.leftWolves = stoi(leftBank.substr(0, index));
    leftBank.erase(0, index+1);

    goalState.leftBoat = stoi(leftBank);

    index = rightBank.find(",", 0);
    goalState.rightChickens = stoi(rightBank.substr(0, index));
    rightBank.erase(0, index+1);

    index = rightBank.find(",", 0);
    goalState.rightWolves = stoi(rightBank.substr(0, index));
    rightBank.erase(0, index+1);

    goalState.rightBoat = stoi(rightBank);

    goalState.index = 0;

    goalState.parentIndex = -1;

    ofstream outFile(argv[4]);

    if(!goalFile.is_open()) {
        cout << "Failed to open output file\n";
        return 1;
    }

    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(outFile.rdbuf());

    if(strcmp("dfs", argv[3]) == 0) {
        dfs(initialState, goalState);
    }

    if(strcmp("bfs", argv[3]) == 0) {
        bfs(initialState, goalState);
    }

    if(strcmp("iddfs", argv[3]) == 0) {
        iddfs(initialState, goalState);
    }

    if(strcmp("astar", argv[3]) == 0) {
        astar(initialState, goalState);
    }

    return 0;
}