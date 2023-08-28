#include "BridgeProperty.h"
#include "QDebug"

BridgeProperty::BridgeProperty()
{
}

void BridgeProperty::ConstuctRotationMatrix(double startPoint[3], double endPoint[3], vtkMatrix4x4* transformMatrix)
{
	// 计算连接这两个点的向量
	double normal[3] = { endPoint[0] - startPoint[0], endPoint[1] - startPoint[1], endPoint[2] - startPoint[2] };

	// 定义两个向量
	double sourceVector[3] = { 0.0, 0.0, 1.0 };
	double targetVector[3] = { normal[0], normal[1], normal[2] };

	// 标准化两个向量
	vtkMath::Normalize(sourceVector);
	vtkMath::Normalize(targetVector);

	// 计算旋转轴和旋转角度
	// 两个向量的点积等于一个向量在另一个向量方向上的投影长度与另一个向量本身长度的乘积，反映了一个向量在另一个向量方向上的分量大小。
	// 除以两个向量的长度即可得到两个向量的夹角
	double rotationAxis[3];
	vtkMath::Cross(sourceVector, targetVector, rotationAxis);
	double rotationAngle = acos(vtkMath::Dot(sourceVector, targetVector));

	// 构建旋转矩阵
	vtkSmartPointer<vtkTransform> rotation = vtkSmartPointer<vtkTransform>::New();
	rotation->RotateWXYZ(rotationAngle * vtkMath::DegreesFromRadians(1.0), rotationAxis);

	// 构建平移矩阵
	vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
	// translation->Translate(startPoint);

	// 构建旋转和平移的组合矩阵
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->Concatenate(translation);
	transform->Concatenate(rotation);

	// 获取变换矩阵
	transform->GetMatrix(transformMatrix);

	// 输出旋转矩阵
	// transformMatrix->Print(cout);
}

std::string BridgeProperty::GenerateCoordinateString(const std::string& input)
{
	std::vector<std::string> parts;
	std::stringstream ss(input);
	std::string part;
	while (std::getline(ss, part, ',')) {
		parts.push_back(part);
	}

	std::vector<std::string> result;
	for (const auto& part : parts) {
		if (part.find('@') != std::string::npos) {
			std::stringstream num_ss(part.substr(part.find('@') + 1));
			int num;
			num_ss >> num;
			std::stringstream count_ss(part.substr(0, part.find('@')));
			int count;
			count_ss >> count;
			for (int i = 0; i < count; i++) {
				result.push_back(std::to_string(num));
			}
		}
		else {
			result.push_back(part);
		}
	}

	std::string expanded = "";
	for (int i = 0; i < result.size(); i++) {
		expanded += result[i];
		if (i < result.size() - 1) {
			expanded += ",";
		}
	}
	return expanded;
}

void BridgeProperty::GeneratePoints(const std::string& input, string flag, double startPoint[3], vtkSmartPointer<vtkPoints> points)
{
	std::vector<std::string> pointStrings;

	// 分割每个点的坐标字符串，得到每个坐标值
	std::stringstream pointSs(input);
	std::string coordinateString;
	while (std::getline(pointSs, coordinateString, ','))
	{
		double coordinate = std::stod(coordinateString);
		pointStrings.push_back(coordinateString);
	}

	points->InsertNextPoint(startPoint[0], startPoint[1], startPoint[2]);

	if (flag == "x")
	{
		for (const auto& pointString : pointStrings)
		{
			double coordinate = std::stod(pointString);
			points->InsertNextPoint(startPoint[0] + coordinate, startPoint[1], startPoint[2]);
			startPoint[0] += coordinate;
		}
	}

	if (flag == "y")
	{

		for (const auto& pointString : pointStrings)
		{
			double coordinate = std::stod(pointString);
			points->InsertNextPoint(startPoint[0], coordinate + startPoint[1], startPoint[2]);
			startPoint[1] += coordinate;
		}
	}
}

