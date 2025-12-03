#include <iostream>
#include <vector>
#include <utility>
#include <limits.h>
#include <chrono>
#include <tuple>

int globalSeed = time(0);

int N;
std::vector<int> queens, queensPerRow, queensPerD1, queensPerD2;

int checkConflicts(int row, int col) {
    int conflicts;

    conflicts = queensPerRow[row];
    conflicts += queensPerD1[N - 1 + col - row];
    conflicts += queensPerD2[col + row];

    if (queens[col] == row) {
        return conflicts - 3;
    }

    return conflicts;
}

void updateQueenArrays(int row, int col) {
    // Decrement values for old position
    queensPerRow[queens[col]]--;
    queensPerD1[N - 1 + col - queens[col]]--;
    queensPerD2[col + queens[col]]--;

    // Update position
    queens[col] = row;

    // Increment values for new position
    queensPerRow[row]++;
    queensPerD1[N - 1 + col - row]++;
    queensPerD2[col + row]++;
}

// Return the row with the minimum conflicts for the queen in column col
// Return a random row with min conflicts if more than one exist
int getRowWithMinConflicts(int col) {
    std::vector<int> candidates;
    int minConflicts = INT_MAX;
    int conflictsAt_i_col;

    for (int i = 0; i < N; i++) {
        if (i == queens[col]) {
            continue;
        }

        conflictsAt_i_col = checkConflicts(i, col);

        if (minConflicts > conflictsAt_i_col) {
            minConflicts = conflictsAt_i_col;
            candidates.clear();
            candidates.push_back(i);
        }
        else if (minConflicts == conflictsAt_i_col) {
            candidates.push_back(i);
        }
    }

    srand(globalSeed++);
    int randomCandidate = rand() % candidates.size();

    return candidates[randomCandidate];
}


class Heap {
private:
    std::vector<int> heap;
    int maxConflicts;
    int queensWithMaxConflicts; // Number of queens with conflicts value equal to maxConflicts

public:
    Heap(const std::vector<int>& queens) {
        maxConflicts = 0; // Will be updated in the insert function

        for (int i = 0; i < N; i++) {
            insert(i);
        }
    }

    // After a queen position was modified, the heap needs to be readjusted to account for the new conflict values
    void reheapify() {
        maxConflicts = 0;

        int conflictsCurQueen;

        for (int i = 0; i < N; i++) {
            int p = i;
            conflictsCurQueen = checkConflicts(queens[heap[p]], heap[p]);

            while(p != 0 && checkConflicts(queens[heap[parrent(p)]], heap[parrent(p)]) < conflictsCurQueen) {
                std::swap(heap[p], heap[parrent(p)]);
                p = parrent(p);
            }
            
            if (maxConflicts < conflictsCurQueen) {
                maxConflicts = conflictsCurQueen;
                queensWithMaxConflicts = i;
            }
            else if (maxConflicts == conflictsCurQueen) {
                queensWithMaxConflicts = i;
            }
        }
    }

    // Insert the queen at column col in the heap
    void insert(int col) {
        heap.push_back(col);

        int index = heap.size() - 1;

        int conflictsCurQueen = checkConflicts(queens[col], col);

        if (maxConflicts < conflictsCurQueen) {
            maxConflicts = conflictsCurQueen;
            queensWithMaxConflicts = index;
        }
        else if (maxConflicts == conflictsCurQueen) {
            queensWithMaxConflicts = index;
        }

        while(index != 0 && checkConflicts(queens[heap[parrent(index)]], heap[parrent(index)]) < conflictsCurQueen) {
            std::swap(heap[index], heap[parrent(index)]);
            index = parrent(index);
        }
    }

    int getColWithQueenWithMaxConflicts() const {
        if (queensWithMaxConflicts > 1) {
            srand(globalSeed++);

            int randomMaxQueen = rand() % queensWithMaxConflicts;

            return heap[randomMaxQueen];
        }

        else {
            return top();
        }
    }

    // Return the first pair of (row, col) which leads to an improvent in the conflicts
    // Return a random position if all columns with max conflicts lead to a worse position (unlikely)
    std::pair<int, int> getImprovedPostion() {
        int newRow;
        int currentConflicts;

        for (int i = 0; i < queensWithMaxConflicts; i++) {
            currentConflicts = checkConflicts(queens[heap[i]], heap[i]);
            if (currentConflicts == 0) {
                continue;
            }

            newRow = getRowWithMinConflicts(heap[i]);
 
            if (checkConflicts(newRow, heap[i]) < currentConflicts) {
//                printf("Returning the position (%d, %d)\n", newRow, heap[i]);
//                printf("The previous number of conflicts in the position was %d\n", currentConflicts);
//                printf("The new number of conflicts in the position is %d\n", checkConflicts(newRow, heap[i]));

                return std::pair<int, int>(newRow, heap[i]);
            }
        }

//        printf("Returning a random position from getImproverPosition()\n");

        int randomCol = rand() % N;
        newRow = getRowWithMinConflicts(randomCol);

        return std::pair<int, int>(newRow, randomCol);
    } 

