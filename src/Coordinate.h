#ifndef COORDINATE_H
#define COORDINATE_H

struct Coordinate {

    int x, z;

    Coordinate(int x, int z);

    Coordinate();

    //bool operator==(Coordinate compare);

    void print();
};

#endif
