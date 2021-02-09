#ifndef SUBPLACEMENT_H
#define SUBPLACEMENT_H

#include <vector>

#include "Coordinate.h"
#include "SubVector.h"
#include "CellUnit.h"

class SubPlacement {
    public:

        std::vector<SubVector<std::vector<CellUnit*>>> sub_placement;

        Coordinate coord_ll, coord_ur;

        int last_split_direction;

        std::string partition_id;

        //SubPlacement(std::vector<std::vector<CellUnit*>>& vec, Coordinate coord_ll, Coordinate coord_ur, int last_split_direction);

        SubPlacement(std::vector<std::vector<CellUnit*>>& vec, Coordinate coord_ll, Coordinate coord_ur, int last_split_direction, std::string partition_id);

        auto& operator[] (std::size_t index);

        CellUnit* get(Coordinate coord);

        void print_sub_placement();

        int size();

        int horizontal_size();

        int vertical_size();

};

#endif
