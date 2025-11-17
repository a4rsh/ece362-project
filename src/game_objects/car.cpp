#include "game_objects/car.h"
#include <cmath>

// Define static id counter
int Car::next_id = 0;

Car::Car(std::string car_name, double x, double y) noexcept
    : id_(next_id++), sprite_(Sprite::Straight), turning_radius(0.0), speed(0.0), acceleration(0.0), heading(0.0), name_(std::move(car_name)), pos_x(x), pos_y(y)
{
}

int Car::id() const noexcept { return id_; }
const std::string& Car::name() const noexcept { return name_; }

void Car::setSpeed(double new_speed) noexcept { speed = new_speed; }
double Car::getSpeed() const noexcept { return speed; }

void Car::setAcceleration(double new_acceleration) noexcept { acceleration = new_acceleration; }
double Car::getAcceleration() const noexcept { return acceleration; }

void Car::setTurningRadius(double radius) noexcept { turning_radius = radius; }
double Car::getTurningRadius() const noexcept { return turning_radius; }

double Car::getHeading() const noexcept { return heading; }
void Car::setHeading(double h) noexcept { heading = h; }

double Car::getX() const noexcept { return pos_x; }
double Car::getY() const noexcept { return pos_y; }

void Car::updatePosition(double time_interval) noexcept
{
    // TODO
}