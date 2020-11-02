#pragma once
#include "sdl_window.hpp"

namespace charlie
{
	struct ClientList {
		ClientList();

		int32 add_client(const uint64 connection);
		int32 find_client(const uint64 connection);
		void remove_client(const uint64 connection);

		struct Client {
			int32  id_{ -1 };
			uint64 connection_{};
		};

		int32 next_;
		DynamicArray<Client> clients_;
	};
}
