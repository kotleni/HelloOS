#include <math.h>

float sin(float angle) {
    float angle3 = angle * angle * angle;
    return angle - (angle3 / 6.0f);  // Approximation of sine (angle - angle^3 / 3!)
}

float cos(float angle) {
    float angle2 = angle * angle;
    return 1.0f - (angle2 / 2.0f);  // Approximation of cosine (1 - angle^2 / 2!)
}