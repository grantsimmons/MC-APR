#ifndef CELLMAP_H
#define CELLMAP_H

#include <map>
#include <string>

#include "LibraryCell.h"
#
#include <json.hpp>

class CellMap {
    private:

    public:

		using json = nlohmann::json;

        std::map< std::string, LibraryCell* > cell_map;

        int all_cells_max_width = 0; //Helper variable for flattening grid to block map

        int all_cells_max_length = 0; //Helper variable for flattening grid to block map

        CellMap();

        LibraryCell* operator[] (std::string cell_key); //Return copy so that we can't overwrite a std cell entry

        void load_library(std::string library_json_file);

};

#endif
