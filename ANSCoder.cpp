#include "priority_queue.h"
#include "ANSCoder.h"

bool save_only_prob = true;

ModelData ANSCoder::analyze_text(const std::vector<std::string>& raw_text) {
    //create table of symbols form text and calculate their frequencies
    ModelData m_data;
    double line_num = 0.0;

    m_data.symb_map['\n'] = 0;
    m_data.symb.push_back('\n');
    m_data.counts.push_back(0);

    for (std::string s : raw_text) {
        for (char c : s) {
            m_data.text.push_back(c);
            line_num++;

            auto search = m_data.symb_map.find(c);
            if (search != m_data.symb_map.end()) {
                m_data.counts[search->second]++;
            } else {
                m_data.counts.push_back(1);
                m_data.symb.push_back(c);
                m_data.symb_map[c] = m_data.symb.size()-1;
            }
        }
        m_data.text.push_back('\n');
        m_data.counts[0]++;
        line_num++;
    }
    return m_data;
}

ModelData ANSCoder::analyze_file(std::ifstream& ifStream){

    std::vector<std::string> raw_text;
    std::string word;
    while( getline(ifStream, word) ){
        raw_text.push_back(word);
    }

    return analyze_text(raw_text);
}

ModelData ANSCoder::analyze_cin(int num, std::istream& iStream){

    std::vector<std::string> raw_text;
    std::string word;
    for(int i = 0; i <= num; i ++ ){
        getline(iStream, word);
        raw_text.push_back(word);
    }

    return analyze_text(raw_text);
}

void ANSCoder::build(bool from_scratch){
    //in this case any initial state from the range (states_num, 2*states_num-1) works
    if (from_scratch) curr_state = states_num + alph_size - 1;
    //if not we are reconstructing ANSCoder to de ode some file, and x was already set

    PriorityQue que(alph_size);
    for(int i = 0; i < alph_size; i ++){
        que.put({i, inv_prob[i]});
    }

    // constructing Decoder table and sym_mult_tab
    Decoder.assign(states_num, {0, 0});

    //each symbol should appear at lest once in decoder
    num_sym_appear.assign(alph_size,1);
    for(int i = 0; i < alph_size; i ++){ //to make each symbol appears at least once
        Decoder[i] =  {i,0};
    }

    //add consecutive symbol appearances according to frequencies
    for(int x = alph_size; x < states_num; x++){
        std::pair<int,int> p = que.pop_min();
        que.put({p.first, p.second + (2.0*inv_prob[p.first])});

        Decoder[x] = {p.first,0};
        num_sym_appear[p.first] ++;
    }

    //fill the next state filed in decoder
    std::vector<int> current_count = num_sym_appear;
    for(int x = 0; x < states_num; x ++){
        Decoder[x].second = current_count[Decoder[x].first];
        current_count[Decoder[x].first] ++;
    }


    //constructing Encoder table

    Encoder.assign(alph_size,std::vector<int>());
    for(int i = 0; i < alph_size; i ++){
        Encoder[i].resize(num_sym_appear[i]);
    }

    for(int x = 0; x < states_num; x ++){
        int symbol = Decoder[x].first;
        int appearance_idx = Decoder[x].second - num_sym_appear[symbol];
        Encoder[symbol][appearance_idx] = x + states_num;
    }
}

ANSCoder::ANSCoder(const ModelData& m_data, int states_multiplicity)
    : alph_size(m_data.symb.size()), states_num(m_data.symb.size()*states_multiplicity),
      symb(m_data.symb), symb_map(m_data.symb_map){
    for(int i = 0; i < alph_size; i ++){
        int i_prob = static_cast<int>(std::floor(1.0 /m_data.counts[i]));
        inv_prob.push_back(i_prob);
    }

    build(true);
}

ANSCoder::~ANSCoder(){}

