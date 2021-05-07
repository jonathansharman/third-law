//! @file
//! @copyright See <a href="LICENSE.txt">LICENSE.txt</a>.

#include "body.hpp"
#include "camera.hpp"
#include "circle_animation.hpp"
#include "polygon_animation.hpp"

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <entt/entt.hpp>

#include <chrono>
#include <fstream>
#include <thread>

auto main() -> int {
	constexpr auto frame_duration = std::chrono::duration<float>(1.0f / 10.0f);

	// entt setup
	entt::registry reg;

	// SFML setup
	sf::RenderWindow window{sf::VideoMode{800, 600}, "Third Law", sf::Style::Default};
	window.setKeyRepeatEnabled(false);
	// Load graphics.
	sf::Texture test_texture;
	test_texture.loadFromFile("resources/test.png");

	law3::camera camera{{0, 0}, 4};

	b2Vec2 const gravity{0, -0.05f};
	b2World world{gravity};

	// Add static bodies.
	{
		auto ground_id = reg.create();
		reg.assign<law3::body>(ground_id, ground_id, &world, b2BodyType::b2_staticBody, b2Vec2{50, -10});
		b2PolygonShape ground_shape;
		ground_shape.SetAsBox(50, 10, {0, 0}, b2_pi / 16);
		b2FixtureDef ground_fixture_def;
		ground_fixture_def.shape = &ground_shape;
		reg.get<law3::body>(ground_id).create_fixture(&ground_fixture_def);
		// Add graphics.
		reg.assign<law3::polygon_animation>(ground_id, ground_id, reg, nullptr);
	}
	{
		auto ground_id = reg.create();
		reg.assign<law3::body>(ground_id, ground_id, &world, b2BodyType::b2_staticBody, b2Vec2{-50, -10});
		b2PolygonShape ground_shape;
		ground_shape.SetAsBox(50, 10, {0, 0}, -b2_pi / 16);
		b2FixtureDef ground_fixture_def;
		ground_fixture_def.shape = &ground_shape;
		reg.get<law3::body>(ground_id).create_fixture(&ground_fixture_def);
		// Add graphics.
		reg.assign<law3::polygon_animation>(ground_id, ground_id, reg, nullptr);
	}

	// Add balls.
	std::vector<entt::entity> ball_ids;
	for (int i = 0; i < 20; ++i) {
		ball_ids.push_back(reg.create());
		auto& ball_body = reg.assign<law3::body>(
			ball_ids.back(), ball_ids.back(), &world, b2BodyType::b2_dynamicBody, b2Vec2{0, 4});
		b2FixtureDef fixture_def;
		b2CircleShape ball_shape;
		ball_shape.m_radius = 1;
		fixture_def.shape = &ball_shape;
		fixture_def.density = 1;
		fixture_def.friction = 0.1f;
		fixture_def.restitution = 0.5f;
		reg.get<law3::body>(ball_ids.back()).create_fixture(&fixture_def);
		// Add graphics.
		reg.assign<law3::circle_animation>(ball_ids.back(), ball_ids.back(), reg, &test_texture);
	}

	bool running = true;
	auto last_update = std::chrono::steady_clock::now();
	while (running) {
		// Handle events.
		sf::Event e;
		while (window.pollEvent(e)) {
			switch (e.type) {
				case sf::Event::Closed:
					running = false;
					break;
				case sf::Event::MouseWheelScrolled:
					camera.scale += e.mouseWheelScroll.delta / 2;
					break;
			}
		}
		// Apply central forces.
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			b2Vec2 const force{0, 0.3f};
			reg.get<law3::body>(ball_ids.back()).apply_force_to_center(force);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			b2Vec2 const force{-0.1f, 0};
			reg.get<law3::body>(ball_ids.back()).apply_force_to_center(force);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			b2Vec2 const force{0.1f, 0};
			reg.get<law3::body>(ball_ids.back()).apply_force_to_center(force);
		}
		// Apply torque.
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
			reg.get<law3::body>(ball_ids.back()).apply_torque(0.15f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
			reg.get<law3::body>(ball_ids.back()).apply_torque(-0.15f);
		}
		// Camera controls.
		constexpr float pan_speed = 0.1f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { camera.position.y -= pan_speed; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { camera.position.y += pan_speed; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { camera.position.x -= pan_speed; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { camera.position.x += pan_speed; }

		// Update world.
		constexpr int32 velocity_iterations = 8;
		constexpr int32 position_iterations = 3;
		world.Step(frame_duration.count(), velocity_iterations, position_iterations);

		// Update animations.
		reg.view<law3::polygon_animation>().each([](auto& animation) { animation.update(); });
		reg.view<law3::circle_animation>().each([](auto& animation) { animation.update(); });

		// Draw.
		window.clear();
		camera.apply(window);
		// Draw polygon shapes.
		reg.view<law3::polygon_animation>().each([&](auto const& animation) { window.draw(animation); });
		// Draw circle shapes.
		reg.view<law3::circle_animation>().each([&](auto const& animation) { window.draw(animation); });
		window.display();
	}
}
