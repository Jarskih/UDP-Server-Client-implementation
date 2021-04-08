// charlie_gameplay.cc

#include "charlie_gameplay.hpp"

namespace charlie {
	namespace gameplay {

		InputSnapshot::InputSnapshot() : tick_(0), input_bits_(0), turret_rotation(0), fire_(false), rotation_(0)
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
			while (snapshots_.size() > buffer_)
			{
				snapshots_.erase(snapshots_.begin());
			}
		}

		void Inputinator::add_snapshot(InputSnapshot snapshot)
		{
			if (snapshots_.size() > static_cast<size_t>(size_))
			{
				snapshots_.pop_front();
			}

			snapshots_.emplace_back(snapshot);
		}

		Vector2 Inputinator::correct_predicted_position(const int32 tick, const Time tick_rate, const Vector2 server_position, const float speed)
		{
			Vector2 position_at_tick = server_position;
			for (const auto& input : snapshots_)
			{
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
					position_at_tick += direction * speed * tick_rate.as_seconds();
					update_predicted_position(input.tick_, position_at_tick);
				}
			}
			return position_at_tick;
		}

		void Inputinator::update_predicted_position(const int32 tick, const Vector2 position)
		{
			for (InputSnapshot& snapshot : snapshots_)
			{
				if (snapshot.tick_ == tick)
				{
					snapshot.position_ = position;
				}
			}
		}

		InputSnapshot Inputinator::get_snapshot(int32 tick)
		{
			for (const InputSnapshot& snapshot : snapshots_)
			{
				if (snapshot.tick_ == tick)
				{
					return snapshot;
				}
			}
			return {};
		}

		std::deque<InputSnapshot> Inputinator::get_snapshots() const
		{
			return snapshots_;
		}

		bool Inputinator::hasSnapshot(int32 server_tick)
		{
			for (const InputSnapshot& snapshot : snapshots_)
			{
				if (snapshot.tick_ == server_tick)
				{
					return true;
				}
			}
			return false;
		}

		ReliableMessageQueue::ReliableMessageQueue() : buffer_{}, index_(0)
		{
		}

		void ReliableMessageQueue::add_message(Message& msg)
		{
			buffer_[index_ % buffer_size_] = msg;
			index_ += 1;
		}

		Message ReliableMessageQueue::get_message(int32 tick)
		{
			return buffer_[tick % buffer_size_];
		}

		void ReliableMessageQueue::mark_received(const int32 id)
		{
			for (auto& msg : buffer_)
			{
				if (msg.id_ == id)
				{
					msg.received_ = true;
					printf("RELIABLE MESSAGE: Received message with id %i \n", (int)msg.id_);
					break;
				}
			}
		}
	} // !gameplay
} // !charlie
