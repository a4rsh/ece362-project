#include "game_objects/dot.h"

GameDot::GameDot(float x, float y, float radius, float speed, uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept
        : x_(x), y_(y), speed_(speed), radius_(radius), vx_(0.f), vy_(0.f),
            r_(r), g_(g), b_(b), a_(a), bounds_w_(0), bounds_h_(0), bounce_(true) {}

void GameDot::update(float dt, float angle_deg) {
    // convert degrees to radians
    const float rad = angle_deg * static_cast<float>(M_PI) / 180.0f;
    vx_ = speed_ * std::cos(rad);
    vy_ = speed_ * std::sin(rad);

    x_ += vx_ * dt;
    y_ += vy_ * dt;

    if (bounce_) handleBounds();
}

void GameDot::handleBounds() noexcept {
    if (bounds_w_ <= 0 || bounds_h_ <= 0) return;

    if (x_ - radius_ < 0.f) {
        x_ = radius_;
        vx_ = -vx_;
        speed_ = std::hypot(vx_, vy_);
    } else if (x_ + radius_ > bounds_w_) {
        x_ = bounds_w_ - radius_;
        vx_ = -vx_;
        speed_ = std::hypot(vx_, vy_);
    }

    if (y_ - radius_ < 0.f) {
        y_ = radius_;
        vy_ = -vy_;
        speed_ = std::hypot(vx_, vy_);
    } else if (y_ + radius_ > bounds_h_) {
        y_ = bounds_h_ - radius_;
        vy_ = -vy_;
        speed_ = std::hypot(vx_, vy_);
    }
}