void BridgeProperty::CreateCrossSectionShapeTube(double radius, double resolution, vtkPolyData* polyData, double startPoint[3], double endPoint[3])
{
	if (!polyData)
	{
		std::cerr << "Failed to create cross-sectional shape!" << std::endl;
		// 添加其他的错误处理代码
	}
	else {
		std::cerr << "Success to create cross-sectional shape!" << std::endl;
	}
	vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	ConstuctRotationMatrix(startPoint, endPoint, transformMatrix);

	vtkSmartPointer<vtkPoints> innerCirclePoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> outerCirclePoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

	// Create points for inner circle
	for (double i = 0; i < 360; i += 10)
	{
		double radians = vtkMath::RadiansFromDegrees(i);
		double x = cos(radians);
		double y = sin(radians);
		innerCirclePoints->InsertNextPoint(x, y, 0.0);
	}

	// Create points for outer circle
	for (double i = 0; i < 360; i += 10)
	{
		double radians = vtkMath::RadiansFromDegrees(i);
		double x = 2.0 * cos(radians);
		double y = 2.0 * sin(radians);
		outerCirclePoints->InsertNextPoint(x, y, 0.0);
	}

	// Create cells
	for (int i = 0; i < 36; i++)
	{
		vtkSmartPointer<vtkTriangle> triangle1 = vtkSmartPointer<vtkTriangle>::New();
		triangle1->GetPointIds()->SetId(0, i);
		triangle1->GetPointIds()->SetId(1, (i + 1) % 36);
		triangle1->GetPointIds()->SetId(2, i + 36);
		cells->InsertNextCell(triangle1);

		vtkSmartPointer<vtkTriangle> triangle2 = vtkSmartPointer<vtkTriangle>::New();
		triangle2->GetPointIds()->SetId(0, (i + 1) % 36);
		triangle2->GetPointIds()->SetId(1, (i + 1) % 36 + 36);
		triangle2->GetPointIds()->SetId(2, i + 36);
		cells->InsertNextCell(triangle2);
	}

	for (vtkIdType i = 0; i < 36; i++)
	{
		double innerPoint[3];
		double outerPoint[3];
		innerCirclePoints->GetPoint(i, innerPoint);
		outerCirclePoints->GetPoint(i, outerPoint);
		innerPoint[0] += startPoint[0];
		innerPoint[1] += startPoint[1];
		innerPoint[2] += startPoint[2];
		outerPoint[0] += startPoint[0];
		outerPoint[1] += startPoint[1];
		outerPoint[2] += startPoint[2];
		innerCirclePoints->SetPoint(i, innerPoint);
		outerCirclePoints->SetPoint(i, outerPoint);
	}

	// Create polydata
	polyData->SetPoints(innerCirclePoints);
	polyData->SetPoints(outerCirclePoints);
	polyData->SetPolys(cells);
}

void BridgeProperty::CreateCrossSectionShapeTube_2(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double startPoint[3], double endPoint[3])
{
	if (!polyData)
	{
		std::cerr << "Failed to create cross-sectional shape!" << std::endl;
		// 添加其他的错误处理代码
	}

	vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	ConstuctRotationMatrix(startPoint, endPoint, transformMatrix);

	const int numSegments = 36;
	int numberOfCell = points->GetNumberOfPoints();
	// transform the points using the rotation matrix
	for (vtkIdType i = 0; i < numberOfCell; i++)
	{
		double point[3];
		points->GetPoint(i, point);
		transformMatrix->MultiplyPoint(point, point);
		point[0] += startPoint[0];
		point[1] += startPoint[1];
		point[2] += startPoint[2];
		points->SetPoint(i, point);
	}

	// 添加面到 CellArray
	vtkNew<vtkCellArray> polygons;

	// Create cells
	for (int i = 0; i < numSegments; i++)
	{
		vtkSmartPointer<vtkTriangle> triangle1 = vtkSmartPointer<vtkTriangle>::New();
		triangle1->GetPointIds()->SetId(0, i);
		triangle1->GetPointIds()->SetId(1, (i + 1) % numSegments);
		triangle1->GetPointIds()->SetId(2, i + numSegments);
		polygons->InsertNextCell(triangle1);

		vtkSmartPointer<vtkTriangle> triangle2 = vtkSmartPointer<vtkTriangle>::New();
		triangle2->GetPointIds()->SetId(0, (i + 1) % numSegments);
		triangle2->GetPointIds()->SetId(1, (i + 1) % numSegments + numSegments);
		triangle2->GetPointIds()->SetId(2, i + numSegments);
		polygons->InsertNextCell(triangle2);
	}

	// 创建 vtkPolyData
	polyData->SetPoints(points);
	polyData->SetPolys(polygons);
}

