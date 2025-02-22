#include <algorithm>
#include <bitset>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Station;
struct Road;
struct Good;

// 站点
struct Station {
  Station(const std::string &_strID, const int &_ID, const int &_pickers)
      : strID(_strID), ID(_ID), pickers(_pickers), leftPickers(_pickers) {}
  void Print() {
    std::cerr << "@ Station: (";
    std::cerr << strID << ", " << ID << ", " << pickers << ", " << leftPickers
              << ")\n";
  }
  std::string strID;  // 站点ID
  int ID;             // 映射ID
  int pickers;        // 拣货员数量
  int leftPickers;    // 剩余捡货员
};
// 轨道
struct Road {
  Road(const std::string &_strID, const int &_ID, Station *_station0,
       Station *_station1)
      : strID(_strID),
        ID(_ID),
        station0(_station0),
        station1(_station1),
        loadedWeight(std::vector<double>(80, 0)) {}
  void Print() {
    std::cerr << "@ Road: (";
    std::cerr << strID << ", " << ID << ") {";
    std::cerr << station0->strID << ", " << station1->strID << "}\n";
  }
  std::bitset<80> &GetPickerStatus(const Station *sta) {
    if (sta->ID == station0->ID) return pickerStatusFrom;
    return pickerStatusTo;
  }
  std::bitset<80> &GetPickerStatus(const int &ID) {
    if (ID == station0->ID) return pickerStatusFrom;
    return pickerStatusTo;
  }
  void SubPicker(const int &ID, int pos) {
    if (ID == station0->ID) {
      if (!pickerStatusFrom[pos]) {
        --station0->leftPickers;
        pickerStatusFrom[pos] = 1;
      }
    } else {
      if (!pickerStatusTo[pos]) {
        --station1->leftPickers;
        pickerStatusTo[pos] = 1;
      }
    }
  }

  std::string strID;                 // 轨道ID
  int ID;                            // 映射ID
  Station *station0;                 // 连接站点0
  Station *station1;                 // 连接站点1
  std::bitset<80> pickerStatusFrom;  // station0的捡货员状态
  std::bitset<80> pickerStatusTo;    // station1的捡货员状态
  std::vector<double> loadedWeight;  // 每个车道已装载重量
};
// 货物
struct Good {
  Good(const std::string &_strID, const int &_ID, Station *_start,
       Station *_end, const double &_quality)
      : strID(_strID), ID(_ID), start(_start), end(_end), quality(_quality) {}
  void Print() {
    std::cerr << "@ Good: (";
    std::cerr << strID << ", " << ID << ", " << quality << ") (";
    std::cerr << start->strID << ", " << end->strID << ") {";
    for (int i = 0; i < requiredStations.size(); ++i) {
      if (i != 0) std::cerr << ", ";
      std::cerr << requiredStations[i]->strID;
    }
    std::cerr << "}\n";
    if (!roadPath.empty()) {
      std::cerr << "- Path: ";
      for (int i = 0; i < roadPath.size(); ++i) {
        if (i != 0) std::cerr << ", ";
        std::cerr << roadPath[i]->strID;
      }
      std::cerr << "\n- Train: ";
      for (int i = 0; i < lanePath.size(); ++i) {
        if (i != 0) std::cerr << ", ";
        std::cerr << lanePath[i];
      }
      std::cerr << "\n";
    }
  }

  std::string strID;                        // 货物ID
  int ID;                                   // 映射ID
  Station *start;                           // 起始站点
  Station *end;                             // 终止站点
  double quality;                           // 货物质量
  std::vector<Station *> requiredStations;  // 必经站点
  std::vector<Road *> roadPath;             // 站点路径
  std::vector<int> lanePath;                // 列车号路径
  std::vector<double> weight;               // 每个列车已经装载的容量
};

class XJBG {
 public:
  void Print();               // 打印调试信息
  void LoadData();            // 加载数据
  void BuildGraph();          // 构建图
  void ArrangePath();         // 安排路径
  bool Dijkstra(Good *good);  // 找路
  void OuputAnswer();         // 输出答案

 private:
  void splitString(const std::string &s, std::vector<std::string> &v,
                   const std::string &c);  // 分割字符串

 private:
  std::unordered_map<std::string, Station *> HashStation;  // 站点映射
  std::unordered_map<std::string, Road *> HashRoad;        // 道路映射

