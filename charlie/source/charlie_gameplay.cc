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

		PositionSnapshot::PositionSnapshot() : tick_(0), rotation(0), turret_rotation(0)
		{
		}


		Interpolator::Interpolator() : acc_(Time(0.0)), interpolate_time_(0.200f)
		{
		}

		int Interpolator::get_snapshot_index(const float interpolate_time) const
		{
			int snapshot_index = 0;

			for (size_t i = snapshots_.size() - 1; i > 0; i--)
			{
				if (snapshots_[i].tick_ < interpolate_time)
				{
					snapshot_index = (int)i;
					break;
				}
			}
			return snapshot_index;
		}

		PositionSnapshot Interpolator::interpolate(Time tickrate, uint32 server_tick, const float rtt) const
		{
			PositionSnapshot snapshot;

			if (snapshots_.size() < 2)
			{
				return snapshot;
			}

			// Find snapshot 200ms or more in the past
			const float interpolate_time = server_tick - (interpolate_time_ / tickrate.as_seconds());
			const int snapshot_index = get_snapshot_index(interpolate_time);

			const PositionSnapshot& start = snapshots_[snapshot_index];
			const PositionSnapshot& end = snapshots_[snapshots_.size() - 1];
			const float delta = tickrate.as_milliseconds() * (end.tick_ - start.tick_);
			const float t = acc_.as_milliseconds() / delta;
			snapshot.position = Vector2::lerp(start.position, end.position, t);
			snapshot.rotation = Vector2::lerp(start.rotation, end.rotation, t);
			snapshot.turret_rotation = Vector2::lerp(start.turret_rotation, end.turret_rotation, t);

			return snapshot;
		}

		void Interpolator::add_position(PositionSnapshot snapshot)
		{
			snapshots_.push_back(snapshot);
		}

		void Interpolator::clear_old_snapshots()
		{
			while (snapshots_.size() > 20)
			{
				snapshots_.erase(snapshots_.begin());
			}
		}

		Inputinator::Inputinator() : buffer_{} {}

		void Inputinator::add_snapshot(InputSnapshot snapshot)
		{
			buffer_[snapshot.tick_ % buffer_size_] = snapshot;
		}

		Vector2 Inputinator::correct_predicted_position(const uint32 tick, const Time tickrate, const Vector2 server_position, const float speed)
		{
			Vector2 position_at_tick = server_position;
			for (int i = 0; i < buffer_size_; i++)
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

					direction.normalize();
					position_at_tick += direction * speed * tickrate.as_seconds();
					modify_position(tick, position_at_tick);
				}
			}
			return position_at_tick;
		}

		Vector2 Inputinator::get_position_from_tick(uint32 tick) const
		{
			return buffer_[tick % buffer_size_].position_;
		}

		void Inputinator::modify_position(const uint32 tick, const Vector2 position)
		{
			buffer_[tick & buffer_size_].position_ = position;
		}

		InputSnapshot Inputinator::get_snapshot(uint32 tick)
		{
			return buffer_[tick % buffer_size_];
		}

		ReliableMessageQueue::ReliableMessageQueue() : buffer_{}, index_(0)
		{
		}

		void ReliableMessageQueue::add_message(Message& msg)
		{
			buffer_[index_ % buffer_size_] = msg;
			index_ += 1;
		}

		Message ReliableMessageQueue::get_message(uint32 tick)
		{
			return buffer_[tick % buffer_size_];
		}

		void ReliableMessageQueue::mark_received(const uint16 id)
		{
			for (auto& msg : buffer_)
			{
				if (msg.seq_ == (int16)id)
				{
					msg.received_ = true;
					printf("RELIABLE MESSAGE: Received message with acq %i \n", id);
					break;
				}
			}
		}
	} // !gameplay
} // !charlie
