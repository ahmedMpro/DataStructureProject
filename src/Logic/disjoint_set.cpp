#include "Logic/disjoint_set.h"  // include header declaring DisjointSet

DisjointSet::DisjointSet(int size)  // constructor definition for DisjointSet
    : parent_(size),  // initialize parent array with required size
      rank_(size, 0)  // initialize rank array with zeros
{
    for (int index = 0; index < size; ++index) {  // fill parent array with each node as its own parent
        parent_[index] = index;  // set parent to self for root initialization
    }
}

int DisjointSet::find(int node)  // find with path compression
{
    if (parent_[node] != node) {  // if node is not its own parent
        parent_[node] = find(parent_[node]);  // recursively compress path by pointing to root
    }

    return parent_[node];  // return the representative of the node's set
}

void DisjointSet::unionSets(int firstRoot, int secondRoot)  // union by rank
{
    if (firstRoot == secondRoot) {  // if they already share the same root, nothing to do
        return;  // early exit when merging identical sets
    }

    if (rank_[firstRoot] < rank_[secondRoot]) {  // attach the shorter tree under the taller tree
        parent_[firstRoot] = secondRoot;  // move first root under second root
    } else if (rank_[secondRoot] < rank_[firstRoot]) {
        parent_[secondRoot] = firstRoot;  // attach second tree under first root
    } else {
        parent_[secondRoot] = firstRoot;  // tie-breaking by choosing first as new parent
        ++rank_[firstRoot];  // increment rank to reflect increased height
    }
}
