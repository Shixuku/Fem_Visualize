#pragma once

#include <QWidget>
#include "ui_DisplacementWindow.h"

class DisplacementWindow : public QWidget
{
	Q_OBJECT

public:
	DisplacementWindow(QWidget *parent = nullptr);
	~DisplacementWindow();

	enum DispType {
		DX,
		DY,
		DZ,
		DXY,
		DYZ,
		DXZ,
		DXYZ,
	};

	int dispType;

signals:
	void signalSendForceType();

private:
	Ui::DisplacementWindowClass ui;
private slots:
	void onCloseWindow();
	void onSendTypeSignal();
};
