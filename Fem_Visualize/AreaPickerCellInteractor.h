#pragma once
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
#include <vtkRenderWindow.h>
#include <vtkCellData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkNamedColors.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkLineSource.h>
#include <vtkProperty.h>
#include <vtkUnsignedCharArray.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkSphereSource.h>
#include <vtkAutoInit.h>
#include <vtkProperty.h>

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

namespace {
	// Define interaction style
	class AreaPickerCellInteractor : public vtkInteractorStyleRubberBandPick
	{
	public:
		static AreaPickerCellInteractor* New();
		vtkTypeMacro(AreaPickerCellInteractor, vtkInteractorStyleRubberBandPick);

		AreaPickerCellInteractor()
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

			vtkNew<vtkExtractPolyDataGeometry> extractGeometry;
			extractGeometry->SetImplicitFunction(frustum);
			extractGeometry->SetInputData(this->Points);
			extractGeometry->Update();

			vtkPolyData* selected = extractGeometry->GetOutput();

			this->SelectedMapper->SetInputData(extractGeometry->GetOutput());
			this->SelectedMapper->ScalarVisibilityOff();


			vtkNew<vtkActor> actor;

			vtkIdTypeArray* ids = dynamic_cast<vtkIdTypeArray*>(
				selected->GetCellData()->GetArray("OriginalIds"));
			for (vtkIdType i = 0; i < ids->GetNumberOfTuples(); i++)
			{
				std::cout << "Id " << i << " : " << ids->GetValue(i) << std::endl;
			}

			this->SelectedActor->GetProperty()->SetColor(
				colors->GetColor3d("Red").GetData());
			//this->SelectedActor->GetProperty()->SetPointSize(5);
			this->SelectedActor->GetProperty()->SetLineWidth(3);
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

	vtkStandardNewMacro(AreaPickerCellInteractor);
} // namespace