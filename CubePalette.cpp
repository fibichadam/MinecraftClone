#include "CubePalette.h"

CubePalette::CubePalette(){
    //m_palette.insert({ Cube::Type::Air, Cube{""} });
    m_palette.insert({ Cube::Type::Grass, Cube{"C:/Users/Adam/Desktop/pg/minecraft/textures/grass.jpg"} });
    m_palette.insert({ Cube::Type::Stone, Cube{"C:/Users/Adam/Desktop/pg/minecraft/textures/stone.jpg"} });
}

const Cube& CubePalette::LookUp(Cube::Type type) const{
    return m_palette.at(type);
}
