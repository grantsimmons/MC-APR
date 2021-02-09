#ifndef LIBRARYCELL_H
#define LIBRARYCELL_H

#include <string>
#include <vector>

class LibraryCell {
    private:

        const std::string name;

        const unsigned int cell_length, cell_width;

        const std::vector<std::vector<int>> cell_blocks;

    public:

        LibraryCell();

        LibraryCell(std::string name, unsigned int cell_length, unsigned int cell_width, std::vector<std::vector<int>> cell_blocks);

        const std::string get_lib_cell_name();
        
        const unsigned int get_cell_length();

        const unsigned int get_cell_width();

        const std::vector<std::vector<int>>& get_cell_blocks();
};

#endif
