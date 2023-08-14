#pragma once

#include <QDialog>
#include "ui_ReactionForceWindow.h"

class ReactionForceWindow : public QDialog
{
	Q_OBJECT

public:
	ReactionForceWindow(QWidget *parent = nullptr);
	~ReactionForceWindow();

	int forceType;

	enum ForceType {
		FX,
		FY,
		FZ,
		FXYZ,
		MX,
		MY,
		MZ,
		MXYZ,
	};

signals:
	void signalSendForceType();

private slots:
	void onCloseWindow();
	void onSendTypeSignal();
private:
	Ui::ReactionForceWindowClass ui;
};
