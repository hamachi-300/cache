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

    // caculate cache_size
    cache_size = ceil(cache_size/(way*1.0));

    // log base 2 of cache_size = bits of sets
    int cache_set_bits = ceil(log(cache_size) / log(2));

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
            for (int i = 0; i < address_bits; i++){
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

        string set_bits = bits.to_string().substr(32-cache_set_bits);
        string tag_bits = bits.to_string().substr(32-address_bits, address_bits-cache_set_bits);

        cout << "set : " << set_bits << endl
             << "tag : " << tag_bits << endl;

        // check hit
        string is_hit = "miss";

        for (int i = 0 ; i < tag_column[0].size(); i++) {
            if (valid_column[(input % cache_size)] == "1" && tag_column[(input % cache_size)][i] == tag_bits){
                is_hit = "hit";
                hit++;

                // LRU (Least Recently Used) implement
                string tempTag = tag_column[(input % cache_size)][i];
                string tempData = data_column[(input % cache_size)][i];

                // remove current tag and data 
                tag_column[(input % cache_size)].erase(tag_column[(input % cache_size)].begin() + i);
                data_column[(input % cache_size)].erase(data_column[(input % cache_size)].begin() + i);

                // insert tag and data at first
                tag_column[(input % cache_size)].insert(tag_column[(input % cache_size)].begin(), tempTag);
                data_column[(input % cache_size)].insert(data_column[(input % cache_size)].begin(), tempData);

                break;
            } 


            // if it is last and can't hit 
            if (i == tag_column[0].size()-1){
                for (int in = 0; in < tag_column.size(); in++){ 
                    for (int jn = 0 ; jn < tag_column[0].size(); jn++) {
                        // it will replace at block that empty or last one
                        if (tag_column[(input % cache_size)][jn] == space || jn == tag_column[0].size()-1) {
                            // tag valid bit 1
                            valid_column[(input % cache_size)] = "1";
                
                            // tag tag column
                            tag_column[(input % cache_size)][jn] = tag_bits;
                
                            // tag data column
                            data_column[(input % cache_size)][jn] = "Memory(" + input_bit + ")";

                            // move first (LRU)
                            // LRU (Least Recently Used) implement
                            string tempTag = tag_column[(input % cache_size)][jn];
                            string tempData = data_column[(input % cache_size)][jn];

                            // remove current tag and data 
                            tag_column[(input % cache_size)].erase(tag_column[(input % cache_size)].begin() + jn);
                            data_column[(input % cache_size)].erase(data_column[(input % cache_size)].begin() + jn);

                            // insert tag and data at first
                            tag_column[(input % cache_size)].insert(tag_column[(input % cache_size)].begin(), tempTag);
                            data_column[(input % cache_size)].insert(data_column[(input % cache_size)].begin(), tempData);
                            
                            goto next;
                        }
                    }
                }
            }
        }
        next:

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
    // caculate hit rate
    double hit_rate =( hit*1.) / (count*1.);
    cout << endl << "Hit rate : " << hit_rate * 100 << "%" << endl;

}

void start_set_associative(){
    cout << "===== N-Way Set Associative =====" << endl << endl;
    // choose way associative ()
    int way;
    cout << "Choose your way of set associative : ";
    cin >> way;

    // input size of cache (16 bit is in range 0 - 65535)
    try_again:
    cout << "Input cache block size (" << way << " - 65535) : ";
    int cache_size;
    cin >> cache_size;

    // input bits of address (range log2(cache_size) - 16)
    int cache_bits = ceil(log(cache_size) / log(2));
    cout << "Input bit of address (" << cache_bits << " - 16) : ";
    int address_bits;
    cin >> address_bits;

    if (cache_size > 65535 || cache_bits > 16 || cache_bits < ceil(log(cache_size) / log(2)) || way < 1 || address_bits > 16 || cache_size > 65535){
        cout << "Your input is invalid try again!!" << endl << endl;
        goto try_again;
    }

    set_associative(cache_size, address_bits, way);
}

void start_fully_associative(){
    cout << "===== Fully Way Associative =====" << endl << endl;
    // input size of cache (16 bit is in range 0 - 65535)
    try_again:
    cout << "Input cache block size (0 - 65535) : ";
    int cache_size;
    cin >> cache_size;

    // input bits of address (range log2(cache_size) - 16)
    int cache_bits = ceil(log(cache_size) / log(2));
    cout << "Input bit of address (" << cache_bits << " - 16) : ";
    int address_bits;
    cin >> address_bits;

    int way = cache_size;

    if (cache_size > 65535 || cache_bits > 16 || cache_bits < ceil(log(cache_size) / log(2))){
        cout << "Your input is invalid try again!!" << endl << endl;
        goto try_again;
    }

    set_associative(cache_size, address_bits, way);
}