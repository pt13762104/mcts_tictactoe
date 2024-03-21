#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
int N, k;
size_t cntnodes = 0;
size_t depth = 1;
size_t start = clock();
bool performCheckRow(vector<vector<char>> &board, char player) {
  int cnt;
  for (int i = 0; i < N; i++) {
    cnt = 0;
    for (int j = 0; j < N; j++) {
      if (board[i][j] == player) {
        cnt++;
      }
      if (cnt == N) {
        return 1;
      }
    }
  }
  return 0;
}
bool performCheckColumn(vector<vector<char>> &board, char player) {
  int cnt;
  for (int i = 0; i < N; i++) {
    cnt = 0;
    for (int j = 0; j < N; j++) {
      if (board[j][i] == player) {
        cnt++;
      }
      if (cnt == N) {
        return 1;
      }
    }
  }
  return 0;
}
bool performCheckDiag(vector<vector<char>> &board, char player) {
  int cnt = 0;
  for (int i = 0; i < N; i++) {
    if (board[i][i] == player) {
      cnt++;
    }
  }
  if (cnt == N) {
    return 1;
  }
  cnt = 0;
  for (int i = 0; i < N; i++) {
    if (board[i][N - i - 1] == player) {
      cnt++;
    }
  }
  if (cnt == N) {
    return 1;
  }
  return 0;
}
int status(vector<vector<char>> &board) {
  int x_count = 0, o_count = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (board[i][j] == 'X') {
        x_count++;
      } else if (board[i][j] == '0') {
        o_count++;
      }
    }
  }
  if (performCheckColumn(board, 'X') || performCheckRow(board, 'X') ||
      performCheckDiag(board, 'X')) {
    return 1;
  }
  if (performCheckColumn(board, '0') || performCheckRow(board, '0') ||
      performCheckDiag(board, '0')) {
    return -1;
  } else {
    if (x_count + o_count < N * N) {
      return 2;
    } else {
      return 0;
    }
  }
}
vector<pair<int, int>> gen(vector<vector<char>> &board) {
  if (status(board) != 2)
    return {};
  vector<pair<int, int>> vaild;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (board[i][j] != 'X' && board[i][j] != '0')
        vaild.push_back({i, j});
    }
  }
  return vaild;
}

