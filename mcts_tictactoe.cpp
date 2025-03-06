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
struct TestRow
{
  char v[N];
  char &operator[](const int &i) { return v[i]; }
  const char &operator[](const int &i) const { return v[i]; }
  bool operator==(const TestRow &x) const
  {
    for (int i = 0; i < N; i++)
      if (v[i] != x.v[i])
        return 0;
    return 1;
  }
  bool operator!=(const TestRow &x) const
  {
    for (int i = 0; i < N; i++)
      if (v[i] != x.v[i])
        return 1;
    return 0;
  }
};
struct TestBoard
{
  TestRow v[N];
  TestRow &operator[](const int &i) { return v[i]; }
  const TestRow &operator[](const int &i) const { return v[i]; }
  bool operator==(const TestBoard &x) const
  {
    for (int i = 0; i < N; i++)
      if (v[i] != x.v[i])
        return 0;
    return 1;
  }
  bool operator!=(const TestBoard &x) const
  {
    for (int i = 0; i < N; i++)
      if (v[i] != x.v[i])
        return 1;
    return 0;
  }
};
bool check(const TestBoard &board, char player)
{
  int cnt, cnt2, cnt3 = 0, cnt4 = 0;
  for (int i = 0; i < N; i++)
  {
    cnt = 0, cnt2 = 0;
    for (int j = 0; j < N; j++)
    {
      cnt += (board[i][j] == player);
      cnt2 += (board[j][i] == player);
    }
    cnt3 += board[i][i] == player;
    cnt4 += board[i][N - i - 1] == player;
    if (cnt >= thresh || cnt2 >= thresh)
      return 1;
  }
  return cnt3 >= thresh || cnt4 >= thresh;
}
int side(const TestBoard &board)
{
  int x_count = 0, o_count = 0;
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      x_count += (board[i][j] == 'X');
      o_count += (board[i][j] == '0');
    }
  }
  int pl = ((x_count - o_count == 1) ? -1 : 1);
  return pl;
}
int status(const TestBoard &board)
{
  int nf_count = 0;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      nf_count += board[i][j] == '.';
  if (check(board, 'X'))
    return 1;
  if (check(board, '0'))
    return -1;
  else
  {
    if (nf_count)
      return 2;
    else
      return 0;
  }
}
vector<pair<int, int>> gen(const TestBoard &board)
{
  if (status(board) != 2)
    return {};
  vector<pair<int, int>> vaild;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      if (board[i][j] != 'X' && board[i][j] != '0')
        vaild.push_back({i, j});
  return vaild;
}
int randgame(TestBoard board)
{
  char pl = (side(board) == 1 ? 'X' : '0');
  vector<pair<int, int>> moves = gen(board);
  shuffle(moves.begin(), moves.end(), rng);
  while (status(board) == 2)
  {
    auto move2 = moves.back();
    board[move2.first][move2.second] = pl;
    pl = (pl == '0' ? 'X' : '0');
    moves.pop_back();
  }
  return status(board);
}
struct chash
{
  uint64_t operator()(int x) const noexcept
  {
    // splitmix64
    uint64_t z = (x += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
  }
};
gp_hash_table<int, TestBoard, chash> boards;
gp_hash_table<int, vector<int>, chash> adj;
gp_hash_table<int, vector<pair<int, int>>, chash> adj2;
gp_hash_table<int, pair<double, double>, chash> nodes_vals;
gp_hash_table<int, bool, chash> ck;
int cur_id = 0, old_cur_id = 0;
void init(int board_id)
{
  TestBoard board = boards[board_id];
  vector<pair<int, int>> moves = gen(board);
  int x_count = 0, o_count = 0;
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      x_count += (board[i][j] == 'X');
      o_count += (board[i][j] == '0');
    }
  }
  char pl = ((x_count - o_count == 1) ? '0' : 'X');
  if (!ck[board_id])
  {
    for (auto &i : moves)
    {
      char last = board[i.first][i.second];
      board[i.first][i.second] = pl;
      adj[board_id].push_back(++cur_id);
      adj2[board_id].push_back(i);
      boards[cur_id] = board;
      board[i.first][i.second] = last;
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
pair<int, int> bestmove(int board)
{
  vector<pair<pair<int, int>, double>> res;
  for (auto &i : adj[board])
    res.push_back({adj2[board][&i - &adj[board][0]], (double)1 - nodes_vals[i].first / nodes_vals[i].second});
  sort(res.begin(), res.end(),
       [](pair<pair<int, int>, double> a, pair<pair<int, int>, double> b)
       {
         return a.second > b.second;
       });
  return res[0].first;
}
void do_simulation(int board)
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
    auto best = bestmove(pos_stack[0]);
    cout << "depth " << depth << " nps "
         << (int)((double)(cur_id - old_cur_id) * 1.0e9 /
                  chrono::duration_cast<chrono::nanoseconds>(Clock.now() -
                                                             start)
                      .count())
         << " nodes " << cur_id << " bestmove ";
    cout << best.first << " " << best.second;
    cout << " score "
         << (int)(log(((double)nodes_vals[pos_stack[0]].first /
                       (nodes_vals[pos_stack[0]].second -
                        nodes_vals[pos_stack[0]].first))) /
                  log(1.5) * 100 * side(boards[pos_stack[0]]))
         << endl;
  }
  depth = max(depth, pos_stack.size() - 1);
  int randres = randgame(boards[board]);
  for (auto &i : pos_stack)
  {
    if (randres == 0)
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
      cin >> board[i][j];
  int id = 0;
  boards[0] = board;
  char pl = (side(board) == 1 ? 'X' : '0');
  while (status(board) == 2)
  {
    depth = 1;
    start = Clock.now();
    init(id);
    for (cntsims = 0; cntsims < k; cntsims++)
      do_simulation(id);
    int nps =
        (double)(cur_id - old_cur_id) * 1.0e9 /
        chrono::duration_cast<chrono::nanoseconds>(Clock.now() - start).count();
    auto best = bestmove(id);
    cout << "depth " << depth << " nps " << nps << " nodes " << cur_id
         << " bestmove ";
    cout << best.first << " " << best.second;
    cout << " score "
         << (int)(log(((double)nodes_vals[id].first /
                       (nodes_vals[id].second -
                        nodes_vals[id].first))) /
                  log(1.5) * 100 * side(boards[id]))
         << endl;
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
  }
  int res = status(board);
  if (res == 1)
  {
    cout << "The first player won" << endl;
  }
  else if (res == -1)
  {
    cout << "The second player won" << endl;
  }
  else
  {
    cout << "Draw" << endl;
  }
}