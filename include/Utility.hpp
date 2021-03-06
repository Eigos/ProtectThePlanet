#pragma once
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>

#include "raylib.hpp"

class ResourceManager;
struct MovementInfo;
struct SizeInfo;
struct Object;
struct Drawable;
struct Point;
struct AABB;
struct Circle;
struct Physics;
struct CollisionMask;

constexpr double DegreeToRadian(const double& degree);
constexpr float DegreeToRadianF(const float& degree);

constexpr double RadianToDegree(const double& radian);
constexpr float RadianToDegreeF(const float& radian);

template<typename T>
static float Clip(const T& value, const T& min, const T& max);


static float getDistance(const float& fromX, const float& fromY, const float& toX, const float& toY);
static float getDistance(const Point& from, const Point& to);


static float getAngle(const float& fromX, const float& fromY, const float& toX, const float& toY);
static float getAngle(const Point& from, const Point& to);

static void DDrawTexturePro(Drawable* drawable, Point pos, float rotaion, Color color);
static void DrawTextureProCustomSize(Drawable* drawable, Point pos, SizeInfo customSize, float rotaion, Color color);

static bool CollisionAABB(const AABB& a, const AABB& b);
static bool CollisionCircle(const Circle& a, const Circle& b);

static bool CollisionCircleRect(const Circle& circle, const Rectangle& rectangle, const float& rectangleRotation);
static bool CollisionCircleRect(const Circle& circle, const AABB& aabb);

struct Point {
	float x = 0.0f;
	float y = 0.0f;

	const Vector2& toVec2() {
		Vector2 newVec{ x,y };

		return newVec;
	}


	Point operator-(Point& point) {
		Point p;

		p.x = x - point.x;
		p.y = y - point.y;

		return p;
	}

	Point operator+(const Point& point) {
		Point p;

		p.x = x + point.x;
		p.y = y + point.y;

		return p;
	}

	Point operator/(const Point& point) {
		Point p;

		p.x = x / point.x;
		p.y = y / point.y;

		return p;
	}

	Point operator*(const Point& point) {
		Point p;

		p.x = x * point.x;
		p.y = y * point.y;

		return p;
	}

	Point& operator-=(const Point& point) {
		x = x - point.x;
		y = y - point.y;

		return (*this);
	}

	Point& operator+=(const Point& point) {
		x = x + point.x;
		y = y + point.y;

		return (*this);
	}

	Point& operator/=(const Point& point) {
		x = x / point.x;
		y = y / point.y;

		return (*this);
	}

	Point& operator*=(const Point& point) {
		x = x * point.x;
		y = y * point.y;

		return (*this);
	}

	Point& operator=(const Point& point) {

		x = point.x;
		y = point.y;

		return (*this);
	}

};

struct MovementInfo {

	float velocity = 0.0f;

	float velocityX = 0.0f;

	float velocityY = 0.0f;

	//Radian
	float direction = 0.0f;

};

struct SizeInfo {
	float widht = 0.0f;
	float height = 0.0f;
	float radius = 0.0f;

	SizeInfo operator*(const int& mult) {
		SizeInfo newSize;
		newSize.widht = mult * widht;
		newSize.height = mult * height;
		newSize.radius = mult * radius;
		return newSize;
	}
};

struct Physics {

	float Mass = 1.0f;

	static float GravitationalPull(const Point& pos1, const float& mass1, const Point& pos2, const float& mass2) {
		return (mass1 * mass2) / powf(getDistance(pos1, pos2), 2);
	}

};

// Axis - Aligned Bounding Box
struct AABB {

	Point point = { 0,0 };
	float width = 0.0f;
	float height = 0.0f;
	float rotation = 0.0f;
	AABB() {};
	AABB(SizeInfo size) : width(size.widht), height(size.height) {}
	AABB(Point point) : point(point) {}
	AABB(Point point, SizeInfo size) : width(size.widht), height(size.height), point(point) {}
	AABB(Point point, SizeInfo size, float rotation) :
		width(size.widht), height(size.height), point(point), rotation(rotation) {}

};

struct Circle {

	Point point = { 0.0f, 0.0f };
	float radius = 0.0f;
	Circle() {};
	Circle(Point point) : point(point) {};
	Circle(float radius) : radius(radius) {};
	Circle(Point point, float radius) : point(point), radius(radius) {};

};

