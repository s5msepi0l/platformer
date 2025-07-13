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
			vec2 move = self->state->input->poll_axis() * speed;
			self->transform.pos += (move * self->state->deltatime);

			LOG("Player pos", this->self->transform.pos.x, this->self->transform.pos.y, "\n");
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
		vec2 player_size = {5, 5};

		player() {
			add_component(std::move(std::make_unique<character_controller>()));
			collidable = true;

			transform.size = player_size;
			transform.pos = player_size;

		}
};