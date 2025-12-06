# N-queens
N-queens is a generilation of the 8-queens problem, where we have an NxN size chessboard and we have to position N queens on it so no two queens are in conflict, i.e. there are no two queens in the same row, column or diagonal.
The main solution is contained in the Homework2_n_queens.cpp file, in the linearMinConflicts() function. I've kept some of the code that was a part of some additional attempts at optimizing the program to show not only what did work but what didn't, as I think that is an important aspect of any attempted solution.

## Recursive algorithm
When a person first encounters the 8-queens problem, the recursive solution is the most common and (probably) intuitive one: 
Let's say that we number each queen 1 to 8 to differentiate between them. Since there can't be two queens on the same column, we can consider that queen M, where M is a number between 1 and 8, is positioned on column M. 
Now we only have to think about on which row to position each queen so that no two are in conflict. These are the recursive steps to follow:
  1. We position the first queen on the first row.
  2. For any queen that isn't the first one (let's say we are on queen M) we check for the following condition:
     -  If there is at least one square on column M where there are no conflicts, we position queen M on that square and continue to queen M+1.
     -  Else (meaning that all squares on column M are being attacked by at least one queen), we've made a mistake on one of the previous steps. We go back to queen M-1 and try to find the next available square for it.
  3. If we've found an availble square for queen 8, then there are no conflicts on the board and the problem is solved.

This solution, while intuitive is very slow - worst case we have to check 8 positions for each queen that means that the above steps have to be repeated 8^8 = 16 777 216 times. 
Most systems will still be able to solve the problem using the recursive algorithm in a relatively short time. However for very large N, say in the tens of thousands, it is simply unfeasible to use the recursive algorithm.

## Min-conflicts
Min-conflicts is a heursitic local search algorithm that repeats the same two actions on each step - it searches for the queen with the most conflicts and positions it on the square with the least conflicts. If each queen has zero conflicts, then the problem is solved. 
Since the algortihm searches localy and uses a heuristic the time complexity is linear, but there is a common issue that the program will almost certainly encounter - it may reach a local maximum. This means that there are still queens in conflict but there are no moves that decrease them.
To escape this local maximum the algorithm has to intentionally make a bad move. If there are no possible improvements, then we make a random move. Since the number of solutions increase as the number of queens increses this measure is enough to ensure that we will find a solution.

To push the capabilities of the algorithm, there are some additional optimizations:
  - The board is initialized with the knight's path, because if two queens are positioned on adjacent squares from the knight's path, they would not be in conflict.
  - The queen positions can be represented in a single array of size N where the index i represents the column and the value at index i is the row.
  - In a similar manner the number of queens on each row, major or counter dioganal can be kept in arrays as well. When it is needed to check the conflicts of a queen, the program only needs to check one index in each of these arrays. Also when a move is made the program only needs to decrement one index in each array and increment another one. Thus the conflict-check and queen move become actions that take constant time.

The result is that for small number queens (<10) the program solves the problem in less than a millisecond, for 10_000 queens it solves it for several tenths of a second, and for 100_000 it takes around a minute.
There is some room for parallelization like the check for conflicts and update of the arrays that can squeeze some more time from the algorithm, but I haven't implemented it here yet. Maybe in the future.
