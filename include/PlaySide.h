#ifndef PLAYSIDE_H
#define PLAYSIDE_H

#define playsidemask 0b10000000

enum PlaySide {
    BLACK = 0,
    WHITE = 0b10000000,
    NONE = 0b11111111 };

#endif // !PLAYSIDE_H
