
#ifndef CREATENODE_H_
#define CREATENODE_H_

#include "STL.h"

#include <osg/Node>
#include <osg/Geode>//Geode是专门管理可绘制几何体的总闸
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/LightModel>
#include <osg/Material>
#include <osgUtil/Optimizer>

osg::ref_ptr<osg::Node> CloneStl(STL* stl,osg::Vec4 color,osg::ref_ptr<osg::Material>& material);

#endif