
#ifndef USER_EQUILIBRIUM_FRINGEGRAPH_H
#define USER_EQUILIBRIUM_FRINGEGRAPH_H

#include <cstdint>
#include <vector>
#include <memory>

typedef uint32_t node_id_t;
typedef uint32_t edge_id_t;
typedef float edge_weight_t;

// Forward declarations for circular reference
class BaseFringeEdge;
class BaseFringeNode;
template<class data_t>
struct FringeSearchHeuristic;
template<class data_t>
struct FringeEdgeWeightCalculation;
class FringeSearch;

struct FringeSearchData {
    // Current best previous node
    BaseFringeNode* previous;
    // Current best cost to get from start to this node
    edge_weight_t g;
    // Cached heuristic value
    edge_weight_t h;
    // Doubly linked list variables
    BaseFringeNode* fringeNext;
    BaseFringeNode* fringePrevious;
    // ID of the search this data belongs to
    std::size_t searchID;
};

class BaseFringeNode {

    friend class FringeSearch;

    node_id_t id;

    std::vector<BaseFringeEdge*> incoming;
    std::vector<BaseFringeEdge*> outgoing;

    /*
     * Variables used in Fringe Search
     */
    FringeSearchData* fringeSearchData;


public:
    BaseFringeNode(node_id_t id);

    virtual ~BaseFringeNode();

    BaseFringeNode& operator=(const BaseFringeNode& other);

    /**
     * Get the unique ID of this node
     *
     * @return A unique ID
     */
    node_id_t getID();

    /**
     * Get the incoming edges.
     *
     * @return The incoming edges
     */
    std::vector<BaseFringeEdge*>& getIncoming();

    /**
     * Get the outgoing edges
     *
     * @return The outgoing edges
     */
    std::vector<BaseFringeEdge*>& getOutgoing();

    /**
     * Find an edge that is outgoing from this node and incoming to other.
     *
     * @param other The node to search an imcoming edge for
     * @return An incident edge, or nullptr if such an edge does not exist
     */
    BaseFringeEdge* getIncident(BaseFringeNode* other);

    /**
     * Add an incoming edge.
     *
     * @param edge The edge to add
     */
    void addIncoming(BaseFringeEdge* edge);

    /**
     * Add an outgoing edge.
     *
     * @param edge The edge to add
     */
    void addOutgoing(BaseFringeEdge* edge);

    /**
     * Calculate the heuristic value h from this node to to, used by fringe search.
     *
     * @param to The node to calculate the heuristic to
     * @return The heuristic value, always 0 in the base implementation
     */
    virtual edge_weight_t calculateHeuristic(BaseFringeNode* to);
};

/**
 * The default node type.
 *
 * This type should be used when constructing a graph with custom fringe
 * search heuristic.
 *
 * To change the heuristic function, instantiate FringeSearchHeuristic.
 *
 * The data type should have a copy constructor. The data stored in nodes
 * is owned by the nodes and deleted when the node is deleted.
 *
 * @tparam data_t The type of data stored in this node and to use the heuristic function of
 */
template <class data_t>
class FringeNode : public BaseFringeNode {

    data_t* data;

public:
    FringeNode(node_id_t id) : BaseFringeNode(id), data(nullptr) {}

    FringeNode(node_id_t id, data_t* data) : BaseFringeNode(id), data(data) {}


    edge_weight_t calculateHeuristic(BaseFringeNode *to) override {
        FringeNode *fringeNodeTo = static_cast<FringeNode*>(to);
        return FringeSearchHeuristic<data_t>::h(this, fringeNodeTo);
    }

    FringeNode &operator=(const FringeNode<data_t> &other) {
        data = new data_t(*other.data);
        BaseFringeNode::operator=(other);
        return *this;
    }

    data_t* getData() {
        return data;
    }

    ~FringeNode() {
        if (data != nullptr) {
            delete data;
        }
    }
};

class BaseFringeEdge {

    edge_id_t id;

    BaseFringeNode* from;
    BaseFringeNode* to;

