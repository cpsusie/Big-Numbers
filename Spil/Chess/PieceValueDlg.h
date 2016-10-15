#pragma once

#include "PlotWinsPValuesThread.h"

class CPieceValueDlg : public CDialog {
private:
  void setInterval(PieceType type);
  PValuePlotParameters m_result;
public:
	CPieceValueDlg(CWnd* pParent = NULL);
    const PValuePlotParameters &getParameters() const;

	enum { IDD = IDD_PIECEVALUE_DIALOG };
	UINT	m_step;
	UINT	m_from;
	UINT	m_to;
	int		m_pieceType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

    afx_msg void OnRadioKing();
    afx_msg void OnRadioQueen();
    afx_msg void OnRadioRook();
    afx_msg void OnRadioBishop();
    afx_msg void OnRadioKnight();
    afx_msg void OnRadioPawn();
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
};

