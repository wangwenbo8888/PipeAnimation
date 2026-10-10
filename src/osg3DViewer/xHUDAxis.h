#ifndef _XHUDAXIS_H_
#define _XHUDAXIS_H_

#include<osg/Geometry>
#include<osg/Geode>
#include<osgViewer/Viewer>
#include<osgViewer/ViewerEventHandlers>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include<osg/DrawPixels>
#include<osg/PositionAttitudeTransform>
#include<osg/computeBoundsVisitor>
#include<osg/MatrixTransform>
#include<osg/ShapeDrawable>
#include<osg/Shape>
#include<osg/PolygonMode>
#include<osg/NodeVisitor>
class HUDAxis :public osg::Camera
{
public:
	HUDAxis();
	HUDAxis( const HUDAxis& copy, osg::CopyOp copyOp = osg::CopyOp::SHALLOW_COPY);
	META_Node(osg, HUDAxis);
	inline void setMainCamera(Camera* camera)
	{ 
		_mainCamera = camera; 
	}
	virtual void traverse(osg::NodeVisitor& nv);
protected:
	virtual ~HUDAxis();
	osg::observer_ptr<Camera> _mainCamera;
};

#endif