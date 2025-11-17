#ifndef GAME_OBJECTS_DOT_H
#define GAME_OBJECTS_DOT_H

#include <cstdint>
#include <cmath>

// GameDot: moves in response to an angle (degrees) typically provided by gyro yaw.
// - Angle is in degrees (0 = +X axis, increases counter-clockwise).
// - update(dt, angle_deg) moves the dot at its current `speed` along that heading.
class GameDot {
public:
    GameDot(float x = 0.f, float y = 0.f, float radius = 4.f, float speed = 0.f,
        uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255) noexcept;

    // Update for dt seconds; `angle_deg` is heading in degrees (from gyro yaw).
    void update(float dt, float angle_deg);

    void setSpeed(float s) noexcept { speed_ = s; }
    float getSpeed() const noexcept { return speed_; }

    void setPosition(float x, float y) noexcept { x_ = x; y_ = y; }
    float x() const noexcept { return x_; }
    float y() const noexcept { return y_; }

    void setRadius(float r) noexcept { radius_ = r; }
    float radius() const noexcept { return radius_; }

    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept {
        r_ = r; g_ = g; b_ = b; a_ = a;
    }

    void setBounds(int w, int h) noexcept { bounds_w_ = w; bounds_h_ = h; }
    void enableBounce(bool b) noexcept { bounce_ = b; }

private:
    void handleBounds() noexcept;

    float x_, y_;
    float speed_;     // scalar speed (units consistent with rest of your simulation)
    float radius_;
    float vx_, vy_;   // last computed velocity components

    uint8_t r_, g_, b_, a_;
    int bounds_w_, bounds_h_;
    bool bounce_;
};

#endif // GAME_OBJECTS_DOT_H