std::vector<char> ANSCoder::decode(std::vector<bool>& v){
    //returns the table of characters

    std::vector<char> word1;
    bool all_digested = false;
    while(!v.empty()) {
        //read pair (symbol, new state)
        std::pair<int,int> p = Decoder[curr_state - states_num];
        curr_state=p.second;

        //digest bits from stream until state is in allowed range (states_num, 2*states_num+1)
        while(curr_state < states_num){
            //shift current state and add reminder
            curr_state = (curr_state << 1) | static_cast<int>(v.back());
            if(v.empty()){
                all_digested = true;
                break;
            }
            v.pop_back();
        }
        //add decoded symbol
        word1.push_back(symb[p.first]);
    }
    //if bitstream ended, the last symbol is incomplete;
    if(all_digested)  word1.pop_back();

    std::reverse(word1.begin(), word1.end());

    return word1;
}

void ANSCoder::encode(std::vector<bool>& v,const std::vector<char>& word){
    //code 'word' and append it to 'v'

    for(char c : word){
        int i = symb_map[c];
        //state normalization - push extra bits to stream
        int upper_bound = ((2 * num_sym_appear[i]) - 1);
        while(curr_state > upper_bound){
            v.push_back(static_cast<bool>(curr_state & 1));// bit-wise 'and' instead of mod 2
            curr_state >>= 1; //bit shift instead of division by 2
        }
        curr_state = Encoder[i][curr_state - num_sym_appear[i]];
    }
    return;
}


void ANSCoder::save(std::ofstream&  stream) const{

    stream.write(reinterpret_cast<const char *>(&alph_size), sizeof(alph_size));//save main parameters
    stream.write(reinterpret_cast<const char *>(&states_num), sizeof(states_num));
    stream.write(reinterpret_cast<const char *>(&curr_state), sizeof(curr_state));


    stream.write(reinterpret_cast<const char*>(symb.data()), symb.size() * sizeof(char));//save alphabet
    if(save_only_prob) {
        stream.write(reinterpret_cast<const char *>(&inv_prob[0]), alph_size * sizeof(inv_prob[0]));

    } else {
        for (int i = 0; i < alph_size; i++) { //save numbers of symbol apperances
            stream.write(reinterpret_cast<const char *>(&num_sym_appear[i]), sizeof(num_sym_appear[i]));
        }
        for (int i = 0; i < states_num; i++) {//save decoder, from which encoder can be reconstructed
            stream.write(reinterpret_cast<const char *>(&(Decoder[i].first)), sizeof(Decoder[i].first));
            stream.write(reinterpret_cast<const char *>(&(Decoder[i].second)), sizeof(Decoder[i].second));
        }
    }
}

ANSCoder::ANSCoder(std::ifstream& stream){

    stream.read(reinterpret_cast<char *>(&alph_size), sizeof(alph_size));//read main parameters
    stream.read(reinterpret_cast<char *>(&states_num), sizeof(states_num));
    stream.read(reinterpret_cast<char *>(&curr_state), sizeof(curr_state));

    char c;
    for(int i = 0; i < alph_size; i ++){//read alphabet
        stream.read(&c,sizeof(c));
        symb.push_back(c);
    }

    for(int i = 0; i < alph_size; i++){ //create translation in both directions
        symb_map[symb[i]] = i;
    }

    if(save_only_prob) {

        int q;
        for (int i = 0; i < alph_size; i++) {
            stream.read(reinterpret_cast<char *>(&q), sizeof(int));
            inv_prob.push_back(q);
        }

        build(false);
    } else {

        num_sym_appear = std::vector<int> (alph_size);
        for (int i = 0; i < alph_size; i++) { //read number of appearances for each symbol
            stream.read(reinterpret_cast<char *>(&num_sym_appear[i]), sizeof(num_sym_appear[i]));
        }

        Decoder = std::vector<std::pair<int,int>> (states_num);

        for (int i = 0; i < states_num; i++) {//read decoder
            std::pair<int,int> p;
            Decoder[i] = p;
            stream.read(reinterpret_cast<char *>(&(Decoder[i].first)), sizeof(Decoder[i].first));
            stream.read(reinterpret_cast<char *>(&(Decoder[i].second)), sizeof(Decoder[i].second));
        }

        Encoder = std::vector<std::vector<int>> (alph_size); //reconstruct encoder
        for (int i = 0; i < alph_size; i++) {
            Encoder[i] = std::vector<int> (num_sym_appear[i]);
        }

        for (int x = 0; x < states_num; x++) {
            Encoder[Decoder[x].first][Decoder[x].second - num_sym_appear[Decoder[x].first]] = x + states_num;
        }
    }
}