struct CollisionMask {

	std::vector<Circle> circleMasks;
	std::vector<AABB> rectMasks;

	Point origin;

	void AddNewMask(Circle mask) {
		circleMasks.push_back(mask);
	}

	void AddNewMask(AABB mask) {
		float angle = getAngle(mask.point, origin);
		if (angle == 0 && mask.rotation == 0) {
			if (mask.point.x < origin.x) {
				mask.rotation = PI;
			}
		}

		if (angle == PI) {
			if (mask.point.x > origin.x) {
				mask.rotation = 0;
			}
		}

		rectMasks.push_back(mask);
	}

	//1st param - 2nd param
	//circle-AABB and circle-circle
	bool CheckCollisionCircle(const Circle& circleCol) {
		for (uint32_t i = 0; i < circleMasks.size(); i++) {
			if (CollisionCircle(circleMasks[i], circleCol)) {
				return true;
			}
		}
		for (uint32_t i = 0; i < rectMasks.size(); i++) {
			if (CollisionCircleRect(circleCol, rectMasks[i])) {
				return true;
			}
		}

		return false;
	}

	//1st param - 2nd param
	//AABB-AABB and AABB-circle
	bool CheckCollisionAABB(const AABB& aabbCol) {
		for (uint32_t i = 0; i < rectMasks.size(); i++) {
			if (CollisionAABB(rectMasks[i], aabbCol)) {
				return true;
			}
		}

		for (uint32_t i = 0; i < circleMasks.size(); i++) {
			if (CollisionCircleRect(circleMasks[i], aabbCol)) {
				return true;
			}
		}
		return false;
	}

	bool CheckCollisionMask(const CollisionMask& mask) {
		for (uint32_t i = 0; i < mask.circleMasks.size(); i++) {
			if (CheckCollisionCircle(mask.circleMasks[i])) {
				return true;
			}
		}
		for (uint32_t i = 0; i < mask.rectMasks.size(); i++) {
			if (CheckCollisionAABB(mask.rectMasks[i])) {
				return true;
			}
		}

		return false;
	}

	void operator+(Point& newPoint) {
		Point dPoint = newPoint - origin;
		origin += dPoint;
		for (uint32_t i = 0; i < circleMasks.size(); i++) {
			circleMasks[i].point += dPoint;
		}

		for (uint32_t i = 0; i < rectMasks.size(); i++) {
			rectMasks[i].point += dPoint;
		}
	}

	void operator-(Point& newPoint) {
		Point dPoint = newPoint - origin;
		for (uint32_t i = 0; i < circleMasks.size(); i++) {
			circleMasks[i].point -= dPoint;
		}

		for (uint32_t i = 0; i < rectMasks.size(); i++) {
			rectMasks[i].point -= dPoint;
		}
	}

	void MoveMask(Point& newPoint) {
		(*this) + newPoint;
	}

	void RotateMask(const float& rad) {

		for (uint32_t i = 0; i < circleMasks.size(); i++) {
			float distance = getDistance(circleMasks[i].point, origin);

			circleMasks[i].point.x += cos(rad) * distance;
			circleMasks[i].point.y += sin(rad) * distance;
		}

		for (uint32_t i = 0; i < rectMasks.size(); i++) {
			float distance = getDistance(rectMasks[i].point, origin);

			rectMasks[i].rotation += rad;

			rectMasks[i].point.x = origin.x + distance * cos(rectMasks[i].rotation);
			rectMasks[i].point.y = origin.y + distance * sin(rectMasks[i].rotation);
		}
	}

};

struct Object {
	Point Position;

	MovementInfo Movement;

	SizeInfo Size;

	Physics Physic;

	CollisionMask CollMask;

	//update players position to current velocity
	void UpdatePosition(float dt = 1.0f) {
		//Update Velocity
		Movement.velocityX = (cosf(Movement.direction) * Movement.velocity) + Movement.velocityX;

		//Update Velocity
		Movement.velocityY = (sinf(Movement.direction) * Movement.velocity) + Movement.velocityY;

		//Update Position
		Position.x += Movement.velocityX * dt;
		Position.y += Movement.velocityY * dt;

		Movement.velocityX = 0;
		Movement.velocityY = 0;
	}

