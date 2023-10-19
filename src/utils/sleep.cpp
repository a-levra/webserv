#include <ctime>

void ftSleep(unsigned int milliseconds) {
	time_t start_time = std::time(NULL);
	while (static_cast<unsigned long long>((time(NULL) - start_time) * 1000)
		< milliseconds) {
		;
	}
}
