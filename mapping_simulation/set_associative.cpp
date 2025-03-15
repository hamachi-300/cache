// set_associative.cpp - Fixed implementation of set associative functions
#include <iostream>
#include <cmath>
#include <vector>
#include <bitset>
#include "cache_functions.h"

using namespace std;

void print_set_associative(vector<string> set_column, vector<string> valid_column, vector< vector<string> > tag_column, vector< vector<string> > data_column, int cache_size) {
    
    int is = 0, vs = 0, ts = 0, ds = 0;
    if (set_column[0].size() > 3){
        is = set_column[0].size() - 3;
    }
    if (valid_column[0].size() > 1){
        vs = valid_column[0].size() - 1;
    }
    if (tag_column[0][0].size() > 3 + 2){
        ts = tag_column[0][0].size() - 3 - 2;
    }
    if (data_column[0][0].size() > 4 + 2){
        ds = data_column[0][0].size() - 4 - 2;
    }

    string iss = "", vss = "", tss = "", dss = "";
    for (int i = 0; i < is; i++){
        iss += " ";
    }
    for (int i = 0; i < vs; i++){
        vss += " ";
    }
    for (int i = 0; i < ts; i++){
        tss += " ";
    }
    for (int i = 0; i < ds; i++){
        dss += " ";
    }

    string column_title = "";

    if (set_column[0].size() != 0) {
        column_title += " | set" + iss;
    }
    column_title += " | V" + vss + " | ";
    for (int i = 0; i < tag_column[0].size(); i++){
        column_title += "tag " + to_string(i) + tss + " | data " + to_string(i) + dss + " | ";
    }

    for (int i = 0; i < column_title.size(); i++){
        cout << "-";
    }
    cout << endl;

    cout << column_title << endl;

    for (int i = 0; i < column_title.size(); i++){
        cout << "-";
    }
    cout << endl;

    for (int i = 0; i < set_column.size(); i++){
        if (set_column[0].size() != 0) {
            cout << " | " << set_column[i];
        }
        cout << " | " << valid_column[i];
        for (int j = 0; j < tag_column[i].size(); j++){
            cout << " | " << tag_column[i][j];
            cout << " | " << data_column[i][j];
        }
        cout << " | ";
        cout << endl;
    }

    for (int i = 0; i < column_title.size(); i++){
        cout << "-";
    }
    cout << endl;
}

