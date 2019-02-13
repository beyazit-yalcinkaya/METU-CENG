#include <fstream>
#include <vector>
#include <queue>
#include <utility>
#include <climits>
#include <algorithm>


class Vertex {

    public:

        int vID;
        std::pair<int, int> distances;
        int ammo;
        int ammoUsed;
        bool inQueue;
        std::pair<Vertex*, Vertex*> prevs;
        std::vector<std::pair<Vertex*, int> > adjacencyList;
        Vertex(int vID) : vID(vID),
                          distances(vID == 1 ? std::make_pair(0, INT_MAX) : std::make_pair(INT_MAX, INT_MAX)),
                          ammo(0),
                          ammoUsed(false),
                          inQueue(false),
                          prevs(std::make_pair(nullptr, nullptr)) {}

};



class Graph {

    public:

        int totalAmmo;
        int ammoLocationCount;
        Vertex* vStart;
        Vertex* vKey;
        Vertex* vScientist;
        Vertex* vChamber;
        Vertex* vAmmo1;
        Vertex* vAmmo2;
        std::vector<Vertex*> vertices;
        std::vector<Vertex*> lockedAtOddPeriods;
        std::vector<Vertex*> lockedAtEvenPeriods;

        Graph() : totalAmmo(0),
                  ammoLocationCount(0),
                  vStart(nullptr),
                  vKey(nullptr),
                  vScientist(nullptr),
                  vChamber(nullptr),
                  vAmmo1(nullptr),
                  vAmmo2(nullptr) {}

        void build_path(Vertex* start, Vertex* end, std::pair<std::vector<Vertex*>, std::vector<Vertex*> >& paths, std::pair<int, int>& sums) {
            sums = end -> distances;
            if ((end -> distances).first != INT_MAX && (end -> distances).second != INT_MAX) {
                std::vector<Vertex*> result1 = build_path_first(start, end, paths);
                std::vector<Vertex*> result2 = build_path_second(start, end, paths);
                paths.first = result1;
                paths.second = result2;
            }
            else if ((end -> distances).first != INT_MAX && (end -> distances).second == INT_MAX) {
                std::vector<Vertex*> result = build_path_first(start, end, paths);
                paths.first = result;
            }
            else if ((end -> distances).first == INT_MAX && (end -> distances).second != INT_MAX) {
                std::vector<Vertex*> result = build_path_second(start, end, paths);
                paths.second = result;
            }
            else {
                paths.first.clear();
                paths.second.clear();
            }
            return;
        }

        std::vector<Vertex*> build_path_first(Vertex* start, Vertex* end, std::pair<std::vector<Vertex*>, std::vector<Vertex*> >& paths) {
            if (start == end) {
                std::vector<Vertex*> out = paths.first;
                return out;
            }
            std::vector<Vertex*> out = build_path_second(start, (end -> prevs).first, paths);
            out.push_back(end);
            return out;
        }

        std::vector<Vertex*> build_path_second(Vertex* start, Vertex* end, std::pair<std::vector<Vertex*>, std::vector<Vertex*> >& paths) {
            if (start == end) {
                std::vector<Vertex*> out = paths.second;
                return out;
            }
            std::vector<Vertex*> out = build_path_first(start, (end -> prevs).second, paths);
            out.push_back(end);
            return out;
        }

        void delta_prime(Vertex* start, Vertex* end, std::pair<std::vector<Vertex*>, std::vector<Vertex*> >& paths, std::pair<int, int>& sums) {
            Vertex* u;
            std::queue<Vertex*> q;
            for (auto& i: vertices) {
                i -> distances = std::make_pair(INT_MAX, INT_MAX);
                i -> prevs = std::make_pair(nullptr, nullptr);
            }
            start -> distances = sums;
            q.push(start);
            start -> inQueue = true;
            while (!q.empty()) {
                u = q.front();
                q.pop();
                u -> inQueue = false;
                for (auto v: u -> adjacencyList) {
                    if (std::find(lockedAtEvenPeriods.begin(), lockedAtEvenPeriods.end(), v.first) == lockedAtEvenPeriods.end() && (u -> distances).first != INT_MAX) {
                        if (((v.first -> distances).second) > ((u -> distances).first + v.second)) {
                            (v.first -> distances).second = (u -> distances).first + v.second;
                            (v.first -> prevs).second = u;
                            if (!(v.first -> inQueue)) {
                                q.push(v.first);
                                v.first -> inQueue = true;
                            }

                        }
                    }
                    if (std::find(lockedAtOddPeriods.begin(), lockedAtOddPeriods.end(), v.first) == lockedAtOddPeriods.end() && (u -> distances).second != INT_MAX) {
                        if (((v.first -> distances).first) > ((u -> distances).second + v.second)) {
                            (v.first -> distances).first = (u -> distances).second + v.second;
                            (v.first -> prevs).first = u;
                            if (!(v.first -> inQueue)) {
                                q.push(v.first);
                                v.first -> inQueue = true;
                            }
                        }
                    }
                }
            }
            build_path(start, end, paths, sums);
            return;
        }


