/******************************************************************************
   osg3DViewer: a 3D file viewer
   Copyright(C) 2015-2015  ronggenmiao@163.com

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software

 *******************************************************************************/

#ifndef _OSGVIEWER_OSGWINDOW_H_
#define _OSGVIEWER_OSGWINDOW_H_
#pragma once


#include <QtCore/QTimer>

#include <osgViewer/CompositeViewer>
#include <osgGA/StateSetManipulator>
#include <osgQt/GraphicsWindowQt>
#include<osg/io_utils>
#include <osg/MatrixTransform>
#include<osg/PositionAttitudeTransform>
#include<osgDB/ReadFile>
#include<osgViewer/Viewer>
#include<iostream>


#include "xPickingHandler.h"
//#include <osgShadow/ShadowedScene>
//#include <osgFX/Scribe>

using namespace std;

//class RotateCallback : public osg::NodeCallback
//{
//public:
//	RotateCallback(osg::Vec3d axis, double rotAngular) : 
//	  _axis(axis), _angle(rotAngular),_speed(0.01),_currentRotation(0.0)
//	  {
//	  }
//
//	  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	  {
//		  osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		  if (mt)
//		  {
//			  _currentRotation += _speed;
//
//			  if(_currentRotation >= _angle)
//			  {
//					return;
//			  }
//
//
//			/*  if (_currentRotation > (2 * osg::PI))
//			  {
//				  _currentRotation -= (osg::PI * 2);
//			  }*/
//
//			  osg::Quat rotQuat(_currentRotation, _axis);
//			  osg::Matrix rotMatrix(rotQuat);
//
//			  mt->setMatrix(rotMatrix);
//			  //mt->setMatrix(osg::Matrix::rotate(osg::inDegrees(-90.0f),_axis));
//		  }
//
//	
//		  traverse(node, nv);
//	  }
//protected:
//	osg::Vec3d  _axis;
//	double      _angle;
//	double      _speed;
//	double      _currentRotation;
//};
//
//class MoveCallback : public osg::NodeCallback
//{
//public:
//	MoveCallback(osg::Vec3d axis, double pos) : 
//	  _axis(axis), _pos(pos),_speed(5.0),_currentPos(0.0)
//	  {
//	  }
//
//	  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	  {
//		  osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		  if (mt)
//		  {
//
//			  if(_currentPos >= _pos)
//			  {
//				  traverse(node,nv);
//				  return;
//			  }
//
//			  _currentPos += _speed;
//			  mt->setMatrix(osg::Matrix::translate(_currentPos,0,0));
//		  }
//
//
//		  traverse(node, nv);
//	  }
//protected:
//	osg::Vec3d  _axis;
//	double      _pos;
//	double      _speed;
//	double      _currentPos;
//};
//
//


class xSceneModel;
class xSelectionManager;
class xSceneView : public QWidget, public osgViewer::CompositeViewer
{
	Q_OBJECT
public:
	xSceneView(QWidget *parent = NULL);
	virtual ~xSceneView(void) {}

	osg::ref_ptr<osgViewer::View> getOsgView();
	void ResumeView();
	void setModel(xSceneModel *pModel);
	void setRefreshPeriod(unsigned int period);
	void setIdle(bool val);
	//void setSceneData(osg::Node *);

	void setLightingEnabled(bool bLightingOn);
	bool getLightingEnabled() const;

	void setTextureEnabled(bool bTextureOn);
	bool getTextureEnabled() const;
	
    void setBackfaceEnabled(bool bBackface);
    bool getBackfaceEnabled() const;

	void setHighlightScene(bool val);

	void setShadowEnabled(bool val);
	void home();

	void resetSelection();

	bool highlight(osg::Node* node);
	bool centerOnNode(osg::Node* node);

	void resizeEvent(QResizeEvent * event);
	void UpdateView();

	osgGA::TrackballManipulator *getTrackballManipulator(){return m_pickHandler.get();}
	void RunSlotUpdateModel();
	

public slots:
	void slotUpdateModel();

	void slotResetHome();
	void slotResetView(bool reset = true);
	void slotPickGeometry(osg::Drawable *d);

signals:
	//void newScreenshotAvailable(osg::Image *);
	void sigNewAspectRatio(const QSize &);
	void sigPicked(osg::Drawable *);

protected:
	QWidget* addViewWidget(osgQt::GraphicsWindowQt* gw, osg::Node* scene);
	osgQt::GraphicsWindowQt* createGraphicsWindow(int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false);
	virtual void paintEvent(QPaintEvent* /* event */)  {frame();}

private:
	 osg::Matrix matrixListtoSingle(const osg::MatrixList &tmplist);

private:
	QTimer *m_timer;
	unsigned int m_refreshPeriod;
	osg::ref_ptr<osg::Camera> m_camera;
	osg::ref_ptr<osgViewer::View> m_view;
	osg::ref_ptr<osgGA::StateSetManipulator> m_statesetManipulator;
	osg::ref_ptr<xPickingHandler>  m_pickHandler;

	bool m_isResetHome;
	osg::Matrixd m_matrix;
	osg::Vec3d m_center;
	double m_distance;

	xSceneModel *m_model;
	xSceneModel *m_Resumemodel;

	xSelectionManager *m_selectionManager;


	osg::Vec3d m_eye1;
	osg::Vec3d m_center1;
	osg::Vec3d m_up1;


	
};

#endif 
