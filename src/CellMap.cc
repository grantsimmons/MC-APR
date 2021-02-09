#include "CellMap.h"

#include <iostream>
#include <fstream>
#include <vector>

CellMap::CellMap() {
}

//Allows the user to access a cell layout with squre brackets. E.g. cell_map["NAND"]
//FIXME: namespace?
LibraryCell* CellMap::operator[] (std::string cell_key) { //Return copy so that we can't overwrite a std cell entry
    LibraryCell* ret = nullptr;
    auto it = cell_map.find(cell_key);
    if ( it != cell_map.end() ) {
        // Found cell_key
        ret = it->second;
    } else {
        // Not found
        std::cout << "ERROR: Standard Cell entry \"" << cell_key << "\" not found in provided library file." << std::endl;
        exit(1);
    }
    return ret;
}

void CellMap::load_library(std::string library_json_file) {
    // Read cell block layouts from given JSON
    std::ifstream f_lib_json(library_json_file);

    json lib;
    f_lib_json >> lib;

    for (auto& it : lib["cells"].items()) {
        std::string cell_name = it.key();
        json j = json::parse(it.value().dump());
        std::vector<std::vector<int>> single_cell_map = j.at("layout").get<std::vector<std::vector<int>>>();

        //Check dimensions
        int cell_max_length = single_cell_map.size();
        if (cell_max_length > this->all_cells_max_length) {
            this->all_cells_max_length = cell_max_length;
        }

        //Get cell max width, check for uniformity
        int cell_max_width = 0;
        for (std::vector<int>& z_val : single_cell_map) {
            cell_max_width = z_val.size() > cell_max_width ? z_val.size() : cell_max_width;
            if (cell_max_width > all_cells_max_width) {
                this->all_cells_max_width = cell_max_width;
            }
        }

        //Resize for max_cell_width
        for (std::vector<int>& z_val : single_cell_map) {
            z_val.resize(cell_max_width, 0);
        }

        std::reverse(single_cell_map.begin(), single_cell_map.end());
        this->cell_map[cell_name] = new LibraryCell(cell_name, cell_max_length, cell_max_width, single_cell_map);
    }
}
