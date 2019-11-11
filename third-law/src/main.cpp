//! @file
//! @copyright See <a href="LICENSE.txt">LICENSE.txt</a>.

#include <Box2D/Box2D.h>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <chrono>
#include <fstream>
#include <thread>

#include "body.hpp"

auto apply_view(sf::RenderWindow& window, sf::Vector2f const& position, float scale) -> void {
	auto const window_size = window.getSize();
	sf::Vector2f const view_center{(position.x) / scale, (position.y) / scale};
	sf::Vector2f const view_size{window_size.x / scale, window_size.y / scale};
	window.setView({view_center, view_size});
}

auto main() -> int {
	constexpr int dflt_x_res = 800;
	constexpr int dflt_y_res = 600;

	constexpr float32 time_step = 1.0f / 60.0f;
	constexpr auto frame_duration = std::chrono::duration<float>(time_step);

	sf::RenderWindow window{sf::VideoMode{dflt_x_res, dflt_y_res}, "Third Law", sf::Style::Default};
	window.setKeyRepeatEnabled(false);

	constexpr float view_scale = 4;
	sf::Vector2f view_position{0, 0};

	b2Vec2 const gravity{0, -0.05f};
	b2World world{gravity};

	// Define ground body.
	law3::body ground_body{&world, b2BodyType::b2_staticBody, {0, -10}};
	b2PolygonShape ground_shape;
	ground_shape.SetAsBox(50, 10, {0, 0}, b2_pi / 16);
	b2FixtureDef ground_fixture_def;
	ground_fixture_def.shape = &ground_shape;
	ground_body.create_fixture(&ground_fixture_def);

	// Define ball body.
	law3::body ball_body{&world, b2BodyType::b2_dynamicBody, {0, 4}};
	b2FixtureDef fixture_def;
	b2CircleShape ball_shape;
	ball_shape.m_radius = 1;
	fixture_def.shape = &ball_shape;
	fixture_def.density = 1;
	fixture_def.friction = 0.1f;
	fixture_def.restitution = 0.5f;
	ball_body.create_fixture(&fixture_def);

	// Ground body graphics.
	sf::RectangleShape ground_sfml_shape{{100, 20}};
	ground_sfml_shape.setOrigin(50, 10);
	ground_sfml_shape.setPosition({0, 10});
	ground_sfml_shape.setRotation(-180.0f / 16);
	// Dynamic body graphics.
	sf::CircleShape ball_sfml_shape{1};
	ball_sfml_shape.setOrigin(1, 1);
	sf::Texture texture;
	texture.loadFromFile("resources/test.png");
	ball_sfml_shape.setTexture(&texture);

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
			}
		}
		// Apply central forces.
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			b2Vec2 const force{0, 0.3f};
			ball_body.apply_force_to_center(force);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			b2Vec2 const force{-0.1f, 0};
			ball_body.apply_force_to_center(force);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			b2Vec2 const force{0.1f, 0};
			ball_body.apply_force_to_center(force);
		}
		// Apply torque.
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) { ball_body.apply_torque(0.15f); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) { ball_body.apply_torque(-0.15f); }
		// Camera controls.
		constexpr float pan_speed = 0.1f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { view_position.y -= pan_speed; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { view_position.y += pan_speed; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { view_position.x -= pan_speed; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { view_position.x += pan_speed; }

		// Update world.
		constexpr int32 velocity_iterations = 8;
		constexpr int32 position_iterations = 3;
		world.Step(time_step, velocity_iterations, position_iterations);

		// Draw.
		window.clear();
		apply_view(window, view_position, view_scale);
		// Draw ground body.
		window.draw(ground_sfml_shape);
		{ // Draw dynamic body.
			b2Vec2 position = ball_body.position();
			ball_sfml_shape.setPosition(position.x, -position.y);
			float const angle = ball_body.angle();
			ball_sfml_shape.setRotation(angle * -180 / b2_pi);
			window.draw(ball_sfml_shape);
		}
		window.display();

		// Do timing.
		auto now = std::chrono::steady_clock::now();
		auto elapsed = now - last_update;
		last_update = now;
		if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed) < frame_duration) {
			std::this_thread::sleep_for(frame_duration - elapsed);
		}
	}
}
