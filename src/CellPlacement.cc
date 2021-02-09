#include "CellPlacement.h"

#include <iostream>
#include <string>
#include <deque>
#include <unordered_map>

CellPlacement::CellPlacement(std::vector<CellUnit*>& netlist) :
	netlist(netlist), 
	placement_x_max(int(ceil(sqrt(netlist.size())))), 
	placement_z_max(int(ceil(sqrt(netlist.size())))) {
		this->placement.resize(placement_z_max, std::vector<CellUnit*>(placement_x_max, nullptr));
}

//CellPlacement::CellPlacement(auto& netlist, unsigned int placement_x_max, unsigned int placement_z_max) :
//	netlist(netlist), 
//	placement_x_max(placement_x_max), 
//	placement_z_max(placement_z_max) {
//		this->placement.resize(placement_z_max, std::vector<CellUnit*>(placement_x_max, nullptr));
//}

void CellPlacement::place(LibraryCell* fill_cell) {
	initial_placement();
	add_fill(fill_cell);
	print_placement();
	refine_placement();
	print_placement();
	//render_flat_placement();
	//print_flat_placement();
}

unsigned int CellPlacement::get_placement_max_cell_width() {
	int max_cell_width = 0;
	for (auto& z : placement) {
		for (auto& x : z) {
			if (x != nullptr) {
				int cell_width = x->lib_cell->get_cell_width();
				if (cell_width > max_cell_width) 
					max_cell_width = cell_width;
			}
		}
	}
	return max_cell_width;
}

unsigned int CellPlacement::get_placement_max_cell_length() {
	int max_cell_length = 0;
	int z = 0;
	for (auto& z : placement) {
		for (auto& x : z) {
			if (x != nullptr) {
				int cell_length = x->lib_cell->get_cell_length();
				if (cell_length > max_cell_length) 
					max_cell_length = cell_length;
			}
		}
	}
	return max_cell_length;
}

int CellPlacement::get_x_max() {
	return this->placement_x_max;
}

int CellPlacement::get_z_max() {
	return this->placement_z_max;
}

void CellPlacement::initial_placement() {
	int curr_x = 0;
	int curr_z = 0;
	for (auto cell : netlist) {
		placement[curr_z][curr_x] = cell;
		curr_x++;
		if (curr_x == placement_x_max) {
			curr_x = 0;
			curr_z++;
			if (curr_z == placement_z_max) {
				std::cout << "ERROR: Ran out of room in initial placement. Expand the allowed placement grid" << std::endl;
				exit(1);
				//if (cell != placement.end()) {
				//    std::cout << "ERROR: Ran out of room in initial placement!" << std::endl;
				//    exit(1);
				//}
			}
		}
	}
}

void CellPlacement::add_fill(LibraryCell* fill_cell) {
	std::cout << "Adding fill cells to design" << std::endl;
	for (int z = 0; z < placement_z_max; z++) {
		for (int x = 0; x < placement_x_max; x++) {
			if(placement[z][x] == nullptr) {
				placement[z][x] = new CellUnit(
						(std::string) "FILL" + std::to_string(x) + std::to_string(z), 
						(LibraryCell*) fill_cell, 
						(std::unordered_map<std::string, std::vector<int>>){});
			}
		}
	}
}

void CellPlacement::swap(auto& cell1, auto& cell2) {
	auto temp = cell1;
	cell1 = cell2;
	cell2 = temp;
}