        void updateCurrentMin(std::vector<Vertex*>& sequence, std::pair<int, std::vector<Vertex*> >& currentMin) {
            std::pair<int, int> sums(0, INT_MAX);
            std::pair<std::vector<Vertex*>, std::vector<Vertex*> > paths({vStart}, {});
            bool visitedKey = false;
            bool visitedScientist = false;
            bool visitedChamber = false;
            bool visitedAmmo1 = false;
            bool visitedAmmo2 = false;
            if (vAmmo1)
                vAmmo1 -> ammoUsed = false;
            if (vAmmo2)
                vAmmo2 -> ammoUsed = false;
            for (int i = 1, pushed = 0; i < sequence.size(); i++, pushed = 0) {
                if (!visitedKey && sequence[i] != vKey) {
                    lockedAtOddPeriods.push_back(vKey);
                    lockedAtEvenPeriods.push_back(vKey);
                    pushed++;
                }
                if (!visitedScientist && sequence[i] != vScientist) {
                    lockedAtOddPeriods.push_back(vScientist);
                    lockedAtEvenPeriods.push_back(vScientist);
                    pushed++;
                }
                if (!visitedChamber && sequence[i] != vChamber) {
                    lockedAtOddPeriods.push_back(vChamber);
                    lockedAtEvenPeriods.push_back(vChamber);
                    pushed++;
                }
                if (!visitedAmmo1 && sequence[i] != vAmmo1) {
                    lockedAtOddPeriods.push_back(vAmmo1);
                    lockedAtEvenPeriods.push_back(vAmmo1);
                    pushed++;
                }
                if (!visitedAmmo2 && sequence[i] != vAmmo2) {
                    lockedAtOddPeriods.push_back(vAmmo2);
                    lockedAtEvenPeriods.push_back(vAmmo2);
                    pushed++;
                }
                delta_prime(sequence[i - 1], sequence[i], paths, sums);
                while (pushed != 0) {
                    lockedAtOddPeriods.pop_back();
                    lockedAtEvenPeriods.pop_back();
                    pushed--;
                }
                if (sums.first == INT_MAX && sums.second == INT_MAX && paths.first.empty() && paths.second.empty())
                    return;
                if (sequence[i] == vKey) {
                    visitedKey = true;
                }
                if (sequence[i] == vScientist) {
                    visitedScientist = true;
                }
                if (sequence[i] == vChamber) {
                    visitedChamber = true;
                }
                if (sequence[i] == vAmmo1) {
                    visitedAmmo1 = true;
                    if (!(vAmmo1 -> ammoUsed)) {
                        if (sums.first != INT_MAX)
                            sums.first -= vAmmo1 -> ammo;
                        if (sums.second != INT_MAX)
                            sums.second -= vAmmo1 -> ammo;
                        vAmmo1 -> ammoUsed = true;
                    }
                }
                if (sequence[i] == vAmmo2) {
                    visitedAmmo2 = true;
                    if (!(vAmmo2 -> ammoUsed)) {
                        if (sums.first != INT_MAX)
                            sums.first -= vAmmo2 -> ammo;
                        if (sums.second != INT_MAX)
                            sums.second -= vAmmo2 -> ammo;
                        vAmmo2 -> ammoUsed = true;
                    }
                }
            }
            if ((sums.first < currentMin.first) || ((sums.first == currentMin.first) && (paths.first.size() < currentMin.second.size()))) {
                currentMin.first = sums.first;
                currentMin.second = paths.first;
            }
            if ((sums.second < currentMin.first) || ((sums.second == currentMin.first) && (paths.second.size() < currentMin.second.size()))) {
                currentMin.first = sums.second;
                currentMin.second = paths.second;
            }
            return;
        }

