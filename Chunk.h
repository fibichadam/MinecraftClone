#pragma once
#include "Cube.h"
#include "ShaderProgram.h"
#include "PerlinNoise.h"
#include "CubePalette.h"
#include "Camera.h"
#include "Ray.h"
#include "AABB.h"

#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

template <uint8_t Depth, uint8_t Width, uint8_t Height>
class Chunk {
	struct CubeData {
		Cube::Type m_type{ Cube::Type::Air };
		bool m_isVisible{ true };
	};

	using FlattenData_t = std::array<CubeData, Depth* Width* Height>;
public:
	struct HitRecord {
		glm::ivec3 m_cubeIndex;
		glm::ivec3 m_neighbourIndex;
	};

	Chunk(const glm::vec2& origin, CubePalette& palette);

	void Generate(const PerlinNoise& rng);

	void Draw(ShaderProgram& shader, Camera& camera) const;

	Ray::HitType Hit(const Ray& ray, Ray::time_t min, Ray::time_t max, HitRecord& record) const;

	bool RemoveBlock(uint8_t width, uint8_t hight, uint8_t depth);
	bool PlaceBlock(uint8_t width, uint8_t hight, uint8_t depth, Cube::Type type);

	glm::vec2 GetOrigin() { return m_origin; };

private:
	size_t CoordsToIndex(size_t depth, size_t width, size_t height) const;
	void UpdateVisibility();

	CubePalette& m_palette;
	FlattenData_t m_data;
	glm::vec2 m_origin;
	AABB m_aabb;
};

template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline Chunk<Depth, Width, Height>::Chunk(const glm::vec2& origin, CubePalette& palette)
	: m_origin(origin), m_palette(palette), m_aabb(glm::vec3(origin.x, 0, origin.y), glm::vec3(origin.x + Depth, Height, origin.y + Width)){
	Generate(PerlinNoise(123));
}

template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::Generate(const PerlinNoise& rng) {
	for (int h = 0; h < Height; h++) {
		for (int w = 0; w < Width; w++) {
			for (int d = 0; d < Depth; d++) {
				float z = rng.At(glm::vec3((float)(d + m_origin.x) / (Depth * 2), (float)(w + m_origin.y) / (Width * 2), (float)h / Height));
				if (z - (float) h / Height > 0.2)
					m_data.at(CoordsToIndex(d, w, h)) = CubeData(Cube::Type::Stone, true);
				else if(z - (float) h / Height > 0.15 && h - 1 >= 0 && m_data.at(CoordsToIndex(d, w ,h - 1)).m_type == Cube::Type::Stone)
					m_data.at(CoordsToIndex(d, w, h)) = CubeData(Cube::Type::Grass, true);
				else
					m_data.at(CoordsToIndex(d, w, h)) = CubeData(Cube::Type::Air, false);
			}
		}
	}
	UpdateVisibility();
}

template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::Draw(ShaderProgram& shader, Camera& camera) const {
	if (glm::distance(glm::vec2{ m_origin.x + Depth/2, m_origin.y + Width/2 }, glm::vec2{ camera.GetPosition().x, camera.GetPosition().z }) < 100) {
		GLuint vao = { 0 };
		shader.Use();
		for (int h = 0; h < Height; h++) {
			for (int w = 0; w < Width; w++) {
				for (int d = 0; d < Depth; d++) {
					if (m_data.at(CoordsToIndex(d, w, h)).m_isVisible) {
						vao = m_palette.LookUp(m_data.at(CoordsToIndex(d, w, h)).m_type).Vao();
						glBindVertexArray(vao);
						glBindTexture(GL_TEXTURE_2D, m_palette.LookUp(m_data.at(CoordsToIndex(d, w, h)).m_type).Texture());
						glm::mat4 model(1.0f);
						model = glm::translate(model, glm::vec3(d + m_origin.x, h, w + m_origin.y));
						shader.SetMat4("mvp", camera.Projection() * camera.View() * model);
						glDrawArrays(GL_TRIANGLES, 0, 36);
					}
				}
			}
		}
	}
	glBindVertexArray(0);
}

template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline size_t Chunk<Depth, Width, Height>::CoordsToIndex(size_t depth, size_t width, size_t height) const {
	return height * static_cast<size_t>(Depth) * static_cast<size_t>(Width)
		+ width * static_cast<size_t>(Depth)
		+ depth;
}

