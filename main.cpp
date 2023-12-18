// variant 5
#include <algorithm>
#include <cmath>
#include <conio.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

const int INF = 1e9;

struct FileNotFoundException : runtime_error {
    explicit FileNotFoundException(const string & path)
    : runtime_error("File '" + path + "' does not found: can't be reading or writing") {}
};
struct MatrixNotFoundException : runtime_error {
    explicit MatrixNotFoundException(const string & arg)
    : runtime_error("Matrix not found exception: " + arg) {}
};
struct NotMatrixException : runtime_error {
    explicit NotMatrixException(const string & arg)
    : runtime_error("Not matrix exception: " + arg) {}
};
struct BadVertexNumberException : runtime_error {
    explicit BadVertexNumberException(int vertexNumber, int from, int to)
    : runtime_error("Bad vertex number exception: received '"
                    + to_string(vertexNumber) + "', but must be in range from '"
                    + to_string(from) + "' to '" + to_string(to) + "'") {}
};

vector<vector<int>> readMatrix(const string & path) {
    ifstream fin(path);
    if (!fin)
        throw FileNotFoundException(path);
    vector<int> line;
    int buffer;
    while (fin >> buffer)
        line.push_back(buffer);
    fin.close();
    if (line.empty())
        throw MatrixNotFoundException("no numbers found");
    int n = int(round(sqrt(line.size())));
    if (line.size() != n * n)
        throw NotMatrixException("count of numbers must be a square of an integer");
    vector<vector<int>> matrix(n, vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            matrix[i][j] = line[i * n + j];
    return matrix;
}

pair<bool, bool> getFlags(const vector<vector<int>> & matrix) {
    int n = int(matrix.size());
    bool isOnlyZerosAndOnes = true, isHasNegative = false;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (i == j) {
                if (matrix[i][i] < 0) {
                    isHasNegative = true;
                    if (!isOnlyZerosAndOnes)
                        return make_pair(isOnlyZerosAndOnes, isHasNegative);
                }
            } else {
                if (matrix[i][j] < 0) {
                    isOnlyZerosAndOnes = false;
                    isHasNegative = true;
                    return make_pair(isOnlyZerosAndOnes, isHasNegative);
                } else if (matrix[i][j] > 1) {
                    isOnlyZerosAndOnes = false;
                    if (isHasNegative)
                        return make_pair(isOnlyZerosAndOnes, isHasNegative);
                }
            }
    return make_pair(isOnlyZerosAndOnes, isHasNegative);
}

pair<vector<int>, vector<int>> algorithmBFS(const vector<vector<int>> & matrix, int s) {
    int n = int(matrix.size());
    queue<int> q;
    q.push(s);
    vector<bool> used(n);
    vector<int> d(n), p(n);
    used[s] = true;
    p[s] = -1;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (size_t v = 0; v < matrix[u].size(); ++v) {
            int len = matrix[u][v];
            if (len != 0 && !used[v]) {
                used[v] = true;
                q.push(v);
                d[v] = d[u] + 1;
                p[v] = u;
            }
        }
    }
    return make_pair(d, p);
}

pair<vector<int>, vector<int>> algorithmDijkstra(const vector<vector<int>> & matrix, int s) {
    int n = int(matrix.size());
    vector<vector<int>> g = matrix;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j)
                g[i][i] = 0;
            else if (g[i][j] == 0)
                g[i][j] = INF;
        }
    }
    vector<int> d(n, INF), p(n, -1);
    d[s] = 0;
    priority_queue<pair<int, int>> q;
    q.push(make_pair(0, s));
    while (!q.empty()) {
        int u = q.top().second, curD = -q.top().first;
        q.pop();
        if (curD > d[u])
            continue;
        for (int v = 0; v < n; ++v) {
            if (u == v)
                continue;
            int len = g[u][v];
            if (d[u] + len < d[v]) {
                d[v] = d[u] + len;
                p[v] = u;
                q.emplace(-d[v], v);
            }
        }
    }
    return make_pair(d, p);
}

struct Edge {
    Edge(int a, int b, int cost): a(a), b(b), cost(cost) {}
    int a, b, cost;
};

vector<Edge> toEdges(const vector<vector<int>> & matrix) {
    int n = int(matrix.size());
    vector<Edge> edges;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (matrix[i][j] != 0)
                edges.emplace_back(i, j, matrix[i][j]);
    return edges;
}

pair<vector<int>, vector<int>> algorithmBellmanFord(const vector<vector<int>> & matrix, int s) {
    vector<Edge> e = toEdges(matrix);
    int n = int(matrix.size());
    int m = int(e.size());
    vector<int> d (n, INF);
    d[s] = 0;
    vector<int> p (n, -1);
    for (int k = 1; k < n; ++k) {
        for (int i = 0; i < m; ++i) {
            if (d[e[i].b] > d[e[i].a] + e[i].cost) {
                d[e[i].b] = max(-INF, d[e[i].a] + e[i].cost);
                p[e[i].b] = e[i].a;
            }
        }
    }
    for (int i = 0; i < m; ++i) {
        if (d[e[i].b] > d[e[i].a] + e[i].cost) {
            d[e[i].b] = -INF;
            p[e[i].b] = e[i].a;
        }
    }
    return make_pair(d, p);
}

