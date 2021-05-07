//! @file
//! @copyright See <a href="LICENSE.txt">LICENSE.txt</a>.

#include "body.hpp"

#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

namespace law3 {
	//! Circle animation component. Requires the entity to have a body with a single circle-shaped fixture.
	struct circle_animation : public sf::Drawable {
		circle_animation(entt::entity id, entt::registry& reg, sf::Texture const* texture) : _id{id}, _reg{&reg} {
			if (texture) { _shape.setTexture(texture); }

			b2Fixture* fixture = _reg->get<law3::body>(_id).fixture();
			auto b2_shape = dynamic_cast<b2CircleShape*>(fixture->GetShape());
			_shape.setRadius(b2_shape->m_radius);
			_shape.setOrigin(b2_shape->m_radius, b2_shape->m_radius);
		}

		//!	Updates this animation to match its associated body.
		auto update() -> void {
			auto& body = _reg->get<law3::body>(_id);
			auto const position = body.position();
			_shape.setPosition(position.x, -position.y);
			_shape.setRotation(body.angle() * -180 / b2_pi);
		}

	private:
		entt::entity _id;
		entt::registry* _reg;
		sf::CircleShape _shape;

		auto draw(sf::RenderTarget& target, sf::RenderStates states) const -> void {
			target.draw(_shape, states);
		}
	};
}
