#pragma once

#include <GL/glew.h> 

class DepthMap {
private:
	GLuint depthMap;
	GLuint depthMapFBO;
	float shadowWidth, shadowHeight;
	static GLuint ProgramID;
public: 
	DepthMap() = default;
	~DepthMap() = default;
	DepthMap(const DepthMap& other) = delete;
	DepthMap& operator= (const DepthMap& other) = delete;
	DepthMap(const DepthMap&& other) = delete;
	DepthMap& operator= (const DepthMap&& other) = delete;
	void CreateFBO();
	void bindFBO();
	void unbindFBO(float oldWidth, float oldHeight);
	inline GLuint getDepthMap() const { return depthMap; }
	inline GLuint getDepthMapFBO() const { return depthMapFBO; }
	inline void setShadowWidth(float width) { shadowWidth = width; }
	inline void setShadowHeight(float height) { shadowWidth = height; };
	inline static void setShader(GLuint prog) { ProgramID = prog; }
	inline static GLuint getShader() { return ProgramID; }

};