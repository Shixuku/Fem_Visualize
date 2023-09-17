#pragma once
#include "D:\danpc\��ҵ����\VTK_9.0\VTK\include\vtk-9.0\vtkInteractorStyleRubberBandPick.h"
#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkRenderWindowInteractor.h>

namespace {
	// Define interaction style
	class AreaPickerNodeInteractor : public vtkInteractorStyleRubberBandPick
	{
	public:
		static AreaPickerNodeInteractor* New();
		vtkTypeMacro(AreaPickerNodeInteractor, vtkInteractorStyleRubberBandPick);

		AreaPickerNodeInteractor()
		{
			this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
			this->SelectedActor = vtkSmartPointer<vtkActor>::New();
			this->SelectedActor->SetMapper(SelectedMapper);
		}

		virtual void OnLeftButtonUp() override
		{
			vtkNew<vtkNamedColors> colors;

			// Forward events
			vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

			vtkPlanes* frustum =
				static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())
				->GetFrustum();

			vtkNew<vtkExtractGeometry> extractGeometry;
			extractGeometry->SetImplicitFunction(frustum);
			extractGeometry->SetInputData(this->Points);
			extractGeometry->Update();

			// �������еĵ㼯ת��Ϊ����Ĺ�����������������д�����Ӧ�Ķ��㣨����ɢ�ĵ�����ת��Ϊ���㣩
			vtkNew<vtkVertexGlyphFilter> glyphFilter;
			glyphFilter->SetInputConnection(extractGeometry->GetOutputPort());
			glyphFilter->Update();

			vtkPolyData* selected = glyphFilter->GetOutput();
			std::cout << "Selected " << selected->GetNumberOfPoints() << " points."
				<< std::endl;
			std::cout << "Selected " << selected->GetNumberOfCells() << " cells."
				<< std::endl;
			this->SelectedMapper->SetInputData(selected);
			this->SelectedMapper->ScalarVisibilityOff();

			vtkIdTypeArray* ids = dynamic_cast<vtkIdTypeArray*>(
				selected->GetPointData()->GetArray("OriginalIds"));
			for (vtkIdType i = 0; i < ids->GetNumberOfTuples(); i++)
			{
				std::cout << "Id " << i << " : " << ids->GetValue(i) << std::endl;
			}

			this->SelectedActor->GetProperty()->SetColor(
				colors->GetColor3d("Red").GetData());
			this->SelectedActor->GetProperty()->SetPointSize(7);

			this->CurrentRenderer->AddActor(SelectedActor);
			this->GetInteractor()->GetRenderWindow()->Render();
			this->HighlightProp(NULL);
		}

		void SetPoints(vtkSmartPointer<vtkPolyData> points)
		{
			this->Points = points;
		}

	private:
		vtkSmartPointer<vtkPolyData> Points;
		vtkSmartPointer<vtkActor> SelectedActor;
		vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
	};

	vtkStandardNewMacro(AreaPickerNodeInteractor);
} // namespace
