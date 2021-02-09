#include "CellUnit.h"

#include <iostream>
#include <iomanip>

CellUnit::CellUnit() {

}

//CellUnit::CellUnit(std::string cell_name, LibraryCell* lib_cell, auto connections) : cell_name(cell_name), lib_cell(lib_cell), pins(connections) { //Assumes that all z-values are equal length in a cell
//
//}

CellUnit::CellUnit(std::string cell_name, LibraryCell* lib_cell, std::unordered_map<std::string, std::vector<int>> connections) : cell_name(cell_name), lib_cell(lib_cell), pins(connections) { //Assumes that all z-values are equal length in a cell

}

CellUnit::CellUnit(std::string cell_name, LibraryCell* lib_cell, std::unordered_map<std::string, std::vector<Net*>> connected_nets, std::vector<Net*> flat_nets) : cell_name(cell_name), lib_cell(lib_cell), connected_nets(connected_nets), flat_nets(flat_nets) { //Assumes that all z-values are equal length in a cell

}

//CellUnit::CellUnit(std::string cell_name, LibraryCell* lib_cell, auto connected_nets, auto flat_nets) : cell_name(cell_name), lib_cell(lib_cell), connected_nets(connected_nets), flat_nets(flat_nets) { //Assumes that all z-values are equal length in a cell
//
//}

std::string CellUnit::get_name() {
    return cell_name;
}

void CellUnit::print_attributes() {
    std::cout << "Cell Name: " << cell_name << std::endl;
    std::cout << "Library Cell Name: " << lib_cell->get_lib_cell_name() << std::endl;
    std::cout << "Pin Connections: " << std::endl;
    for (auto key : connected_nets) {
        std::cout << key.first << ", ";
        for (auto val : key.second) {
            std::cout << val->get_net_number();
        }
        std::cout << std::endl;
    }
    std::cout << "Partition ID: " << partition_id << std::endl;
}

std::string CellUnit::get_cell_name() {
    return this->cell_name;
}

std::string CellUnit::get_lib_cell_name() {
    return this->lib_cell->get_lib_cell_name();
}

void CellUnit::print() {
    auto& cell_blocks = lib_cell->get_cell_blocks();
    for (int cell_z = 0; cell_z < lib_cell->get_cell_length(); cell_z++) {
        for (int cell_x = 0; cell_x < lib_cell->get_cell_width(); cell_x++) {
            std::cout << std::setfill('0') << std::setw(4) << cell_blocks[cell_z][cell_x] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void CellUnit::set_partition_id(std::string new_partition_id) {
    this->partition_id = new_partition_id;
}