void BridgeProperty::CreateCrossSectionShapeCir(double radius, double resolution, vtkPolyData* polyData, double startPoint[3], double endPoint[3])
{
	if (!polyData)
	{
		std::cerr << "Failed to create cross-sectional shape!" << std::endl;
		// 添加其他的错误处理代码
	}

	vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	ConstuctRotationMatrix(startPoint, endPoint, transformMatrix);

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	for (int i = 0; i < resolution; i++)
	{
		double angle = 2.0 * vtkMath::Pi() * static_cast<double>(i) / static_cast<double>(resolution);
		double x = radius * cos(angle);
		double y = radius * sin(angle);
		double z = 0.0;
		points->InsertNextPoint(x, y, z);
	}

	// transform the points using the rotation matrix and the center point
	for (vtkIdType i = 0; i < resolution; i++)
	{
		double point[3];
		points->GetPoint(i, point);
		transformMatrix->MultiplyPoint(point, point);
		point[0] += startPoint[0];
		point[1] += startPoint[1];
		point[2] += startPoint[2];
		points->SetPoint(i, point);
	}

	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	vtkIdType* vertices = new vtkIdType[resolution + 1];
	for (int i = 0; i < resolution; i++)
	{
		vertices[i] = i;
	}
	cells->InsertNextCell(resolution, vertices);

	polyData->SetPoints(points);
	polyData->SetPolys(cells);
	delete[] vertices;
}

void BridgeProperty::CreateCrossSectionShapeCir_2(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double startPoint[3], double endPoint[3])
{
	if (!polyData)
	{
		std::cerr << "Failed to create cross-sectional shape!" << std::endl;
		// 添加其他的错误处理代码
	}

	double resolution = 20;
	vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	ConstuctRotationMatrix(startPoint, endPoint, transformMatrix);

	// transform the points using the rotation matrix and the center point
	for (vtkIdType i = 0; i < resolution; i++)
	{
		double point[3];
		points->GetPoint(i, point);
		transformMatrix->MultiplyPoint(point, point);
		point[0] += startPoint[0];
		point[1] += startPoint[1];
		point[2] += startPoint[2];
		points->SetPoint(i, point);		
	}

	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	vtkIdType* vertices = new vtkIdType[resolution + 1];
	for (int i = 0; i < resolution; i++)
	{
		vertices[i] = i;
	}
	cells->InsertNextCell(resolution, vertices);

	polyData->SetPoints(points);
	polyData->SetPolys(cells);
	delete[] vertices;
}

void BridgeProperty::CreateCrossSectionShapeRec(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double stratPoint[3], double endPoint[3])
{
	if (!polyData)
	{
		std::cerr << "Failed to create cross-sectional shape!" << std::endl;
		// 添加其他的错误处理代码
	}

	vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	ConstuctRotationMatrix(stratPoint, endPoint, transformMatrix);

	// transform the points using the rotation matrix and the center point
	for (vtkIdType i = 0; i < 4; i++)
	{
		double point[3];
		points->GetPoint(i, point);
		transformMatrix->MultiplyPoint(point, point);
		point[0] += stratPoint[0];
		point[1] += stratPoint[1];
		point[2] += stratPoint[2];
		points->SetPoint(i, point);
	}

	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	vtkIdType vertices[4] = { 0, 1, 2, 3 };
	cells->InsertNextCell(4, vertices);

	// Create a polydata object for the cross-sectional shape
	polyData->SetPoints(points);
	polyData->SetPolys(cells);
	g_PolyDataList.push_back(polyData);
}

