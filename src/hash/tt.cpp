#include "tt.h"

constexpr int TT_SIZE = 1 << 22; // ~4M entries × 24 bytes = ~96MB
static TTEntry TT[TT_SIZE];

TTEntry* ttGet(uint64_t key){
    TTEntry& e=TT[key&(TT_SIZE-1)];
    return (e.depth >= 0 && e.key==key) ? &e : nullptr;
}
void ttStore(uint64_t key,int depth,int score,TTFlag flag,Move best){
    TTEntry& e=TT[key&(TT_SIZE-1)];
    if(e.depth<=depth){  // replace if new entry has deeper search
        e={key,score,depth,flag,best};
    }
}
void ttClear(){
    for(TTEntry& e : TT) e = TTEntry{};
}
