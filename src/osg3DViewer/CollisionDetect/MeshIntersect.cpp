
#include "MeshIntersect.h"
#include "GYDevillersTriangle.h"
#include "bound.h"

#include <omp.h>

#include "qstring.h"

#include "ExportStl.h"

bool IsGeometryIntersect(const osg::ref_ptr<osg::Vec3Array>& vertices0,
	const QString name0,
	const osg::ref_ptr<osg::Vec3Array>& vertices1,
	const QString name1)
{
	if (vertices0 == NULL|| vertices1==NULL)
	{
		return false;
	}

	MeshIntersect inter(vertices0,name0,vertices1,name1);
	return inter.Calc();
}

double Magnitude(osg::Vec3 p)
{
	return sqrt(p.x() * p.x() + p.y() * p.y() + p.z() * p.z());
}

osg::Vec3 VectorCross(const osg::Vec3 &a, const osg::Vec3 &b)
{
	return osg::Vec3(
		a.y() * b.z() - a.z() * b.y(),
		a.z() * b.x() - a.x() * b.z(),
		a.x() * b.y() - a.y() * b.x()
		);
}

void Normalize(osg::Vec3& normal)
{
	double mag = Magnitude(normal);

	if(mag == 0.0)
		return;

	double invMag = 1.0 / mag;
	normal.x() *= invMag;
	normal.y() *= invMag;
	normal.z() *= invMag;
}

void copy_point2D( point2D& p, osg::Vec3 f )
{  
	p.x = f.x();
	p.y = f.y();
}

//利用叉积计算点p相对线段p1p2的方位  0--在线段上  1---在线段上面  -1----在下面
int direction( point2D p1, point2D p2, point2D p )
{  
	double temp = (p.x - p1.x) * (p2.y - p1.y) - (p2.x - p1.x) * (p.y - p1.y);
	return temp > ALGO_FTOL ? 1 : temp< -ALGO_FTOL ? -1 : 0;
} 

//判断线段p1p2与线段p3p4是否相交的主函数  
int segments_intersert(point2D p1, point2D p2, point2D p3, point2D p4 )
{  
    int d1, d2, d3, d4;
	d1 = direction( p3, p4, p1 );
	d2 = direction( p3, p4, p2 );
	d3 = direction( p1, p2, p3 );
	d4 = direction( p1, p2, p4 );
    if ( d1 * d2 < 0 && d3 * d4 < 0 )  
    {  
        return 1;  
    }  
	/*else if ( d1 == 0 && on_segment( p3, p4, p1 ) == 1 )  
	{  
	return 1;  
	}  
	else if ( d2 == 0 && on_segment( p3, p4, p2 ) == 1 )  
	{  
	return 1;  
	}  
	else if ( d3 == 0 && on_segment( p1, p2, p3 ) == 1 )  
	{  
	return 1;  
	}  
	else if ( d4 == 0 && on_segment( p1, p2, p4 ) == 1 )  
	{  
	return 1;  
	}  */
    return 0;  
}  

//判断同一平面的直线和三角形是否相交
bool line_triangle_intersert_inSamePlane(Triangle* tri, osg::Vec3 f1, osg::Vec3 f2) 
{  
	point2D p1, p2, p3, p4;

	copy_point2D(p1, f1);

	copy_point2D(p2, f2);

	copy_point2D(p3, tri->Vertex_1);

	copy_point2D(p4, tri->Vertex_2);  

	if ( segments_intersert(p1, p2, p3, p4 ))  
	{  
		return true;  
	}  

	copy_point2D(p3, tri->Vertex_2);  

	copy_point2D(p4, tri->Vertex_3);  

	if ( segments_intersert(p1, p2, p3, p4)) 
	{  
		return true;  
	}  

	copy_point2D(p3, tri->Vertex_1);  

	copy_point2D(p4, tri->Vertex_3);  

	if ( segments_intersert(p1, p2, p3, p4))  
	{  
		return true;  
	}  

	return false;
}

void get_central_point2D(osg::Vec3& centralpoint2D,Triangle* tri)  
{  
	centralpoint2D = (tri->Vertex_1+tri->Vertex_2+tri->Vertex_3)*0.333333;
}

//向量之差  
void get_vector_diff( osg::Vec3& aimV, const osg::Vec3 a, const osg::Vec3 b )  
{
	aimV = b-a;
}  

//向量内积
double Dot( const osg::Vec3& v1, const osg::Vec3& v2 )  
{      
	return v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();
}  

