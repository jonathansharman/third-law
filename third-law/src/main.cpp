//! @file
//! @copyright See <a href="LICENSE.txt">LICENSE.txt</a>.

#include <Box2D/Box2D.h>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <chrono>
#include <fstream>
#include <thread>

auto main() -> int {
	constexpr float32 time_step = 1.0f / 60.0f;
	constexpr auto frame_duration = std::chrono::duration<float>(time_step);

	sf::RenderWindow window{sf::VideoMode{800, 600}, "Third Law", sf::Style::Default};
	window.setKeyRepeatEnabled(false);

	b2Vec2 const gravity{0.0f, -0.05f};
	b2World world{gravity};

	b2BodyDef ground_body_def;
	ground_body_def.position.Set(0.0f, -10.0f);
	b2Body* ground_body = world.CreateBody(&ground_body_def);
	b2PolygonShape ground_box;
	ground_box.SetAsBox(50.0f, 10.0f);
	ground_body->CreateFixture(&ground_box, 0.0f);

	b2BodyDef ball_body_def;
	ball_body_def.type = b2_dynamicBody;
	ball_body_def.position.Set(0.0f, 4.0f);
	b2Body* ball_body = world.CreateBody(&ball_body_def);
	b2CircleShape ball;
	ball.m_radius = 1.0f;

	b2FixtureDef fixture_def;
	fixture_def.shape = &ball;
	fixture_def.density = 1.0f;
	fixture_def.friction = 0.1f;
	ball_body->CreateFixture(&fixture_def);

	constexpr float scale = 15.0f;
	// Ground body graphics.
	sf::RectangleShape ground_sfml_shape{{100.0f * scale, 20.0f * scale}};
	ground_sfml_shape.setOrigin(50.0f * scale, 10.0f * scale);
	ground_sfml_shape.setPosition({400.0f, 300.0f - -10.0f * scale});
	// Dynamic body graphics.
	sf::CircleShape ball_sfml_shape{1.0f * scale};
	ball_sfml_shape.setOrigin(1.0f * scale, 1.0f * scale);
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
				case sf::Event::KeyPressed:
					switch (e.key.code) {
						case sf::Keyboard::Up: {
							b2Vec2 const force{0.0f, 200.0f};
							ball_body->ApplyForceToCenter(force, true);
							break;
						}
					}
					break;
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			b2Vec2 const force{-0.1f, 0.0f};
			ball_body->ApplyForceToCenter(force, true);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			b2Vec2 const force{0.1f, 0.0f};
			ball_body->ApplyForceToCenter(force, true);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) { ball_body->ApplyTorque(0.15f, true); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) { ball_body->ApplyTorque(-0.15f, true); }

		// Update world.
		constexpr int32 velocity_iterations = 8;
		constexpr int32 position_iterations = 3;
		world.Step(time_step, velocity_iterations, position_iterations);

		// Draw.
		window.clear();
		// Draw ground body.
		window.draw(ground_sfml_shape);
		{ // Draw dynamic body.
			b2Vec2 position = ball_body->GetPosition();
			ball_sfml_shape.setPosition(400.0f + position.x * scale, 300.0f - position.y * scale);
			float const angle = ball_body->GetAngle();
			ball_sfml_shape.setRotation(angle * -180.0f / b2_pi);
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
