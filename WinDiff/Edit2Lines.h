#pragma once

#include "WinDiffDoc.h"

class CEdit2Lines : public CEdit {
private:
  const StrDiff *m_diff;
public:
    CEdit2Lines();
    void substituteControl(CWnd *parent, int id, const StrDiff &diff);
    void paint(CDC &dc);

public:
    virtual ~CEdit2Lines();

protected:
    afx_msg void OnPaint();

    DECLARE_MESSAGE_MAP()
};