bool is_point2D_within_triangle( Triangle* tri, osg::Vec3 point2D )  
{
	osg::Vec3 v0;
	get_vector_diff( v0, tri->Vertex_1, tri->Vertex_3 );
	osg::Vec3 v1;
	get_vector_diff( v1, tri->Vertex_1, tri->Vertex_2 );
	osg::Vec3 v2;
	get_vector_diff( v2, tri->Vertex_1, point2D );
	double dot00 = Dot( v0, v0 );
	double dot01 = Dot( v0, v1 );
	double dot02 = Dot( v0, v2 );
	double dot11 = Dot( v1, v1 );
	double dot12 = Dot( v1, v2 );
	double inverDeno = 1 / ( dot00* dot11 - dot01* dot01 );    
	double u = ( dot11* dot02 - dot01* dot12 ) * inverDeno;   
	if ( u < 0.00001 || u > 0.99999 ) // if u out of range, return directly  
		return false;

	double v = ( dot00* dot12 - dot01* dot02 ) * inverDeno;   
	if ( v < 0.00001 || v > 0.99999 ) // if v out of range, return directly  
		return false;

	return u + v <= 1;
}

int triangle_intersert_inSamePlane( Triangle* tri1, Triangle* tri2 )  
{  
	if ( line_triangle_intersert_inSamePlane( tri2, tri1->Vertex_1, tri1->Vertex_2 ) )  
	{  
		return 2;
	}  
	else if ( line_triangle_intersert_inSamePlane( tri2, tri1->Vertex_2, tri1->Vertex_3 ) )  
	{  
		return 2;
	}  
	else if ( line_triangle_intersert_inSamePlane( tri2, tri1->Vertex_1, tri1->Vertex_3 ) )  
	{  
		return 2;
	}  
	else  
	{  
		osg::Vec3 centralpoint2D1,centralpoint2D2; 

		get_central_point2D(centralpoint2D1,tri1);  

		get_central_point2D(centralpoint2D2,tri2);  

		if(is_point2D_within_triangle( tri2, centralpoint2D1 ) || is_point2D_within_triangle( tri1, centralpoint2D2 ) )  
		{  
			return 1;  
		}  

		return 0;  
	}
}

osg::Vec3 MeshIntersect::CalcTriNormal(const osg::Vec3& p0,const osg::Vec3& p1,const osg::Vec3& p2)
{
	//Point3D p00(p0.x(),p0.y(),p0.z());
	//Point3D p11(p1.x(),p1.y(),p1.z());
	//Point3D p22(p2.x(),p2.y(),p2.z());
	//Point3D norm;

	//Point3D xy = p11 - p00;
	//Point3D xz = p22 - p00;
	osg::Vec3 xy = p1 - p0;
	osg::Vec3 xz = p2 - p0;

	osg::Vec3 norm = VectorCross(xy, xz);
	if (Magnitude(norm) == 0.0)
		return osg::Vec3(0.0, 0.0, 0.0);

	Normalize(norm);

	return norm;
}
//四点行列式  
int get_vector4_det( osg::Vec3 v1, osg::Vec3 v2, osg::Vec3 v3, osg::Vec3 v4 )
{  
	double a[3][3];

	a[0][0] = v1.x() - v4.x(); 
	a[1][0] = v2.x() - v4.x(); 
	a[2][0] = v3.x() - v4.x();
	a[0][1] = v1.y() - v4.y(); 
	a[1][1] = v2.y() - v4.y();
	a[2][1] = v3.y() - v4.y();
	a[0][2] = v1.z() - v4.z();
	a[1][2] = v2.z() - v4.z();
	a[2][2] = v3.z() - v4.z();

	double temp = a[0][0] * a[1][1] * a[2][2]   
	+ a[0][1] * a[1][2] * a[2][0]   
	+ a[0][2] * a[1][0] * a[2][1]   
	- a[0][2] * a[1][1] * a[2][0]   
	- a[0][1] * a[1][0] * a[2][2]   
	- a[0][0] * a[1][2] * a[2][1]; 

	return temp > ALGO_FTOL? 1 : temp< -ALGO_FTOL? -1 : 0;
} 

