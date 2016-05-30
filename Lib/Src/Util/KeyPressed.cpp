#include "pch.h"

bool keyPressed(int vk) {
  return (GetAsyncKeyState(vk) & 0x8000) ? true : false;
}

bool shiftKeyPressed() {
  return keyPressed(VK_SHIFT);
}

bool ctrlKeyPressed() {
  return keyPressed(VK_CONTROL);
}