    bool isSolved() const {
        return maxConflicts == 0;
    }

    int getMaxConflicts() const {
        return maxConflicts;
    }

    int getNumberOfQueensWithMaxConflicts() const {
        return queensWithMaxConflicts;
    }

    int top() const {
        return heap[0];
    }

    int parrent(int index) const{
        return (index - 1) / 2;
    }

    int left(int index) const{
        return (index * 2) + 1;
    }

    int right(int index) const{
        return (index * 2) + 2;
    }

    void print() const {
        for (int i = 0; i < N; i++) {
            std::cout << "Queen at (" << queens[heap[i]] << ", " << heap[i] << ") has " << checkConflicts(queens[heap[i]], heap[i]) << " conflicts" << std::endl;
        }
    }
};

void init(int n){
    queens.resize(n);
    queensPerRow.resize(n);
    queensPerD1.resize(2*n - 1);
    queensPerD2.resize(2*n - 1);

    int step = 0;
    for (int i = 0; i < n; i++){
        if (i % 2 == 0) {
            queensPerRow[step]++;
            queensPerD1[n - 1 + i - step]++;
            queensPerD2[i + step]++;
            queens[i] = step;
        }
        else {
            queensPerRow[n - step - 1]++;
            queensPerD1[i + step]++;
            queensPerD2[n - 1 - step + i]++;
            queens[i] = n - step - 1;
            step++;
        }
    }
}


void minConflicts(int n) {
    auto start = std::chrono::high_resolution_clock::now();
    init(n);

    Heap queenHeap(queens);

    int col;
    int row;

    int limit = n*n;

    for(int iter = 0; iter < 10000*n; iter++) {
        if (iter > 800) {
            printf("Iteration %d\n", iter);
        }

        if (queenHeap.isSolved()) {
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            printf("Solved on iteration %d\nTime needed to find a solution was %f\n", iter, ((double)duration.count())/1000);
            return;
        }



        std::pair<int, int> newPosition = queenHeap.getImprovedPostion();

        row = newPosition.first;
        col = newPosition.second;

        updateQueenArrays(row, col);

        queenHeap.reheapify();
    }
    
    
    printf("Failed to find a solution\n");
}

int linearGetMaxConflicts() {
    int maxConflicts = 0;
    for (int i = 0; i < N; i++) {
        if (maxConflicts < checkConflicts(queens[i], i)) {
            maxConflicts = checkConflicts(queens[i], i);
        }
    }

    return maxConflicts;
}

std::vector<int> linearGetMaxQueen() {
    std::vector<int> candidates;
    int candidatesSize = 0;

    int maxConflicts = linearGetMaxConflicts();

    if (maxConflicts == 0) {
        return candidates;
    }

    for (int i = 0; i < N; i++) {
        if (maxConflicts == checkConflicts(queens[i], i)) {
            candidates.push_back(i);
            candidatesSize++;
        }
    }

    return candidates;
}

void linearMinConflicts(int n) {
    auto start = std::chrono::high_resolution_clock::now();
    init(n);

    int col;
    int row;

    int limit = n*n;

    std::vector<int> candidates;

    for(int iter = 0; iter < 10*n; iter++) {
        candidates = linearGetMaxQueen();
        if (candidates.empty()) {
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            printf("Solved on iteration %d\nTime needed to find a solution was %f\n", iter, ((double)duration.count())/1000);
            return;
        }

        int i = 0;
        int numCandidates = candidates.size();

        do {
            col = candidates[i++];
            row = getRowWithMinConflicts(col);
        } while (checkConflicts(queens[col], col) <= checkConflicts(row, col) && i < numCandidates);


        if (i >= numCandidates && checkConflicts(queens[col], col) <= checkConflicts(row, col)) {
            // If the new position isn't an improvement on the last, execute a random move
            srand(globalSeed++);

            col = rand() % n;
            row = getRowWithMinConflicts(col);
        }

        updateQueenArrays(row, col);
    }
}

int main() {
    std::cin >> N;

    linearMinConflicts(N);

    if (N < 100) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (queens[j] == i) {
                    std::cout << "* ";
                }
                else {
                    std::cout << "_ ";
                }
            }
            std::cout << std::endl;
        }
    }

    return 0;
}