void set_associative(int cache_size, int address_bits, int way){
    int hit = 0, count = 0;

    // calculate cache_size
    int original_cache_size = cache_size; // Save original size
    cache_size = ceil(cache_size/(way*1.0));

    // log base 2 of cache_size = bits of sets
    int cache_set_bits = ceil(log2(cache_size)); // Fixed: using log2 directly

    // create set column
    vector<string> set_column;
    for (int i = 0; i < cache_size; i++){
        bitset<32> bits(i);
        set_column.push_back(bits.to_string().substr(32-cache_set_bits));
    }

    // create valid bit column
    vector<string> valid_column;
    for (int i = 0; i < cache_size; i++){
        valid_column.push_back("0");
    }

    // create tag column
    string space = "";
    for (int i = 0; i < address_bits - cache_set_bits; i++){
        space += " ";
    }
    vector< vector<string> > tag_column(cache_size);
    for (int i = 0; i < cache_size; i++){
        for (int j = 0; j < way; j++){
            tag_column[i].push_back(space);
        }
    }

    // create data column
    vector< vector<string> > data_column(cache_size);
    for (int i = 0; i < cache_size; i++){
        for (int j = 0; j < way; j++){
            string space = "";
            for (int k = 0; k < address_bits; k++){ // Fixed: used k instead of i
                space += " ";
            }
            space += "        ";
            data_column[i].push_back(space);
        }
    }

    print_set_associative(set_column, valid_column, tag_column, data_column, cache_size);

    cout << "Enter -1 to exit.." << endl;
    while (true) {
        int input;
        cout << "Enter Input (" << "0 - " << pow(2, address_bits) - 1 << "): ";
        cin >> input;

        // check exit
        if (input == -1) {
            break;
        }

        // check valid input
        if (input > pow(2, address_bits) - 1 || input < 0) {
            cout << "Error : Your input are out of range try again!!" << endl;
            continue;
        }

        count++;

        cout << endl << endl << "================================================================================" << endl << endl;

        // convert input to bit
        bitset<32> bits(input);
        string input_bit = bits.to_string().substr(32-address_bits);
        cout << input << " --> " << input_bit << endl;

        // Fixed: Correctly calculate set index
        int set_index = input % cache_size;
        string set_bits = bits.to_string().substr(32-cache_set_bits);
        string tag_bits = bits.to_string().substr(32-address_bits, address_bits-cache_set_bits);

        cout << "set : " << set_bits << endl
             << "tag : " << tag_bits << endl;

        // check hit
        string is_hit = "miss";

        for (int i = 0 ; i < tag_column[0].size(); i++) {
            if (valid_column[set_index] == "1" && tag_column[set_index][i] == tag_bits){
                is_hit = "hit";
                hit++;

                // LRU (Least Recently Used) implement
                string tempTag = tag_column[set_index][i];
                string tempData = data_column[set_index][i];

                // remove current tag and data 
                tag_column[set_index].erase(tag_column[set_index].begin() + i);
                data_column[set_index].erase(data_column[set_index].begin() + i);

                // insert tag and data at first
                tag_column[set_index].insert(tag_column[set_index].begin(), tempTag);
                data_column[set_index].insert(data_column[set_index].begin(), tempData);

                break;
            } 

            // if it is last and can't hit 
            if (i == tag_column[0].size()-1){
                // Fixed: simplified miss handling logic
                bool found_empty = false;
                
                // First look for empty slots
                for (int j = 0; j < tag_column[set_index].size(); j++) {
                    if (tag_column[set_index][j] == space) {
                        // Set valid bit to 1
                        valid_column[set_index] = "1";
                        
                        // Set tag and data
                        tag_column[set_index][j] = tag_bits;
                        data_column[set_index][j] = "Memory(" + input_bit + ")";
                        
                        found_empty = true;
                        break;
                    }
                }
                
                // If no empty slot found, replace the last one (LRU)
                if (!found_empty) {
                    int last_idx = tag_column[set_index].size() - 1;
                    
                    // Set valid bit to 1
                    valid_column[set_index] = "1";
                    
                    // Set tag and data
                    tag_column[set_index][last_idx] = tag_bits;
                    data_column[set_index][last_idx] = "Memory(" + input_bit + ")";
                }
                break; // Fixed: added break to exit the loop
            }
        }

        print_set_associative(set_column, valid_column, tag_column, data_column, cache_size);
        cout << endl;
        // output result
        cout << "Decimal address of reference                 : " << input << endl << endl
             << "Binary address of reference                  : " << input_bit << endl << endl
             << "Hit or miss in cache                         : " << is_hit << endl << endl
             << "Assigned tag                                 : " << tag_bits << endl << endl;

        if (set_column[0].size() != 0) {
            cout << "Assigned cache block (where found or placed) : " << input_bit << " mod " << cache_size << " = " << set_bits << endl << endl;
        }
    }
    // calculate hit rate
    double hit_rate = (count > 0) ? (hit * 100.0) / count : 0; // Fixed: handle divide by zero
    cout << endl << "Hit rate : " << hit_rate << "%" << endl;
}

void start_set_associative(){
    cout << "===== N-Way Set Associative =====" << endl << endl;
    
    int way;
    int cache_size;
    int address_bits;
    
    // Fixed: Added proper validation
    way_input:
    cout << "Choose your way of set associative (1 or more): ";
    cin >> way;
    
    if (cin.fail() || way < 1) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Way must be at least 1." << endl << endl;
        goto way_input;
    }

    // input size of cache (16 bit is in range 0 - 65535)
    cache_input:
    cout << "Input cache block size (" << way << " - 65535) : ";
    cin >> cache_size;
    
    if (cin.fail() || cache_size < way || cache_size > 65535) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Your input is invalid try again!!" << endl << endl;
        goto cache_input;
    }

    // input bits of address (range log2(cache_size) - 16)
    int min_address_bits = ceil(log2(cache_size / way));
    if (min_address_bits < 1) min_address_bits = 1; // Fixed: ensure minimum of 1 bit
    
    address_input:
    cout << "Input bit of address (" << min_address_bits << " - 16) : ";
    cin >> address_bits;
    
    if (cin.fail() || address_bits < min_address_bits || address_bits > 16) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Your input is invalid try again!!" << endl << endl;
        goto address_input;
    }

    set_associative(cache_size, address_bits, way);
}

void start_fully_associative(){
    cout << "===== Fully Way Associative =====" << endl << endl;
    
    int cache_size;
    int address_bits;
    
    // input size of cache
    cache_input:
    cout << "Input cache block size (1 - 65535) : "; // Fixed: minimum 1 instead of 0
    cin >> cache_size;
    
    if (cin.fail() || cache_size < 1 || cache_size > 65535) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Your input is invalid try again!!" << endl << endl;
        goto cache_input;
    }

    // input bits of address
    int min_address_bits = 1; // Fixed: ensure minimum of 1 bit
    
    address_input:
    cout << "Input bit of address (" << min_address_bits << " - 16) : ";
    cin >> address_bits;
    
    if (cin.fail() || address_bits < min_address_bits || address_bits > 16) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Your input is invalid try again!!" << endl << endl;
        goto address_input;
    }

    // In fully associative cache, way equals cache_size
    int way = cache_size;
    
    set_associative(cache_size, address_bits, way);
}