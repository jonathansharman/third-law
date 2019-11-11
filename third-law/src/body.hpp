//! @file
//! @copyright See <a href="LICENSE.txt">LICENSE.txt</a>.

#pragma once

#include <Box2D/Box2D.h>

namespace law3 {
	//! Box2D body wrapper type with more idiomatic C++.
	struct body {
		b2World* world;

		body(b2World* world, b2BodyType type, b2Vec2 position) : world{world} {
			b2BodyDef def;
			def.type = type;
			def.position = position;
			_body = world->CreateBody(&def);
		}

		~body() noexcept {
			world->DestroyBody(_body);
		}

		auto create_fixture(b2FixtureDef* fixture_def) {
			_body->CreateFixture(fixture_def);
		}

		auto position() const {
			return _body->GetPosition();
		}

		auto angle() const {
			return _body->GetAngle();
		}

		auto apply_force_to_center(b2Vec2 const& force) {
			_body->ApplyForceToCenter(force, true);
		}

		auto apply_torque(float const& torque) {
			_body->ApplyTorque(torque, true);
		}

	private:
		b2Body* _body;
	};
}