	void ApplyPhysics(const Object& centerObject) {

		//F / m = a
		float acceleration = Physics::GravitationalPull(Position, Physic.Mass, centerObject.Position,
			centerObject.Physic.Mass) / Physic.Mass;
		acceleration *= 5000 * 120;

		float angle = getAngle(Position, centerObject.Position);

		//Update X
		Movement.velocityX += cosf(angle) * acceleration;

		//Update Y
		Movement.velocityY += sinf(angle) * acceleration;

	}

};

struct Drawable {

private:
	//Increases or decreases the width and height
	float widthSpecifier = 0;
	float heightSpecifier = 0;


public:

	Drawable(const Texture2D& newTexture) {
		texture = newTexture;
		source = Rectangle{ 0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height) };
		dest = Rectangle{ 0, 0, static_cast<float>(texture.width) - widthSpecifier, static_cast<float>(texture.height) - heightSpecifier };
		origin = { (static_cast<float>(texture.width) - widthSpecifier) / 2, (static_cast<float>(texture.height) - heightSpecifier) / 2 };

	}

	Drawable(const Drawable& drawable) : widthSpecifier(drawable.widthSpecifier), heightSpecifier(drawable.heightSpecifier),
		source(drawable.source), dest(drawable.dest), origin(drawable.origin), texture(drawable.texture) {};

	//image data
	Texture2D texture;

	//image original size
	Rectangle source;

	//image size 
	Rectangle dest;

	//image center
	Point origin;

	//moves the image to the desired point
	static Rectangle DestWPosition(const Point& point, const Rectangle& dest) {
		Rectangle rect = dest;
		rect.x = point.x;
		rect.y = point.y;
		return rect;
	}

	static Rectangle DestWPosition(const Point& point, const float& width, const float& height) {
		Rectangle rect;
		rect.x = point.x;
		rect.y = point.y;
		rect.width = width;
		rect.height = height;
		return rect;
	}


	void NewTextureWidth(const float& newWidth) {
		dest.width += widthSpecifier;
		origin.x += widthSpecifier / 2;

		widthSpecifier = dest.width - newWidth;
		origin.x -= widthSpecifier / 2;
		dest.width -= widthSpecifier;
	}

	void NewTextureHeight(const float& newHeight) {
		dest.height += heightSpecifier;
		origin.y += heightSpecifier / 2;

		heightSpecifier = dest.height - newHeight;
		origin.y -= heightSpecifier / 2;
		dest.height -= heightSpecifier;
	}

};

/* TODO
class Animation : public Drawable {

private:
	uint32_t frameCountVertical;
	uint32_t frameCountHorizontal;
	uint32_t currentFrameIndex;

	SizeInfo frameSize;

	uint32_t targetFps;
	uint32_t currentTick;
	



	SizeInfo CalculateFrameSize() {
		SizeInfo size;
		size.widht = texture.width / frameCountHorizontal;
		size.height = texture.height / frameCountVertical;
	};

	uint32_t getFrameCount() {
		return frameCountVertical * frameCountHorizontal;
	}

public:

	Animation(Texture2D texture) :
		Drawable(texture), frameCountVertical(0), frameCountHorizontal(0), currentFrameIndex(0) {};

	Animation(Texture2D texture, uint32_t frameCountVertical, uint32_t frameCountHorizontal) :
		Drawable(texture), frameCountHorizontal(frameCountHorizontal), frameCountVertical(frameCountVertical), currentFrameIndex(0),
		frameSize(CalculateFrameSize()) {};

	Animation(Drawable* drawable, uint32_t frameCountVertical, uint32_t frameCountHorizontal) :
		Drawable(*drawable), frameCountHorizontal(frameCountHorizontal), frameCountVertical(frameCountVertical), currentFrameIndex(0),
		frameSize(CalculateFrameSize()) {};

	void setFrameCountVertical(uint32_t newFrameCount) {
		frameCountVertical = newFrameCount;
		frameSize = CalculateFrameSize();

	}

	void setFrameCountHorizontal(uint32_t newFrameCount) {
		frameCountHorizontal = newFrameCount;
		frameSize = CalculateFrameSize();
	}

	uint32_t getFrameCountVertical() {
		return frameCountVertical;
	}

	uint32_t getFrameCountHorizontal() {
		return frameCountHorizontal;
	}

	uint32_t getFrameCurrentIndex() {
		return currentFrameIndex;
	}

	Rectangle getCurrentFrame() {
		Rectangle newRect;

		uint32_t xOffset = currentFrameIndex % frameCountHorizontal;
		uint32_t yOffset = currentFrameIndex / frameCountVertical;

		newRect.x = xOffset * frameSize.widht;
		newRect.y = yOffset * frameSize.height;

		newRect.width = frameSize.widht;
		newRect.height = frameSize.height;

		return newRect;
	}

	void TickFrame() {
		currentTick++;


	}

	//TO-DO: make it frame independent by passing delta time
	void setTargetFps(uint32_t fps) {

	}

};
*/

