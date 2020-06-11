#include "pch.h"

bool keyPressed(int vk) {
  return (GetAsyncKeyState(vk) & 0x8000) ? true : false;
}
