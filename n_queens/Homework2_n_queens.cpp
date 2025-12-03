#include <iostream>
#include <vector>
#include <utility>
#include <limits.h>
#include <chrono>

int globalSeed = time(0);

int N;
std::vector<int> queens, queensPerRow, queensPerD1, queensPerD2;

int checkConflicts(int row, int col) {
    int conflicts = queensPerRow[row] - 1;
    conflicts += queensPerD1[N - 1 + row - col] - 1;
    conflicts += queensPerD2[col + row] - 1;
    
    return conflicts;
}

void updateQueenArrays(int row, int col) {
    // Decrement values for old position
    queensPerRow[queens[col]]--;
    queensPerD1[N - 1 + queens[col] - col]--;
    queensPerD2[col + queens[col]]--;

    // Update position
    queens[col] = row;

    // Increment values for new position
    queensPerRow[row]++;
    queensPerD1[N - 1 + row - col]++;
    queensPerD2[col + row]++;
}

int getRowWithMinConflicts(int col, int currentConflicts) {
    std::vector<int> candidates;
    int minConflicts = currentConflicts;
    int conflictsAt_i_col;

    for (int i = 0; i < N; i++) {
        if (i == queens[col]) {
            continue;
        }

        conflictsAt_i_col = checkConflicts(i, col) + 3;

        if (minConflicts > conflictsAt_i_col) {
            minConflicts = conflictsAt_i_col;
            candidates.clear();
            candidates.push_back(i);
        }
        else if (minConflicts == conflictsAt_i_col) {
            candidates.push_back(i);
        }
    }

    if (candidates.empty()) {
        return -1;
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

        for (int j = 0; j < N; j++) {
            int i = j;
            conflictsCurQueen = checkConflicts(queens[heap[i]], heap[i]);

            while(i != 0 && checkConflicts(queens[heap[parrent(i)]], heap[parrent(i)]) < conflictsCurQueen) {
                std::swap(heap[i], heap[parrent(i)]);
                i = parrent(i);
            }
            
            if (maxConflicts < conflictsCurQueen) {
                maxConflicts = conflictsCurQueen;
                queensWithMaxConflicts = 1;
            }
            else if (maxConflicts == conflictsCurQueen) {
                queensWithMaxConflicts++;
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
            queensWithMaxConflicts = 1;
        }
        else if (maxConflicts == conflictsCurQueen) {
            queensWithMaxConflicts++;
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

    bool isSolved() const {
        return maxConflicts == 0;
    }

    int getMaxConflicts() const {
        return maxConflicts;
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
            queensPerD1[n - 1 + step - i]++;
            queensPerD2[i + step]++;
            queens[i] = step;
        }
        else {
            queensPerRow[n - step - 1]++;
            queensPerD1[n - 1 + (n - step - 1) - i]++;
            queensPerD2[i + n - step - 1]++;
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

    for(int iter = 0; iter < 1000*n; iter++) {
        if (queenHeap.isSolved()) {
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            printf("Solved on iteration %d\nTime needed to find a solution was %f\n", iter, ((double)duration.count())/1000);

            return;
        }
        col = queenHeap.getColWithQueenWithMaxConflicts();
        row = getRowWithMinConflicts(col, INT_MAX);

        if (checkConflicts(queens[col], col) <= checkConflicts(row, col)) {
            // If the new position isn't an improvement on the last, execute a random move
            srand(globalSeed++);

            col = rand() % n;
            row = getRowWithMinConflicts(col, INT_MAX);
        }

        updateQueenArrays(row, col);
        if (iter % 5 == 0) {
            queenHeap.reheapify();
        }
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

int linearGetMaxQueen() {
    std::vector<int> candidates;
    int candidatesSize = 0;

    int maxConflicts = linearGetMaxConflicts();

    if (maxConflicts == 0) {
        return -1;
    }

    for (int i = 0; i < N; i++) {
        if (maxConflicts == checkConflicts(queens[i], i)) {
            candidates.push_back(i);
            candidatesSize++;
        }
    }

    srand(globalSeed++);

    int randomCandidate = rand() % candidatesSize;

    return candidates[randomCandidate];
}

void linearMinConflicts(int n) {
    auto start = std::chrono::high_resolution_clock::now();
    init(n);

    int col;
    int row;

    int limit = n*n;

    for(int iter = 0; iter < 1000*n; iter++) {
        col = linearGetMaxQueen();
        if (col == -1) {
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            printf("Solved on iteration %d\nTime needed to find a solution was %f\n", iter, ((double)duration.count())/1000);
            
            return;
        }
        row = getRowWithMinConflicts(col, checkConflicts(queens[col], col));

        if (row == -1) {
            // If the new position isn't an improvement on the last, execute a random move
            srand(globalSeed++);

            col = rand() % n;
            row = getRowWithMinConflicts(col, INT_MAX);
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