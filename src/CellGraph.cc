#include "CellGraph.h"

#include <fstream>
#include <unordered_map>
#include <iostream>

#include "LibraryCell.h"
#include "Net.h"

CellGraph::CellGraph() {

}

void CellGraph::parse_netlist(std::string netlist_json_file, CellMap& cell_map) {
	// Read Yosys Netlist from JSON
	std::ifstream netlist_json(netlist_json_file);
	netlist_json >> this->yosys_netlist;

	std::vector<std::string> module_names;
	for (auto& it : yosys_netlist["modules"].items()) {
		std::cout << it.key() << std::endl;
		module_names.push_back(it.key());
	}

	std::vector<Net*> nets;

	for (auto& module : module_names) {
		for (auto it : yosys_netlist["modules"][module]["cells"].items()) {
			auto cell_json = json::parse((std::string)it.value().dump());

			//Create and connect net to the cell
			auto connections = cell_json.at("connections").get<std::unordered_map<std::string, std::vector<int>>>();
			std::unordered_map<std::string, std::vector<Net*>> pin_map;
			std::vector<Net*> flat_nets;
			for (auto& conn : connections) {
				std::vector<Net*> temp_nets;
				for (auto net_num : conn.second) {
					bool found = false;
					for (auto& net : nets) {
						if (net_num == net->get_net_number()) {
							found = true;
							temp_nets.push_back(net);
						}
					}
					if (!found) {
						Net* new_net = new Net(net_num);
						temp_nets.push_back(new_net);
						nets.push_back(new_net);
					}
				}
				pin_map[conn.first] = temp_nets;
				flat_nets.insert(flat_nets.end(), temp_nets.begin(), temp_nets.end());
			}

			CellUnit* cell = new CellUnit(
					it.key(), 
					cell_map[cell_json.at("type").get<std::string>()], 
					pin_map,
					flat_nets
					);

			//Connect cell to all connected nets
			for (auto& net : flat_nets) {
				net->add_connection(cell);
			}

			internal_netlist.push_back(cell);
		}
	}

	//TODO: Parse ports as well

}

int CellGraph::generate_graph() {
	return 0;
}

std::vector<CellUnit*>& CellGraph::get_netlist() {
	return internal_netlist;
}

void CellGraph::print_netlist() {
	for (auto& it : internal_netlist) {
		it->print_attributes();
	}
}

void CellGraph::print_graph() {
	std::cout << this->yosys_netlist["modules"]["booth"]["cells"] << std::endl;
}
