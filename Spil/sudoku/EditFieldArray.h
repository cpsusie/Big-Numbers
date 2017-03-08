#pragma once

class MarkableEditField : public CEdit {
private:
  bool m_fixed;
  bool m_marked;
  bool m_wrong;
  void adjustEnable();
public:
  MarkableEditField();
  bool isFixed()  const { return m_fixed;  }
  bool isMarked() const { return m_marked; }
  void setMarked(bool marked);
  void setFixed( bool fixed );
  void setError( bool wrong );
  int  getValue() const;
  void putValue(int v);
  void OnPaint();
};

#define GREEN RGB(0,255,0)
#define RED   RGB(255,0,0)
#define WHITE RGB(255,255,255)

class EditFieldArray {
private:
  CWnd             *m_parent;
  MarkableEditField m_fields[9][9];
  bool              m_fieldsCreated, m_fontIsSet;

public:
  EditFieldArray() {
    m_parent = NULL;
    m_fieldsCreated = m_fontIsSet = false;
  }
  void        putMatrix(const FieldMatrix &m);
  FieldMatrix getMatrix() const;
  MarkableEditField *findField(const CPoint &p);
  MarkableEditField  &getEditField(int r, int c) {
    return m_fields[r][c];
  }
  void Create(CWnd *parent);
  void OnPaint();
};
