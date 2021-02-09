#include "SubPlacement.h"

#include <iostream>
#include <iomanip>

//SubPlacement(std::vector<std::vector<CellUnit*>>& vec, Coordinate coord_ll, Coordinate coord_ur, int last_split_direction) : coord_ll(coord_ll), coord_ur(coord_ur), last_split_direction(last_split_direction) {
//    sub_placement.resize((coord_ur.z - coord_ll.z), SubVector<std::vector<CellUnit*>>(vec[0].begin()+coord_ll.x, vec[0].begin()+coord_ur.x));
//    for (std::size_t i = 0; i < (coord_ur.z - coord_ll.z); i++) {
//        sub_placement[i] = SubVector<std::vector<CellUnit*>>(vec[(coord_ll.z+i)].begin()+coord_ll.x, vec[(coord_ll.z+i)].begin()+coord_ur.x);
//    }
//}

SubPlacement::SubPlacement(std::vector<std::vector<CellUnit*>>& vec, Coordinate coord_ll, Coordinate coord_ur, int last_split_direction, std::string partition_id) : coord_ll(coord_ll), coord_ur(coord_ur), last_split_direction(last_split_direction), partition_id(partition_id) {
	sub_placement.resize((coord_ur.z - coord_ll.z), SubVector<std::vector<CellUnit*>>(vec[0].begin()+coord_ll.x, vec[0].begin()+coord_ur.x));
	for (int i = 0; i < (coord_ur.z - coord_ll.z); i++) {
		sub_placement[i] = SubVector<std::vector<CellUnit*>>(vec[(coord_ll.z+i)].begin()+coord_ll.x, vec[(coord_ll.z+i)].begin()+coord_ur.x);
	}
	for (auto& row : sub_placement) {
		for (auto& cell : row) {
			if (cell != nullptr) {
				cell->set_partition_id(partition_id);
			}
		}
	}
}

auto& SubPlacement::operator[] (std::size_t index) {
	return sub_placement[index];
}

CellUnit* SubPlacement::get(Coordinate coord) {
	return sub_placement[coord.z][coord.x];
}

void SubPlacement::print_sub_placement() {
	std::cout << std::endl << "LL ";
	coord_ll.print();
	std::cout << "UR ";
	coord_ur.print();
	std::cout << "Horizontal Size: ";
	std::cout << this->horizontal_size() << std::endl;
	std::cout << "Vertical Size: ";
	std::cout << this->vertical_size() << std::endl;
	std::cout << "Last split direction: " << this->last_split_direction << std::endl;
	for (auto& row : sub_placement) {
		for (auto& cell : row) {
			if (cell != nullptr) {
				std::cout << std::setw(4) << cell->get_lib_cell_name() << " ";
			}
		}
		std::cout << std::endl;
	}
}

int SubPlacement::size() {
	return sub_placement.size();
}

int SubPlacement::horizontal_size() {
	return coord_ur.x - coord_ll.x;
}

int SubPlacement::vertical_size() {
	return coord_ur.z - coord_ll.z;
}