int IsTrianglesIntersection(Triangle* tri1, Triangle* tri2)
{  
	//设tri1所在的平面为p1,tri2所在的平面为p2  
	int p1_tri2_vertex1 = get_vector4_det( tri1->Vertex_1, tri1->Vertex_2, tri1->Vertex_3, tri2->Vertex_1 );  
	int p1_tri2_vertex2 = get_vector4_det( tri1->Vertex_1, tri1->Vertex_2, tri1->Vertex_3, tri2->Vertex_2 );  
	int p1_tri2_vertex3 = get_vector4_det( tri1->Vertex_1, tri1->Vertex_2, tri1->Vertex_3, tri2->Vertex_3 );  

	if ((p1_tri2_vertex1 > 0 && p1_tri2_vertex2 > 0 && p1_tri2_vertex3 > 0) ||
		(p1_tri2_vertex1 < 0 && p1_tri2_vertex2 < 0 && p1_tri2_vertex3 < 0))
	{
		return 0;
	}

	if (p1_tri2_vertex1 == 0 && p1_tri2_vertex2 == 0 && p1_tri2_vertex3 == 0)
	{
		int ret = triangle_intersert_inSamePlane(tri1, tri2);
		if (ret == 1)
		{
			return 2;//重叠
		}else if (ret == 2)
		{
			return 3;//重合且相交
		}
		else
			return 0;
	}

	if ((p1_tri2_vertex1 == 0 && p1_tri2_vertex2 == 0) || 
		(p1_tri2_vertex2 == 0 && p1_tri2_vertex3 == 0) ||
		(p1_tri2_vertex3 == 0 && p1_tri2_vertex1 == 0))
	{
		return 0;
	}

	if ((p1_tri2_vertex1 == 0 && p1_tri2_vertex2 * p1_tri2_vertex3 > 0) ||
		(p1_tri2_vertex2 == 0 && p1_tri2_vertex1 * p1_tri2_vertex3 > 0) ||
		(p1_tri2_vertex3 == 0 && p1_tri2_vertex1 * p1_tri2_vertex2 > 0)
		)
	{
		return 0;
	}

	int p2_tri1_vertex1 = get_vector4_det(tri2->Vertex_1, tri2->Vertex_2, tri2->Vertex_3, tri1->Vertex_1);  
	int p2_tri1_vertex2 = get_vector4_det(tri2->Vertex_1, tri2->Vertex_2, tri2->Vertex_3, tri1->Vertex_2);  
	int p2_tri1_vertex3 = get_vector4_det(tri2->Vertex_1, tri2->Vertex_2, tri2->Vertex_3, tri1->Vertex_3);  

	if ((p2_tri1_vertex1 > 0 && p2_tri1_vertex2 > 0 && p2_tri1_vertex3 > 0) ||
		p2_tri1_vertex1 < 0 && p2_tri1_vertex2 < 0 && p2_tri1_vertex3 < 0
		)
	{
		return 0;
	}

	if ((p2_tri1_vertex1 == 0 && p2_tri1_vertex2 == 0) || 
		(p2_tri1_vertex2 == 0 && p2_tri1_vertex3 == 0) ||
		(p2_tri1_vertex3 == 0 && p2_tri1_vertex1 == 0))
	{
		return 0;
	}

	if ((p2_tri1_vertex1 == 0 && p2_tri1_vertex2 * p2_tri1_vertex3 > 0) ||
		(p2_tri1_vertex2 == 0 && p2_tri1_vertex1 * p2_tri1_vertex3 > 0) ||
		p2_tri1_vertex3 == 0 && p2_tri1_vertex1 * p2_tri1_vertex2 > 0)
	{
		return 0;
	}

	double array[18];
	array[0]=tri1->Vertex_1.x();
	array[1]=tri1->Vertex_1.y();
	array[2]=tri1->Vertex_1.z();
	array[3]=tri1->Vertex_2.x();
	array[4]=tri1->Vertex_2.y();
	array[5]=tri1->Vertex_2.z();
	array[6]=tri1->Vertex_3.x();
	array[7]=tri1->Vertex_3.y();
	array[8]=tri1->Vertex_3.z();
	array[9]=tri2->Vertex_1.x();
	array[10]=tri2->Vertex_1.y();
	array[11]=tri2->Vertex_1.z();
	array[12]=tri2->Vertex_2.x();
	array[13]=tri2->Vertex_2.y();
	array[14]=tri2->Vertex_2.z();
	array[15]=tri2->Vertex_3.x();
	array[16]=tri2->Vertex_3.y();
	array[17]=tri2->Vertex_3.z();

	double* tri1_a = &array[0];
	double* tri1_b = &array[3];
	double* tri1_c = &array[6];
	double* tri2_a = &array[9];
	double* tri2_b = &array[12];
	double* tri2_c = &array[15];

	double* m;
	int im;

	if (p2_tri1_vertex2 * p2_tri1_vertex3 >= 0 && p2_tri1_vertex1 != 0 )  
	{  
		if ( p2_tri1_vertex1 < 0 )  
		{  
			m = tri2_b;  
			tri2_b = tri2_c;  
			tri2_c = m;  

			im = p1_tri2_vertex2;  
			p1_tri2_vertex2 = p1_tri2_vertex3;  
			p1_tri2_vertex3 = im;  
		}  
	}  
	else if ( p2_tri1_vertex1 * p2_tri1_vertex3 >= 0 && p2_tri1_vertex2 != 0 )  
	{  
		m = tri1_a;  
		tri1_a = tri1_b;  
		tri1_b = tri1_c;  
		tri1_c = m;  

		if ( p2_tri1_vertex2 < 0 )  
		{  
			m = tri2_b;  
			tri2_b = tri2_c;  
			tri2_c = m;  

			im = p1_tri2_vertex2;  
			p1_tri2_vertex2 = p1_tri2_vertex3;  
			p1_tri2_vertex3 = im;  
		}  
	}  
	else if ( p2_tri1_vertex1 * p2_tri1_vertex2 >= 0 && p2_tri1_vertex3 != 0 )  
	{  
		m = tri1_a;

		tri1_a = tri1_c;

		tri1_c = tri1_b;  

		tri1_b = m;  

		if ( p2_tri1_vertex3 < 0 )  
		{  
			m = tri2_b;  
			tri2_b = tri2_c;  
			tri2_c = m;  

			im = p1_tri2_vertex2;  
			p1_tri2_vertex2 = p1_tri2_vertex3;  
			p1_tri2_vertex3 = im;  
		}  
	}  

	if ( p1_tri2_vertex2 * p1_tri2_vertex3 >= 0 && p1_tri2_vertex1 != 0 )  
	{  
		if ( p1_tri2_vertex1 < 0 )  
		{  
			m = tri1_b;  
			tri1_b = tri1_c;  
			tri1_c = m;  
		}  
	}  
	else if ( p1_tri2_vertex1 * p1_tri2_vertex3 >= 0 && p1_tri2_vertex2 != 0 )  
	{  
		m = tri2_a;  

		tri2_a = tri2_b;  

		tri2_b = tri2_c;  

		tri2_c = m;  

		if ( p1_tri2_vertex2 < 0 )  
		{  
			m = tri1_b;  
			tri1_b = tri1_c;  
			tri1_c = m;  
		}  
	}  
	else if ( p1_tri2_vertex1 * p1_tri2_vertex2 >= 0 && p1_tri2_vertex3 != 0 )  
	{  
		m = tri2_a;

		tri2_a = tri2_c;

		tri2_c = tri2_b;

		tri2_b = m;

		if ( p1_tri2_vertex3 < 0 )
		{  
			m = tri1_b;  
			tri1_b = tri1_c;  
			tri1_c = m;  
		}
	}
	osg::Vec3 a1 = osg::Vec3(tri1_a[0], tri1_a[1], tri1_a[2]);
	osg::Vec3 b1 = osg::Vec3(tri1_b[0], tri1_b[1], tri1_b[2]);
	osg::Vec3 c1 = osg::Vec3(tri1_c[0], tri1_c[1], tri1_c[2]);
	osg::Vec3 a2 = osg::Vec3(tri2_a[0], tri2_a[1], tri2_a[2]);
	osg::Vec3 b2 = osg::Vec3(tri2_b[0], tri2_b[1], tri2_b[2]);
	osg::Vec3 c2 = osg::Vec3(tri2_c[0], tri2_c[1], tri2_c[2]);
	if ( get_vector4_det(a1, b1, a2, b2) < 0 && get_vector4_det( a1, c1, c2, a2) < 0 )  
	{
		return 1;//只相交
	}
	else
	{
		return 0;
	}
}

