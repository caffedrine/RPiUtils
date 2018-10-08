#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>


/* Get time stamp in milliseconds */
static inline uint64_t millis()
{
	return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

/* Get time stamp in microseconds */
static inline uint64_t micros()
{
	return (uint64_t)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

/* Get time stamp in nanoseconds */
static inline uint64_t nanos()
{
	return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

/* Current timestamp for logging */
static std::string formatted_time_now()
{
	using namespace std::chrono;
	
	// get current time
	auto now = system_clock::now();
	
	// get number of milliseconds for the current second
	// (remainder after division into seconds)
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
	
	// convert to std::time_t in order to convert to std::tm (broken time)
	auto timer = system_clock::to_time_t(now);
	
	// convert to broken time
	std::tm bt = *std::localtime(&timer);
	
	std::ostringstream oss;
	
	oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
	oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
	
	return oss.str();
}