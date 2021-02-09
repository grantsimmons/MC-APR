#include "LibraryCell.h"

LibraryCell::LibraryCell() : name("FILL"), cell_length(1), cell_width(1), cell_blocks({{0}}) {

}

LibraryCell::LibraryCell(std::string name, unsigned int cell_length, unsigned int cell_width, std::vector<std::vector<int>> cell_blocks) : 
	name(name), 
	cell_length(cell_length),
	cell_width(cell_width),
	cell_blocks(cell_blocks) {

	}

const std::string LibraryCell::get_lib_cell_name() {
	return name;
}

const unsigned int LibraryCell::get_cell_length() {
	return cell_length;
}

const unsigned int LibraryCell::get_cell_width() {
	return cell_width;
}

const std::vector<std::vector<int>>& LibraryCell::get_cell_blocks() {
	return this->cell_blocks;
}
