
#ifndef USER_EQUILIBRIUM_FRINGESEARCH_H
#define USER_EQUILIBRIUM_FRINGESEARCH_H

#include <list>
#include <vector>
#include <unordered_map>
#include <utility>

#include "FringeGraph.h"

typedef std::pair<BaseFringeNode*, edge_weight_t> node_parent_t;

/**
 * Implementation of the fringe search algorithm.
 */
class FringeSearch {

    // searchID counter
    static std::size_t nextSearchID;

    // The ID of this search, used to see if search data was created by this search
    std::size_t searchID;

    // The first node of the fringe
    BaseFringeNode* fringeStart;

    // The last node of the fringe
    BaseFringeNode* fringeEnd;

    // The search' starting node
    BaseFringeNode* start;

public:
    /**
     * Create a fringe search instance, but do not initialize.
     *
     * Call reset() before calling search().
     */
    FringeSearch();

    /**
     * Initialize the search given a source node.
     *
     * @param start The source node
     */
    FringeSearch(BaseFringeNode* start);

    /**
     * Search for a target node.
     *
     * @param end The target node.
     * @return The nodes to visit excluding start, including end
     */
    std::vector<BaseFringeNode*>* search(BaseFringeNode* end);

    /**
     * Get the cost to the given target node.
     *
     * Call search() before calling this.
     *
     * @param end The target node
     * @return cost The cost of the path
     */
    edge_weight_t cost(BaseFringeNode* end);

    /**
     * Reset the search so a search with a different starting node can start
     *
     * @param start The new starting node
     */
    void reset(BaseFringeNode* start);

private:
    void removeFromFringe(const BaseFringeNode *node);

    void allocateSearchData(BaseFringeNode* node);

    void setStartingNode(BaseFringeNode* start);
};

#endif //USER_EQUILIBRIUM_FRINGESEARCH_H
