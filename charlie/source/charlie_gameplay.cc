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

		/*
		void Stage::draw(Renderer& renderer)
		{
			for (auto& system : systems_) {
				if (!system->is_active()) {
					continue;
				}

				system->draw(renderer, components_, events_);
			}
		}
		*/

		InputSnapshot::InputSnapshot() : tick_(0), input_bits_(0), rotation_(0), turret_rotation(0), fire_(false)
		{
		}

		PosSnapshot::PosSnapshot() : tick_(0), rotation(0), turret_rotation(0)
		{
		}

		Interpolator::Interpolator() : interpolateTime_(Time(0.2)), acc_(Time(0.0)), buffersize_(60)
		{
		}

		Vector2 Interpolator::interpolate_pos(const float rtt) const
		{
			if (snapshots_.size() < 2)
			{
				return Vector2(0, 0);
			}
			const auto start = snapshots_[snapshots_.size() - 2];
			const auto end = snapshots_[snapshots_.size() - 1];
			const float t = acc_.as_milliseconds() / interpolateTime_.as_milliseconds() + rtt / 2;

			const Vector2 newPos = Vector2::lerp(start.position, end.position, t);
			return newPos;
		}

		float Interpolator::interpolate_rot() const
		{
			if (snapshots_.size() < 2)
			{
				return 0;
			}
			const auto start = snapshots_[snapshots_.size() - 2];
			const auto end = snapshots_[snapshots_.size() - 1];
			const float t = acc_.as_milliseconds() / interpolateTime_.as_milliseconds();

			return Vector2::lerp(start.rotation, end.rotation, t);;
		}

		float Interpolator::interpolate_turret_rot() const
		{
			if (snapshots_.size() < 2)
			{
				return 0;
			}
			const auto start = snapshots_[snapshots_.size() - 2];
			const auto end = snapshots_[snapshots_.size() - 1];
			const float t = acc_.as_milliseconds() / interpolateTime_.as_milliseconds();

			return Vector2::lerp(start.turret_rotation, end.turret_rotation, t);;
		}

		void Interpolator::add_position(PosSnapshot snapshot)
		{
			snapshots_.push_back(snapshot);
		}

		Inputinator::Inputinator() : buffer_{} {}

		/*
		void Inputinator::add_snapshot(InputSnapshot snapshot)
		{
			if (snapshot.tick_ == 0)
			{
				printf("missing tick from snapshot \n");
			}
			inputSnapshots_.push_back(snapshot);
		}
		*/

		void Inputinator::add_snapshot(InputSnapshot snapshot)
		{
			buffer_[snapshot.tick_ % buffersize_] = snapshot;
		}

		Vector2 Inputinator::get_corrected_position(const uint32 tick, const Time tickrate, const Vector2 serverpos, const float speed) const
		{
			Vector2 startingPos = serverpos;
			for (int i = 0; i < buffersize_; i++)
			{
				const auto input = buffer_[i];

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
			return inputSnapshots_[tick % buffersize_].position_;
		}

		void Inputinator::clear_old_inputs(uint32 tick)
		{
			return;
			/*
			DynamicArray<InputSnapshot> new_array;
			for (auto& input : inputSnapshots_)
			{
				if (input.tick_ >= tick)
				{
					new_array.push_back(input);
				}
			}
			for (auto& input : new_array)
			{
				inputSnapshots_.push_back(input);
			}
			*/
		}

		/*
		InputSnapshot Inputinator::get_snapshot(uint32 index)
		{
			for (auto& snapshot : inputSnapshots_)
			{
				if (snapshot.tick_ == index)
				{
					return snapshot;
				}
			}
			printf("no snapshot found \n");
			return InputSnapshot{};
		}
		*/
		InputSnapshot Inputinator::get_snapshot(uint32 index)
		{
			return buffer_[index % buffersize_];
		}

		ReliableMessageQueue::ReliableMessageQueue() : index_(-1), buffer_()
		{
		}

		void ReliableMessageQueue::add_message(uint32 tick, Message& msg)
		{
			buffer_[tick % 50] = msg;
			index_++;
		}

		Message ReliableMessageQueue::get_message(uint32 tick)
		{
			return buffer_[tick % 50];
		}
	} // !gameplay
} // !charlie