void BridgeProperty::CreateCrossSectionShapeBox(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double startPoint[3], double endPoint[3])
{
	if (!polyData)
	{
		std::cerr << "Failed to create cross-sectional shape!" << std::endl;
		// 添加其他的错误处理代码
	}

	vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	ConstuctRotationMatrix(startPoint, endPoint, transformMatrix);

	const int numSegments = 4;
	int numberOfCell = points->GetNumberOfPoints();
	// transform the points using the rotation matrix
	for (vtkIdType i = 0; i < numberOfCell; i++)
	{
		double point[3];
		points->GetPoint(i, point);
		transformMatrix->MultiplyPoint(point, point);
		point[0] += startPoint[0];
		point[1] += startPoint[1];
		point[2] += startPoint[2];
		points->SetPoint(i, point);
	}

	// 添加面到 CellArray
	vtkNew<vtkCellArray> polygons;

	// Create cells
	for (int i = 0; i < numSegments; i++)
	{
		vtkSmartPointer<vtkTriangle> triangle1 = vtkSmartPointer<vtkTriangle>::New();
		triangle1->GetPointIds()->SetId(0, i);
		triangle1->GetPointIds()->SetId(1, (i + 1) % numSegments);
		triangle1->GetPointIds()->SetId(2, i + numSegments);
		polygons->InsertNextCell(triangle1);

		vtkSmartPointer<vtkTriangle> triangle2 = vtkSmartPointer<vtkTriangle>::New();
		triangle2->GetPointIds()->SetId(0, (i + 1) % numSegments);
		triangle2->GetPointIds()->SetId(1, (i + 1) % numSegments + numSegments);
		triangle2->GetPointIds()->SetId(2, i + numSegments);
		polygons->InsertNextCell(triangle2);
	}

	// 创建 vtkPolyData
	polyData->SetPoints(points);
	polyData->SetPolys(polygons);
}

void BridgeProperty::AddNewRectangular()
{
	double startPoint[3];
	double endPoint[3];
	double length, width;
	cout << "Enter start point (x y z): ";
	cin >> startPoint[0] >> startPoint[1] >> startPoint[2];
	cout << "Enter end point (x y z): ";
	cin >> endPoint[0] >> endPoint[1] >> endPoint[2];
	cout << "Enter length width: ";
	cin >> length >> width;

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	// CreateCrossSectionShapeRec(length, width, polyData, startPoint, endPoint);
	// testModel(length, width, polyData, startPoint, endPoint);
	g_PolyDataList.push_back(polyData);

	// Create linear extrusion filter for the first cross-sectional shape
	vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
	extrude->SetInputData(polyData);
	// 设置第一个截面的拉伸方向
	extrude->SetExtrusionTypeToNormalExtrusion();
	extrude->SetVector(endPoint[0] - startPoint[0], endPoint[1] - startPoint[1], endPoint[2] - startPoint[2]);
	g_ExtrusionList.push_back(extrude);
}

void BridgeProperty::AddNewCylindrical()
{
	double startPoint[3];
	double endPoint[3];
	double radius, resolution;
	cout << "Enter start point (x y z): ";
	cin >> startPoint[0] >> startPoint[1] >> startPoint[2];
	cout << "Enter end point (x y z): ";
	cin >> endPoint[0] >> endPoint[1] >> endPoint[2];
	cout << "Enter radius resolution: ";
	cin >> radius >> resolution;

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	CreateCrossSectionShapeCir(radius, resolution, polyData, startPoint, endPoint);
	g_PolyDataList.push_back(polyData);

	// Create linear extrusion filter for the first cross-sectional shape
	vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
	extrude->SetInputData(polyData);
	// 设置第一个截面的拉伸方向
	extrude->SetExtrusionTypeToNormalExtrusion();
	extrude->SetVector(endPoint[0] - startPoint[0], endPoint[1] - startPoint[1], endPoint[2] - startPoint[2]);
	g_ExtrusionList.push_back(extrude);
}

void BridgeProperty::AddNewCircular()
{
	double startPoint[3];
	double endPoint[3];
	double outRadius, inRadius;
	cout << "Enter start point (x y z): ";
	cin >> startPoint[0] >> startPoint[1] >> startPoint[2];
	cout << "Enter end point (x y z): ";
	cin >> endPoint[0] >> endPoint[1] >> endPoint[2];
	cout << "Enter inRadius outRadius: ";
	cin >> inRadius >> outRadius;

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	CreateCrossSectionShapeTube(inRadius, outRadius, polyData, startPoint, endPoint);
	g_PolyDataList.push_back(polyData);

	// Create linear extrusion filter for the first cross-sectional shape
	vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
	extrude->SetInputData(polyData);
	// 设置第一个截面的拉伸方向
	extrude->SetExtrusionTypeToNormalExtrusion();
	extrude->SetVector(endPoint[0] - startPoint[0], endPoint[1] - startPoint[1], endPoint[2] - startPoint[2]);
	g_ExtrusionList.push_back(extrude);
}

