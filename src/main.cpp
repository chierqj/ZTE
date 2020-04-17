#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// 站点
struct Station {
  std::string strID;  // 站点ID
  int ID;             // 映射ID
  int pickers;        // 拣货员数量
  void Print() {
    std::cerr << "@ Station: (";
    std::cerr << strID << ", " << ID << ", " << pickers << ")\n";
  }
};
// 轨道
struct Track {
  std::string strID;  // 轨道ID
  int ID;             // 映射ID
  Station *station0;  // 连接站点0
  Station *station1;  // 连接站点1
  void Print() {
    std::cerr << "@ Track: (";
    std::cerr << strID << ", " << ID << ") {";
    std::cerr << station0->strID << ", " << station1->strID << "}\n";
  }
};
// 货物
struct Good {
  std::string strID;                        // 货物ID
  int ID;                                   // 映射ID
  Station *start;                           // 起始站点
  Station *end;                             // 终止站点
  double quality;                           // 货物质量
  std::vector<Station *> requiredStations;  // 必经站点
  void Print() {
    std::cerr << "@ Good: (";
    std::cerr << strID << ", " << ID << ", " << quality << ") (";
    std::cerr << start->strID << ", " << end->strID << ") {";
    for (int i = 0; i < requiredStations.size(); ++i) {
      if (i != 0) std::cerr << ", ";
      std::cerr << requiredStations[i]->strID;
    }
    std::cerr << "}\n";
  }
};

class XJBG {
 public:
  void LoadData();
  void Print();

 private:
  std::unordered_map<std::string, int> HashStation;  // 站点映射
  std::unordered_map<std::string, int> HashTrack;    // 轨道映射

 private:
  int m_stationNum;                   // 站点数目
  int m_trackNum;                     // 轨道数目
  int m_trainNum;                     // 列车数目
  int m_capacity;                     // 单个列车容量
  int m_goodNum;                      // 货物数目
  std::vector<Station *> m_stations;  // 站点
  std::vector<Track *> m_tracks;      // 轨道
  std::vector<Good *> m_goods;        // 货物
};

void XJBG::Print() {
  std::cerr << "-------------------------------------------\n";
  std::cerr << "@ 站点: " << m_stationNum << "\n";
  std::cerr << "@ 轨道: " << m_trackNum << "\n";
  std::cerr << "@ 列车: " << m_trainNum << "\n";
  std::cerr << "@ 容量: " << m_capacity << "\n";
  std::cerr << "@ 货物: " << m_goodNum << "\n";
  for (int i = 0; i < 5; ++i) m_stations[i]->Print();
  for (int i = 0; i < 5; ++i) m_tracks[i]->Print();
  for (int i = 0; i < 5; ++i) m_goods[i]->Print();
  std::cerr << "-------------------------------------------\n";
}

void XJBG::LoadData() {
  std::string line;
  std::getline(std::cin, line);
  std::stringstream ss(line);
  char c;
  ss >> m_stationNum >> c >> m_trackNum >> c >> m_trainNum >> c >> m_capacity;
  for (int i = 0; i < m_stationNum; ++i) {
    std::getline(std::cin, line);
    line = line.substr(0, line.size() - 1);
    int pos = line.find(',');
    std::string x = line.substr(0, pos);
    int y = std::stoi(line.substr(pos + 1, line.size() - pos - 1));
    Station *station = new Station;
    station->strID = x;
    station->ID = i;
    station->pickers = y;
    m_stations.emplace_back(station);
    HashStation[station->strID] = station->ID;
  }
  for (int i = 0; i < m_trackNum; ++i) {
    std::getline(std::cin, line);
    line = line.substr(0, line.size() - 1);
    int pos = line.find(',');
    std::string x = line.substr(0, pos);
    line = line.substr(pos + 1, line.size() - pos - 1);
    pos = line.find(',');
    std::string y = line.substr(0, pos);
    std::string z = line.substr(pos + 1, line.size() - pos - 1);
    Track *track = new Track;
    track->strID = x;
    track->ID = i;
    track->station0 = m_stations[HashStation[x]];
    track->station1 = m_stations[HashStation[y]];
    m_tracks.emplace_back(track);
    HashTrack[track->strID] = track->ID;
  }
  std::getline(std::cin, line);
  m_goodNum = std::stoi(line);
  for (int i = 0; i < m_goodNum; ++i) {
    std::getline(std::cin, line);
    line = line.substr(0, line.size() - 1);
    int pos = line.find(',');
    std::string id = line.substr(0, pos);
    line = line.substr(pos + 1, line.size() - pos - 1);
    pos = line.find(',');
    std::string st = line.substr(0, pos);
    line = line.substr(pos + 1, line.size() - pos - 1);
    pos = line.find(',');
    std::string ed = line.substr(0, pos);
    line = line.substr(pos + 1, line.size() - pos - 1);
    pos = line.find(',');
    double x = std::stoi(line.substr(0, pos));
    line = line.substr(pos + 1, line.size() - pos - 1);
    std::vector<Station *> vt;
    while (true) {
      pos = line.find(',');
      if (pos == std::string::npos) {
        if (line != "null") {
          vt.emplace_back(m_stations[HashStation[line]]);
        }
        break;
      }
      std::string s = line.substr(0, pos);
      vt.emplace_back(m_stations[HashStation[s]]);
      line = line.substr(pos + 1, line.size() - pos - 1);
    }
    Good *good = new Good;
    good->strID = id;
    good->ID = i;
    good->start = m_stations[HashStation[st]];
    good->end = m_stations[HashStation[ed]];
    good->quality = x;
    good->requiredStations = vt;
    m_goods.emplace_back(good);
  }
  this->Print();
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cerr << std::fixed << std::setprecision(3);
  std::cout << std::fixed << std::setprecision(3);

  XJBG *xjbg = new XJBG;
  xjbg->LoadData();

  return 0;
}