#include "priority_queue.h"

PriorityQue::PriorityQue(): size (0), capacity(32) {
    Tab = new std::pair<int,int>[capacity];
}

PriorityQue::PriorityQue(int s): size (0), capacity(s) {
    Tab = new std::pair<int,int>[capacity];
}

PriorityQue::~PriorityQue(){
    delete [] Tab;
}

bool PriorityQue::is_empty() const {
    return (size == 0);
}

std::pair<int,double> PriorityQue::pop_min(){
    if (size > 0) {
        size--;
        return Tab[size];
    }
    return std::pair<int,double>(-1, -1);
}

void PriorityQue::put(const std::pair<int,double>& p){

    if(size == capacity){

        capacity *= 2;
        std::pair<int,int>* Tab1 = new std::pair<int,int>[capacity];
        for(int i = 0; i < size; i ++){
            Tab1[i] = Tab[i];
        }
        delete [] Tab;
        Tab = Tab1;
    }

    int j = size - 1;
    while(j >= 0 && p.second > Tab[j].second){
        Tab[j + 1] = Tab[j];
        j--;
    }
    Tab[j + 1] = p;
    size++;
}
