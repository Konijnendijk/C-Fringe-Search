
#include "FringeGraph.h"

// Default heuristic implementation, always underestimates so is admissible
template <>
edge_weight_t FringeSearchHeuristic<void>::h(FringeNode<void>* from, FringeNode<void>* to) {
    return 0;
}

/*
 * FringeNode implementation
 */

BaseFringeNode::BaseFringeNode(node_id_t id) : id(id), fringeSearchData(nullptr) {}

BaseFringeNode::~BaseFringeNode() {}

BaseFringeNode &BaseFringeNode::operator=(const BaseFringeNode &other) {
    id = other.id;
    incoming = other.incoming;
    outgoing = other.outgoing;

    return *this;
}

node_id_t BaseFringeNode::getID() {
    return id;
}

std::vector<BaseFringeEdge*> &BaseFringeNode::getIncoming() {
    return incoming;
}

std::vector<BaseFringeEdge*> &BaseFringeNode::getOutgoing() {
    return outgoing;
}

BaseFringeEdge *BaseFringeNode::getIncident(BaseFringeNode* other) {
    for (BaseFringeEdge* edge : getOutgoing()) {
        if (edge->getTo() == other) {
            return edge;
        }
    }
    return nullptr;
}

void BaseFringeNode::addIncoming(BaseFringeEdge *edge) {
    incoming.push_back(edge);
}

void BaseFringeNode::addOutgoing(BaseFringeEdge *edge) {
    outgoing.push_back(edge);
}

edge_weight_t BaseFringeNode::calculateHeuristic(BaseFringeNode *to) {
    return 0;
}

// Default edge weight calculation implementation, always returns the default weight
template <>
edge_weight_t FringeEdgeWeightCalculation<void>::weight(FringeEdge<void>* edge, edge_weight_t costToFrom) {
    return edge->getWeight();
}

/*
 * FringeEdge implementation
 */

BaseFringeEdge::BaseFringeEdge(edge_id_t id) : id(id), from(nullptr), to(nullptr) {}

BaseFringeEdge::BaseFringeEdge(edge_id_t id, BaseFringeNode *from, BaseFringeNode *to, edge_weight_t weight)
        : id(id), from(from), to(to), weight(weight) {
    from->addOutgoing(this);
    to->addIncoming(this);
}

BaseFringeEdge::~BaseFringeEdge() {}

BaseFringeEdge &BaseFringeEdge::operator=(const BaseFringeEdge &other) {
    from = other.from;
    to = other.to;
    weight = other.weight;

    return *this;
}

edge_weight_t BaseFringeEdge::calculateWeight(edge_weight_t costToFrom) {
    return getWeight();
}

edge_id_t BaseFringeEdge::getID() {
    return id;
}

BaseFringeNode* BaseFringeEdge::getFrom() {
    return from;
}

void BaseFringeEdge::setFrom(BaseFringeNode *node) {
    from = node;
    node->addOutgoing(this);
}

BaseFringeNode* BaseFringeEdge::getTo() {
    return to;
}

void BaseFringeEdge::setTo(BaseFringeNode *node) {
    to = node;
    node->addIncoming(this);
}

edge_weight_t BaseFringeEdge::getWeight() {
    return weight;
}

void BaseFringeEdge::setWeight(edge_weight_t weight) {
    this->weight = weight;
}



