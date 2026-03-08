#ifndef ANS_PRIORITY_QUEUE
#define ANS_PRIORITY_QUEUE

#include <utility>

class PriorityQue{
//Low level implementation of priority queue in the table, just for fun
private:
    int size;
    int capacity;
    std::pair<int,int>* Tab;

public:
    PriorityQue();

    PriorityQue(int s);

    ~PriorityQue();

    bool is_empty() const;

    std::pair<int,double> pop_min();

    void put(const std::pair<int,double>& p);
};

#endif //ANS_PRIORITY_QUEUE