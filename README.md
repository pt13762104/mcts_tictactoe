# mcts_tictactoe
A n^2-game MCTS engine
## Build instructions
`g++ <or another C++11 compiler> -O3 mcts_tictactoe.cpp -o mcts_tictactoe`
## Usage
Input format:

```
n x
<board>
```

`<board>` is a n*n array of `char`s.

`X` represent the first player, and `0` represent the second player. 

Any other character means an empty cell. `x` is the number of simulations per position.
### If the game is unterminated 
It prints in this format after a new depth is reached:
`depth,nps,nodes,bestmove,score`
```
depth: last maximum depth
nps: simulations/second
nodes: total number of simulations
bestmove: The move with the highest probability of winning
score: Calculated as 100*(log base 1.5 of white_score/black_score).
```
Example for a 3x3 game:
```
depth 1 nps 11627 nodes 10 bestmove 0 1 score 0
depth 2 nps 9108 nodes 51 bestmove 0 2 score 118
depth 3 nps 6425 nodes 203 bestmove 0 2 score 166
depth 4 nps 5691 nodes 554 bestmove 0 2 score 154
depth 5 nps 4520 nodes 1578 bestmove 1 1 score 139
depth 6 nps 3888 nodes 2718 bestmove 1 1 score 165
depth 7 nps 3587 nodes 3699 bestmove 1 1 score 185
depth 8 nps 3419 nodes 4595 bestmove 1 1 score 217
depth 9 nps 3407 nodes 4666 bestmove 1 1 score 219
depth 10 nps 2969 nodes 10000 bestmove 1 1 score 311
```
Then it prompts the user to input a move in the format `i j`.

`i` is the index of the row top down.

`j` is the index of the column from left to right.
### If the game is terminated
It prints either one of those 3:
```
The first player won
The second player won
Draw
```
