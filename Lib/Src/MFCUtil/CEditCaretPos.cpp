#include "pch.h"

void setCaretPos(CEdit *edit, int pos) {
  edit->SetSel(pos, pos);
}

int getCaretPos(CEdit *edit) {
  return edit->CharFromPos(edit->GetCaretPos());
}

void moveCaret(CEdit *edit, int amount) {
  setCaretPos(edit, getCaretPos(edit) + amount);
}