pair<vector<int>, vector<int>> variantTask(const vector<vector<int>> & matrix, int s) {
    pair<bool, bool> flags = getFlags(matrix);
    bool isOnlyZerosAndOnes = flags.first, isHasNegative = flags.second;
    if (isOnlyZerosAndOnes)
        return algorithmBFS(matrix, s);
    else if (!isHasNegative)
        return algorithmDijkstra(matrix, s);
    return algorithmBellmanFord(matrix, s);
}

void pressAnyKeyForContinue() noexcept {
    cout << "Press any key for continue..." << flush;
    getch();
}

string toString(const vector<vector<int>> & matrix) {
    int n = int(matrix.size());
    vector<int> columnsSizes(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            columnsSizes[j] = max(columnsSizes[j], int(to_string(matrix[i][j]).size()));
    stringstream result;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            result << setw(columnsSizes[j]) << matrix[i][j] << (j == n - 1 ? "" : " ");
        result << (i == n - 1 ? "" : "\n");
    }
    return result.str();
}

string getCurrentDateTime(const string& s) {
    time_t now = time(nullptr);
    struct tm timeStruct{};
    char  buf[80];
    timeStruct = *localtime(&now);
    if(s == "now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &timeStruct);
    else if(s == "date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &timeStruct);
    return buf;
}

void log(const char * filePath, const string& logMsg) {
    string now = getCurrentDateTime("now");
    ofstream ofs(filePath, std::ios_base::out | std::ios_base::app );
    if (!ofs.is_open())
        ofs = ofstream("errors.txt", std::ios_base::out | std::ios_base::app);
    ofs << now << '\t' << logMsg << '\n';
    ofs.close();
}

int main(int argc, char * argv[]) {
    if (argc != 4) {
        cerr << "[Utility format]: BasicGraphAlgorithms input.txt output.txt error.txt";
        return 1;
    }

    vector<vector<int>> matrix;
    while (true) {
        try {
            system("cls");
            cout << "1) variant task\n"
                 << "2) read matrix from file\n"
                 << "3) print matrix\n"
                 << "4) write matrix to file\n"
                 << "0) exit\n"
                 << "Select item >" << flush;
            key_reading:
            char key = char(getch());
            if (key == '0')
                break;
            if (key == '1') { // opinion task
                system("cls");
                auto newMatrix = readMatrix(argv[1]);
                int n = int(newMatrix.size());
                cout << "Enter first vertex (from 1 to " << n << "): " << flush;
                int s;
                cin >> s;
                if (s < 1 || s > n)
                    throw BadVertexNumberException(s, 1, n);
                --s;
                cout << "Enter maximal route length: " << flush;
                int maxRouteLength;
                cin >> maxRouteLength;
                auto result = variantTask(newMatrix, s);
                vector<int> & d = result.first, & p = result.second;
                vector<pair<int, vector<int>>> distancesAndRoutes;
                for (int i = 0; i < n; ++i) {
                    if (d[i] == -INF)
                        distancesAndRoutes.emplace_back(d[i], vector<int>(1, i));
                    else if (i != s && d[i] != INF && d[i] <= maxRouteLength) {
                        vector<int> route;
                        for (int j = i; j != s; j = p[j]) {
                            route.push_back(j);
                            if (route.size() > n) {
                                route.clear();
                                break;
                            }
                        }
                        if (route.empty())
                            route.push_back(i);
                        else
                            route.push_back(s);
                        reverse(route.begin(), route.end());
                        distancesAndRoutes.emplace_back(d[i], route);
                    }
                }
                if (distancesAndRoutes.empty())
                    cout << "Routes not founded\n";
                else {
                    cout << "Routes:\n";
                    for (auto & distanceAndRoute : distancesAndRoutes) {
                        auto & route = distanceAndRoute.second;
                        int v = route.back(), distance = distanceAndRoute.first;
                        if (distance < -INF)
                            distance = -INF;
                        else if (distance > INF)
                            distance = INF;
                        cout << "Final vertex: " << (v + 1)
                             << ", route length: " << (distance == INF ? "INF" : (distance == -INF ? "-INF" : to_string(distance)));
                        if (distance == -INF) {
                            cout << "\n";
                            continue;
                        }
                        cout << ", route: ";
                        for (int i = 0; i < route.size(); ++i)
                            cout << (route[i] + 1) << (i != route.size() - 1 ? " -> " : "\n");
                    }
                }
                matrix = newMatrix;
                pressAnyKeyForContinue();
            } else if (key == '2') { // read matrix from file
                system("cls");
                cout << "Enter filename with matrix: " << flush;
                string path;
                cin >> path;
                matrix = readMatrix(path);
            } else if (key == '3') { // print matrix
                system("cls");
                cout << "Matrix:\n" << toString(matrix) << "\n";
                pressAnyKeyForContinue();
            } else if (key == '4') { // write matrix to file
                system("cls");
                cout << "Enter filename for matrix writing: " << flush;
                string path;
                cin >> path;
                ofstream fout(path);
                if (!fout.is_open())
                    throw FileNotFoundException(path);
            } else
                goto key_reading;
        } catch (runtime_error & exc) {
            log(argv[3], exc.what());
            cout << exc.what() << endl;
            pressAnyKeyForContinue();
        }
    }
    return 0;
}