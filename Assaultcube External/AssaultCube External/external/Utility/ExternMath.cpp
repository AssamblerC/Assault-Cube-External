#include "ExternMath.h"

std::wstring Mathematics::generateRandomWideString() const {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> lengthDist(8, 14);
	std::uniform_int_distribution<> charDist(0, 51);

	int length = lengthDist(gen);
	std::wstring randomWideString;
	randomWideString.reserve(length);

	for (int i = 0; i < length; ++i) {
		char c = charDist(gen);
		if (c < 26) {
			c += 'A';
		}
		else {
			c = (c - 26) + 'a';
		}
		randomWideString += wchar_t(c);
	}

	return randomWideString;
}

std::vector<float> Mathematics::rotateVector(std::vector<float>& vector, float angle) const {

    float cosA = std::cos(angle);
    float sinA = std::sin(angle);
    float rotatedX = cosA * vector[0] + sinA * vector[1];
    float rotatedY = - sinA * vector[0] + cosA * vector[1];
    rotatedY = -rotatedY; // Inverting the Y component if necessary
    return { rotatedX, rotatedY };
}

std::vector<float> Mathematics::GetNextP(float xvector, float yvector, float angle, float length) const {

	float x2;
	float y2;
	
	x2 = xvector + length * cos(angle);
	y2 = yvector + length * sin(angle);

	return {x2, y2};
}

float Mathematics::normalizeAngle(float angle) const {
    while (angle >= 360.0f) {
        angle -= 360.0f;
    }
    while (angle < 0.0f) {
        angle += 360.0f;
    }
    return angle;
}

bool Mathematics::WorldToScreen(std::vector<float> pos, vec3d_f& screen, float matrix[16], float windowWidth, float windowHeight) const
{
	vec4d_f clipCoords; 

	clipCoords.x = pos[0] * matrix[0] + pos[1] * matrix[4] + pos[2] * matrix[8] + matrix[12];
	clipCoords.y = pos[0] * matrix[1] + pos[1] * matrix[5] + pos[2] * matrix[9] + matrix[13];
	clipCoords.z = pos[0] * matrix[2] + pos[1] * matrix[6] + pos[2] * matrix[10] + matrix[14];
	clipCoords.w = pos[0] * matrix[3] + pos[1] * matrix[7] + pos[2] * matrix[11] + matrix[15];

	if (clipCoords.w < 0.1f) //Check if in Screen
	{
		return false;
	}

	vec4d_f NDC;

	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	//Mathematics::screen.x = (NDC.x + 1.0f) * windowWidth / 2.0f;
	//Mathematics::screen.y = (1.0f - NDC.y) * windowHeight / 2.0f;
	screen.x = (NDC.x + 1.0f) * windowWidth / 2.0f;
	screen.y = (1.0f - NDC.y) * windowHeight / 2.0f;

	return true;
}

float Mathematics::GetDistance3D(std::vector<float> m_pos, std::vector<float> en_pos) const 
{
	return (float)(sqrt(((en_pos[0] - m_pos[0]) * (en_pos[0] - m_pos[0])) + ((en_pos[1] - m_pos[1]) * (en_pos[1] - m_pos[1])) + ((en_pos[2] - m_pos[2]) * (en_pos[2] - m_pos[2]))));
}

float Mathematics::GetDistance2D(std::vector<float>  m_pos, std::vector<float>  en_pos) const
{
	return (float)(sqrt(((en_pos[0] - m_pos[0]) * (en_pos[0] - m_pos[0])) + ((en_pos[1] - m_pos[1]) * (en_pos[1] - m_pos[1]))));
}

float Mathematics::GetDistance2D(const vec3d_f& m_pos, const vec3d_f& en_pos) const
{
    float dx = m_pos.x - en_pos.x;
    float dy = m_pos.y - en_pos.y;
    return std::sqrt(dx * dx + dy * dy);
}

vec2d_f Mathematics::CalculateAngles(std::vector<float>  m_pos, std::vector<float>  en_pos) const
{

	vec2d_f values = { 0,0};

	float aTriangle = en_pos[0] - m_pos[0];
	float bTriangle = en_pos[1] - m_pos[1];
	float yTriangle = en_pos[2] - m_pos[2];

	float triangleHyp = GetDistance2D(m_pos, en_pos);

	float yaw = -(float)(atan2(aTriangle, bTriangle) * (HALF_CIRCLE / MATH_PI) + HALF_CIRCLE);
	float pitch = (float)((atan2(yTriangle, triangleHyp)) * (HALF_CIRCLE / MATH_PI));

	values.x = yaw;
	values.y = pitch;
	return values;
}