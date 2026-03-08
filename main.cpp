#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>
#include "ANSCoder.h"

using namespace std;

//functions fo read and write from compressed binary files

void binary_write(ofstream&  stream,const vector<bool>& x){
    uint64_t n = x.size();
    stream.write(reinterpret_cast<const char *>(&n), sizeof(n));

    if (n == 0) return;

    uint64_t num_bytes =  (n+7)/8;;
    vector<uint8_t> buffer(num_bytes,0);


    for(uint64_t i = 0; i < n; i ++){
        if (x[i]) {
            buffer[i/8] |= (1 << (i % 8));
        }
    }

    stream.write(reinterpret_cast<const char*>(buffer.data()), num_bytes*sizeof(uint8_t));
}

void binary_read(ifstream& stream, vector<bool>& x){
    uint64_t n;
    if (!stream.read(reinterpret_cast<char *>(&n), sizeof(n))) return;

    //x.clear();
    if (n == 0) return;

    uint64_t num_bytes = (n+7)/8;
    x.resize(n);

    vector<uint8_t>buffer(num_bytes);
    stream.read(reinterpret_cast<char*>(buffer.data()), num_bytes*sizeof(uint8_t));

    for (uint64_t i = 0; i < n; i ++) {
        uint8_t mask = 1 << (i%8);
        x[i] = (buffer[i/8] & mask) != 0;
    }
}

//functions to handle inputs form terminal

void clear_input() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool has_extension(const string& filename, const string& ext) {
    if (filename.size() < ext.size()) return false;
    return filename.compare(filename.size() - ext.size(), ext.size(), ext) == 0;
}

string get_filename(const string& prompt, const string& ext) {
    string name;
    while (true) {
        cout << prompt << " (*." << ext << "):" << endl;
        cin >> name;
        if (has_extension(name, ext)) return name;
        cout << "Wrong extension try again." << endl;
    }
}

int get_number_of_lines() {
    while (true) {
        int num;
        cout<<"Write number of lines to process (>0)"<<endl;
        cin>>num;
        if(num > 0) return num;
        cout<<"Cannot process "<<num<<" lines"<<endl;
    }
}

//compression and decompression methods

void run_compression(bool from_terminal) {
    ModelData m_data;
    if (from_terminal) {
        int num = get_number_of_lines();
        cout<<"Start writing"<<endl;
        m_data =  ANSCoder::analyze_cin(num, cin);

    } else {
        string input_file = get_filename("Write source file name: ","txt");
        ifstream iStream(input_file, ios::in);
        if (!iStream) {
            cerr << "Cannot open the file." << endl;
            return;
        }

        m_data = ANSCoder::analyze_file(iStream);
        iStream.close();
    }

    ANSCoder coder(m_data, 8); // states_multiplicity = 8
    vector<bool> code;
    coder.encode(code, m_data.text);


    string output_file = get_filename("Write destination file name","bin");
    ofstream oStream(output_file, ios::out | ios::binary);
    coder.save(oStream);
    binary_write(oStream, code);
    oStream.close();

    cout << "Compression successful." << endl;
    return;
}

void run_decompression(bool to_terminal) {
    string input_file = get_filename("Write source file name","bin");
    ifstream iStream(input_file, ios::in | ios::binary);
    if (!iStream) {
        cerr << "Cannot open binary file." << endl;
        return;
    }

    ANSCoder coder(iStream);
    vector<bool> code;
    binary_read(iStream, code);
    iStream.close();

    vector<char> decoded = coder.decode(code);

    if (to_terminal) {
        for (char c : decoded) cout << c;
        cout << endl;
    } else {
        string output_file = get_filename("Write destination file name","txt");
        ofstream oStream(output_file, ios::out);
        for (char c : decoded) oStream << c;
        oStream.close();
        cout << "Decompression successful." << endl;
    }
}



int main(){

    int flag;

    while(true) {
        cout<<"ANS Compresion system for text files. \n Choose operation: \n"
              "1: Compression from file into file, \n"
              "2: Decompression from file into file, \n"
              "3: Decompression form file to terminal \n"
              "4: Compression form terminal into file \n"
              "5: Exit \n Option: "<<endl;

        if (!(cin>>flag)) {
            cin.clear();
            clear_input();
        }

        switch (flag) {
            case 1:
                run_compression(false);
                break;
            case 2:
                run_decompression(false);
                break;
            case 3:
                run_decompression(true);
                break;
            case 4:
                run_compression(true);
                break;
            case 5:
                return 0;
        }
    }
    return 0;
}