#include <stdio.h>
#include <pthread.h>
#include <string>
#include <mutex>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <unistd.h>
#include "barrier.h"
#include "utils.h"

using namespace std;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
barrier barrier1(10);
vector<vector<int> > buyers;
vector<vector<string> > seat_chart;
int num_customers;
int turned_away;
int h_customers;
int m_customers;
int l_customers;
bool windows_closed;



class seller{
public:
    seller(char seller_type, int seller_number){
        this->seller_number = seller_number;
        this->seller_type = seller_type;
    }
    char seller_type;
    int seller_number;
};


int get_processing_time(seller* seller_info){
    srand(time(NULL));
    switch(seller_info->seller_type){
        case 'H':
            return rand()%2 + 1;
        case 'M':
            return rand()%3 + 2;
        case 'L':
            return rand()%4 + 4;
        default:
            break;
    }
    return -1;
}

pair<int,int> findSeat(vector<int>rows){
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            if(seat_chart[rows[i]][j] == ""){
                pair<int,int> return_value;
                return_value.first = rows[i];
                return_value.second = j;
                return return_value;
            }
        }
    }
    pair<int,int> return_value;
    return_value.first = -1;
    return_value.second = -1;
    return return_value;
};

void print_log(int time, string message, seller* seller_info){
    pthread_mutex_lock(&print_mutex);
    if(message == "Sold Out, Buyer turned away") {
        turned_away++;
        if(!windows_closed){
            cout << "0:";
            if(time < 10)
                cout << "0";
            cout << time << ":  ";
            cout << "SOLD OUT! ALL WINDOWS WILL CLOSE AFTER SERVING EXISTING CUSTOMERS" << endl;
            windows_closed = true;
        }
    }

    cout << "0:";
    if(time < 10)
        cout << "0";
    cout << time << ":  ";

    int seller_number;
    switch(seller_info->seller_type){
        case 'H':
            seller_number = 0;
            break;
        case 'M':
            seller_number = seller_info->seller_number;
            break;
        case 'L':
            seller_number = seller_info->seller_number - 3;
            break;
        default:
            break;
    }
    cout << seller_info->seller_type << seller_number << "  ";
    cout << message << endl;
    pthread_mutex_unlock(&print_mutex);
}

bool assignSeat(seller* seller_info, int time){
    pthread_mutex_lock(&mutex1);
    bool found = false;
    vector<int> rows;
    string customer = "";
    int buyer_num = num_customers - buyers[seller_info->seller_number].size() + 1;
    switch(seller_info->seller_type){
        case 'H':
            rows = {0,1,2,3,4,5,6,7,8,9};
            customer += "H0";
            break;
        case 'M':
            rows = {4,5,3,6,2,7,1,8,0,9};
            customer += "M" + to_string(seller_info->seller_number);
            break;
        case 'L':
            rows = {9,8,7,6,5,4,3,2,1,0};
            customer += "L" + to_string(seller_info->seller_number-3);
            break;
        default:
            break;
    }

    pair<int,int> seat_location = findSeat(rows);
    if(seat_location.first == -1){
        pthread_mutex_unlock(&mutex1);
        return false;
    }

    switch(seller_info->seller_type){
        case 'H':
            h_customers++;
            break;
        case 'M':
            m_customers++;
            break;
        case 'L':
            l_customers++;
            break;
        default:
            break;
    }

    if(buyer_num < 10){
        customer += "0";
    }

    customer.append(to_string(buyer_num));

    seat_chart[seat_location.first][seat_location.second] = customer;
    print_log(time, "Assigned Seat", seller_info);
    pthread_mutex_lock(&print_mutex);
    print_seating_chart(seat_chart);
    pthread_mutex_unlock(&print_mutex);
    pthread_mutex_unlock(&mutex1);
    return true;
}

void log_arriving_buyers(seller *seller_info, int time) {
    for(int i = 0; i < buyers[seller_info->seller_number].size(); i++){
        if(buyers[seller_info->seller_number][i] == time){
            print_log(time, "Buyer Arrived", seller_info);
        }
    }
}

void* sell(void *seller_i) {
    barrier1.wait();
    srand(time(NULL));
    seller* seller_info = (seller*)seller_i;
    int time = 0;
    bool has_buyer = false;
    int min_remaining = -1;
    while (time < 60) {
        if(buyers[seller_info->seller_number].size() == 0) {
            barrier1.wait();
            time++;
            continue;
        }

        if(has_buyer) {
            min_remaining--;
        }

        log_arriving_buyers(seller_info, time);

        if(!has_buyer){
            int arrival_time = buyers[seller_info->seller_number].back();
            if(arrival_time <= time){
                if(!assignSeat(seller_info, time)){
                    print_log(time, "Sold Out, Buyer turned away", seller_info);
                    buyers[seller_info->seller_number].pop_back();
                    barrier1.wait();
                    time++;
                    continue;
                }
                buyers[seller_info->seller_number].pop_back();
                has_buyer = true;
                min_remaining = get_processing_time(seller_info);
            }
        }

        if(min_remaining == 0){
            has_buyer = false;
            min_remaining = -1;
            print_log(time, "Buyer Completed Purchase", seller_info);
        }
        //print out seats
        time++;
        barrier1.wait();
    }
    if(min_remaining>0){
        print_log(60, "Completed Last Purchase and Closed Window", seller_info);
    } else if(!windows_closed){
        print_log(60, "Closed Window", seller_info);
    }
    return NULL; // thread exits
}

int start_simulation() {
    int i;
    pthread_t tids[10];
    char seller_type;
    // Create necessary data structures for the simulator.
    initialize_seat_chart(seat_chart);
    initialize_buyers(buyers, num_customers);
    print_buyers(buyers);


    seller_type = 'H';
    pthread_create(&tids[0], NULL, sell, (void*)new seller(seller_type, 0));

    seller_type = 'M';
    for (i = 1; i < 4; i++)
        pthread_create(&tids[i], NULL, sell, (void*)new seller(seller_type, i));


    seller_type = 'L';
    for (i = 4; i < 10; i++)
        pthread_create(&tids[i], NULL, sell, (void*)new seller(seller_type, i));

    // wakeup all seller threads

    for (i = 0; i < 10; i++)
        pthread_join(tids[i], NULL);
    // Printout simulation result............
    return(0);
}


int main(int argc, char* argv[]) {
    if(argc != 2){
        cout << "Wrong number of arguments!" << endl;
        return -1;
    }
    num_customers = atoi(argv[1]);
    turned_away=0;
    h_customers=0;
    m_customers=0;
    l_customers=0;
    windows_closed = false;
    start_simulation();
    print_seating_chart(seat_chart);
    cout<< "Number of H customers = " << h_customers << endl;
    cout<< "Number of M customers = " << m_customers << endl;
    cout<< "Number of L customers = " << l_customers << endl;
    cout<< "Number of turned away = " << turned_away << endl;

    return 0;
}

