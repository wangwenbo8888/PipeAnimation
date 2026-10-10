#include "xHUDAxis.h"

HUDAxis::HUDAxis()
{

}

HUDAxis::HUDAxis(const HUDAxis& copy, osg::CopyOp copyOp /*= osg::CopyOp::SHALLOW_COPY*/):Camera(copy, copyOp),
	_mainCamera(copy._mainCamera)
{

}
// 每次回调时，该函数就会被执行
void HUDAxis::traverse(osg::NodeVisitor& nv)
{
	double fovy, aspectRatio, vNear, vFar;
	_mainCamera->getProjectionMatrixAsPerspective(fovy, aspectRatio, vNear, vFar);

	// 改为正投影，正投影不会随相机的拉近拉远而放大、缩小，这样就剔除了缩放效果
	this->setProjectionMatrixAsOrtho(-10.0*aspectRatio, 10.0*aspectRatio, -10.0, 10.0, 2.0, 4.0); //设置投影矩阵，使缩放不起效果

	// 让坐标轴模型位于窗体右下角。
	osg::Vec3 trans(8.5*aspectRatio, -8.5, -8.0); 
	if (_mainCamera.valid())
	{
		osg::Matrix matrix = _mainCamera->getViewMatrix();//改变视图矩阵，让移动位置固定
		// 让移动固定，否则鼠标左键按住模型可以拖动或按空格键时模型会动
		matrix.setTrans(trans);  
		this->setViewMatrix(matrix);
	}
	osg::Camera::traverse(nv);
}

HUDAxis::~HUDAxis()
{

}
