#pragma once

// Singleton class for the systems to inherit from
template<typename T>
class System {

public:
	static constexpr inline T& Instance() {
		static T m_instance;
		return m_instance;
	};

};