 private:
  int m_StationNum;                   // 站点数目
  int m_RoadNum;                      // 道路数目
  int m_LaneNum;                      // 车道数目
  double m_Capacity;                  // 单个列车容量
  int m_GoodNum;                      // 货物数目
  int m_Pickers;                      // 总捡货员
  std::vector<Station *> m_stations;  // 站点
  std::vector<Road *> m_roads;        // 轨道
  std::vector<Good *> m_goods;        // 货物
 private:
  std::vector<std::vector<std::pair<Station *, Road *>>> Graph;  // 双向地图
  std::vector<std::vector<Road *>> RoadTable;                    // 轨道
};

void XJBG::Print() {
#ifdef TEST
  std::cerr << "-------------------------------------------\n";
  std::cerr << "@ 站点: " << m_StationNum << "\n";
  std::cerr << "@ 轨道: " << m_RoadNum << "\n";
  std::cerr << "@ 列车: " << m_LaneNum << "\n";
  std::cerr << "@ 容量: " << m_Capacity << "\n";
  std::cerr << "@ 货物: " << m_GoodNum << "\n";
  std::cerr << "@ 捡货员: " << m_Pickers << "\n";
  std::cerr << "-------------------------------------------\n";
#endif
}

void XJBG::splitString(const std::string &s, std::vector<std::string> &v,
                       const std::string &c) {
  v.clear();
  std::string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  while (std::string::npos != pos2) {
    v.push_back(s.substr(pos1, pos2 - pos1));
    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if (pos1 != s.length()) v.push_back(s.substr(pos1));
}

void XJBG::LoadData() {
  std::string line;
  std::cin >> line;
  char c;
  std::stringstream ss(line);
  ss >> m_StationNum >> c >> m_RoadNum >> c >> m_LaneNum >> c >> m_Capacity;
  std::vector<std::string> info;
  for (int i = 0; i < m_StationNum; ++i) {
    std::cin >> line;
    this->splitString(line, info, ",");
    int y = std::stoi(info[1]);
    Station *station = new Station(info[0], i, y);
    m_stations.emplace_back(station);
    HashStation[station->strID] = station;
    m_Pickers += y;
  }
  for (int i = 0; i < m_RoadNum; ++i) {
    std::cin >> line;
    this->splitString(line, info, ",");
    Road *road =
        new Road(info[0], i, HashStation[info[1]], HashStation[info[2]]);
    m_roads.emplace_back(road);
    HashRoad[road->strID] = road;
  }
  std::cin >> line;
  m_GoodNum = std::stoi(line);
  for (int i = 0; i < m_GoodNum; ++i) {
    std::cin >> line;
    this->splitString(line, info, ",");
    Good *good = new Good(info[0], i, HashStation[info[1]],
                          HashStation[info[2]], std::stod(info[3]));
    if (info[4] != "null") {
      for (int j = 4; j < info.size(); ++j) {
        good->requiredStations.emplace_back(HashStation[info[j]]);
      }
    }
    m_goods.emplace_back(good);
  }
  this->Print();
}

void XJBG::BuildGraph() {
  Graph.resize(m_stations.size());
  RoadTable = std::vector<std::vector<Road *>>(
      m_stations.size(), std::vector<Road *>(m_stations.size()));
  for (auto &it : m_roads) {
    int u = it->station0->ID, v = it->station1->ID;
    Graph[u].emplace_back(std::make_pair(it->station1, it));
    Graph[v].emplace_back(std::make_pair(it->station0, it));
    RoadTable[u][v] = RoadTable[v][u] = it;
  }
  std::sort(m_goods.begin(), m_goods.end(),
            [&](const Good *g1, const Good *g2) {
              return g1->quality > g2->quality;
            });
}

void XJBG::OuputAnswer() {
  int failed = 0;
  double failedSum = 0;
  for (auto &it : m_goods) {
    if (it->roadPath.empty()) {
      ++failed;
      failedSum += it->quality;
    }
  }
  std::cout << failed << "," << failedSum << "\n";
  for (auto &it : m_goods) {
    std::cout << it->strID << "\n";
    if (it->roadPath.empty()) {
      std::cout << "null\nnull\n";
    } else {
      for (int i = 0; i < it->roadPath.size(); ++i) {
        if (i != 0) std::cout << ",";
        std::cout << it->roadPath[i]->strID;
      }
      std::cout << "\n";
      for (int i = 0; i < it->lanePath.size(); ++i) {
        if (i != 0) std::cout << ",";
        std::cout << it->lanePath[i] + 1;
      }
      std::cout << "\n";
    }
  }
}

bool XJBG::Dijkstra(Good *good) {
  struct Node {
    Station *station;  // 站点
    int dis;           // 路径长度
    int left;          // 总共剩多少picker
    int last;          // 上次个节点是否使用picker
    bool operator<(const Node &r) const {
      if (dis == r.dis) return left < r.left;
      return dis > r.dis;
    }
  };

  std::priority_queue<Node> Q;
  std::vector<bool> vis(m_StationNum, false);
  std::vector<int> Dis(m_StationNum, 1000000);
  std::vector<int> Left(m_StationNum, 0);
  int pre[m_StationNum][2];

  for (int i = 0; i < m_LaneNum; ++i) {
    Q.push(Node{good->start, 0, good->start->leftPickers, 0});
  }
  Dis[good->start->ID] = 0;
  Left[good->start->ID] = good->start->leftPickers;

  auto judge = [&](int u, int v, int d) {
    if (Dis[u] + 1 < Dis[v]) {
      Dis[v] = Dis[u] + 1;
      Left[v] = Left[u] + d;
      return true;
    }
    if (Dis[u] + 1 == Dis[v] && Left[u] + d > Left[v]) {
      Left[v] = Left[u] + d;
      return true;
    }
    return false;
  };

  bool ok = false;
  while (!Q.empty()) {
    auto head = Q.top();
    Q.pop();
    if (head.station->ID == good->end->ID) {
      ok = true;
      break;
    }
    if (vis[head.station->ID]) continue;
    vis[head.station->ID] = true;

    int u = head.station->ID;
    for (auto &it : Graph[head.station->ID]) {
      Station *nxtsta = it.first;
      Road *nxtroad = it.second;
      int v = nxtsta->ID;

      if (vis[v]) continue;
      const auto &pickFrom = nxtroad->GetPickerStatus(head.station);
      const auto &pickTo = nxtroad->GetPickerStatus(nxtsta);

      for (int i = 0; i < m_LaneNum; ++i) {
        if (nxtroad->loadedWeight[i] + good->quality > m_Capacity) continue;
        if (pickTo[i] && pickFrom[i] && judge(u, v, nxtsta->leftPickers)) {
          pre[nxtsta->ID][0] = head.station->ID;
          pre[nxtsta->ID][1] = i;
          Q.push(Node{nxtsta, Dis[v], Left[v], 0});
          continue;
        }
        if (!pickFrom[i] && head.station->leftPickers - head.last < 1) continue;
        if (!pickTo[i] && nxtsta->leftPickers < 1) continue;
        if (pickTo[i]) {
          if (judge(u, v, nxtsta->leftPickers)) {
            pre[nxtsta->ID][0] = head.station->ID;
            pre[nxtsta->ID][1] = i;
            Q.push(Node{nxtsta, Dis[v], Left[v], 0});
          }
        } else if (judge(u, v, nxtsta->leftPickers - 1)) {
          pre[nxtsta->ID][0] = head.station->ID;
          pre[nxtsta->ID][1] = i;
          Q.push(Node{nxtsta, Dis[v], Left[v], 1});
        }
      }
    }
  }
  if (!ok) return false;
  int cur = good->end->ID;
  do {
    int now = pre[cur][0];
    int lane = pre[cur][1];
    Road *road = RoadTable[now][cur];
    road->loadedWeight[lane] += good->quality;
    if (road->loadedWeight[lane] > m_Capacity) {
      std::cerr << "Error OverLoad! " << road->loadedWeight[lane] << "\n";
    }
    road->SubPicker(cur, lane);
    road->SubPicker(now, lane);
    good->roadPath.emplace_back(road);
    good->lanePath.emplace_back(lane);
    cur = now;
  } while (cur != good->start->ID);
  return true;
}

void XJBG::ArrangePath() {
  int count = 0;
  for (auto &good : m_goods) {
    if (!good->requiredStations.empty()) continue;
    if (this->Dijkstra(good)) {
      ++count;
    }
  }
#ifdef TEST
  for (auto &it : m_stations) {
    if (it->leftPickers < 0) {
      std::cerr << "Error: ";
      it->Print();
      break;
    }
  }
  std::cerr << "@ dijkstra count: " << count << "\n";
#endif
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cerr << std::fixed << std::setprecision(3);
  std::cout << std::fixed << std::setprecision(3);

  XJBG *xjbg = new XJBG;
  xjbg->LoadData();
  xjbg->BuildGraph();
  xjbg->ArrangePath();
  xjbg->OuputAnswer();

  return 0;
}