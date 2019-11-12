//! @file
//! @copyright See <a href="LICENSE.txt">LICENSE.txt</a>.

#pragma once

#include <Box2D/Box2D.h>
#include <boost/units/unit.hpp>
#include <entt/entt.hpp>

#include <memory>

namespace law3 {
	//! Box2D body wrapper type with more idiomatic C++.
	struct body {
		entt::entity id;
		b2World* world;

		body(entt::entity id, b2World* world, b2BodyType type, b2Vec2 position);

		auto create_fixture(b2FixtureDef* fixture_def) -> void;

		auto position() const -> b2Vec2;

		auto angle() const -> float;

		auto fixture() -> b2Fixture*;
		auto fixture() const -> b2Fixture const*;

		auto apply_force_to_center(b2Vec2 const& force) -> void;

		auto apply_torque(float const& torque) -> void;

	private:
		struct deleter {
			b2World* world;
			auto operator()(b2Body* body) -> void;
		};

		std::unique_ptr<b2Body, deleter> _body;
	};
}
