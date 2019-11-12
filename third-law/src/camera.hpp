//! @file
//! @copyright See <a href="LICENSE.txt">LICENSE.txt</a>.

#include <SFML/Graphics.hpp>

namespace law3 {
	struct camera {
		sf::Vector2f position;
		float scale;

		auto apply(sf::RenderWindow& window) const -> void {
			auto const window_size = window.getSize();
			sf::Vector2f const view_center{(position.x) / scale, (position.y) / scale};
			sf::Vector2f const view_size{window_size.x / scale, window_size.y / scale};
			window.setView({view_center, view_size});
		}
	};
}
