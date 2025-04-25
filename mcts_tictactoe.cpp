#include <algorithm>
#include <chrono>
#include <cmath>
#include <ext/pb_ds/assoc_container.hpp>
#include <iostream>
#include <map>
#include <random>
#include <vector>
using namespace std;
using namespace __gnu_pbds;
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
const uint64_t RANDOM = rng();
#ifdef SZ
const int N = SZ;
#else
const int N = 3;
#endif
#ifdef THRESH
const int thresh = THRESH;
#else
const int thresh = N;
#endif
int k;
size_t cntsims = 0;
size_t depth = 1;
chrono::high_resolution_clock Clock;
auto start = Clock.now();
const int BSZ = (N * N + 3) / 4;
struct TestBoard
{
  unsigned char v[BSZ]{};
  inline int get(int i, int j) const
  {
    i = i * N + j;
    return (v[i >> 2] >> (2 * (i & 3))) & 3;
  }
  inline void set(int i, int j, int k)
  {
    const int o = get(i, j);
    i = i * N + j;
    v[i >> 2] += (k - o) * (1 << (2 * (i & 3)));
  }
  bool operator==(const TestBoard &x) const
  {
    for (int i = 0; i < BSZ; i++)
      if (v[i] != x.v[i])
        return 0;
    return 1;
  }
  bool operator!=(const TestBoard &x) const
  {
    for (int i = 0; i < BSZ; i++)
      if (v[i] != x.v[i])
        return 1;
    return 0;
  }
};
bool check(const TestBoard &board, int player)
{
  int mxcnt = 0, cnt;
  for (int i = 0; i < N; i++)
  {
    cnt = 0;
    for (int j = 0; j < N; j++)
    {
      if (board.get(i, j) == player)
        cnt++;
      else
        cnt = 0;
      mxcnt = max(mxcnt, cnt);
    }
  }
  for (int j = 0; j < N; j++)
  {
    cnt = 0;
    for (int i = 0; i < N; i++)
    {
      if (board.get(i, j) == player)
        cnt++;
      else
        cnt = 0;
      mxcnt = max(mxcnt, cnt);
    }
  }
  cnt = 0;
  for (int i = 0; i < N; i++)
  {
    if (board.get(i, i) == player)
      cnt++;
    else
      cnt = 0;
    mxcnt = max(mxcnt, cnt);
  }
  cnt = 0;
  for (int i = 0; i < N; i++)
  {
    if (board.get(i, N - i - 1) == player)
      cnt++;
    else
      cnt = 0;
    mxcnt = max(mxcnt, cnt);
  }
  return mxcnt >= thresh;
}
int side(const TestBoard &board)
{
  int x_count = 0, o_count = 0;
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      x_count += (board.get(i, j) == 1);
      o_count += (board.get(i, j) == 0);
    }
  }
  return ((x_count - o_count == 1) ? 0 : 1);
}
int status(const TestBoard &board)
{
  int nf_count = 0;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      nf_count += board.get(i, j) == 2;
  if (check(board, 1))
    return 1;
  if (check(board, 0))
    return 0;
  if (nf_count)
    return 2;
  return 3;
}
vector<pair<int, int>> gen(const TestBoard &board)
{
  if (status(board) != 2)
    return {};
  vector<pair<int, int>> vaild;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      if (board.get(i, j) != 1 && board.get(i, j) != 0)
        vaild.push_back({i, j});
  return vaild;
}
int randgame(TestBoard board)
{
  int pl = side(board);
  vector<pair<int, int>> moves = gen(board);
  shuffle(moves.begin(), moves.end(), rng);
  while (status(board) == 2)
  {
    auto move2 = moves.back();
    board.set(move2.first, move2.second, pl);
    pl ^= 1;
    moves.pop_back();
  }
  return status(board);
}
vector<TestBoard> boards(10000000);
vector<vector<int>> adj(10000000);
vector<vector<pair<int, int>>> adj2(10000000);
vector<pair<double, double>> nodes_vals(10000000);
vector<bool> ck(10000000);
int cur_id = 0, old_cur_id = 0;
void init(int board_id)
{
  TestBoard board = boards[board_id];
  vector<pair<int, int>> moves = gen(board);
  int pl = side(board);
  if (!ck[board_id])
  {
    for (auto &i : moves)
    {
      int last = board.get(i.first, i.second);
      board.set(i.first, i.second, pl);
      adj[board_id].push_back(++cur_id);
      adj2[board_id].push_back(i);
      boards[cur_id] = board;
      board.set(i.first, i.second, last);
    }
    ck[board_id] = 1;
  }
}
int max_uctpos(int board_id)
{
  double max_uctval = -999999999;
  vector<int> max_uctmoves;
  for (auto &id : adj[board_id])
  {
    double uctval =
        (nodes_vals[id].second ? (nodes_vals[id].second - nodes_vals[id].first) /
                                         nodes_vals[id].second +
                                     sqrt(2 * log(nodes_vals[board_id].second) / nodes_vals[id].second)
                               : INFINITY);
    if (uctval > max_uctval)
    {
      max_uctval = uctval;
      max_uctmoves = {id};
    }
    else if (uctval == max_uctval)
      max_uctmoves.push_back(id);
  }
  return max_uctmoves[rng() % max_uctmoves.size()];
}
pair<int, pair<int, int>> bestmove(int board)
{
  pair<double, pair<int, pair<int, int>>> res = {-1, {-1, {-1, -1}}};
  for (auto &i : adj[board])
    res = max(res, {(double)1 - nodes_vals[i].first / nodes_vals[i].second, {i, adj2[board][&i - &adj[board][0]]}});
  return res.second;
}
pair<int, vector<pair<int, int>>> pv(int board)
{
  vector<pair<int, int>> res;
  while (ck[board])
  {
    auto [nb, move] = bestmove(board);
    if (move == pair<int, int>{-1, -1})
      break;
    res.push_back(move);
    board = nb;
  }
  return {status(boards[board]), res};
}
void do_simulation(int board, int cntsims)
{
  vector<int> pos_stack;
  while (ck[board])
  {
    if (status(boards[board]) != 2)
      break;
    pos_stack.push_back(board);
    board = max_uctpos(board);
  }
  init(board);
  pos_stack.push_back(board);
  if (pos_stack.size() - 1 > depth)
  {
    cout << "depth " << pos_stack.size() - 1 << " nps "
         << (int)((double)(cur_id - old_cur_id) * 1.0e9 /
                  chrono::duration_cast<chrono::nanoseconds>(Clock.now() -
                                                             start)
                      .count())
         << " sps "
         << (int)((double)(cntsims) * 1.0e9 /
                  chrono::duration_cast<chrono::nanoseconds>(Clock.now() -
                                                             start)
                      .count())
         << " nodes " << cur_id << " pv ";
    auto [stat, PV] = pv(pos_stack[0]);
    for (auto &i : PV)
      cout << "(" << i.first << "," << i.second << ") ";
    cout << "score ";
    if (stat > 1)
      cout << (int)(log(((double)nodes_vals[pos_stack[0]].first /
                         (nodes_vals[pos_stack[0]].second -
                          nodes_vals[pos_stack[0]].first))) /
                    log(1.5) * 100 * (side(boards[pos_stack[0]]) ? 1 : -1))
           << endl;
    else
      cout << "#" << (stat ? 1 : -1) * (int)PV.size() << endl;
  }
  depth = max(depth, pos_stack.size() - 1);
  int randres = randgame(boards[board]);
  for (auto &i : pos_stack)
  {
    if (randres == 3)
      nodes_vals[i].first += 0.5;
    else if (side(boards[i]) == randres)
      nodes_vals[i].first += 1;
    nodes_vals[i].second += 1;
  }
}
int main()
{
  cin >> k;
  TestBoard board;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
    {
      char k;
      cin >> k;
      int val = (k == 'X' ? 1 : (k == '0' ? 0 : 2));
      board.set(i, j, val);
    }
  int id = 0;
  boards[0] = board;
  int pl = side(board);
  while (status(board) == 2)
  {
    adj.resize(adj.size() + k * N);
    adj2.resize(adj2.size() + k * N);
    nodes_vals.resize(nodes_vals.size() + k * N);
    boards.resize(boards.size() + k * N);
    ck.resize(ck.size() + k * N);
    depth = 0;
    start = Clock.now();
    init(id);
    for (cntsims = 1; cntsims <= k; cntsims++)
      do_simulation(id, cntsims);
    int nps =
        (double)(cur_id - old_cur_id) * 1.0e9 /
        chrono::duration_cast<chrono::nanoseconds>(Clock.now() - start).count();
    cout << "depth " << depth << " nps " << nps << " sps "
         << (int)((double)(cntsims) * 1.0e9 /
                  chrono::duration_cast<chrono::nanoseconds>(Clock.now() -
                                                             start)
                      .count())
         << " nodes " << cur_id
         << " pv ";
    auto [stat, PV] = pv(id);
    for (auto &i : PV)
      cout << "(" << i.first << "," << i.second << ") ";
    cout << "score ";
    if (stat > 1)
      cout << (int)(log(((double)nodes_vals[id].first /
                         (nodes_vals[id].second -
                          nodes_vals[id].first))) /
                    log(1.5) * 100 * (side(boards[id]) ? 1 : -1))
           << endl;
    else
      cout << "#" << (stat ? 1 : -1) * (int)PV.size() << endl;
    pair<int, int> move;
    cin >> move.first >> move.second;
    int cur_i = -1;
    for (int i = 0; i < adj2[id].size(); i++)
      if (move == adj2[id][i])
        cur_i = i;
    if (cur_i != -1)
    {
      id = adj[id][cur_i];
      board = boards[id];
    }
    else
    {
      cout << "Invaild move" << endl;
      exit(0);
    }
    old_cur_id = cur_id;
    adj.resize(cur_id);
    adj2.resize(cur_id);
    nodes_vals.resize(cur_id);
    boards.resize(cur_id);
    ck.resize(cur_id);
  }
  int res = status(board);
  if (res == 1)
  {
    cout << "The first player won" << endl;
  }
  else if (res == 0)
  {
    cout << "The second player won" << endl;
  }
  else
  {
    cout << "Draw" << endl;
  }
}