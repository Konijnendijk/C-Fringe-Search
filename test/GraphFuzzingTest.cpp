
#include "catch.hpp"

#include "FringeGraph.h"
#include "FringeSearch.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/random_device.hpp>

#include <cmath> 

// Number of graphs to compare shortest paths on
static const unsigned int NUM_TEST_GRAPHS = 1000;
// Number of nodes per test graph
static const unsigned int NODES_PER_TEST_GRAPH = 1000;
// The Erdos-Renyi input parameter
static const float ER_PARAMETER = 0.005;

typedef boost::adjacency_list< boost::listS, boost::vecS, boost::directedS, boost::no_property, boost::property < boost::edge_weight_t, float > > graph_t;
typedef boost::erdos_renyi_iterator<boost::minstd_rand, graph_t> er_generator_t;
typedef boost::graph_traits < graph_t >::vertex_descriptor vertex_descriptor;

TEST_CASE("Fringe search returns the same paths as Boost's Dijkstra implementation on random graphs") {

    boost::random_device rd;
    boost::minstd_rand gen(rd);
    SECTION("Generate a random graph and compare paths") {
        for (unsigned int i = 0; i < NUM_TEST_GRAPHS; i++) {
            graph_t g(er_generator_t(gen, NODES_PER_TEST_GRAPH, ER_PARAMETER), er_generator_t(), NODES_PER_TEST_GRAPH);

            // Set random weights
            auto unweightedEdges = boost::edges(g);
            for (auto eit = unweightedEdges.first; eit != unweightedEdges.second; eit++) {
                float weight = 10.0f * (gen() - gen.min()) / (gen.max() - gen.min());
                boost::put(boost::edge_weight_t(), g, *eit, weight);
            }

            std::vector<vertex_descriptor> predecessors(num_vertices(g));
            std::vector<float> distances(num_vertices(g));
            vertex_descriptor source(boost::vertex(0, g));
            boost::dijkstra_shortest_paths(g, source,
                                           boost::predecessor_map(&predecessors[0]).distance_map(&distances[0]));

            std::vector<vertex_descriptor> path;
            vertex_descriptor target(boost::vertex(NODES_PER_TEST_GRAPH - 1, g));

            // Check that the target was reachable
            if (predecessors[target] != target) {
                vertex_descriptor current = target;
                while (current != source) {
                    path.push_back(current);
                    current = predecessors[current];
                }
            }

            // Convert boost graph to fringe search graph

            // Create nodes
            std::vector<FringeNode<void>* > fringeNodes;
            for (unsigned int n = 0; n < NODES_PER_TEST_GRAPH; n++) {
                fringeNodes.push_back(new FringeNode<void>(n));
            }

            // Convert edges
            auto edges = boost::edges(g);
            edge_id_t currentId = 0;
            for (auto eit = edges.first; eit != edges.second; eit++) {
                float weight = boost::get(boost::edge_weight_t(), g, *eit);
                FringeNode<void> *edgeSource = fringeNodes[(*eit).m_source];
                FringeNode<void> *edgeTarget = fringeNodes[(*eit).m_target];
                FringeEdge<void> *edge = new FringeEdge<void>(currentId++, edgeSource, edgeTarget, weight);
            }

            FringeSearch search(fringeNodes[0]);
            FringeNode<void> * fringeTarget = fringeNodes[NODES_PER_TEST_GRAPH - 1];
            std::vector<BaseFringeNode *> *fringePath = search.search(fringeTarget);

            if (path.size() == 0) {
                // If the target was not reachable, no path should be output
                REQUIRE(fringePath == nullptr);
            } else {
                REQUIRE(fringePath != nullptr);
                float weightDifference = abs(distances[target] - search.cost(fringeTarget));
                std::cout<< weightDifference << std::endl;
                REQUIRE(weightDifference < 1E-6);
            }
        }
    }
}
