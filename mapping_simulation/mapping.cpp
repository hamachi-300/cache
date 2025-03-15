#include <iostream>
#include <cmath>
#include <vector>
#include <bitset>
#include "cache_functions.h"

using namespace std;

int main(){

    start:

    cout << "==============================" << endl
         << "Choose your type of mapping   " << endl
         << "==============================" << endl
         << "1) Direct Mapping             " << endl
         << "2) N-Way Set Associative      " << endl
         << "3) Fully Way Associative      " << endl
         << "4) Exit                       " << endl
         << "Choose : ";

    int input;
    cin >> input;

    if (cin.fail()) {
        cin.clear();
        cin.ignore();
        cout << "Invalid input!!" << endl << endl;
        goto start;
    }

    switch (input){
        case (1):
            start_direct_map();
            break;
        case (2):
            start_set_associative();
            break;
        case (3):
            start_fully_associative();
            break;
        case (4):
            cout << "Exiting...";
            goto end;
            break;
        default:
            cout << "Invalid Input!!" << endl << endl;
            goto start;
    }
    goto start;

    end:

    return 0;
}