        void solve() {
            std::ofstream out;
            out.open("the3.out");
            std::vector<std::vector<Vertex*> > permutations = {{vStart, vKey, vScientist, vAmmo1, vAmmo2, vChamber}, 
                                                               {vStart, vKey, vScientist, vAmmo2, vAmmo1, vChamber}, 
                                                               {vStart, vKey, vAmmo1, vScientist, vAmmo2, vChamber}, 
                                                               {vStart, vKey, vAmmo2, vScientist, vAmmo1, vChamber}, 
                                                               {vStart, vKey, vAmmo1, vAmmo2, vScientist, vChamber}, 
                                                               {vStart, vKey, vAmmo2, vAmmo1, vScientist, vChamber}, 
                                                               {vStart, vAmmo1, vKey, vScientist, vAmmo2, vChamber}, 
                                                               {vStart, vAmmo1, vKey, vAmmo2, vScientist, vChamber}, 
                                                               {vStart, vAmmo2, vKey, vScientist, vAmmo1, vChamber}, 
                                                               {vStart, vAmmo2, vKey, vAmmo1, vScientist, vChamber}, 
                                                               {vStart, vAmmo1, vAmmo2, vKey, vScientist, vChamber}, 
                                                               {vStart, vAmmo2, vAmmo1, vKey, vScientist, vChamber}, 
                                                               {vStart, vAmmo1, vKey, vScientist, vChamber}, 
                                                               {vStart, vKey, vAmmo1, vScientist, vChamber}, 
                                                               {vStart, vKey, vScientist, vAmmo1, vChamber}, 
                                                               {vStart, vKey, vScientist, vChamber}};

            std::pair<int, std::vector<Vertex*> > currentMin(INT_MAX, std::vector<Vertex*>());
            switch (ammoLocationCount) {
                case 2:
                    for (int i = 0; i < 12; i++)
                        updateCurrentMin(permutations[i], currentMin);
                case 1:
                    updateCurrentMin(permutations[12], currentMin);
                    updateCurrentMin(permutations[13], currentMin);
                    updateCurrentMin(permutations[14], currentMin);
                case 0:
                    updateCurrentMin(permutations[15], currentMin);
            }
            out << (totalAmmo - currentMin.first) << std::endl;
            int n = currentMin.second.size();
            out << n << std::endl;
            out << (currentMin.second[0] -> vID);
            for (int i = 1; i < n; i++ )
                out << " " << (currentMin.second[i] -> vID);
            out.close();
            return;
        }

        friend std::ifstream& operator>>(std::ifstream& in, const Graph& graph);

};


std::ifstream& operator>>(std::ifstream& in, Graph& graph) {
    int n;
    int u;
    int v;
    int c;
    int m;
    in >> graph.totalAmmo;
    in >> n;
    for (int i = 1; i <= n; i++)
        graph.vertices.push_back(new Vertex(i));
    graph.vStart = graph.vertices[0];
    in >> u;
    graph.vChamber = graph.vertices[u - 1];
    in >> u;
    graph.vKey = graph.vertices[u - 1];
    in >> u;
    graph.vScientist = graph.vertices[u - 1];
    in >> u;
    for (int i = 0; i < u; i++) {
        in >> v;
        graph.lockedAtOddPeriods.push_back(graph.vertices[v - 1]);
    }
    in >> u;
    for (int i = 0; i < u; i++) {
        in >> v;
        graph.lockedAtEvenPeriods.push_back(graph.vertices[v - 1]);
    }
    in >> m;
    for (int i = 0, found = 0, updated = 0; i < m; i++, found = 0, updated = 0) {
        in >> u;
        in >> v;
        in >> c;
        for (auto& temp: graph.vertices[u - 1] -> adjacencyList) {
            if (temp.first == graph.vertices[v - 1]) {
                found = 1;
                if (temp.second > c) {
                    updated = 1;
                    temp.second = c;
                }
                break;
            }
        }
        if (found == 1) {
            if (updated == 1) {
                for (auto& temp: graph.vertices[v - 1] -> adjacencyList) {
                    if (temp.first == graph.vertices[u - 1]) {
                        temp.second = c;
                        break;
                    }
                }
            }
        }
        else {
            graph.vertices[u - 1] -> adjacencyList.push_back(std::make_pair(graph.vertices[v - 1], c));
            graph.vertices[v - 1] -> adjacencyList.push_back(std::make_pair(graph.vertices[u - 1], c));
        }
    }
    in >> graph.ammoLocationCount;
    switch (graph.ammoLocationCount) {
        case 2:
            in >> v;
            in >> c;
            graph.vAmmo2 = graph.vertices[v - 1];
            graph.vAmmo2 -> ammo = c;
        case 1:
            in >> v;
            in >> c;
            graph.vAmmo1 = graph.vertices[v - 1];
            graph.vAmmo1 -> ammo = c;
        case 0:
            break;
    }
    return in;
}


int main() {
    Graph graph;
    std::ifstream inp("the3.inp");
    inp >> graph;
    graph.solve();
    return 0;
}