static int flag = 0;

MeshIntersect::MeshIntersect(const osg::ref_ptr<osg::Vec3Array>& pmesh1,
	const QString name0,
	const osg::ref_ptr<osg::Vec3Array>& pmesh2,
	const QString name1)
	: m_pArray0(pmesh1)
	, m_pArray1(pmesh2)
	, m_name0(name0)
	, m_name1(name1)
{
}

MeshIntersect::~MeshIntersect()
{
}

bool MeshIntersect::Calc()
{
	if (m_pArray0 ==NULL || m_pArray1 ==NULL)
		return false;

	//pMesh1的所有包围盒
	//std::vector<Triangle>& tris1 = m_pMesh1->m_vTriangles;
	size_t triSize1 = m_pArray0->size()/3;
	std::vector<Bound> vBounds1;
	vBounds1.resize(triSize1);
	Bound bound1;
	bound1 = Bound((*m_pArray0)[0],(*m_pArray0)[1],(*m_pArray0)[2]);
	vBounds1[0] = bound1;

	#pragma omp parallel for
	for (int i = 1; i<triSize1; ++i)
	{		
		Bound& boundTemp = Bound((*m_pArray0)[3*i],(*m_pArray0)[3*i+1],(*m_pArray0)[3*i+2]);

		if(boundTemp.xMax>bound1.xMax)
			bound1.xMax = boundTemp.xMax;

		if(boundTemp.xMin<bound1.xMin)
			bound1.xMin = boundTemp.xMin;

		if(boundTemp.yMax>bound1.yMax)
			bound1.yMax = boundTemp.yMax;

		if(boundTemp.yMin<bound1.yMin)
			bound1.yMin = boundTemp.yMin;

		if(boundTemp.zMax>bound1.zMax)
			bound1.zMax = boundTemp.zMax;

		if(boundTemp.zMin<bound1.zMin)
			bound1.zMin = boundTemp.zMin;

		vBounds1[i] = boundTemp;
	}

	//pMesh2的所有包围盒
	//std::vector<Triangle>& tris2 = m_pMesh2->m_vTriangles;
	size_t triSize2 = m_pArray1->size()/3;
	std::vector<Bound> vBounds2;
	vBounds2.resize(triSize2);

	Bound bound2;
	bound2 = Bound((*m_pArray1)[0],(*m_pArray1)[1],(*m_pArray1)[2]);
	vBounds2[0] = bound2;

	#pragma omp parallel for
	for (int i = 1; i<triSize2; ++i)
	{		
		Bound& boundTemp = Bound((*m_pArray1)[3*i],(*m_pArray1)[3*i+1],(*m_pArray1)[3*i+2]);

		if(boundTemp.xMax>bound2.xMax)
			bound2.xMax = boundTemp.xMax;

		if(boundTemp.xMin<bound2.xMin)
			bound2.xMin = boundTemp.xMin;

		if(boundTemp.yMax>bound2.yMax)
			bound2.yMax = boundTemp.yMax;

		if(boundTemp.yMin<bound2.yMin)
			bound2.yMin = boundTemp.yMin;

		if(boundTemp.zMax>bound2.zMax)
			bound2.zMax = boundTemp.zMax;

		if(boundTemp.zMin<bound2.zMin)
			bound2.zMin = boundTemp.zMin;

		vBounds2[i] = boundTemp;
	}

	if (!bound2.OverLap(bound1))
		return false;

	//m_pMesh1->CalcMeshNormal();
	//m_pMesh2->CalcMeshNormal();
	std::vector<osg::Vec3> normals1;
	normals1.resize(triSize1);

	#pragma omp parallel for
	for (int i = 0; i < triSize1;++i)
	{
		normals1[i] = CalcTriNormal((*m_pArray0)[3*i],(*m_pArray0)[3*i+1],(*m_pArray0)[3*i+2]);
	}

	std::vector<osg::Vec3> normals2;
	normals2.resize(triSize2);

	#pragma omp parallel for
	for (int i = 0; i < triSize2;++i)
	{
		normals2[i] = CalcTriNormal((*m_pArray1)[3*i],(*m_pArray1)[3*i+1],(*m_pArray1)[3*i+2]);
	}

	int num =0;
	std::string path("D:\\testdata\\");

	//#pragma omp parallel for
	for (int i = 0;i<triSize1;++i)
	{
		Triangle tri1(normals1[i],(*m_pArray0)[3*i],(*m_pArray0)[3*i+1],(*m_pArray0)[3*i+2]);
		for (int j = 0; j<triSize2; ++j)
		{
			if (!vBounds1[i].OverLap(vBounds2[j]))
				continue;

			Triangle tri2(normals2[j],(*m_pArray1)[3*j],(*m_pArray1)[3*j+1],(*m_pArray1)[3*j+2]);

			int result =IsTrianglesIntersection(&tri1,&tri2);

			//int result =isTriangleTntersect(&tri1,&tri2);
			if (result)
			{
				//std::string file0 = path+m_name0.toStdString();
				//file0.append(".stl");
				//SaveStlASCIINew(file0.c_str(),m_pArray0,normals1,false);

				//std::string file1 = path+m_name1.toStdString();
				//file1.append(".stl");
				//SaveStlASCIINew(file1.c_str(),m_pArray1,normals2,false);
				
				return true;
			}
		}
	}

	return false;
}