void BridgeProperty::GenerateBridgeDecks(vtkSmartPointer<vtkPoints> deckPoints, vtkSmartPointer<vtkPoints> deckShapePoints, QString setionShape)
{
	double startPoint[3];
	double endPoint[3];

	for (vtkIdType i = 0; i < deckPoints->GetNumberOfPoints() - 1; i++)
	{
		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		deckPoints->GetPoint(i, startPoint);
		deckPoints->GetPoint(i + 1, endPoint);

		vtkSmartPointer<vtkPoints> tempPoints = vtkSmartPointer<vtkPoints>::New();
		tempPoints->DeepCopy(deckShapePoints); // 将原始的截面点复制给临时对象

		ChooseCreateSectionShape(tempPoints, polyData, startPoint, endPoint, setionShape);

		vtkSmartPointer<vtkFeatureEdges> featureEdges = vtkSmartPointer<vtkFeatureEdges>::New();
		featureEdges->SetInputData(polyData);
		featureEdges->BoundaryEdgesOn();
		featureEdges->FeatureEdgesOff();
		featureEdges->ManifoldEdgesOff();
		featureEdges->NonManifoldEdgesOff();
		featureEdges->Update();
		g_PolyDataList.push_back(polyData);

		// Create linear extrusion filter for the first cross-sectional shape
		vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
		extrude->SetInputData(polyData);


		// 设置第一个截面的拉伸方向
		extrude->SetExtrusionTypeToNormalExtrusion();
		extrude->SetVector(endPoint[0] - startPoint[0], endPoint[1] - startPoint[1], endPoint[2] - startPoint[2]);
		g_ExtrusionList.push_back(extrude);


		if (featureEdges->GetOutput()->GetNumberOfCells() == 0) {
			std::cerr << "Feature edges output is empty!" << std::endl;
			// 添加其他的错误处理代码
		}

		deckAppendFilter->AddInputConnection(extrude->GetOutputPort());
		duckEdgesAppendFilter->AddInputConnection(featureEdges->GetOutputPort());
	}
	deckAppendFilter->Update();
	duckEdgesAppendFilter->Update();
}

void BridgeProperty::GenerateBridgeTower(double towerHeight, vtkSmartPointer<vtkPoints> towerPoint, double point[3])
{
	double startPoint[3];
	double endPoint[3];

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	startPoint[0] = point[0];
	startPoint[1] = point[1] - towerHeight;
	startPoint[2] = point[2];

	CreateCrossSectionShapeCir(1, 50, polyData, startPoint, point);
	g_PolyDataList.push_back(polyData);

	// Create linear extrusion filter for the first cross-sectional shape
	vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
	extrude->SetInputData(polyData);

	// 设置第一个截面的拉伸方向 
	extrude->SetExtrusionTypeToNormalExtrusion();
	extrude->SetVector(point[0] - startPoint[0], point[1] - startPoint[1], point[2] - startPoint[2]);
	g_ExtrusionList.push_back(extrude);

	towerAppendFilter->AddInputConnection(extrude->GetOutputPort());


	for (vtkIdType i = 0; i < towerPoint->GetNumberOfPoints() - 1; i++)
	{
		if (i == 0) return;
		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		towerPoint->GetPoint(i, startPoint);
		towerPoint->GetPoint(i + 1, endPoint);
		CreateCrossSectionShapeCir(1, 50, polyData, startPoint, endPoint);
		g_PolyDataList.push_back(polyData);

		// Create linear extrusion filter for the first cross-sectional shape
		vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
		extrude->SetInputData(polyData);

		// 设置第一个截面的拉伸方向
		extrude->SetExtrusionTypeToNormalExtrusion();
		extrude->SetVector(endPoint[0] - startPoint[0], endPoint[1] - startPoint[1], endPoint[2] - startPoint[2]);
		g_ExtrusionList.push_back(extrude);

		towerAppendFilter->AddInputConnection(extrude->GetOutputPort());
	}
	towerAppendFilter->Update();
}

