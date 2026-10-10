/**************************************************************************
///Copyright (C), .     
///Version:       V1.0                                     // 版本        
///Author:        wangwenbo                                // 作者
///Date:          20141225                                 // 完成日期
///File name:     ip_mesh_algorithm.cpp                    // 文件名
///Description:   STL文件边界处理源文件                    // 用于详细说明此程序文件完成的主要功能
///History:       20141225 wangwenbo 创建                  // 修改历史记录列表，每条修改记录应包括修改日期、修改
                                                           // 者及修改内容简述  
***************************************************************************/
//#include "xj_mesh_algorithm.h"
//#include "xj_data_structure_bound.h"
//#include "wwb_mesh_algorithm.h"

#include "bound.h"


double Bound::TOL = 1e-8;

// 构造三点构成的包围盒
Bound::Bound(const osg::Vec3 & p1, const osg::Vec3 & p2, const osg::Vec3 & p3)
{
    xMax = xMin = p1.x();
    yMax = yMin = p1.y();
    zMax = zMin = p1.z();
    
    CheckVertex(p2);
    CheckVertex(p3);
}

Bound::Bound()
{ 
	init();
}

Bound::~Bound()
{
}

void Bound::init()
{
	xMax = -FLT_MAX;
	xMin = FLT_MAX;
	yMax = -FLT_MAX;
	yMin = FLT_MAX;
	zMax = -FLT_MAX;
	zMin = FLT_MAX;
}

// 判断两包围盒是否相交 false：不相交
bool Bound::OverLap(Bound & bound)
{
    if((xMin>bound.xMax+TOL)||(xMax<bound.xMin-TOL)||(yMin>bound.yMax+TOL)||(yMax<bound.yMin-TOL)||(zMin>bound.zMax+TOL)||(zMax<bound.zMin-TOL))
    {
        return false;
    }
    else
    {
        return true;
    }
}

// 检查点
void Bound::CheckVertex(const osg::Vec3 & vertex)
{
    if(vertex.x() > xMax)
    {
        xMax = vertex.x();
    }
    else if(vertex.x()<xMin)
    {
        xMin = vertex.x();
    }
    
    if(vertex.y()>yMax)
    {
        yMax = vertex.y();
    }
    else if(vertex.y()<yMin)
    {
        yMin = vertex.y();
    }
    
    if(vertex.z()>zMax)
    {
        zMax = vertex.z();
    }
    else if(vertex.z()<zMin)
    {
        zMin = vertex.z();
    }
}

double& Bound::Max(int index)
{
	if (index == 0)
		return xMax;
	else if(index == 1)
		return yMax;
	else
		return zMax;
}

double& Bound::Min(int index)
{
	if (index == 0)
		return xMin;
	else if(index == 1)
		return yMin;
	else
		return zMin;
}

bool Bound::valid() const
{
	return xMax >= xMin &&  yMax >= yMin &&  zMax >= zMin;
}

void Bound::expandBy(const Bound& bb)
{
	if (!bb.valid()) return;

	if(bb.xMin<xMin)
		xMin = bb.xMin;

	if(bb.xMax>xMax) 
		xMax = bb.xMax;

	if(bb.yMin<yMin) 
		yMin = bb.yMin;

	if(bb.yMax>yMax) 
		yMax = bb.yMax;

	if(bb.zMin<zMin)
		zMin = bb.zMin;

	if(bb.zMax>zMax)
		zMax = bb.zMax;
}

void Bound::expandBy(const osg::Vec3& p)
{
	if(p.x() < xMin)
		xMin = p.x();

	if (p.x() > xMax)
		xMax = p.x();

	if(p.y() < yMin)
		yMin = p.y();

	if (p.y() > yMax)
		yMax = p.y();

	if(p.z() < zMin)
		zMin = p.z();

	if (p.z() > zMax)
		zMax = p.z();
}