std::pair<SubPlacement*, SubPlacement*> CellPlacement::split(SubPlacement* sub_placement) { 
	//std::cout << "Splitting Sub Placement" << std::endl;
	// direction: 0 = horizontal, 1 = vertical
	if (sub_placement->last_split_direction == 0) { //Last split horizontally
		if (sub_placement->horizontal_size() > 1) {
			//Vertical Split
			std::string left_partition_id = std::to_string(sub_placement->coord_ll.x) + std::to_string(sub_placement->coord_ll.z) + std::to_string((sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2) + std::to_string(sub_placement->coord_ur.z);
			SubPlacement* left = new SubPlacement(placement, {sub_placement->coord_ll.x,sub_placement->coord_ll.z}, {(sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2,sub_placement->coord_ur.z}, 1, left_partition_id);

			std::string right_partition_id = std::to_string((sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2) + std::to_string(sub_placement->coord_ll.z) + std::to_string(sub_placement->coord_ur.x) + std::to_string(sub_placement->coord_ur.z);
			SubPlacement* right = new SubPlacement(placement, {(sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2,sub_placement->coord_ll.z}, {sub_placement->coord_ur.x,sub_placement->coord_ur.z}, 1, right_partition_id);

			return (std::pair<SubPlacement*, SubPlacement*>) {left, right};
		}
		else if (sub_placement->vertical_size() > 1) {
			//Resort to horizontal split
			std::string low_partition_id = std::to_string(sub_placement->coord_ll.x) + std::to_string(sub_placement->coord_ll.z) + std::to_string(sub_placement->coord_ur.x) + std::to_string((sub_placement->coord_ll.z+sub_placement->coord_ur.z)/2);
			SubPlacement* low = new SubPlacement(placement, {sub_placement->coord_ll.x,sub_placement->coord_ll.z}, {sub_placement->coord_ur.x,(sub_placement->coord_ll.z+sub_placement->coord_ur.z)/2}, 0, low_partition_id);

			std::string high_partition_id = std::to_string(sub_placement->coord_ll.x) + std::to_string((sub_placement->coord_ll.z + sub_placement->coord_ur.z)/2) + std::to_string(sub_placement->coord_ur.x) + std::to_string(sub_placement->coord_ur.z);
			SubPlacement* high = new SubPlacement(placement, {sub_placement->coord_ll.x,(sub_placement->coord_ll.z + sub_placement->coord_ur.z)/2}, {sub_placement->coord_ur.x,sub_placement->coord_ur.z}, 0, high_partition_id);

			return (std::pair<SubPlacement*, SubPlacement*>) {low, high};
		}
		else if (sub_placement->vertical_size() == 1 && sub_placement->vertical_size() == 1) {
			//Return nothing
			//Done partitioning
			return (std::pair<SubPlacement*, SubPlacement*>) {nullptr, nullptr};
		}
		else {
			std::cout << "ERROR: Something went wrong durring graph partitioning" << std::endl;
			exit(1);
		}
	}
	else if (sub_placement->last_split_direction == 1) { //Last split vertically
		if (sub_placement->vertical_size() > 1) {
			//Horizontal split
			std::string low_partition_id = std::to_string(sub_placement->coord_ll.x) + std::to_string(sub_placement->coord_ll.z) + std::to_string(sub_placement->coord_ur.x) + std::to_string((sub_placement->coord_ll.z+sub_placement->coord_ur.z)/2);
			SubPlacement* low = new SubPlacement(placement, {sub_placement->coord_ll.x,sub_placement->coord_ll.z}, {sub_placement->coord_ur.x,(sub_placement->coord_ll.z+sub_placement->coord_ur.z)/2}, 0, low_partition_id);

			std::string high_partition_id = std::to_string(sub_placement->coord_ll.x) + std::to_string((sub_placement->coord_ll.z + sub_placement->coord_ur.z)/2) + std::to_string(sub_placement->coord_ur.x) + std::to_string(sub_placement->coord_ur.z);
			SubPlacement* high = new SubPlacement(placement, {sub_placement->coord_ll.x,(sub_placement->coord_ll.z + sub_placement->coord_ur.z)/2}, {sub_placement->coord_ur.x,sub_placement->coord_ur.z}, 0, high_partition_id);

			return (std::pair<SubPlacement*, SubPlacement*>) {low, high};
		}
		else if (sub_placement->vertical_size() > 1) {
			//Resort to vertical split
			std::string left_partition_id = std::to_string(sub_placement->coord_ll.x) + std::to_string(sub_placement->coord_ll.z) + std::to_string((sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2) + std::to_string(sub_placement->coord_ur.z);
			SubPlacement* left = new SubPlacement(placement, {sub_placement->coord_ll.x,sub_placement->coord_ll.z}, {(sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2,sub_placement->coord_ur.z}, 1, left_partition_id);

			std::string right_partition_id = std::to_string((sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2) + std::to_string(sub_placement->coord_ll.z) + std::to_string(sub_placement->coord_ur.x) + std::to_string(sub_placement->coord_ur.z);
			SubPlacement* right = new SubPlacement(placement, {(sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2,sub_placement->coord_ll.z}, {sub_placement->coord_ur.x,sub_placement->coord_ur.z}, 1, right_partition_id);

			return (std::pair<SubPlacement*, SubPlacement*>) {left, right};
		}
		else if (sub_placement->vertical_size() == 1 && sub_placement->vertical_size() == 1) {
			//Return nothing
			//Done partitioning
			return (std::pair<SubPlacement*, SubPlacement*>) {nullptr, nullptr};
		}
		else {
			std::cout << "ERROR: Something went wrong durring graph partitioning" << std::endl;
			exit(1);
		}
	}
	else {
		std::cout << "ERROR: Invalid last_split: " << sub_placement->last_split_direction << std::endl;
		exit(1);
	}
}

std::pair<SubPlacement*, SubPlacement*> CellPlacement::run_kl(SubPlacement* in) {
	auto[part1, part2] = split(in); //Assign new partition IDs

	if (part1 != nullptr && part2 != nullptr) {
		//Perform KL Algorithm on two subregions
		std::string partition_id_1 = part1->partition_id;
		std::string partition_id_2 = part2->partition_id;
		std::unordered_map<std::string, std::list<CoordCost>> cell_costs, swap_candidates;

		//Calculate cost of initial cut
		for (int z = 0; z < in->vertical_size(); z++) {
			for (int x = 0; x < in->horizontal_size(); x++) {
				Coordinate coord(x,z);
				CellUnit* cell = in->get(coord);
				std::string this_partition_id = cell->partition_id;
				std::string other_partition_id;

				if (this_partition_id == partition_id_1) {
					other_partition_id = partition_id_2;
				}
				else if (this_partition_id == partition_id_2) {
					other_partition_id = partition_id_1;
				}
				else {
					std::cout <<"ERROR: Partition ID not found" << std::endl;
					exit(1);
				}

				int cut_cost = 0;

				for (auto& net : cell->flat_nets) {
					for (auto& connected_cell : net->get_connected_cells()) {
						if (connected_cell->partition_id == other_partition_id) {
							cut_cost++;
						}
						else if (connected_cell->partition_id == this_partition_id) { //FIXME: Is this right?
							cut_cost--;
						}
					}
				}
				cell_costs[this_partition_id].push_back({coord,cut_cost});
				if (cut_cost > 0) {
					swap_candidates[this_partition_id].push_back({coord,cut_cost});
				}
			}
		}

		//std::unordered_map<std::string, CoordCost*> max;
		std::unordered_map<std::string, CoordCost> max;

		for (auto& part : swap_candidates) { //Iterate Map
			std::cout << "Swap candidates for ";
			std::cout << part.first << std::endl;
			for (auto& coordcost : part.second) { //Iterate List
				if (max.find(part.first) == max.end()) {
					//Part ID not found
					max[part.first] = coordcost;
				} 
				else {
					//Part ID found
					if (coordcost.cost > max[part.first].cost) {
						max[part.first] = coordcost;
						std::cout << "New Max = " << coordcost.cost << std::endl;
					}
				}
				std::cout << "Cell: ";
				std::cout << in->get(coordcost.coord)->get_cell_name() << std::endl;
				std::cout << "  Cut Cost: " << coordcost.cost << std::endl;
			}
		}

		//int correction_factor = 0;

		//int delta_g = sp_a_max_cost.second + sp_b_max_cost.second - correction;
		//if (delta_g > 0) {
		//    std::cout << "Swapping " << (*sp_a)[sp_a_max_cost.first.z][sp_a_max_cost.first.x]->get_cell_name() << " with " << (*sp_b)[sp_b_max_cost.first.z][sp_b_max_cost.first.x]->get_cell_name() << std::endl;
		//    swap((*sp_a)[sp_a_max_cost.first.z][sp_a_max_cost.first.x], (*sp_b)[sp_b_max_cost.first.z][sp_b_max_cost.first.x]);
		//}
		return std::pair<SubPlacement*, SubPlacement*>(part1, part2);
	}
	else if (part1 == nullptr && part2 == nullptr) {
		return std::pair<SubPlacement*, SubPlacement*>(nullptr, nullptr);
	}
	else {
		std::cout << "ERROR: Only one partition valid when splitting subplacement" << std::endl;
		exit(1);
	}
}

void CellPlacement::refine_placement() {
	std::cout << "Refining placement..." << std::endl;

	std::deque<SubPlacement*> sp_queue;
	std::deque<SubPlacement*> final_queue; //Remove later, only for testing

	SubPlacement* top = new SubPlacement(placement, {0,0}, {placement_x_max, placement_z_max}, 0, std::to_string(0));

	sp_queue.push_back(top); //Seed sub_placement queue

	//Queue handler
	while(!sp_queue.empty()) {
		SubPlacement* curr_sp = sp_queue.front();

		auto[part1, part2] = run_kl(curr_sp); //Run KL Logic

		if (part1 == nullptr && part2 == nullptr) {
			final_queue.push_front(part1);
			final_queue.push_front(part2);
		}
		else {
			sp_queue.push_back(part1);
			sp_queue.push_back(part2);
		}

		//Remove current subregion pointer
		sp_queue.pop_front();
		delete curr_sp;
		//exit(1);
	}

}

void CellPlacement::print_placement() {
	std::cout << "Placement:" << std::endl;
	for (auto& row : placement) {
		for (auto& cell : row) {
			if (cell != nullptr) {
				std::cout << std::setw(4) << cell->get_lib_cell_name() << " ";
			}
		}
		std::cout << std::endl;
	}
}

auto CellPlacement::get_placement() {
	return placement;
}

void CellPlacement::render_flat_placement() {
	unsigned int max_cell_length = get_placement_max_cell_length();
	unsigned int max_cell_width = get_placement_max_cell_width();
	unsigned int initial_block_value = 0;

	flat_placement.resize(placement_z_max * max_cell_length, std::vector<int>(placement_x_max * max_cell_width, initial_block_value));

	for (int grid_z = 0; grid_z < placement_z_max; grid_z++) {
		for (int grid_x = 0; grid_x < placement_x_max; grid_x++) {
			CellUnit* this_cell = placement[grid_z][grid_x];
			if (this_cell != nullptr) { //Cell found
				auto& cell_blocks = this_cell->lib_cell->get_cell_blocks();
				for (int cell_z = 0; cell_z < max_cell_length; cell_z++) {
					for (int cell_x = 0; cell_x < max_cell_width; cell_x++) {
						if (cell_x < this_cell->lib_cell->get_cell_width() && cell_z < this_cell->lib_cell->get_cell_length()) { //Must make sure that iterators are within cell bounds
							//flat_map[grid_z * cell_map.all_cells_max_length + cell_z][grid_x * cell_map.all_cells_max_width + cell_x] = this_cell->cell_blocks[cell_z][cell_x];
							flat_placement[grid_z * max_cell_length + cell_z][grid_x * max_cell_width + cell_x] = cell_blocks[cell_z][cell_x];
						} 
						else { //Otherwise we will place a blank block here
							flat_placement[grid_z * max_cell_length + cell_z][grid_x * max_cell_width + cell_x] = initial_block_value;
						}
					}
				}
			}
			else { //No cell found, must fill in entire cell with filler
				for (int cell_z = 0; cell_z < max_cell_length; cell_z++) {
					for (int cell_x = 0; cell_x < max_cell_width; cell_x++) {
						flat_placement[grid_z * max_cell_length + cell_z][grid_x * max_cell_width + cell_x] = initial_block_value;
					}
				}
			}
		}
	}
}

void CellPlacement::print_flat_placement() {
	std::cout << "Size: " << flat_placement.size() << std::endl;
	std::cout << "Size (inner): " << flat_placement[0].size() << std::endl;
	std::cout << " X: ";
	for (int x = 0; x < flat_placement[0].size(); x++) {
		std::cout << std::setfill('0') << std::setw(4) << x << " ";
	}
	std::cout << "\n";
	for (int z = flat_placement.size() - 1; z >= 0; z--) { //Iterate backwards for physical layout
		std::vector<int> row = flat_placement[z];
		std::cout << std::setfill('0') << std::setw(2) << z << ": ";
		for (int x = 0; x < flat_placement[z].size(); x++) {
			std::cout << std::setfill('0') << std::setw(4) << row[x] << " ";
		}
		std::cout << std::endl;
	}
}