class ResourceManager {

public:

	enum class SourceType {
		Unknown,
		Drawable,
		Sound,
		Music,
		Shader
	};

private:


	struct Resource {
		std::string name;
		SourceType type;
		void* data;
	};

	std::vector<Resource> sourceList;

public:

	template<typename Type>
	struct Source
	{
		std::string name;
		Type* data;
	};

	void AddSource(const std::string& name, Drawable* drawable) {
		Resource newSource;

		newSource.name = name;
		newSource.type = SourceType::Drawable;
		newSource.data = drawable;

		sourceList.push_back(newSource);
	};

	void AddSource(const std::string& name, Sound* sound) {
		Resource newSource;

		newSource.name = name;
		newSource.type = SourceType::Sound;
		newSource.data = sound;

		sourceList.push_back(newSource);
	};

	void AddSource(const std::string& name, Music* music) {
		Resource newSource;

		newSource.name = name;
		newSource.type = SourceType::Music;
		newSource.data = music;

		sourceList.push_back(newSource);
	};

	void AddSource(const std::string& name, Shader* shader) {
		Resource newSource;

		newSource.name = name;
		newSource.type = SourceType::Shader;
		newSource.data = shader;


		sourceList.push_back(newSource);
	};

	void ClearSource(const std::string& name) {
		for (size_t i = 0; i < sourceList.size(); i++) {
			if (sourceList[i].name == name) {
				sourceList.erase(sourceList.begin() + i);
				return;
			}
		}

	};

	template<typename Type>
	Type* getSource(const std::string& name) {
		for (size_t i = 0; i < sourceList.size(); i++) {
			if (sourceList[i].name == name) {

				return static_cast<Type*>(sourceList[i].data);
			}
		}
		return nullptr;
	}

	template<typename Type>
	std::vector<Source<Type>> getSourceList(SourceType sourceType) {
		std::vector<Source<Type>> newList;
		for (size_t i = 0; i < sourceList.size(); i++) {
			if (sourceList[i].type == sourceType) {
				Source<Type> newSource;
				newSource.name = sourceList[i].name;
				newSource.data = static_cast<Type*>(sourceList[i].data);

				newList.push_back(newSource);
			}
		}
		return newList;
	}

	size_t CountOfType(SourceType type) {
		size_t count = 0;

		for (size_t i = 0; i < sourceList.size(); i++) {
			if (sourceList[i].type == type) {
				count++;
			}
		}

		return count;
	}

	SourceType getType(const std::string& name) {

		for (size_t i = 0; i < sourceList.size(); i++) {
			if (sourceList[i].name == name) {
				return sourceList[i].type;
			}
		}

		return SourceType::Unknown;
	}

};

constexpr double DegreeToRadian(const double& degree)
{
	return degree * (3.14159265359 / 180);
}

constexpr float DegreeToRadianF(const float& degree)
{
	return degree * (float)(3.14159265359 / 180);
}

constexpr float RadianToDegreeF(const float& radian)
{
	return radian / (static_cast<float>(3.14159265359) / 180.0f);
}

constexpr double RadianToDegree(const double& radian)
{
	return radian / (float)(3.14159265359 / 180);
}

template<typename T>
static float Clip(const T& value, const T& min, const T& max) {
	return std::max(min, std::min(value, max));
}

static float getDistance(const float& fromX, const float& fromY, const float& toX, const float& toY) {
	float dX = abs(fromX - toX);
	float dY = abs(fromY - toY);

	return sqrt((dX * dX) + (dY * dY));
}

