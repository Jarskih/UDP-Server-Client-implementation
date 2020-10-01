// charlie_gameplay.cc

#include "charlie_gameplay.hpp"

namespace charlie {
	namespace gameplay {
		// static 
		uint32 ComponentBase::next()
		{
			static uint32 index_ = 0;
			return index_++;
		}

		ComponentContext::~ComponentContext()
		{
			for (auto& container : containers_) {
				delete container;
			}
		}

		// static 
		uint32 EventBase::next()
		{
			static uint32 index_ = 0;
			return index_++;
		}

		EventContext::~EventContext()
		{
			for (auto& queue : queues_) {
				delete queue;
			}
		}

		//static 
		uint32 SystemBase::next()
		{
			static uint32 index = 0;
			return index++;
		}

		System::System()
			: active_(false)
		{
		}

		bool System::is_active() const
		{
			return active_;
		}

		void System::activate()
		{
			active_ = true;
		}

		void System::deactivate()
		{
			active_ = false;
		}

		Stage::Stage()
			: active_(false)
		{
		}

		Stage::~Stage()
		{
			for (auto& system : systems_) {
				delete system;
			}
		}

		bool Stage::is_active() const
		{
			return active_;
		}

		void Stage::activate()
		{
			active_ = true;
		}

		void Stage::deactivate()
		{
			active_ = false;
		}

		void Stage::update(const Time& dt)
		{
			for (auto& system : systems_) {
				if (!system->is_active()) {
					continue;
				}

				system->update(dt, components_, events_);
			}

			events_.clear_all();
		}

		void Stage::draw(Renderer& renderer)
		{
			for (auto& system : systems_) {
				if (!system->is_active()) {
					continue;
				}

				system->draw(renderer, components_, events_);
			}
		}

		InputSnapshot::InputSnapshot() : tick_(0), input_bits_(0)
		{
		}

		Interpolator::Interpolator() : inputSnapshots_{}, interpolateTime_(Time(0.1)), index_(0), bufferSize_(0),
			acc_(0)
		{
			interpolateTime_ = 0.1f;
			bufferSize_ = 20;
		}

		Vector2 Interpolator::interpolate(const Vector2 start, const Vector2 end, const Time rtt) const
		{
			const auto startTime = Time::now().as_seconds() - interpolateTime_.as_seconds() - rtt.as_seconds() / 2;
			const auto endTime = Time::now();
			const auto t = endTime - startTime + acc_;
			printf("lerp t: %f\n", t.as_seconds());
			const Vector2 newPos = Vector2::lerp(start, end, t.as_seconds());
			return newPos;
		}

		void Interpolator::add_snapshot(InputSnapshot snapshot)
		{
			inputSnapshots_[index_ % bufferSize_] = snapshot;
			index_++;
		}

		Vector2 Interpolator::get_position(uint32 tick, const Time tickrate)
		{
			Vector2 startingPos;
			for (auto& snapshot : inputSnapshots_)
			{
				if (snapshot.tick_ == tick)
				{
					startingPos = snapshot.position_;
				}

				if (snapshot.tick_ > tick)
				{

				}
				// note: update player
				const bool player_move_up = snapshot.input_bits_ & (1 << int32(gameplay::Action::Up));
				const bool player_move_down = snapshot.input_bits_ & (1 << int32(gameplay::Action::Down));
				const bool player_move_left = snapshot.input_bits_ & (1 << int32(gameplay::Action::Left));
				const bool player_move_right = snapshot.input_bits_ & (1 << int32(gameplay::Action::Right));

				Vector2 direction;
				if (player_move_up) {
					direction.y_ -= 1.0f;
				}
				if (player_move_down) {
					direction.y_ += 1.0f;
				}
				if (player_move_left) {
					direction.x_ -= 1.0f;
				}
				if (player_move_right) {
					direction.x_ += 1.0f;
				}

				const float speed = 100.0;
				if (direction.length() > 0.0f) {
					direction.normalize();
					startingPos += direction * speed * tickrate.as_seconds();
				}
			}

			return startingPos;
		}
	} // !gameplay
} // !charlie
