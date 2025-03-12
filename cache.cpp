#include <iostream>
#include <cmath>
#include <vector>
#include <bitset>
using namespace std;

void print_direct_map(vector<string> index_column, vector<string> valid_column, vector<string> tag_column, vector<string> data_column, int cache_size) {
    string size = " | " + index_column[0] + " | " + valid_column[0] + " | " + tag_column[0] + " | " + data_column[0] + " | ";
    for (int i = 0; i < size.size(); i++) {
        cout << "-";
    }
    cout << endl;

    int is = 0, vs = 0, ts = 0, ds = 0;
    if (index_column[0].size() > 3){
        is = index_column[0].size() - 3;
    }
    if (valid_column[0].size() > 1){
        vs = valid_column[0].size() - 1;
    }
    if (tag_column[0].size() > 3){
        ts = tag_column[0].size() - 3;
    }
    if (data_column[0].size() > 4){
        ds = data_column[0].size() - 4;
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

    cout << " | idx" << iss << " | V" << vss << " | tag" << tss << " | data" << dss << " |" << endl;

    for (int i = 0; i < size.size(); i++) {
        cout << "-";
    }
    cout << endl;
    for (int i = 0; i < cache_size; i++){
        cout << " | " + index_column[i] + " | " + valid_column[i] + " | " + tag_column[i] + " | " + data_column[i] + " | " << endl; 
    }
    for (int i = 0; i < size.size(); i++) {
        cout << "-";
    }
    cout << endl;
}

void direct_map(int cache_size, int address_bits) {

    int hit = 0, count = 0;

    // log base 2 of cache_size = bits of indexs
    int cache_index_bits = ceil(log(cache_size) / log(2));

    // create index column
    vector<string> index_column;
    for (int i = 0; i < cache_size; i++){
        bitset<32> bits(i);
        index_column.push_back(bits.to_string().substr(32-cache_index_bits));
    }

    // create valid bit column
    vector<string> valid_column;
    for (int i = 0; i < cache_size; i++){
        valid_column.push_back("0");
    }

    // create tag column
    vector<string> tag_column;
    for (int i = 0; i < cache_size; i++){
        string space = "";
        for (int i = 0; i < address_bits - cache_index_bits; i++){
            space += " ";
        }
        tag_column.push_back(space);
    }

    // create data column
    vector<string> data_column;
    for (int i = 0; i < cache_size; i++){
        string space = "";
        for (int i = 0; i < address_bits; i++){
            space += " ";
        }
        space += "        ";
        data_column.push_back(space);
    }
    
    print_direct_map(index_column, valid_column, tag_column, data_column, cache_size);
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

        // convert input to bit
        bitset<32> bits(input);
        string input_bit = bits.to_string().substr(32-address_bits);
        cout << input << " --> " << input_bit << endl;

        string index_bits = bits.to_string().substr(32-cache_index_bits);
        string tag_bits = bits.to_string().substr(32-address_bits, address_bits-cache_index_bits);

        cout << "index : " << index_bits << endl
             << "tag : " << tag_bits << endl;

        // check hit
        string isHit = "miss";
        if (valid_column[(input % cache_size)] == "1" && tag_column[(input % cache_size)] == tag_bits){
            isHit = "hit";
            hit++;
        } else {
            // set valid bit 1
            valid_column[(input % cache_size)] = "1";

            // set tag column
            tag_column[(input % cache_size)] = tag_bits;

            // set data column
            data_column[(input % cache_size)] = "Memory(" + input_bit + ")";
        }

        print_direct_map(index_column, valid_column, tag_column, data_column, cache_size);
        cout << endl;
        // output result
        cout << "Decimal address of reference                 : " << input << endl << endl
             << "Binary address of reference                  : " << input_bit << endl << endl
             << "Hit or miss in cache                         : " << isHit << endl << endl
             << "Assigned cache block (where found or placed) : " << input_bit << " mod " << cache_size << " = " << index_bits << endl << endl
             << "Assigned tag                                 : " << tag_bits << endl << endl;
    }
    // caculate hit rate
    double hit_rate =( hit*1.) / (count*1.);
    cout << endl << "Hit rate : " << hit_rate * 100 << "%" << endl;
}

void start_direct_map(){
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

    if (cache_size > 65535 || cache_bits > 16 || cache_bits < ceil(log(cache_size) / log(2))){
        cout << "Your input is invalid try again!!" << endl << endl;
        goto try_again;
    }

    direct_map(cache_size, address_bits);
}

void start_set_associative(){
    int way = 2;

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

    if (cache_size > 65535 || cache_bits > 16 || cache_bits < ceil(log(cache_size) / log(2))){
        cout << "Your input is invalid try again!!" << endl << endl;
        goto try_again;
    }

    direct_map(cache_size, address_bits);

}

int main(){
    
    start_direct_map();

    return 0;
}