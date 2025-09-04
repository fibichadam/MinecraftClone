#include "World.h"

#include <iostream>

World::World() {
}

World::~World() {
}

void World::updateChunks(const Camera& cam, CubePalette& palette) {
	int posx = (int)std::ceil(cam.GetPosition().x / CHUNK_DEPTH);
	int posy = (int)std::ceil(cam.GetPosition().z / CHUNK_WIDTH);
	for (int depth = -5; depth <= 3; depth++) {
		for (int width = -5; width <= 3; width++) {
			chunks.insert(std::make_pair(glm::ivec2(posx + depth, posy + width), std::make_shared<Chunk<CHUNK_DEPTH, CHUNK_WIDTH, CHUNK_HEIGHT>>(glm::vec2(CHUNK_DEPTH * (posx + depth), CHUNK_WIDTH * (posy + width)), palette)));
		}
	}

	/*for (auto it = chunks.begin(); it != chunks.end();) {
		if (glm::distance(glm::vec2(it->first.x * CHUNK_DEPTH, it->first.y * CHUNK_WIDTH), glm::vec2(posx * CHUNK_DEPTH , posy * CHUNK_WIDTH)) > 100) {
			it = chunks.erase(it);
		}
		else {
			++it;
		}
	}*/

}