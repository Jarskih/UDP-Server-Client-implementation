#pragma once

namespace charlie
{
	template <typename T>
	class Singleton
	{
	private:
		static T* m_instance;
		Singleton() {};
	public:
		static void Set(T* p_instance) { m_instance = p_instance; };
		static T* Get() { return m_instance; };
	};

	template <typename T>
	T* Singleton<T>::m_instance = nullptr;
}