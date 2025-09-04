#pragma once
#include "Chunk.h"
#include "Camera.h"
#include "CubePalette.h"

#define GLM_ENABLE_EXPERIMENTAL 
#include "glm/gtx/hash.hpp"


#define CHUNK_DEPTH 6
#define CHUNK_WIDTH 6
#define CHUNK_HEIGHT 16

class World {
public:
	World();
	~World();

	void updateChunks(const Camera& cam, CubePalette& palette);

	std::unordered_map<glm::ivec2, std::shared_ptr<Chunk<CHUNK_DEPTH, CHUNK_WIDTH, CHUNK_HEIGHT>>> getChunks() { return chunks; };

private:
	std::unordered_map<glm::ivec2, std::shared_ptr<Chunk<CHUNK_DEPTH, CHUNK_WIDTH, CHUNK_HEIGHT>>> chunks;
};