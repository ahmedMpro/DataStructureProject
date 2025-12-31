#include "Logic/disjoint_set.h"  

DisjointSet::DisjointSet(int size)  
        : parent_(size), 
            rank_(size, 0)  
{
    for (int index = 0; index < size; ++index) {  
        parent_[index] = index; 
    }
}

int DisjointSet::find(int node)  
{
    if (parent_[node] != node) { 
        parent_[node] = find(parent_[node]);  
    }

    return parent_[node];  
}

void DisjointSet::unionSets(int firstRoot, int secondRoot)  
{
    if (firstRoot == secondRoot) {  
        return;  
    }

    if (rank_[firstRoot] < rank_[secondRoot]) { 
        parent_[firstRoot] = secondRoot;  
    } else if (rank_[secondRoot] < rank_[firstRoot]) {
        parent_[secondRoot] = firstRoot;  
    } else {
        parent_[secondRoot] = firstRoot;  
        ++rank_[firstRoot];  
    }
}
