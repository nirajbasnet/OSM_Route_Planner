//
// Created by niraj on 3/21/20.
//
#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

   	start_node=&m_Model.FindClosestNode(start_x,start_y);
   	end_node=&m_Model.FindClosestNode(end_x,end_y);
}

float RoutePlanner::CalculateHValue(const RouteModel::Node *node) {
    return  node->distance(*end_node);
}

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    current_node->FindNeighbors();
    for (auto neighbour : current_node->neighbors){
       	neighbour->parent = current_node;
        neighbour->g_value = current_node->g_value+current_node->distance(*neighbour);
        neighbour->h_value = CalculateHValue(neighbour);
      	neighbour->visited=true;
        open_list.push_back(neighbour);
    }
}

RouteModel::Node *RoutePlanner::NextNode() {
    std::sort(open_list.begin(), open_list.end(), [](const auto &node_a,const auto &node_b) {
        return (node_a->g_value + node_a->h_value) > (node_b->g_value + node_b->h_value);
    	});          
    RouteModel::Node* lowest_sum_node=open_list.back();
    open_list.pop_back();
    return lowest_sum_node;
}


std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    
    while(current_node->parent != nullptr){
      	distance+= current_node->distance(*current_node->parent);
        path_found.push_back(*current_node);
        current_node=current_node->parent;
    }  	
  	
  	path_found.push_back(*current_node);
  	std::reverse(path_found.begin(), path_found.end());
    
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;
}

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;
    
    start_node->visited=true;
    open_list.push_back(start_node);

    while(open_list.size()>0){
        current_node=NextNode();
        if(current_node->distance(*end_node)==0){
            m_Model.path=ConstructFinalPath(current_node);
            return;
        }
        AddNeighbors(current_node);
    }
  	std::cout << "No path was found! :(\n";
    return;
}

