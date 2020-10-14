// charlie_gameplay.cc

#include "charlie_gameplay.hpp"

namespace charlie {
	namespace gameplay {
		Entity::Entity() : id_(0)
		{
		}

		Entity::Entity(const Vector2 pos, const uint32 id) : position_(pos), id_(0)
		{
		}

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

		PosSnapshot::PosSnapshot() : tick_(0)
		{
		}

		Interpolator::Interpolator() : interpolateTime_(Time(0.2)), acc_(Time(0.0)), buffersize_(60)
		{
		}

		Vector2 Interpolator::interpolate() const
		{
			if (snapshots_.size() < 2)
			{
				return Vector2(0, 0);
			}
			const auto start = snapshots_[snapshots_.size() - 2];
			const auto end = snapshots_[snapshots_.size() - 1];
			const float t = acc_.as_milliseconds() / interpolateTime_.as_milliseconds();

			const Vector2 newPos = Vector2::lerp(start.position, end.position, t);
			return newPos;
		}

		void Interpolator::add_position(PosSnapshot snapshot)
		{
			snapshots_.push_back(snapshot);
			acc_ = Time(0.0);
		}

		Inputinator::Inputinator()
		{
		}

		void Inputinator::add_snapshot(InputSnapshot snapshot)
		{
			inputSnapshots_.push(snapshot);
		}

		Vector2 Inputinator::get_position(const uint32 tick, const Time tickrate, const Vector2 serverpos, const float speed)
		{
			Vector2 startingPos = serverpos;
			const auto inputSnapshots = inputSnapshots_;
			for (int i = 0; i < static_cast<int>(inputSnapshots.size()); i++)
			{
				const auto input = inputSnapshots_.front();

				if (inputSnapshots_.empty())
				{
					break;
				}
				inputSnapshots_.pop();

				if (input.tick_ > tick)
				{
					// simulate player past movement
					const bool player_move_up = input.input_bits_ & (1 << int32(gameplay::Action::Up));
					const bool player_move_down = input.input_bits_ & (1 << int32(gameplay::Action::Down));
					const bool player_move_left = input.input_bits_ & (1 << int32(gameplay::Action::Left));
					const bool player_move_right = input.input_bits_ & (1 << int32(gameplay::Action::Right));

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

					if (direction.length() > 0.0f) {
						direction.normalize();
						startingPos += direction * speed * tickrate.as_seconds();
					}
				}
			}
			return startingPos;
		}

		Vector2 Inputinator::old_pos(uint32 tick)
		{
			const auto inputSnapshots = inputSnapshots_;
			for (int i = 0; i < static_cast<int>(inputSnapshots.size()); i++)
			{
				const auto input = inputSnapshots_.front();
				inputSnapshots_.pop();
				if (input.tick_ < tick)
				{
					continue;
				}
				if (input.tick_ == tick)
				{
					return input.position_;
				}
			}
			return Vector2(0, 0);
		}
	} // !gameplay
} // !charlie
