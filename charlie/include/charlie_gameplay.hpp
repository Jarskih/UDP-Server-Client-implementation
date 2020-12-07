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
			uint32 id_;
			uint8 input_bits_;
			float rot_;
			bool fire_;
		};

		struct InputSnapshot {
			InputSnapshot();
			uint32 tick_;
			uint8 input_bits_;
			Vector2 position_;
			float rotation_;
			float turret_rotation;
			bool fire_;
		};

		struct PositionSnapshot
		{
			PositionSnapshot();
			uint32 tick_;
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
		};

		struct Inputinator
		{
			Inputinator();
			void add_snapshot(InputSnapshot snapshot);
			Vector2 correct_predicted_position(uint32 tick, Time tickrate, Vector2 server_position, float speed);
			Vector2 get_position_from_tick(uint32 tick) const;
			void modify_position(uint32 tick, const Vector2 position);
			InputSnapshot get_snapshot(uint32 index);
		private:
			static constexpr int buffer_size_ = 60;
			InputSnapshot buffer_[buffer_size_];
		};

		struct Message
		{
			uint16 seq_;
			uint32 id_;
			bool received_;
		};

		struct ReliableMessageQueue
		{
			ReliableMessageQueue();
			void add_message(Message& msg);
			Message get_message(uint32 tick);
			void mark_received(uint32 id);
			static constexpr int buffer_size_ = 100;
			Message buffer_[buffer_size_];
			uint32 index_;
		};

	} // !gameplay
} // !charlie

#endif // !CHARLIE_GAMEPLAY_HPP_INCLUDED
