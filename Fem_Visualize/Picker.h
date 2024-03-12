#pragma once

#include <QObject>
#include "PointPickerCellInteractor.h"

class Picker : public QObject, public PointPickerCellInteractor
{
	Q_OBJECT
public:
	Picker(QObject* parent);

	virtual void OnLeftButtonDown() override
	{
		PointPickerCellInteractor::OnLeftButtonDown();
		emit SendCellNum(cellNum);
	}

	~Picker();
signals:
	void SendCellNum(int cellNum);
};
