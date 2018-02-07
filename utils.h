//
// Created by Maxen Chung on 2/4/18.
//

#ifndef PROJECT_3_UTILS_H
#define PROJECT_3_UTILS_H

//
// Created by Maxen Chung on 2/4/18.
//
#include <stdio.h>
#include <pthread.h>
#include <string>
#include <mutex>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "barrier.h"

using namespace std;

void initialize_seat_chart(vector<vector<string> > &seat_chart);
bool int_sort(int lhs, int rhs);

void initialize_buyers(vector<vector<int> > &buyers, int N);

void print_seating_chart(vector<vector<string> > seat_chart);
void print_buyers(vector<vector<int> >);
#endif //PROJECT_3_UTILS_H
