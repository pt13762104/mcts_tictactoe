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
#ifdef SZ
const int N = SZ;
#else
const int N = 3;
#endif
int k;
size_t cntnodes = 0;
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
    if (cnt == N || cnt2 == N)
      return 1;
  }
  return cnt3 == N || cnt4 == N;
}
int side(const TestBoard &board)
{
  int x_count = 0, o_count = 0;
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      if (board[i][j] == 'X')
      {
        x_count++;
      }
      else if (board[i][j] == '0')
      {
        o_count++;
      }
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
  {
    return 1;
  }
  if (check(board, '0'))
  {
    return -1;
  }
  else
  {
    if (nf_count)
      return 2;
    else
    {
      return 0;
    }
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
void shuffle(vector<pair<int, int>> &p)
{
  for (auto &i : p)
    swap(i, p[rng() % p.size()]);
}
int randgame(TestBoard board)
{
  char pl = (side(board) == 1 ? 'X' : '0');
  vector<pair<int, int>> moves = gen(board);
  shuffle(moves);
  while (status(board) == 2)
  {
    auto move2 = moves.back();
    board[move2.first][move2.second] = pl;
    pl = (pl == '0' ? 'X' : '0');
    moves.pop_back();
  }
  return status(board);
}
uint64_t randnum[N][N];
struct chash
{
  uint64_t operator()(const TestBoard &x) const
  {
    uint64_t hash = 0;
    for (int i = 0; i < N; i++)
      for (int j = 0; j < N; j++)
        hash ^= randnum[i][j] * x[i][j];
    return hash;
  }
};
cc_hash_table<TestBoard, pair<double, int>, chash> nodes_map;
TestBoard max_uctpos(TestBoard &board)
{
  vector<pair<int, int>> moves = gen(board);
  TestBoard par = board;
  int x_count = 0, o_count = 0;
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      if (board[i][j] == 'X')
      {
        x_count++;
      }
      else if (board[i][j] == '0')
      {
        o_count++;
      }
    }
  }
  char pl = ((x_count - o_count == 1) ? '0' : 'X');
  vector<TestBoard> zeromoves;
  double max_uctval = -999999999;
  vector<TestBoard> max_uctmoves;
  for (auto &i : moves)
  {
    char last = board[i.first][i.second];
    board[i.first][i.second] = pl;
    if (nodes_map.find(board) == nodes_map.end())
      zeromoves.push_back(board);
    else
    {
      double uctval =
          (nodes_map[board].second - nodes_map[board].first) /
              nodes_map[board].second +
          sqrt(2 * log(nodes_map[par].second) / nodes_map[board].second);
      if (uctval > max_uctval)
      {
        max_uctval = uctval;
        max_uctmoves = {board};
      }
      else if (uctval == max_uctval)
      {
        max_uctmoves.push_back(board);
      }
    }
    board[i.first][i.second] = last;
  }
  if (zeromoves.size())
  {
    return zeromoves[rng() % zeromoves.size()];
  }
  else
  {
    return max_uctmoves[rng() % max_uctmoves.size()];
  }
}
pair<int, int> bestmove(TestBoard &board)
{
  vector<pair<pair<int, int>, double>> res;
  for (auto i : gen(board))
  {
    char last = board[i.first][i.second];
    char pl = (side(board) == 1 ? 'X' : '0');
    board[i.first][i.second] = pl;
    res.push_back(
        {i, (double)1 - nodes_map[board].first / nodes_map[board].second});
    board[i.first][i.second] = last;
  }
  sort(res.begin(), res.end(),
       [](pair<pair<int, int>, double> a, pair<pair<int, int>, double> b)
       {
         return a.second > b.second;
       });
  return res[0].first;
}
void do_simulation(TestBoard board)
{
  vector<TestBoard> pos_stack;
  while (nodes_map.find(board) != nodes_map.end())
  {
    pos_stack.push_back(board);
    if (status(board) != 2)
      break;
    board = max_uctpos(board);
  }
  pos_stack.push_back(board);
  if (pos_stack.size() - 1 > depth)
  {
    auto best = bestmove(pos_stack[0]);
    cout << "depth " << depth << " nps "
         << (int)((double)cntnodes * 1.0e9 /
                  chrono::duration_cast<chrono::nanoseconds>(Clock.now() -
                                                             start)
                      .count())
         << " nodes " << cntnodes << " bestmove ";
    cout << best.first << " " << best.second;
    cout << " score "
         << (int)(log(((double)nodes_map[pos_stack[0]].first /
                       (nodes_map[pos_stack[0]].second -
                        nodes_map[pos_stack[0]].first))) /
                  log(1.5) * 100 * side(pos_stack[0]))
         << endl;
  }
  depth = max(depth, pos_stack.size() - 1);
  nodes_map[board] = {0, 0};
  int randres = randgame(board);
  for (auto &i : pos_stack)
  {
    if (randres == 0)
    {
      nodes_map[i].first += 0.5;
    }
    else if (side(i) == randres)
    {
      nodes_map[i].first += 1;
    }
    nodes_map[i].second += 1;
  }
}
int main()
{
  cin >> k;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      randnum[i][j] = rng();
  TestBoard board;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      cin >> board[i][j];
  char pl = (side(board) == 1 ? 'X' : '0');
  while (status(board) == 2)
  {
    depth = 1;
    nodes_map = {};
    start = Clock.now();
    for (cntnodes = 0; cntnodes < k; cntnodes++)
      do_simulation(board);
    int nps =
        (double)cntnodes * 1.0e9 /
        chrono::duration_cast<chrono::nanoseconds>(Clock.now() - start).count();
    auto best = bestmove(board);
    cout << "depth " << depth << " nps " << nps << " nodes " << cntnodes
         << " bestmove ";
    cout << best.first << " " << best.second;
    cout << " score "
         << (int)(log((nodes_map[board].first /
                       (nodes_map[board].second - nodes_map[board].first))) /
                  log(1.5) * 100 * side(board))
         << endl;
    pair<int, int> move;
    cin >> move.first >> move.second;
    board[move.first][move.second] = pl;
    pl = (pl == 'X' ? '0' : 'X');
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