static float getDistance(const Point& from, const Point& to) {
	float dX = abs(from.x - to.x);
	float dY = abs(from.y - to.y);

	return sqrt((dX * dX) + (dY * dY));
}

//Formula: double angle = atan2(y2 - y1, x2 - x1) * 180 / PI;
static float getAngle(const float& fromX, const float& fromY, const float& toX, const float& toY) {
	return atan2f(toY - fromY, toX - fromX);
}

static float getAngle(const Point& from, const Point& to) {
	return atan2f(to.y - from.y, to.x - from.x);
}

static void DDrawTexturePro(Drawable* drawable, Point pos, float rotaion, Color color) {
	DrawTexturePro(
		drawable->texture,
		drawable->source,
		drawable->DestWPosition({ pos.x, pos.y }, drawable->dest),
		{ drawable->origin.x, drawable->origin.y },
		rotaion,
		color);
}

static void DrawTextureProCustomSize(Drawable* drawable, Point pos, SizeInfo customSize, float rotaion, Color color) {
	DrawTexturePro(
		drawable->texture,
		drawable->source,
		drawable->DestWPosition({ pos.x, pos.y }, customSize.widht, customSize.height),
		//{ drawable->origin.x, drawable->origin.y },
		{ customSize.widht / 2, customSize.height / 2 },
		rotaion,
		color);
}

static bool CollisionAABB(const AABB& a, const AABB& b) {

	// collision x-axis?
	bool collisionX = a.point.x + a.width >= b.point.x &&
		b.point.x + b.width >= a.point.x;

	// collision y-axis?
	bool collisionY = a.point.y + a.height >= b.point.y &&
		b.point.y + b.height >= a.point.y;

	// collision only if on both axes
	return collisionX && collisionY;
}

static bool CollisionCircle(const Circle& a, const Circle& b) {
	float dist = sqrtf(powf((a.point.x - b.point.x), 2.0f) + powf((a.point.y - b.point.y), 2.0f));

	return dist < (a.radius + b.radius);
}

//https://gist.github.com/snorpey/8134c248296649433de2
static bool CollisionCircleRect(const Circle& circle, const Rectangle& rectangle, const float& rectangleRotation) {

	float rectCenterX = rectangle.x;
	float rectCenterY = rectangle.y;

	float rectX = rectCenterX - rectangle.width / 2;
	float rectY = rectCenterY - rectangle.height / 2;

	float rectReferenceX = rectX;
	float rectReferenceY = rectY;

	// Rotate circle's center point back
	float unrotatedCircleX = cos(rectangleRotation) * (circle.point.x - rectCenterX) -
		sin(rectangleRotation) * (circle.point.y - rectCenterY) + rectCenterX;

	float unrotatedCircleY = sin(rectangleRotation) * (circle.point.x - rectCenterX) +
		cos(rectangleRotation) * (circle.point.y - rectCenterY) + rectCenterY;

	// Closest point in the rectangle to the center of circle rotated backwards(unrotated)
	float closestX, closestY;

	if (unrotatedCircleX < rectReferenceX) {
		closestX = rectReferenceX;
	}
	else if (unrotatedCircleX > rectReferenceX + rectangle.width) {
		closestX = rectReferenceX + rectangle.width;
	}
	else {
		closestX = unrotatedCircleX;
	}


	// Find the unrotated closest y point from center of unrotated circle
	if (unrotatedCircleY < rectReferenceY) {
		closestY = rectReferenceY;
	}
	else if (unrotatedCircleY > rectReferenceY + rectangle.height) {
		closestY = rectReferenceY + rectangle.height;
	}
	else {
		closestY = unrotatedCircleY;
	}


	// Determine collision
	bool collision = false;
	float distance = getDistance(unrotatedCircleX, unrotatedCircleY, closestX, closestY);

	if (distance < circle.radius) {
		collision = true;
	}
	else {
		collision = false;
	}

	return collision;
}

static bool CollisionCircleRect(const Circle& circle, const AABB& aabb) {
	Rectangle newRect;
	newRect.x = aabb.point.x;
	newRect.y = aabb.point.y;
	newRect.width = aabb.width;
	newRect.height = aabb.height;

	return CollisionCircleRect(circle, newRect, aabb.rotation);

}

