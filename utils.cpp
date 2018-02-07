//
// Created by Maxen Chung on 2/4/18.
//
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <mutex>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <time.h>
#include "barrier.h"

using namespace std;

void initialize_seat_chart(vector<vector<string> > &seat_chart){
    for(int i = 0; i < 10; i++){
        vector<string> temp;
        seat_chart.push_back(temp);
        for(int j = 0; j < 10; j++){
            seat_chart[i].push_back("");
        }
    }
}

bool int_sort(int lhs, int rhs){return lhs>rhs;}

void initialize_buyers(vector<vector<int> > &buyers, int N){
    srand(time(NULL));
    for(int i = 0; i < 10; i++){
        vector<int> temp;
        buyers.push_back(temp);
        for(int j = 0; j < N; j++){
            buyers[i].push_back(rand()%60);
        }
        std::sort(buyers[i].begin(), buyers[i].end(), int_sort);
    }
}

void print_seating_chart(vector<vector<string> > seat_chart){

    cout << endl;
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            if(seat_chart[i][j] == ""){
                std::cout << " ---- ";
            } else {
                std::cout << " " << seat_chart[i][j] << " ";
            }
        }
        std::cout << std::endl;
    }
    cout << endl;
}

void print_buyers(vector<vector<int> > buyers){
    for(int i = 0; i < 10; i++){
        cout << i << ": ";
        for(int j = 0; j < buyers[i].size(); j++){
            cout << buyers[i][j] << "  ";
        }
        std::cout << std::endl;
    }
}