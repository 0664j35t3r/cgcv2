#ifndef INPUT_H
#define INPUT_H

enum class Key
{
  UNKNOWN,
  A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 
  ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, 
  SPACE, BACKSPACE,
  LEFT, RIGHT, UP, DOWN
};

enum class KeyAction
{
  PRESS, RELEASE, REPEAT
};

enum class MouseButton
{
  LEFT, MIDDLE, RIGHT, UNKNOWN
};

enum class MouseButtonAction
{
  PRESS, RELEASE
};

#endif
