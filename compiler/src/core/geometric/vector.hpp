#pragma once

struct Vec3 {

    float x;
    float y;
    float z;

    Vec3 operator+(const Vec3& other) const {

        return {
            x + other.x,
            y + other.y,
            z + other.z
        };

    }

};
