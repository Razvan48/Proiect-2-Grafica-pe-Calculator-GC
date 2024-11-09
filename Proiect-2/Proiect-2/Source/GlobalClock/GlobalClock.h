#pragma once

class GlobalClock
{
private:
	GlobalClock(float currentTime, float lastTime, float deltaTime);
	~GlobalClock();
	GlobalClock(const GlobalClock& other) = delete;
	GlobalClock& operator= (const GlobalClock& other) = delete;
	GlobalClock(const GlobalClock&& other) = delete;
	GlobalClock& operator= (const GlobalClock&& other) = delete;

	float currenTime;
	float lastTime;
	float deltaTime;

public:
	static GlobalClock& get();
	void update();

	inline float getCurrentTime() const { return this->currenTime; }
	inline float getLastTime() const { return this->lastTime; }
	inline float getDeltaTime() const { return this->deltaTime; }
};

