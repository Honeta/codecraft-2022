#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

vector<string> client_name, site_name;
vector<vector<int>> demand;
vector<int> bandwidth;
int qos[35][135], qos_limit, ans[8928][35][135];

void input();
void output();

int main() {
    input();
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
    while (ifs >> s) {
        demand.emplace_back();
        for (auto &ch : s)
            if (ch == ',')
                ch = ' ';
        ss << s;
        ss >> tmp;
        while (ss >> tmp)
            demand.back().push_back(atoi(tmp.c_str()));
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
    ifs.close();
}

void output() {
    ofstream ofs;
    ofs.open("./output/solution.txt");
    for (int t = 0; t < demand.size(); ++t)
        for (int i = 0; i < client_name.size(); ++i) {
            ofs << client_name[i] << ':';
            for (int j = 0; j < site_name.size(); ++j)
                if (ans[t][i][j]) {
                    if (j) ofs << ',';
                    ofs << '<' << site_name[j] << ',' << ans[t][i][j] << '>';
                }
            ofs << endl;
        }
}