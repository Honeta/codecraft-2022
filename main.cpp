#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

vector<string> client_name, site_name;
vector<vector<int>> demand;
vector<int> bandwidth;
int qos[35][135], qos_limit, ans[8928][35][135];
vector<vector<int>> qos_after_select, demand_after_sort;

void input();
void output();
void choose_site();
void baseline();
void balance_very_slow();

int main() {
    input();
    //baseline();
    balance_very_slow();
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
bool cmp(int a, int b)
{
    return bandwidth[a] == bandwidth[b] ? a < b : bandwidth[a] > bandwidth[b];
}
void choose_site()
{
    for (int i = 0; i < client_name.size(); ++i)
    {
        qos_after_select.emplace_back();
        for (int j = 0; j < site_name.size(); ++j)
        {
            if(qos[i][j] < qos_limit)
            {
                qos_after_select.back().push_back(j);
            }
        }
        if(qos_after_select.back().size())
        {   
            sort(qos_after_select.back().begin(), qos_after_select.back().end(), cmp);
            //cerr<<qos_after_select.back().size()<<endl;
        }
        
    }
}

void baseline()
{
    for (int t = 0; t < demand.size(); ++t) {
        vector<int> bandwidth = ::bandwidth;
        for (int i = 0; i < client_name.size(); ++i) {
            for (int j = 0; j < site_name.size() && demand[t][i]; ++j)
                if (qos[i][j] < qos_limit && bandwidth[j] >= demand[t][i]) {
                    ans[t][i][j] = demand[t][i];
                    bandwidth[j] -= demand[t][i];
                    demand[t][i] = 0;
                } else if (qos[i][j] < qos_limit && bandwidth[j]) {
                    ans[t][i][j] = bandwidth[j];
                    demand[t][i] -= bandwidth[j];
                    bandwidth[j] = 0;
                }
            if (demand[t][i]) throw exception();
        }
    }
}

void balance_very_slow()
{
    choose_site();
    for (int t = 0; t < demand.size(); ++t)
    {
        vector<int> bandwidth = ::bandwidth;
        int flag = 1;
        while(flag)
        {
            flag = 0;
            for (int i = 0; i < client_name.size(); ++i)
            {
                if(demand[t][i] > 0)
                {
                    flag = 1;
                    int siz = qos_after_select[i].size();
                    for(int j = 0; j < siz; j++)
                    if(bandwidth[qos_after_select[i][j]] && demand[t][i])
                    {
                        bandwidth[qos_after_select[i][j]]--,demand[t][i]--;
                        ans[t][i][qos_after_select[i][j]]++;
                    }
                }
            }
        }
    }
}