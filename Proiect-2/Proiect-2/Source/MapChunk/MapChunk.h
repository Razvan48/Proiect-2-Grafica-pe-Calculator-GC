#pragma once

class MapChunk
{
private:
	int x;
	int y;

public:
	MapChunk(int x, int y);
	~MapChunk();

	inline int getX() const { return x; }
	inline int getY() const { return y; }

	void draw();
	void update();

	static int calculateChunkX(float x);
	static int calculateChunkY(float y);
};
