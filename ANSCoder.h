#ifndef ANS_ANSCODER_H
#define ANS_ANSCODER_H

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>

struct ModelData {
    std::vector<char> text;           // Przetworzony tekst (z nowymi liniami)
    std::vector<char> symb;           // Alfabet
    std::unordered_map<char, int> symb_map; // map symbol->index
    std::vector<int> counts; // unnormalized probabilities
};


class ANSCoder {//Coder translates text to bitstring
private:

//Coder works in binary system

    int alph_size;//alphabet size
    int states_num;//number of states
    int curr_state; //current state

    std::vector<int> inv_prob;// floor of inverse probability

    std::vector<char> symb; // alphabet
    std::unordered_map<char, int> symb_map; // map symbol->index
    std::vector<int> num_sym_appear;    //count how many times each symbol appears in range (states_num,2*states_num+1)

    std::vector<std::pair<int,int>> Decoder;
    std::vector<std::vector<int>> Encoder;

    void build(bool from_scratch);

	static ModelData analyze_text(const std::vector<std::string>& raw_text);

public:

    static ModelData analyze_file(std::ifstream& ifStream);

    static ModelData analyze_cin(int num, std::istream& iStream);

    ANSCoder(const ModelData& m_data, int states_multiplicity);

    ~ANSCoder();

    std::vector<char> decode(std::vector<bool>& v);

    void encode(std::vector<bool>& v, const std::vector<char>& word);

    void save(std::ofstream&  stream) const;

	ANSCoder(std::ifstream& stream);

};


#endif //ANS_ANSCODER_H
