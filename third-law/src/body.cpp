//! @file
//! @copyright See <a href="LICENSE.txt">LICENSE.txt</a>.

#pragma once

#include "body.hpp"

namespace law3 {
	body::body(entt::entity id, b2World* world, b2BodyType type, b2Vec2 position) : id{id}, world{world} {
		b2BodyDef def;
		def.type = type;
		def.position = position;
		_body = std::unique_ptr<b2Body, deleter>(world->CreateBody(&def), deleter{world});
	}

	auto body::create_fixture(b2FixtureDef* fixture_def) -> void {
		_body->CreateFixture(fixture_def);
	}

	auto body::position() const -> b2Vec2 {
		return _body->GetPosition();
	}

	auto body::angle() const -> float {
		return _body->GetAngle();
	}

	auto body::fixture() -> b2Fixture* {
		return _body->GetFixtureList();
	}
	auto body::fixture() const -> b2Fixture const* {
		return _body->GetFixtureList();
	}

	auto body::apply_force_to_center(b2Vec2 const& force) -> void {
		_body->ApplyForceToCenter(force, true);
	}

	auto body::apply_torque(float const& torque) -> void {
		_body->ApplyTorque(torque, true);
	}

	auto body::deleter::operator()(b2Body* body) -> void {
		world->DestroyBody(body);
	}
}
