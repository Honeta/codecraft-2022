#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <queue>
#include <cstring>
#include <cmath>
using namespace std;

vector<string> client_name, site_name;
vector<vector<int>> demand;
vector<int> bandwidth;
int qos[35][135], qos_limit, ans[8928][35][135];
vector<int> ans_sum[135];

struct edge { int to, w, v, next; } e[10000];
int s = 0, t = 171, cnt;
int head[200], d[200], f[200], prep[200], pree[200];
bool vis[200];
queue<int> q;

void input();
void output();
void debug();
void init();
void add(int x, int y, int flow, int value);
bool spfa();

int main() {
    input();
    for (int k = 0; k < demand.size(); ++k) {
        init();
        for (int i = 0; i < client_name.size(); ++i) {
            add(0, i + 1, demand[k][i], 0);
            add(i + 1, 0, 0, 0);
        }
        for (int i = 0; i < client_name.size(); ++i)
            for (int j = 0; j < site_name.size(); ++j)
                if (qos[i][j] < qos_limit) {
                    add(i + 1, 36 + j, 0x7fffffff, 0);
                    add(36 + j, i + 1, 0, 0);
                }
        for (int j = 0; j < site_name.size(); ++j) {
            ans_sum[j].emplace_back();
            vector<int> ans_sum(::ans_sum[j]);
            ans_sum.resize(demand.size());
            sort(ans_sum.begin(), ans_sum.end());
            int sum95 = ans_sum.size() >= 20 ? ans_sum[ceil(0.95 * demand.size())] / 1000 : 0;
            add(36 + j, 171, bandwidth[j], sum95);
            add(171, 36 + j, 0, -sum95);
        }
        while (spfa()) {
            int now = t;
            while (now != s) {
                e[pree[now]].w -= f[t];
                e[pree[now] ^ 1].w += f[t];
                now = prep[now];
            }
        }
        for (int i = 0; i < client_name.size(); ++i)
            for (int j = head[i + 1]; j; j = e[j].next)
                if (!(j & 1)) {
                    ans[k][i][e[j].to - 36] = e[j ^ 1].w;
                    ans_sum[e[j].to - 36][k] += e[j ^ 1].w;
                }
    }
    output();
    debug();
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

void init() {
    memset(e, 0, sizeof(e));
    memset(head, 0, sizeof(head));
    memset(pree, 0, sizeof(pree));
    memset(prep, 0, sizeof(prep));
    cnt = 1;
}

void add(int x, int y, int flow, int value) {
    e[++cnt].next = head[x];
    head[x] = cnt;
    e[cnt].to = y;
    e[cnt].w = flow;
    e[cnt].v = value;
}

bool spfa() {
    memset(d, 0x3f, sizeof(d));
    memset(f, 0x3f, sizeof(f));
    memset(vis, 0, sizeof(vis));
    d[s] = 0;
    pree[t] = 0;
    q.push(s);
    while (q.size()) {
        int now = q.front();
        q.pop();
        vis[now] = 0;
        for (int i = head[now]; i; i = e[i].next)
            if (e[i].w && d[e[i].to] > d[now] + e[i].v) {
                d[e[i].to] = d[now] + e[i].v;
                f[e[i].to] = min(f[now], e[i].w);
                prep[e[i].to] = now;
                pree[e[i].to] = i;
                if (!vis[e[i].to]) {
                    vis[e[i].to] = 1;
                    q.push(e[i].to);
                }
            }
    }
    return pree[t];
}