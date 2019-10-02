#include "LoboTetMeshGraph.h"
#include "LoboVolumtricMesh/LoboTetMesh.h"

Lobo::LoboTetMeshGraph::LoboTetMeshGraph(LoboTetMesh* tetmesh_)
{
    this->tetmesh = tetmesh_;
}

Lobo::LoboTetMeshGraph::~LoboTetMeshGraph()
{

}

void Lobo::LoboTetMeshGraph::init()
{

}

void Lobo::LoboTetMeshGraph::compute_dijkstra_shortest_paths(int startnode,std::vector<double>& distance)
{

}

void Lobo::LoboTetMeshGraph::initEdgeList()
{
    
}