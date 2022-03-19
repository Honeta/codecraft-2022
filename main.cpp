#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
using namespace std;

vector<string> client_name, site_name;
vector<vector<int>> demand;
vector<int> bandwidth;
int qos[35][135], qos_limit, ans[8928][35][135];

void input();
void debug();
void output();

int main() {
    input();
    vector<pair<int, int>> bandwidth;
    for (int i = 0; i < site_name.size(); ++i)
        bandwidth.push_back({i, ::bandwidth[i]});
    for (int t = 0; t < demand.size(); ++t) {
        vector<pair<int, int>> demand;
        for (int i = 0; i < client_name.size(); ++i)
            demand.push_back({i, ::demand[t][i]});
        sort(demand.begin(), demand.end(), [](pair<int, int> x, pair<int, int> y) { return x.second > y.second; });
        for (int i = 0; i < site_name.size(); ++i)
            bandwidth[i].second = ::bandwidth[bandwidth[i].first];
        random_shuffle(bandwidth.begin(), bandwidth.end());
        for (int i = 0; i < client_name.size(); ++i) {
            for (int j = 0; j < site_name.size() && demand[i].second; ++j)
                if (qos[demand[i].first][bandwidth[j].first] < qos_limit && bandwidth[j].second >= demand[i].second) {
                    ans[t][demand[i].first][bandwidth[j].first] = demand[i].second;
                    bandwidth[j].second -= demand[i].second;
                    demand[i].second = 0;
                } else if (qos[demand[i].first][bandwidth[j].first] < qos_limit && bandwidth[j].second) {
                    ans[t][demand[i].first][bandwidth[j].first] = bandwidth[j].second;
                    demand[i].second -= bandwidth[j].second;
                    bandwidth[j].second = 0;
                }
            if (demand[i].second) throw exception();
        }
    }
    debug();
    output();
    return 0;
}

void input() {
    ifstream ifs;
    stringstream ss;
    string s, tmp;

    // read demand.csv
    ifs.open("./data/demand.csv");
    ifs >> s;
    for (auto &ch : s)
        if (ch == ',')
            ch = ' ';
    ss << s;
    ss >> tmp;
    while (ss >> tmp)
        client_name.push_back(tmp);
    ss.clear();
    while (ifs >> s) {
        demand.emplace_back();
        for (auto &ch : s)
            if (ch == ',')
                ch = ' ';
        ss << s;
        ss >> tmp;
        while (ss >> tmp)
            demand.back().push_back(atoi(tmp.c_str()));
        ss.clear();
    }
    ifs.close();

    // read site_bandwidth.csv
    ifs.open("./data/site_bandwidth.csv");
    ifs >> s;
    while (ifs >> s) {
        for (auto &ch : s)
            if (ch == ',')
                ch = ' ';
        ss << s;
        ss >> tmp;
        site_name.push_back(tmp);
        ss >> tmp;
        bandwidth.push_back(atoi(tmp.c_str()));
        ss.clear();
    }
    ifs.close();

    // read qos.csv
    ifs.open("./data/qos.csv");
    ifs >> s;
    for (int i = 0; i < site_name.size(); ++i) {
        ifs >> s;
        for (auto &ch : s)
            if (ch == ',')
                ch = ' ';
        ss << s;
        ss >> tmp;
        for (int j = 0; j < client_name.size(); ++j) {
            ss >> tmp;
            qos[j][i] = atoi(tmp.c_str());
        }
        ss.clear();
    }
    ifs.close();

    // read config.ini
    ifs.open("./data/config.ini");
    ifs >> s;
    ifs >> s;
    for (auto &ch : s)
        if (ch == '=')
            ch = ' ';
    ss << s;
    ss >> tmp;
    ss >> qos_limit;
    ss.clear();
    ifs.close();
}

void output() {
    ofstream ofs;
    ofs.open("./output/solution.txt");
    for (int t = 0; t < demand.size(); ++t)
        for (int i = 0; i < client_name.size(); ++i) {
            if (i || t) ofs << endl;
            ofs << client_name[i] << ':';
            bool is_first = true;
            for (int j = 0; j < site_name.size(); ++j)
                if (ans[t][i][j]) {
                    if (!is_first) ofs << ',';
                    is_first = false;
                    ofs << '<' << site_name[j] << ',' << ans[t][i][j] << '>';
                }
        }
}

#include <cmath>

void debug() {
    ofstream ofs;
    ofs.open("./output/debug.txt");
    int sum_ans[135][8928] = {0}, sum95 = 0;
    for (int t = 0; t < demand.size(); ++t)
        for (int i = 0; i < client_name.size(); ++i)
            for (int j = 0; j < site_name.size(); ++j)
                sum_ans[j][t] += ans[t][i][j];
    for (int j = 0; j < site_name.size(); ++j) {
        sort(sum_ans[j], sum_ans[j] + demand.size());
        ofs << "site " << site_name[j] << " with bandwidth " << bandwidth[j] << " : ";
        for (int t = 0; t < demand.size(); ++t)
            ofs << sum_ans[j][t] << ' ';
        ofs << endl;
        sum95 += sum_ans[j][(int)ceil(0.95 * demand.size()) - 1];
    }
    ofs << "Total: " << sum95 << endl;
    ofs.close();
}