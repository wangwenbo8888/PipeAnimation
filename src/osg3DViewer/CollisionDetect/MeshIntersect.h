
#include <osg/Vec3>
#include <osg/Array>
#include <osg/ref_ptr>
#include <osg/Geometry>

#include "qstring.h"

#define ALGO_FTOL 1e-8

struct Triangle
{  
	Triangle(const osg::Vec3& normal,const osg::Vec3& p0,const osg::Vec3& p1,const osg::Vec3& p2)
	{
		Normal_0 = normal;
		Vertex_1 = p0;
		Vertex_2 = p1;
		Vertex_3 = p2;
	}

	osg::Vec3 Normal_0;
	osg::Vec3 Vertex_1, Vertex_2, Vertex_3;  
}; 

struct point2D  
{  
	double x, y;
};

bool IsGeometryIntersect(const osg::ref_ptr<osg::Vec3Array>& vertices0,
	const QString name0,
	const osg::ref_ptr<osg::Vec3Array>& vertices1,
    const QString name1);

class MeshIntersect
{
public:

	MeshIntersect(const osg::ref_ptr<osg::Vec3Array>& pmesh1,
		const QString name0,
		const osg::ref_ptr<osg::Vec3Array>& pmesh2,
		const QString name1);
	~MeshIntersect();

	bool Calc();

private:
	osg::Vec3 CalcTriNormal(const osg::Vec3& p0,const osg::Vec3& p1,const osg::Vec3& p2);

	const osg::ref_ptr<osg::Vec3Array>& m_pArray0;
	const osg::ref_ptr<osg::Vec3Array>& m_pArray1;

	const QString m_name0;
	const QString m_name1;
};