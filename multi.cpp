#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <thread>

using namespace std;
using namespace std::chrono;

struct Matrix {
    unsigned int cols = 0;
    unsigned rows = 0;
    int **data;

    Matrix() {};
    Matrix(int r, int c) {
        cols = c;
        rows = r;
        data = new int* [rows];
        for (int i = 0; i < rows; ++i) {
            data[i] = new int[cols];
            for (int j = 0; j < cols; ++j) 
                data[i][j] = 0;            
        }
    }
};

Matrix Read(string fileName) {    
    fstream file(fileName, fstream::in);
    Matrix matrix;
    try {
        if (file.is_open()) {            
            int rows, cols;
            file >> rows >> cols;
            matrix = Matrix(rows, cols);

            for (int r = 0; r < matrix.rows; ++r) 
                for (int c = 0; c < matrix.cols; ++c) 
                    file >> matrix.data[r][c];                
            
            file.close();
            cout << "File: " << fileName << " read.\n";
        }
        else cout << "could not open file: " << fileName << "\n";             
        return matrix;
    }
    catch (exception ex) {
        if (file.is_open())file.close();
        throw ex;
    }
}

void MultiplyRow(Matrix m1, Matrix m2, Matrix result, int row) {    
    for (int c = 0; c < m2.cols; ++c)
        for (int i = 0; i < m1.cols; ++i) 
            result.data[row][c] += (m1.data[row][i] * m2.data[i][c]);       
    string str = ("Row: " + to_string(row) + " done.\n");
    cout << str;
}


Matrix MultiplyMatrices(Matrix m1, Matrix m2) {
    try {
        Matrix matrix;
        if (m1.cols == m2.rows) {        
            
            vector<thread> threads;            
            matrix = Matrix(m1.rows, m2.cols);            
            
            for (int i = 0; i < m1.rows; ++i) {
                threads.emplace_back(MultiplyRow, m1, m2, matrix, i);
            }

            for (int i = 0; i < threads.size(); ++i) {
                threads[i].join();
            }

            cout << "Matrices multiplied.\n";
        }
        else {
            cout << "cannot multiply matrices\n";
        }
        return matrix;
    }
    catch (exception ex) {
        throw ex;
    }
}

void Write(int **data, int rows, int cols, string fileName) {
    try {
        ofstream file(fileName);
        char SPACE = ' ';
        if (file.is_open()) {
            file << rows << SPACE << cols << "\n";
            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    file << data[r][c] << SPACE;
                }
                file << "\n";
            }
        }
        cout << "Matrix written to: " << fileName << ".\n";
        file.close();
    }
    catch (exception ex) {
        throw ex;
    }
}
Matrix mulmat (Matrix m1, Matrix m2) {
    Matrix rslt;
    rslt = Matrix(m1.rows, m2.cols); 
    for (int i = 0; i < m1.rows; i++) {
        for (int j = 0; j < m2.cols; j++) {
            for (int k = 0; k < m1.rows; k++) {
                rslt.data [i][j] += (m1.data[i][k] * m2.data[k][j]);
            }
        }
    }
    return rslt;
}

int main(int argc, char** argv) {
    Matrix matrix1 = Read(argv[1]); 
    Matrix matrix2 = Read(argv[2]);
    string file = argv[3];
    
    auto start = high_resolution_clock::now();
    Matrix result = MultiplyMatrices(matrix1, matrix2);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by multi thread: " << duration.count() << " microseconds" << endl;
    Write(result.data, result.rows, result.cols, file);

    auto start2 = high_resolution_clock::now();
    Matrix rslt = mulmat(matrix1, matrix2);
    auto stop2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(stop2 - start2);
    cout << "Time taken by normal: " << duration2.count() << " microseconds" << endl;
}