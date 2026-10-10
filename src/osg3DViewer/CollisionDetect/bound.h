/***************************************************************
///Copyright (C),    2020-2040,       
///Version     :      V1.0   //版本        
///Author      :      wangwenbo 
///Date        :      
///File name   :      bound.h
///Description :      mesh处理包围盒定义头文件 
///History     :      wangwenbo create
***************************************************************/

#ifndef IP_MESH_DATA_STRUCTURE_BOUND_H_
#define IP_MESH_DATA_STRUCTURE_BOUND_H_

//#include "xj_data_structure_vector.h"
//#include "xj_mesh_algorithm.h"

//class VectorSet;
#include <osg/Vec3>

class Bound
{
public:
    double xMax;
    double xMin;
    double yMax;
    double yMin;
    double zMax;
    double zMin;
    
    Bound();
    Bound(const osg::Vec3& p1, const osg::Vec3& p2, const osg::Vec3& p3);
    virtual ~Bound();
    
    bool OverLap(Bound& bound);

    static double TOL;

	void expandBy(const Bound& bb);
	void expandBy(const osg::Vec3& p);

	bool valid() const;

	double& Min(int index);
	
	double& Max(int index);

	void init();

	/** Calculates and returns the bounding box center. */
	osg::Vec3 center()
	{
		return osg::Vec3((xMax+xMin)*0.5, (yMax+yMin)*0.5, (zMax+zMin)*0.5);
	}

	//double size()
	//{
	//	osg::Vec3 vMin(xMin, yMin, zMin);
	//	osg::Vec3 vMax(xMax, yMax, zMax);

	//	return (vMax-vMin).Magnitude();
	//}

private:

    void CheckVertex(const osg::Vec3 & vertex);
    //void CheckVertex(const VertexBool& ver);
};



#endif // 