void shuffle(vector<pair<int, int>> &p) {
  for (auto &i : p)
    swap(i, p[rand() % p.size()]);
}
int randgame(vector<vector<char>> board) {
  int x_count = 0, o_count = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (board[i][j] == 'X') {
        x_count++;
      } else if (board[i][j] == '0') {
        o_count++;
      }
    }
  }
  char pl = ((x_count - o_count == 1) ? '0' : 'X');
  vector<pair<int, int>> moves = gen(board);
  shuffle(moves);
  while (status(board) == 2) {
    auto move2 = moves.back();
    board[move2.first][move2.second] = pl;
    pl = (pl == '0' ? 'X' : '0');
    moves.pop_back();
  }
  return status(board);
}
map<vector<vector<char>>, pair<double, int>> tree;
vector<vector<char>> max_uctpos(vector<vector<char>> &board) {
  vector<pair<int, int>> moves = gen(board);
  vector<vector<char>> par = board;
  int x_count = 0, o_count = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (board[i][j] == 'X') {
        x_count++;
      } else if (board[i][j] == '0') {
        o_count++;
      }
    }
  }
  char pl = ((x_count - o_count == 1) ? '0' : 'X');
  vector<vector<vector<char>>> zeromoves;
  double max_uctval = -999999999;
  vector<vector<vector<char>>> max_uctmoves;
  for (auto i : moves) {
    char last = board[i.first][i.second];
    board[i.first][i.second] = pl;
    if (tree.find(board) == tree.end())
      zeromoves.push_back(board);
    else {
      double uctval =
          (tree[board].second - tree[board].first) / tree[board].second +
          sqrt(2 * log(tree[par].second) / tree[board].second);
      if (uctval > max_uctval) {
        max_uctval = uctval;
        max_uctmoves = {board};
      } else if (uctval == max_uctval) {
        max_uctmoves.push_back(board);
      }
    }
    board[i.first][i.second] = last;
  }
  if (zeromoves.size()) {
    return zeromoves[rand() % zeromoves.size()];
  } else {
    return max_uctmoves[rand() % max_uctmoves.size()];
  }
}
int side(vector<vector<char>> &board) {
  int x_count = 0, o_count = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (board[i][j] == 'X') {
        x_count++;
      } else if (board[i][j] == '0') {
        o_count++;
      }
    }
  }
  int pl = ((x_count - o_count == 1) ? -1 : 1);
  return pl;
}
pair<int, int> bestmove(vector<vector<char>> &board) {
  vector<pair<pair<int, int>, double>> res;
  for (auto i : gen(board)) {
    char last = board[i.first][i.second];
    char pl = (side(board) == 1 ? 'X' : '0');
    board[i.first][i.second] = pl;
    res.push_back({i, (double)1 - tree[board].first / tree[board].second});
    board[i.first][i.second] = last;
  }
  sort(res.begin(), res.end(),
       [](pair<pair<int, int>, double> a, pair<pair<int, int>, double> b) {
         return a.second > b.second;
       });
  return res[0].first;
}
void do_simulation(vector<vector<char>> board) {
  vector<vector<vector<char>>> pos_stack;
  while (tree.find(board) != tree.end()) {
    pos_stack.push_back(board);
    if (status(board) != 2)
      break;
    board = max_uctpos(board);
  }
  pos_stack.push_back(board);
  if (pos_stack.size() - 1 > depth) {
    auto best = bestmove(pos_stack[0]);
    cout << "depth " << depth << " nps "
         << (int)((double)cntnodes / (clock() - start) * CLOCKS_PER_SEC)
         << " nodes " << cntnodes << " bestmove ";
    cout << best.first << " " << best.second;
    cout << " score "
         << (int)(log((
                      (double)tree[pos_stack[0]].first /
                      (tree[pos_stack[0]].second - tree[pos_stack[0]].first))) /
                  log(1.5) * 100 * side(pos_stack[0]))
         << endl;
  }
  depth = max(depth, pos_stack.size() - 1);
  tree[board] = {0, 0};
  int randres = randgame(board);
  for (auto i : pos_stack) {
    if (randres == 0) {
      tree[i].first += 0.5;
    } else if (side(i) == randres) {
      tree[i].first += 1;
    }
    tree[i].second += 1;
  }
}
int main() {
  srand(time(0));
  cin >> N >> k;
  vector<vector<char>> board(N, vector<char>(N));
  int x_count = 0, o_count = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      cin >> board[i][j];
      if (board[i][j] == 'X') {
        x_count++;
      } else if (board[i][j] == '0') {
        o_count++;
      }
    }
  }
  char pl = (side(board) == 1 ? 'X' : '0');
  while (status(board) == 2) {
    cntnodes = 0;
    depth = 1;
    start = clock();
    tree = {};
    while (cntnodes < k) {
      do_simulation(board);
      cntnodes++;
    }
    auto best = bestmove(board);
    cout << "depth " << depth << " nps "
         << (int)((double)cntnodes / (clock() - start) * CLOCKS_PER_SEC)
         << " nodes " << cntnodes << " bestmove ";
    cout << best.first << " " << best.second;
    cout << " score "
         << (int)(log(((double)tree[board].first /
                       (tree[board].second - tree[board].first))) /
                  log(1.5) * 100 * side(board))
         << endl;
    pair<int, int> move;
    cin >> move.first >> move.second;
    board[move.first][move.second] = pl;
    pl = (pl == 'X' ? '0' : 'X');
  }
  int res = status(board);
  if (res == 1) {
    cout << "The first player won" << endl;
  } else if (res == -1) {
    cout << "The second player won" << endl;
  } else {
    cout << "Draw" << endl;
  }
}
