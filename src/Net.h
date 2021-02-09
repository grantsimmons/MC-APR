#ifndef NET_H
#define NET_H

#include <iostream>
#include <vector>

#include "CellUnit.h"

class CellUnit;

class Net {
    private:

        int net_number;

        std::vector<CellUnit*> connected_cells;

    public:

        Net(int net_number);

        void add_connection(CellUnit* new_cell);

        int get_net_number();

        std::vector<CellUnit*> get_connected_cells();
};

#endif
