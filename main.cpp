#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

#include "ShaderProgram.h"
#include "Camera.h"
#include "Ray.h"
#include "PerlinNoise.h"
#include "Chunk.h"
#include "World.h"


#define SCR_WIDTH 800	
#define SCR_HEIGHT 600


int main() {
	bool running = true;
	sf::ContextSettings contextSettings;
	contextSettings.depthBits = 24;
	contextSettings.sRgbCapable = false;
	contextSettings.majorVersion = 4;
	contextSettings.minorVersion = 6;

	sf::RenderWindow window(sf::VideoMode({ SCR_WIDTH, SCR_HEIGHT }), "Minecraft", sf::State::Windowed, contextSettings);
	window.setVerticalSyncEnabled(false);
	window.setFramerateLimit(120);
	window.setActive(true);

	ImGui::SFML::Init(window, true);

	gladLoadGL();
	glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));

	ShaderProgram shaderProgram;
	Camera camera(glm::vec3( 0.0f, 20.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), -90.0f, -25.0f);

	CubePalette palette;
	World world;
	Chunk<CHUNK_DEPTH, CHUNK_WIDTH, CHUNK_HEIGHT>::HitRecord hitRecord{};

	sf::Vector2i mousePosition = sf::Mouse::getPosition();
	sf::Clock clock;
	sf::Clock uiClock;

	while (running) {
		static float sinceLastWorldUpdate = 2;
		const float dt = clock.restart().asSeconds();
		const sf::Vector2i newMousePosition = sf::Mouse::getPosition();
		camera.Rotate(newMousePosition - mousePosition);
		mousePosition = newMousePosition;

		sinceLastWorldUpdate += dt;
		if (sinceLastWorldUpdate > 2.0f) {
			world.updateChunks(camera, palette);
			sinceLastWorldUpdate = 0;
		}
		
		
		while (const std::optional event = window.pollEvent()) {
			if(event.has_value())
				ImGui::SFML::ProcessEvent(window, *event);


			if (event->is<sf::Event::Closed>()) {
				running = false;
				break;
			}
			else if (const auto* sz = event->getIf<sf::Event::Resized>()) {
				glViewport(0, 0, sz->size.x, sz->size.y);
			}
			
			if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
				if (key->scancode == sf::Keyboard::Scan::W) {
					camera.MoveForward(dt);
				}
				if (key->scancode == sf::Keyboard::Scan::S) {
					camera.MoveBackward(dt);
				}
				if (key->scancode == sf::Keyboard::Scan::A){
					camera.MoveLeft(dt);
				}
				if (key->scancode == sf::Keyboard::Scan::D){
					camera.MoveRight(dt);
				}
				if (key->scancode == sf::Keyboard::Scan::Q){
					camera.MoveDown(dt);
				}
				if (key->scancode == sf::Keyboard::Scan::E) {
					camera.MoveUp(dt);
				}
			}
			
			if (const auto* mouseClicked = event->getIf<sf::Event::MouseButtonReleased>()) {
				if (mouseClicked->button == sf::Mouse::Button::Left) {
					Ray ray{ camera.GetPosition() + glm::vec3{0.5f, 0.5f, 0.5f}, camera.GetFront() };
					for (auto& chunk : world.getChunks()) {
						Ray::HitType type = chunk.second->Hit(ray, 0.1f, 7.0f, hitRecord);
						if (type == Ray::HitType::Hit) {
							chunk.second->RemoveBlock(hitRecord.m_cubeIndex.x, hitRecord.m_cubeIndex.y, hitRecord.m_cubeIndex.z);
							break;
						}
					}
				}
				else if (mouseClicked->button == sf::Mouse::Button::Right) {
					Ray ray{ camera.GetPosition() + glm::vec3{0.5f, 0.5f, 0.5f}, camera.GetFront() };
					for (auto& chunk : world.getChunks()) {
						Ray::HitType type = chunk.second->Hit(ray, 0.1f, 7.0f, hitRecord);
						if (type == Ray::HitType::Hit) {
							if (hitRecord.m_neighbourIndex.x == CHUNK_DEPTH) {
								world.getChunks().at(chunk.first + glm::ivec2{ 1, 0 })->PlaceBlock(0, hitRecord.m_neighbourIndex.y, hitRecord.m_neighbourIndex.z, Cube::Type::Stone);
							}
							else if (hitRecord.m_neighbourIndex.x < 0) {
								world.getChunks().at(chunk.first - glm::ivec2{ 1, 0 })->PlaceBlock(CHUNK_DEPTH - 1, hitRecord.m_neighbourIndex.y, hitRecord.m_neighbourIndex.z, Cube::Type::Stone);
							}
							else if (hitRecord.m_neighbourIndex.y == CHUNK_WIDTH) {
								world.getChunks().at(chunk.first + glm::ivec2{ 0, 1 })->PlaceBlock(hitRecord.m_neighbourIndex.x, 0, hitRecord.m_neighbourIndex.z, Cube::Type::Stone);
							}
							else if (hitRecord.m_neighbourIndex.y < 0) {
								world.getChunks().at(chunk.first - glm::ivec2{ 0, 1 })->PlaceBlock(hitRecord.m_neighbourIndex.x, CHUNK_WIDTH - 1, hitRecord.m_neighbourIndex.z, Cube::Type::Stone);
							}
							else {
								chunk.second->PlaceBlock(hitRecord.m_neighbourIndex.x, hitRecord.m_neighbourIndex.y, hitRecord.m_neighbourIndex.z, Cube::Type::Stone);
							}
							break;
						}
					}
				}
			}
		}
		
		ImGui::SFML::Update(window, uiClock.restart());

		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		for (auto& chunk : world.getChunks()) {
			chunk.second->Draw(shaderProgram, camera);
		}


		Ray ray{ camera.GetPosition() + glm::vec3{ 0.5, 0.5, 0.5 }, camera.GetFront() };
		Ray::HitType type = Ray::HitType::Miss;
		for (auto& chunk : world.getChunks()) {
			type = chunk.second->Hit(ray, 0.1f, 7.0f, hitRecord);
		}
		ImGui::Begin("Debug info");
		ImGui::Text("%ffps", 1/dt);
		ImGui::Text("%fms", dt*1000);
		ImGui::Text("x:%f, y:%f, z:%f", camera.GetPosition().x + 0.5, camera.GetPosition().y + 0.5, camera.GetPosition().z + 0.5);
		ImGui::Text("Looking at:\nx:%d, y:%d, z:%d", hitRecord.m_cubeIndex.x, hitRecord.m_cubeIndex.z, hitRecord.m_cubeIndex.y);
		ImGui::End();

		ImGui::SFML::Render(window);

		static sf::RectangleShape crosshair({ 10, 10 });
		crosshair.setFillColor(sf::Color({ 255,255,255 }));
		crosshair.setPosition({ SCR_WIDTH / 2 - crosshair.getSize().x / 2, SCR_HEIGHT / 2 - crosshair.getSize().y / 2 });
		window.draw(crosshair);
		window.display();
	}

	ImGui::SFML::Shutdown();
	window.close();
}