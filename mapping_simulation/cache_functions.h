#ifndef CACHE_FUNCTIONS_H
#define CACHE_FUNCTIONS_H

#include <vector>
#include <string>

using namespace std;

// direct map functions
void print_direct_map(vector<string> index_column, vector<string> valid_column, vector<string> tag_column, vector<string> data_column, int cache_size);
void direct_map(int cache_size, int address_bits);
void start_direct_map();

// set associative functions
void print_set_associative(vector<string> set_column, vector<string> valid_column, vector< vector<string> > tag_column, vector< vector<string> > data_column, int cache_size);
void set_associative(int cache_size, int address_bits, int way);
void start_set_associative();
void start_fully_associative();

#endif