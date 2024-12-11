#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

#include "ShaderProgram.h"
#include "Camera.h"
#include "Cube.h"
#include "PerlinNoise.h"
#include "Chunk.h"
#include "Ray.h"


#define SCR_WIDTH 800
#define SCR_HEIGHT 600


int main() {
	bool running = true;
	sf::ContextSettings contextSettings;
	contextSettings.depthBits = 24;
	contextSettings.sRgbCapable = false;
	contextSettings.majorVersion = 4;
	contextSettings.minorVersion = 6;

	sf::Window window(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "Minecraft", sf::Style::Default, contextSettings);
	window.setVerticalSyncEnabled(true);
	window.setActive(true);

	gladLoadGL();
	glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	ShaderProgram shaderProgram;
	Camera camera(glm::vec3( 16.0f, 16.0f, 40.0f), glm::vec3(0.0f, 0.0f, 0.0f), -90.0f, -25.0f);

	CubePalette palette;
	auto chunk = std::make_unique<Chunk<32, 32, 64>>(glm::vec2(0.0f), palette);
	Chunk<32, 32, 64>::HitRecord hitRecord;

	sf::Vector2i mousePosition = sf::Mouse::getPosition();
	sf::Clock clock;

	while (running) {
		const float dt = clock.restart().asSeconds();
		const sf::Vector2i newMousePosition = sf::Mouse::getPosition();
		camera.Rotate(newMousePosition - mousePosition);
		mousePosition = newMousePosition;

		sf::Event e;
		if (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}
			else if (e.type == sf::Event::Resized) {
				glViewport(0, 0, e.size.width, e.size.height);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				camera.MoveForward(dt);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				camera.MoveBackward(dt);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				camera.MoveLeft(dt);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				camera.MoveRight(dt);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
				camera.MoveDown(dt);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
				camera.MoveUp(dt);
			}

			if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
				Ray ray{ camera.GetPosition() + glm::vec3{0.5f, 0.5f, 0.5f}, camera.GetFront() };
				Ray::HitType type = chunk->Hit(ray, 0.3f, 7.0f, hitRecord);
				if (type == Ray::HitType::Hit) {
					chunk->RemoveBlock(hitRecord.m_cubeIndex.y, hitRecord.m_cubeIndex.z, hitRecord.m_cubeIndex.x);
				}
			}
			
			if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Right) {
				Ray ray{ camera.GetPosition() + glm::vec3{0.5f, 0.5f, 0.5f}, camera.GetFront() };
				Ray::HitType type = chunk->Hit(ray, 0.3f, 7.0f, hitRecord);
				if (type == Ray::HitType::Hit) {
					chunk->PlaceBlock(hitRecord.m_neighbourIndex.y, hitRecord.m_neighbourIndex.z, hitRecord.m_neighbourIndex.x, Cube::Type::Stone);
				}
			}

		}
		
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		chunk->Draw(shaderProgram, camera);

		window.display();
	}
}