#include "ClientList.h"

namespace charlie
{
	ClientList::ClientList()
		: next_(0)
	{
	}

	int32 ClientList::add_client(const uint64 connection)
	{
		const int32 id = next_++;
		clients_.push_back({ id, connection });
		return id;
	}

	int32 ClientList::find_client(const uint64 connection)
	{
		for (auto& client : clients_) {
			if (client.connection_ == connection) {
				return client.id_;
			}
		}
		return -1;
	}

	void ClientList::remove_client(const uint64 connection)
	{
		auto it = clients_.begin();
		while (it != clients_.end()) {
			if ((*it).connection_ == connection) {
				clients_.erase(it);
				break;
			}
			++it;
		}
	}
}