void BridgeProperty::GenerateBridgeTower_2(double towerHeight, vtkSmartPointer<vtkPoints> towerPoint, double point[3], vtkSmartPointer<vtkPoints> towerShapePoints, QString sectionShape)
{
	double startPoint[3];
	double endPoint[3];

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	startPoint[0] = point[0];
	startPoint[1] = point[1] - towerHeight;
	startPoint[2] = point[2];

	vtkSmartPointer<vtkPoints> tempPoints = vtkSmartPointer<vtkPoints>::New();
	tempPoints->DeepCopy(towerShapePoints); // 将原始的截面点复制给临时对象

	ChooseCreateSectionShape(tempPoints, polyData, startPoint, point, sectionShape);
	g_PolyDataList.push_back(polyData);

	// Create linear extrusion filter for the first cross-sectional shape
	vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
	extrude->SetInputData(polyData);

	// 设置第一个截面的拉伸方向 
	extrude->SetExtrusionTypeToNormalExtrusion();
	extrude->SetVector(point[0] - startPoint[0], point[1] - startPoint[1], point[2] - startPoint[2]);
	g_ExtrusionList.push_back(extrude);

	towerAppendFilter->AddInputConnection(extrude->GetOutputPort());

	for (vtkIdType i = 0; i < towerPoint->GetNumberOfPoints() - 1; i++)
	{

		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		towerPoint->GetPoint(i, startPoint);
		towerPoint->GetPoint(i + 1, endPoint);

		// 将原始的截面点复制给临时对象
		vtkSmartPointer<vtkPoints> tempPoints = vtkSmartPointer<vtkPoints>::New();
		tempPoints->DeepCopy(towerShapePoints); 

		ChooseCreateSectionShape(tempPoints, polyData, startPoint, endPoint, sectionShape);

		g_PolyDataList.push_back(polyData);

		// Create linear extrusion filter for the first cross-sectional shape
		vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
		extrude->SetInputData(polyData);

		// 设置第一个截面的拉伸方向
		extrude->SetExtrusionTypeToNormalExtrusion();
		extrude->SetVector(endPoint[0] - startPoint[0], endPoint[1] - startPoint[1], endPoint[2] - startPoint[2]);
		g_ExtrusionList.push_back(extrude);

		towerAppendFilter->AddInputConnection(extrude->GetOutputPort());
	}
	towerAppendFilter->Update();
}

void BridgeProperty::GenerateInclinedRopes(vtkSmartPointer<vtkPoints> deckPoints, vtkSmartPointer<vtkPoints> towerPoints)
{
	double startPoint[3];
	double endPoint[3];
	int pointNum = deckPoints->GetNumberOfPoints() < towerPoints->GetNumberOfPoints() ?
		deckPoints->GetNumberOfPoints() : towerPoints->GetNumberOfPoints();
	for (vtkIdType i = 0; i < pointNum - 1; i++)
	{
		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		deckPoints->GetPoint(i + 1, startPoint);
		towerPoints->GetPoint(towerPoints->GetNumberOfPoints() - i - 1, endPoint);
		CreateCrossSectionShapeCir(0.1, 20, polyData, startPoint, endPoint);
		g_PolyDataList.push_back(polyData);

		// Create linear extrusion filter for the first cross-sectional shape
		vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
		extrude->SetInputData(polyData);

		// 设置第一个截面的拉伸方向
		extrude->SetExtrusionTypeToNormalExtrusion();
		extrude->SetVector(endPoint[0] - startPoint[0], endPoint[1] - startPoint[1], endPoint[2] - startPoint[2]);
		g_ExtrusionList.push_back(extrude);

		ropeAppendFilter->AddInputConnection(extrude->GetOutputPort());
	}
	ropeAppendFilter->Update();
}

void BridgeProperty::GenerateInclinedRopes_2(vtkSmartPointer<vtkPoints> deckPoints, vtkSmartPointer<vtkPoints> towerPoints, vtkSmartPointer<vtkPoints> ropeShapePoints, QString sectionShape)
{
	double startPoint[3];
	double endPoint[3];
	int pointNum = deckPoints->GetNumberOfPoints() < towerPoints->GetNumberOfPoints() ?
		deckPoints->GetNumberOfPoints() : towerPoints->GetNumberOfPoints();
	for (vtkIdType i = 0; i < pointNum - 1; i++)
	{
		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		deckPoints->GetPoint(i + 1, startPoint);
		towerPoints->GetPoint(towerPoints->GetNumberOfPoints() - i - 2, endPoint);

		// 将原始的截面点复制给临时对象
		vtkSmartPointer<vtkPoints> tempPoints = vtkSmartPointer<vtkPoints>::New();
		tempPoints->DeepCopy(ropeShapePoints);

		ChooseCreateSectionShape(tempPoints, polyData, startPoint, endPoint, sectionShape);
		g_PolyDataList.push_back(polyData);

		// Create linear extrusion filter for the first cross-sectional shape
		vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
		extrude->SetInputData(polyData);

		// 设置第一个截面的拉伸方向
		extrude->SetExtrusionTypeToNormalExtrusion();
		extrude->SetVector(endPoint[0] - startPoint[0], endPoint[1] - startPoint[1], endPoint[2] - startPoint[2]);
		g_ExtrusionList.push_back(extrude);

		ropeAppendFilter->AddInputConnection(extrude->GetOutputPort());
	}
	ropeAppendFilter->Update();
}

