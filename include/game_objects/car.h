#ifndef GAME_OBJECTS_CAR_H
#define GAME_OBJECTS_CAR_H

#include <string>

class Car {
private:
    static int next_id;
    int id_;

    enum class Sprite { Straight = 0, Left = 1, Right = 2 };
    Sprite sprite_;

    // Physics state
    double turning_radius;   // meters (0 => straight)
    double speed;            // meters per second
    double acceleration;     // meters per second squared
    double heading;          // radians, 0 == +x axis

    // Identity / rendering
    std::string name_;
    double pos_x; // meters
    double pos_y; // meters

public:
    explicit Car(std::string car_name, double x = 0.0, double y = 0.0) noexcept;

    // Disable copying to preserve unique id semantics
    Car(const Car&) = delete;
    Car& operator=(const Car&) = delete;

    int id() const noexcept;
    const std::string& name() const noexcept;

    // Accessors
    void setSpeed(double new_speed) noexcept;
    double getSpeed() const noexcept;

    void setAcceleration(double new_acceleration) noexcept;
    double getAcceleration() const noexcept;

    void setTurningRadius(double radius) noexcept;
    double getTurningRadius() const noexcept;

    double getHeading() const noexcept;
    void setHeading(double h) noexcept;

    double getX() const noexcept;
    double getY() const noexcept;

    // Update position over `time_interval` seconds using a simple bicycle/circular arc model.
    void updatePosition(double time_interval) noexcept;
};

#endif // GAME_OBJECTS_CAR_H
