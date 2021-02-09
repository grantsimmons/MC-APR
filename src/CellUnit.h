#ifndef CELLUNIT_H
#define CELLUNIT_H

#include <vector>
#include <unordered_map>
#include <string>

#include "LibraryCell.h"
#include "Net.h"

class Net;

class CellUnit {
    public:

        LibraryCell* lib_cell;

        std::string cell_name;

        std::unordered_map<std::string, std::vector<int>> pins;

        std::unordered_map<std::string, std::vector<Net*>> connected_nets;

        std::vector<Net*> flat_nets;

        std::string partition_id;

        bool fixed = 0;

        CellUnit();

        //CellUnit(std::string cell_name, LibraryCell* lib_cell, (auto connections);

        CellUnit(std::string cell_name, LibraryCell* lib_cell, std::unordered_map<std::string, std::vector<int>> connections);

        //CellUnit(std::string cell_name, LibraryCell* lib_cell, auto connected_nets, auto flat_nets);
		//auto keyword breaks linter. Need more careful casting?

		CellUnit(std::string cell_name, LibraryCell* lib_cell, std::unordered_map<std::string, std::vector<Net*>> connected_nets, std::vector<Net*> flat_nets); //Assumes that all z-values are equal length in a cell
        
        std::string get_name();

        void print_attributes();

        std::string get_cell_name();

        std::string get_lib_cell_name();

        void print();

        void set_partition_id(std::string new_partition_id);
};

#endif