void BridgeProperty::VisualPoint(vtkSmartPointer<vtkActor> actor)
{
	vtkSmartPointer<vtkPoints> totalPoints = vtkSmartPointer<vtkPoints>::New();
	totalPoints->InsertPoints(totalPoints->GetNumberOfPoints(), leftDeckPoints->GetNumberOfPoints(), 0, leftDeckPoints);
	totalPoints->InsertPoints(totalPoints->GetNumberOfPoints(), centerLeftDeckPoints->GetNumberOfPoints(), 0, centerLeftDeckPoints);
	totalPoints->InsertPoints(totalPoints->GetNumberOfPoints(), centerRightDeckPoints->GetNumberOfPoints(), 0, centerRightDeckPoints);
	totalPoints->InsertPoints(totalPoints->GetNumberOfPoints(), rightDeckPoints->GetNumberOfPoints(), 0, rightDeckPoints);
	totalPoints->InsertPoints(totalPoints->GetNumberOfPoints(), leftTowerPoints->GetNumberOfPoints(), 0, leftTowerPoints);
	totalPoints->InsertPoints(totalPoints->GetNumberOfPoints(), rightTowerPoints->GetNumberOfPoints(), 0, rightTowerPoints);

	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	for (int i = 0; i < totalPoints->GetNumberOfPoints(); i++)
	{
		cells->InsertCellPoint(i);
	}

	vtkSmartPointer<vtkPolyData> nodePolyData = vtkSmartPointer<vtkPolyData>::New();
	nodePolyData->SetPoints(totalPoints);
	nodePolyData->SetLines(cells);

	// Create sphere for glyphing
	vtkSmartPointer<vtkSphereSource> sphereSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetRadius(0.35); // radius of spheres
	sphereSource->SetThetaResolution(20);
	sphereSource->SetPhiResolution(20);

	// Glyph the points with spheres
	vtkSmartPointer<vtkGlyph3D> glyph =
		vtkSmartPointer<vtkGlyph3D>::New();
	glyph->SetInputData(nodePolyData);
	glyph->SetSourceConnection(sphereSource->GetOutputPort());
	glyph->SetScaleFactor(1); // scale of spheres
	glyph->Update();

	// Create mapper and actor for glyphed points
	vtkSmartPointer<vtkPolyDataMapper> nodeMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	nodeMapper->SetInputConnection(glyph->GetOutputPort());

	actor->SetMapper(nodeMapper);
	actor->GetProperty()->SetColor(1, 0, 0);
}

QString BridgeProperty::GetSectionShape(QString shape)
{
	return shape;
}

void BridgeProperty::ChooseCreateSectionShape(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double startPoint[3], double endPoint[3], QString sectionShape)
{
	if (sectionShape == "SoildCir")
	{
		CreateCrossSectionShapeCir_2(points, polyData, startPoint, endPoint);
	}
	else if (sectionShape == "SoildRec")
	{
		CreateCrossSectionShapeRec(points, polyData, startPoint, endPoint);
	}
	else if (sectionShape == "Tube")
	{
		CreateCrossSectionShapeTube_2(points, polyData, startPoint, endPoint);
	}
	else if (sectionShape == "Box")
	{
		CreateCrossSectionShapeBox(points, polyData, startPoint, endPoint);
	}
}

void BridgeProperty::PrintPoint(vtkSmartPointer<vtkPoints> points)
{
	if (!points)
	{
		qDebug() << "Points object is null.";
		return;
	}
	qDebug() << "points number: " << points->GetNumberOfPoints();
	for (int i = 0; i < points->GetNumberOfPoints(); i++)
	{
		double point[3];
		points->GetPoint(i, point);
		qDebug() << "Point " << i << ": (" << point[0] << ", " << point[1] << ", " << point[2] << ")";
	}
}

