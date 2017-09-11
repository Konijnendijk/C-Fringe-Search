
#include "FringeSearch.h"

#include <limits>
#include <cmath>

std::size_t FringeSearch::nextSearchID = 0;

FringeSearch::FringeSearch() {}

FringeSearch::FringeSearch(BaseFringeNode *start) {
    setStartingNode(start);
}

std::vector<BaseFringeNode*> *FringeSearch::search(BaseFringeNode *end) {
    bool found = false;
    edge_weight_t limit = start->calculateHeuristic(end);

    while (!found && fringeStart != nullptr) {
        edge_weight_t minF = std::numeric_limits<edge_weight_t>::max();

        BaseFringeNode* current = fringeStart;
        BaseFringeNode* next;
        while (current != nullptr) {

            FringeSearchData* currentData = current->fringeSearchData;

            edge_weight_t h;
            if (currentData->h >= 0) {
                h = currentData->h;
            } else {
                h = current->calculateHeuristic(end);
                currentData->h = h;
            }

            edge_weight_t f = currentData->g + h;

            if (f > limit) {
                if (f < minF) {
                    minF = f;
                }
                next = currentData->fringeNext;

                // Do not remove current, so it will implicitly be considered later
            } else {
                // We reached the goal
                if (current == end) {
                    found = true;
                    break;
                }
                // Expand children
                for (BaseFringeEdge* edge : current->getOutgoing()) {
                    edge_weight_t g = currentData->g + edge->calculateWeight(currentData->g);

                    BaseFringeNode *child = edge->getTo();

                    FringeSearchData* childData = child->fringeSearchData;

                    // Did we already consider this child?
                    if (childData!=nullptr && childData->searchID == searchID) {
                        // Do not consider the child if a better route already exists
                        if (g > childData->g) {
                            continue;
                        }
                    } else {
                        allocateSearchData(child);
                        childData = child->fringeSearchData;
                    }
                    childData->previous = current;

                    childData->g = g;

                    // Add the child for immediate consideration, causing it to be removed from elsewhere in the fringe
                    removeFromFringe(child);
                    if (fringeEnd != nullptr) {
                        FringeSearchData* fringeEndData = fringeEnd->fringeSearchData;
                        fringeEndData->fringeNext = child;
                    }
                    childData->fringePrevious = fringeEnd;
                    childData->fringeNext = nullptr;
                    fringeEnd = child;
                }

                next = currentData->fringeNext;

                // Erase current
                removeFromFringe(current);
                currentData->fringeNext = nullptr;
                currentData->fringePrevious = nullptr;
            }
            // Move one forward
            current = next;
        }

        if (found) {
            break;
        }

        limit = minF;
    }

    if (found) {
        std::vector<BaseFringeNode*>* result = new std::vector<BaseFringeNode*>();

        BaseFringeNode* current = end;
        while (current != start) {
            result->push_back(current);
            FringeSearchData* currentData = current->fringeSearchData;
            current = currentData->previous;
        }
        return result;
    } else {
        return nullptr;
    }
}

void FringeSearch::removeFromFringe(const BaseFringeNode *node) {
    FringeSearchData* nodeData = node->fringeSearchData;

    if (node == fringeStart) {
        fringeStart = nodeData->fringeNext;
    } else if (nodeData->fringePrevious != nullptr) {
        FringeSearchData* previousData = nodeData->fringePrevious->fringeSearchData;
        previousData->fringeNext = nodeData->fringeNext;
    }

    if (node == fringeEnd) {
        fringeEnd = nodeData->fringePrevious;
    } else if (nodeData->fringeNext != nullptr) {
        FringeSearchData* nextData = nodeData->fringeNext->fringeSearchData;
        nextData->fringePrevious = nodeData->fringePrevious;
    }
}

edge_weight_t FringeSearch::cost(BaseFringeNode *end) {
    FringeSearchData* endData = end->fringeSearchData;
    return endData->g;
}

void FringeSearch::allocateSearchData(BaseFringeNode *node) {
    FringeSearchData* data = node->fringeSearchData;
    if (data == nullptr) {
        data = new FringeSearchData();
        node->fringeSearchData = data;
    }

    data->previous = nullptr;
    data->g = 0;
    data->h = -1;
    data->fringeNext = nullptr;
    data->fringePrevious = nullptr;
    data->searchID = searchID;
}

void FringeSearch::reset(BaseFringeNode* start) {
    searchID = nextSearchID++;
    setStartingNode(start);
}

void FringeSearch::setStartingNode(BaseFringeNode *start) {
    this->start = start;
    fringeStart = start;
    fringeEnd = start;

    allocateSearchData(start);
}

