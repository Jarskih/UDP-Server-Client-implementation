// charlie_gameplay.hpp

#ifndef CHARLIE_GAMEPLAY_HPP_INCLUDED
#define CHARLIE_GAMEPLAY_HPP_INCLUDED

#include <charlie.hpp>
#include <string>

#include "charlie_messages.hpp"

namespace charlie {
	namespace gameplay {

		enum class Action {
			Up,
			Down,
			Left,
			Right,
		};

		struct InputCommand
		{
			int32 id_;
			uint8 input_bits_;
			float rot_;
			bool fire_;
			int32 tick_;
		};

		struct InputSnapshot {
			InputSnapshot();
			int32 tick_;
			uint8 input_bits_;
			Vector2 position_;
			float turret_rotation;
			bool fire_;
			float rotation_;
		};

		struct PositionSnapshot
		{
			PositionSnapshot();
			int32 tick_;
			Vector2 position;
			Time servertime_;
			float rotation;
			float turret_rotation;
		};

		struct Interpolator {
			Interpolator();
			void clear_old_snapshots();
			PositionSnapshot interpolate(Time tickrate, uint32 server_tick, float rtt) const;
			void add_position(PositionSnapshot snapshot);
			int get_snapshot_index(float interpolate_time) const;
			Time acc_;
			const float interpolate_time_;
			DynamicArray<PositionSnapshot> snapshots_;
		private:
			const int buffer_ = 20;
		};

		struct Inputinator
		{
			void add_snapshot(InputSnapshot snapshot);
			Vector2 correct_predicted_position(int32 tick, Time tick_rate, Vector2 server_position, float speed);
			void update_predicted_position(int32 tick, Vector2 position);
			InputSnapshot get_snapshot(int32 index);
			std::deque<InputSnapshot> get_snapshots() const;
			bool hasSnapshot(int32 server_tick);
		private:
			std::deque<InputSnapshot> snapshots_;
			int size_ = 32;
		};

		struct Message
		{
			uint16 seq_;
			int32 id_;
			bool received_;
		};

		struct ReliableMessageQueue
		{
			ReliableMessageQueue();
			void add_message(Message& msg);
			Message get_message(int32 tick);
			void mark_received(int32 id);
			static constexpr int buffer_size_ = 100;
			Message buffer_[buffer_size_];
			uint32 index_;
		};

	} // !gameplay
} // !charlie

#endif // !CHARLIE_GAMEPLAY_HPP_INCLUDED
