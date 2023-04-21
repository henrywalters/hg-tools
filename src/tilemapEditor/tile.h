//
// Created by henry on 4/19/23.
//

#ifndef HGTOOLS_TILE_H
#define HGTOOLS_TILE_H

#include <hagame/core/component.h>

enum class TileMode {
    Color,
    Sprite,
};

class Tile : public hg::Component {
public:

    TileMode mode;
    int layer;

};

#endif //HGTOOLS_TILE_H