    edge_weight_t weight;

public:
    /**
     * Create a new edge.
     *
     * Use setFrom(), setTo() and setWeight() to fill this edge.
     */
    BaseFringeEdge(edge_id_t id);

    /**
     * Create a new edge.
     *
     * Will add this edge to the from and to nodes.
     *
     * @param from Source node
     * @param to Target node
     * @param weight Default edge weight
     */
    BaseFringeEdge(edge_id_t id, BaseFringeNode* from, BaseFringeNode* to, edge_weight_t weight);

    virtual ~BaseFringeEdge();

    BaseFringeEdge& operator=(const BaseFringeEdge& other);

    /**
     * Calculate the weight given the cost to go from the initial node
     * (used to start the fringe search) to this node.
     *
     * @param costToFrom The cost to go to the from node of this edge, not used in the base implementation
     * @return The weight
     */
    virtual edge_weight_t calculateWeight(edge_weight_t costToFrom);

    /**
     * Get this edge's ID.
     *
     * @return The ID
     */
    edge_id_t getID();

    /**
     * Get the source node.
     *
     * @return The source node
     */
    BaseFringeNode* getFrom();

    /**
     * Set the source node.
     *
     * Will add this edge to the node as outgoing edge.
     *
     * @param node The source node
     */
    void setFrom(BaseFringeNode* node);

    /**
     * Get the target node.
     *
     * @return The target node
     */
    BaseFringeNode* getTo();

    /**
     * Set the target node.
     *
     * Will add this edge to the node as incoming edge.
     *
     * @param node The target node
     */
    void setTo(BaseFringeNode* node);

    /**
     * Get the default weight of this edge.
     *
     * @return The weight
     */
    edge_weight_t getWeight();

    /**
     * Set the default weight of this edge.
     *
     * @param weight The weight
     */
    void setWeight(edge_weight_t weight);
};

/**
 * The default edge type.
 *
 * This type should be used when constructing a graph with a custom weight function.
 *
 * To change the weight calculation function, instantiate FringeEdgeWeightCalculation.
 *
 * @tparam node_t
 */
template <class data_t>
class FringeEdge : public BaseFringeEdge {

    data_t* data;

public:
    FringeEdge(edge_id_t id, BaseFringeNode *from, BaseFringeNode *to, edge_weight_t weight)
            : BaseFringeEdge(id, from, to, weight), data(nullptr) {}

    FringeEdge(edge_id_t id, BaseFringeNode *from, BaseFringeNode *to, edge_weight_t weight, data_t* data)
            : BaseFringeEdge(id, from, to, weight), data(data) {}

    /**
     * Get the data stored in this edge.
     *
     * @return The data
     */
    data_t* getData() {
        return data;
    }

    /**
     * Set this edge's data.
     *
     * @param data The data
     */
    void setData(data_t* data) {
        this->data = data;
    }

    edge_weight_t calculateWeight(edge_weight_t costToFrom) override {
        return FringeEdgeWeightCalculation<data_t>::weight(this, costToFrom);
    }

    FringeEdge& operator=(const FringeEdge& other) {
        data = new data_t(other.data);
        BaseFringeEdge::operator=(other);
        return *this;
    }

    ~FringeEdge() {
        if (data != nullptr) {
            delete data;
        }
    }
};

/**
 * Instantiate this templated struct to implement custom search heuristics.
 *
 * @tparam data_t The type of data in the nodes passed to the heuristic function
 */
template <class data_t>
struct FringeSearchHeuristic {
    static edge_weight_t h(FringeNode<data_t>* from, FringeNode<data_t>* to);
};

/**
 * Instantiate this templated struct to implement custom weight functions
 *
 * @tparam data_t The type of data in the edges passed to the weight function
 */
template <class data_t>
struct FringeEdgeWeightCalculation {
    static edge_weight_t weight(FringeEdge<data_t>* edge, edge_weight_t costToFrom);
};

typedef FringeEdge<void> fringe_edge_t;
typedef FringeNode<void> fringe_node_t;



#endif //USER_EQUILIBRIUM_FRINGEGRAPH_H
