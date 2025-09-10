#pragma once
#include <cmath>
#include <vector>
#include <iostream>
#include <random>
#include <string>

typedef struct
{
	float x, y;
} vec2d_f;

typedef struct
{
	float x, y, z;
} vec3d_f;

typedef struct
{
	float x, y, z, w;
} vec4d_f;

class Mathematics {

	private:
		const double MATH_PI = 3.14159265359f;
		const int HALF_CIRCLE = 180;

	public:

		std::wstring generateRandomWideString() const;
		std::vector<float> GetNextP(float xvector, float yvector, float angle, float length) const;
		std::vector<float> rotateVector(std::vector<float>& vector, float angle) const;
		float normalizeAngle(float angle) const;

        bool WorldToScreen(std::vector<float> pos, vec3d_f& screen, float matrix[16], float windowWidth, float windowHeight) const;

        float GetDistance3D(std::vector<float>  m_pos, std::vector<float>  en_pos) const;
        float GetDistance2D(std::vector<float>  m_pos, std::vector<float>  en_pos) const;
		float GetDistance2D(const vec3d_f& m_pos, const vec3d_f& en_pos) const;

        vec2d_f CalculateAngles(std::vector<float>  m_pos, std::vector<float>  en_pos) const;
};
