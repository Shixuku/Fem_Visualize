#include <vtkActor.h>
#include <vtkCellPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractSelection.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>
#include <vtkUnstructuredGrid.h>

namespace {

	// Catch mouse events.
	class PointPickerCellInteractor1 : public vtkInteractorStyleTrackballCamera
	{
	public:
		static PointPickerCellInteractor1* New();

		PointPickerCellInteractor1()
		{
			selectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
			selectedActor = vtkSmartPointer<vtkActor>::New();
		}

		virtual void OnLeftButtonDown() override
		{
			vtkNew<vtkNamedColors> colors;

			// Get the location of the click (in window coordinates).
			int* pos = this->GetInteractor()->GetEventPosition();

			vtkCellPicker* picker = static_cast<vtkCellPicker*>(this->GetInteractor()->GetPicker());

			// Pick from this location.
			picker->Pick(pos[0], pos[1], pos[2], this->GetDefaultRenderer());

			double* worldPosition = picker->GetPickPosition();
			std::cout << "Cell id is: " << picker->GetCellId() << std::endl;

			if (picker->GetCellId() != -1)
			{

				std::cout << "Pick position is: (" << worldPosition[0] << ", "
					<< worldPosition[1] << ", " << worldPosition[2] << ")" << std::endl;

				vtkNew<vtkIdTypeArray> ids;
				ids->SetNumberOfComponents(1);
				ids->InsertNextValue(picker->GetCellId());

				vtkNew<vtkSelectionNode> selectionNode;
				selectionNode->SetFieldType(vtkSelectionNode::CELL);
				selectionNode->SetContentType(vtkSelectionNode::INDICES);
				selectionNode->SetSelectionList(ids);

				vtkNew<vtkSelection> selection;
				selection->AddNode(selectionNode);

				vtkNew<vtkExtractSelection> extractSelection;
				extractSelection->SetInputData(0, this->Data);
				extractSelection->SetInputData(1, selection);
				extractSelection->Update();

				// In selection
				vtkNew<vtkUnstructuredGrid> selected;
				selected->ShallowCopy(extractSelection->GetOutput());

				std::cout << "Number of points in the selection: "
					<< selected->GetNumberOfPoints() << std::endl;
				std::cout << "Number of cells in the selection : "
					<< selected->GetNumberOfCells() << std::endl;
				cellNum = selected->GetNumberOfCells();
				selectedMapper->SetInputData(selected);
				selectedMapper->ScalarVisibilityOff();
				selectedActor->SetMapper(selectedMapper);
				selectedActor->GetProperty()->SetColor(1, 0, 0);

				selectedActor->GetProperty()->SetLineWidth(3);

				this->Interactor->GetRenderWindow()
					->GetRenderers()
					->GetFirstRenderer()
					->AddActor(selectedActor);
			}
			else
			{
				this->Interactor->GetRenderWindow()
					->GetRenderers()
					->GetFirstRenderer()
					->RemoveActor(selectedActor);
			}
			// Forward events.
			vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
		}

		int cellNum;
		vtkSmartPointer<vtkPolyData> Data;
		vtkSmartPointer<vtkDataSetMapper> selectedMapper;
		vtkSmartPointer<vtkActor> selectedActor;
	};

	vtkStandardNewMacro(PointPickerCellInteractor1);

} // namespace