template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::UpdateVisibility() {
	for (int h = 0; h < Height; h++) {
		for (int w = 0; w < Width; w++) {
			for (int d = 0; d < Depth; d++) {
				if (d + 1 < Depth && m_data.at(CoordsToIndex(d + 1, w, h)).m_type != Cube::Type::Air &&
					w + 1 < Width && m_data.at(CoordsToIndex(d, w + 1, h)).m_type != Cube::Type::Air &&
					h + 1 < Height && m_data.at(CoordsToIndex(d, w, h + 1)).m_type != Cube::Type::Air &&
					d - 1 >= 0 && m_data.at(CoordsToIndex(d - 1, w, h)).m_type != Cube::Type::Air &&
					w - 1 >= 0 && m_data.at(CoordsToIndex(d, w - 1, h)).m_type != Cube::Type::Air &&
					h - 1 >= 0 && m_data.at(CoordsToIndex(d, w, h - 1)).m_type != Cube::Type::Air)
				{
					m_data.at(CoordsToIndex(d, w, h)).m_isVisible = false;
				}
				else if (m_data.at(CoordsToIndex(d, w, h)).m_type != Cube::Type::Air)
				{
					m_data.at(CoordsToIndex(d, w, h)).m_isVisible = true;
				}
			}
		}
	}
}


template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline Ray::HitType Chunk<Depth, Width, Height>::Hit(const Ray& ray, Ray::time_t min, Ray::time_t max, HitRecord& record) const {
	Ray::time_t maxTime = max;
	AABB::HitRecord hitRecord;
	Ray::HitType hitType = m_aabb.Hit(ray, min, max, hitRecord);
	if (hitType == Ray::HitType::Miss) {
		return Ray::HitType::Miss;
	}
	for (int w = 0; w < Width; w++) {
		for (int h = 0; h < Height; h++) {
			for (int d = 0; d < Depth; d++) {
				CubeData cube = m_data.at(CoordsToIndex(d, w, h));
				if (cube.m_isVisible && cube.m_type != Cube::Type::Air) {
					AABB cubeAABB = AABB(glm::vec3(m_origin.x + d, h, m_origin.y + w), glm::vec3(m_origin.x + d + 1, h + 1, m_origin.y + w + 1));
					hitType = cubeAABB.Hit(ray, min, max, hitRecord);
					if (hitType == Ray::HitType::Hit && hitRecord.m_time < maxTime) {
						//std::cout << "\nb:" << d << "   " << w << "   " << h;
						maxTime = hitRecord.m_time;
						record.m_cubeIndex = glm::ivec3{d, w, h};

						glm::vec3 cubeCenter = glm::vec3{ d + 0.5f + m_origin.x , h + 0.5f, w + 0.5f + m_origin.y};
						glm::vec3 n = ray.At(maxTime) - cubeCenter;
						n = glm::round(n);
						record.m_neighbourIndex = glm::ivec3(record.m_cubeIndex.x + n.x, record.m_cubeIndex.y + n.z, record.m_cubeIndex.z + n.y);
					}
				}
			}
		}
	}
	return maxTime < max ? Ray::HitType::Hit : Ray::HitType::Miss;
}

template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline bool Chunk<Depth, Width, Height>::RemoveBlock(uint8_t depth, uint8_t width, uint8_t height) {
	size_t coords = CoordsToIndex(depth, width, height);
	std::cout << "trying to remove: " << (int)depth << "  " << (int)height << "  " << (int)width << "\n";
	if (m_data.at(coords).m_type != Cube::Type::Air) {
		m_data.at(coords).m_type = Cube::Type::Air;
		m_data.at(coords).m_isVisible = false;
		UpdateVisibility();
		return true;
	}

	return false;
}


template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline bool Chunk<Depth, Width, Height>::PlaceBlock(uint8_t depth, uint8_t width, uint8_t height, Cube::Type type) {
	size_t coords = CoordsToIndex(depth, width, height);
	std::cout << "trying to place: " << (int)depth << "  " << (int)height << "  " << (int)width << "\n";
	if (height < Height && m_data.at(coords).m_type == Cube::Type::Air) {
		m_data.at(coords).m_type = type;
		UpdateVisibility();
		return true;
	}

	return false;
}
