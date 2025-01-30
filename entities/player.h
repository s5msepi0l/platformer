#pragma once

#include "../src/game_engine.h"

class character_controller: public game_engine::component {
	public:
		f32 speed = 0.1;

		void init(){
			std::cout << "character_controller::init()\n";
		}

		void stop(){

			std::cout << "character_controller::stop()\n";
		}

		void tick(){
			vec2 move = game_engine::state::state.input->poll_axis() * speed;
			self->transform.pos += (move * game_engine::state::state.deltatime);
			//self->transform.pos += move;
			//std::cout << "deltatime: " << game_engine::state::state.deltatime << "\n";
			//std::cout << "x: " << self->transform.pos.x << "\n";
			//std::cout << "y: " << self->transform.pos.y << "\n";
		}

		void collision_enter(game_engine::entity *other) {};
        void collision_exit(game_engine::entity *other) {};
};

class player : public game_engine::entity {
	public:
		default_rgb *model;
		
		vec2 player_size = {5, 5};

		player() {
			add_component(std::move(std::make_unique<character_controller>()));
			collidable = true;

			graphics.init(render_type::DEFAULT_RGB);
			
			transform.size = player_size;

			// remove this later, im just testing if this works 
			model = static_cast<default_rgb*>(graphics.data);
			model->pixels.reserve((u32)player_size.x, (u32)player_size.y);

			//load the character with a basic color
			for (u32 x = 0; x < player_size.x; x++){
				for (u32 y = 0; y < player_size.y; y++) {
					model->pixels[x][y] = {
						u8(255 / (x + 1)),
						u8(255 / (y + 1)),
						u8(255 / (x + 1)),
					};
				}
			}


		}
};