
#include "CreatePipe.h"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"

#include "NovaPoint3D.h"

#include <osg/LightModel>
#include <osg/Material>
#include "xObjectLoader.h"
#include <osgUtil/Optimizer>

#include "ExportStl.h"

// 管子半径缩小的量
const double Pipe_Shrink = 2.0;

PipeStraight::PipeStraight(double length,double radius,int step)
	: m_dLength(length)
	, m_dRadius(radius)
	, m_iStep(step)
{
	m_dRadius -= Pipe_Shrink;
	m_Points = new osg::Vec3Array();
}

PipeStraight::~PipeStraight()
{

}
// 判断是大端存储还是小端存储
static bool we_are_little_endian()
{
	int tmp = 1;
	return !!(* (unsigned char *) &tmp);
}

static inline void swap_32(unsigned char *p)
{
	* (unsigned *) p = _byteswap_ulong(* (unsigned *) p);
}

static inline void swap_int(int &x)
{
	swap_32((unsigned char *) &x);
}

static inline void swap_float(float &x)
{
	swap_32((unsigned char *) &x);
}
//bool PipeStraight::WriteBinarySTL(const osg::Vec3Array& vs , const osg::Vec3Array& ns, const std::string& sFileName)
//{
//	//if (sFileName.empty())
//	//	return false;
//
//	FILE *f = NULL;
//	errno_t eType = fopen_s(&f,sFileName.c_str(), "wb");
//	if (0!=eType)
//		return false;
//
//	bool need_swap = !we_are_little_endian();
//	char header[80];
//	memset(header, ' ', 80);
//	FWRITE(header, 80, 1, f);
//
//	int nfaces = vs.size()/3;
//	if (need_swap)
//		swap_int(nfaces);
//	FWRITE(&nfaces, 4, 1, f);
//
//	//if (m_vMeshNormal.size() < 1)
//	//	CalcMeshNormal();
//
//	unsigned int iSize = nfaces;
//	for (size_t i = 0; i < iSize; i++)
//	{
//		float fbuf[12];
//		fbuf[0] = (float)(ns)[3*i].x();
//		fbuf[1] = (float)(ns)[3*i].y();
//		fbuf[2] = (float)(ns)[3*i].z();
//		fbuf[3]  = (float)(vs)[3*i].x();
//		fbuf[4]  = (float)(vs)[3*i].y();
//		fbuf[5]  = (float)(vs)[3*i].z();
//		fbuf[6]  = (float)(vs)[3*i+1].x();
//		fbuf[7]  = (float)(vs)[3*i+1].y();
//		fbuf[8]  = (float)(vs)[3*i+1].z();
//		fbuf[9]  = (float)(vs)[3*i+2].x();
//		fbuf[10] = (float)(vs)[3*i+2].y();
//		fbuf[11] = (float)(vs)[3*i+2].z();
//		if (need_swap) 
//		{
//			for (int j = 0; j < 12; j++)
//				swap_float(fbuf[j]);
//		}
//		FWRITE(fbuf, 48, 1, f);
//		unsigned char att[2] = { 0, 0 };
//		FWRITE(att, 2, 1, f);
//	}
//	if (NULL!=f)
//		fclose(f);
//
//	return true;
//}


osg::Geode* PipeStraight::Create()
{
	osg::ref_ptr<osg::Geode> aGeode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> aGeometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> aPoints = new osg::Vec3Array();

	for (int i = 1; i <= m_iStep; ++i)
	{
		for (int j = 1; j <= ARC_STEP;++j)
		{
			double theta = 2.0*PI*(j-1)/ARC_STEP;
			aPoints->push_back(osg::Vec3(m_dRadius*cos(theta), m_dRadius*sin(theta), m_dLength*(i-1)/m_iStep));
			theta = 2.0*PI*j/ARC_STEP;
			aPoints->push_back(osg::Vec3(m_dRadius*cos(theta), m_dRadius*sin(theta), m_dLength*(i-1)/m_iStep));
			theta = 2.0*PI*(j-1)/ARC_STEP;
			aPoints->push_back(osg::Vec3(m_dRadius*cos(theta), m_dRadius*sin(theta), m_dLength*i/m_iStep));
			aPoints->push_back(osg::Vec3(m_dRadius*cos(theta), m_dRadius*sin(theta), m_dLength*i/m_iStep));
			theta = 2.0*PI*j/ARC_STEP;
			aPoints->push_back(osg::Vec3(m_dRadius*cos(theta), m_dRadius*sin(theta), m_dLength*(i-1)/m_iStep));
			aPoints->push_back(osg::Vec3(m_dRadius*cos(theta), m_dRadius*sin(theta), m_dLength*i/m_iStep));
		}
	}

	//aGeometry->setVertexArray(aUVPoints);
	aGeometry->setVertexArray(aPoints);
	aGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,aPoints->size()));

	aGeode->addDrawable(aGeometry);

	//?set?material?for?the?surface
	osg::ref_ptr<osg::StateSet>	aStateSet = aGeode->getOrCreateStateSet();
	osg::ref_ptr<osg::Material>	aMaterial = new osg::Material();

	aMaterial->setDiffuse(osg::Material::FRONT, osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	aMaterial->setSpecular(osg::Material::FRONT, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	aMaterial->setShininess(osg::Material::FRONT, 100.0f);

	aStateSet->setAttribute(aMaterial);

	return	aGeode.release();
}

osg::ref_ptr<osg::Geode> PipeStraight::Create(Vec3 pos,Vec3 nor)
{
	osg::ref_ptr<osg::Geode> aGeode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> aGeometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> aPoints = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> aNormals = new osg::Vec3Array();

	Point3D center(pos.x(),pos.y(),pos.z());

	Point3D dir(nor.x(),nor.y(),nor.z());
	Point3D z_axis(0.0,0.0,1.0);
	double delta;
	Point3D rotate;
	double matrix[16] = {1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0};

	Point3D tempDir = dir;
	tempDir.Normalize();
	VectorCross(tempDir,z_axis,rotate);

	delta = VectorDot(z_axis,tempDir);
	delta = acos(delta);
	rotate.Normalize();

	rotate.Normalize();
	double u = rotate.x();
	double v = rotate.y();
	double w = rotate.z();

	matrix[0] = cos(delta) + (u * u) * (1 - cos(delta));
	matrix[1] = u * v * (1 - cos(delta)) + w * sin(delta);
	matrix[2] = u * w * (1 - cos(delta)) - v * sin(delta);
	matrix[3] = center.x();

	matrix[4] = u * v * (1 - cos(delta)) - w * sin(delta);
	matrix[5] = cos(delta) + v * v * (1 - cos(delta));
	matrix[6] = w * v * (1 - cos(delta)) + u * sin(delta);
	matrix[7] = center.y();

	matrix[8] = u * w * (1 - cos(delta)) + v * sin(delta);
	matrix[9] = v * w * (1 - cos(delta)) - u * sin(delta);
	matrix[10] = cos(delta) + w * w * (1 - cos(delta));
	matrix[11] = center.z();

	for (int i = 1; i <= m_iStep; ++i)
	{
		for (int j = 1; j <= ARC_STEP;++j)
		{

			double theta = 2.0*PI*(j-1)/ARC_STEP;
			osg::Vec3 p0(matrix[0]*m_dRadius*cos(theta) + matrix[1]*m_dRadius*sin(theta) + matrix[2]*m_dLength*(i-1)/m_iStep  + matrix[3],
				matrix[4]*m_dRadius*cos(theta) + matrix[5]*m_dRadius*sin(theta) + matrix[6]*m_dLength*(i-1)/m_iStep  + matrix[7],
				matrix[8]*m_dRadius*cos(theta) + matrix[9]*m_dRadius*sin(theta) + matrix[10]*m_dLength*(i-1)/m_iStep + matrix[11]);
			theta = 2.0*PI*j/ARC_STEP;
			// aPoints->push_back(osg::Vec3(m_dRadius*cos(theta), m_dRadius*sin(theta), m_dLength*(i-1)/m_iStep));
			osg::Vec3 p1(matrix[0]*m_dRadius*cos(theta) + matrix[1]*m_dRadius*sin(theta) + matrix[2]*m_dLength*(i-1)/m_iStep  + matrix[3],
				matrix[4]*m_dRadius*cos(theta) + matrix[5]*m_dRadius*sin(theta) + matrix[6]*m_dLength*(i-1)/m_iStep  + matrix[7],
				matrix[8]*m_dRadius*cos(theta) + matrix[9]*m_dRadius*sin(theta) + matrix[10]*m_dLength*(i-1)/m_iStep + matrix[11]);
			theta = 2.0*PI*(j-1)/ARC_STEP;
			//aPoints->push_back(osg::Vec3(m_dRadius*cos(theta), m_dRadius*sin(theta), m_dLength*i/m_iStep));
			//aPoints->push_back(osg::Vec3(m_dRadius*cos(theta), m_dRadius*sin(theta), m_dLength*i/m_iStep));
			osg::Vec3 p2(matrix[0]*m_dRadius*cos(theta) + matrix[1]*m_dRadius*sin(theta) + matrix[2]*m_dLength*i/m_iStep  + matrix[3],
				matrix[4]*m_dRadius*cos(theta) + matrix[5]*m_dRadius*sin(theta) + matrix[6]*m_dLength*i/m_iStep  + matrix[7],
				matrix[8]*m_dRadius*cos(theta) + matrix[9]*m_dRadius*sin(theta) + matrix[10]*m_dLength*i/m_iStep + matrix[11]);

			osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
			aPoints->push_back(p0);
			aPoints->push_back(p1);
			aPoints->push_back(p2);
			aNormals->push_back(nor0);
			aNormals->push_back(nor0);
			aNormals->push_back(nor0);

			osg::Vec3 p3(matrix[0]*m_dRadius*cos(theta) + matrix[1]*m_dRadius*sin(theta) + matrix[2]*m_dLength*i/m_iStep  + matrix[3],
				matrix[4]*m_dRadius*cos(theta) + matrix[5]*m_dRadius*sin(theta) + matrix[6]*m_dLength*i/m_iStep  + matrix[7],
				matrix[8]*m_dRadius*cos(theta) + matrix[9]*m_dRadius*sin(theta) + matrix[10]*m_dLength*i/m_iStep + matrix[11]);
			theta = 2.0*PI*j/ARC_STEP;
			//aPoints->push_back(osg::Vec3(m_dRadius*cos(theta), m_dRadius*sin(theta), m_dLength*(i-1)/m_iStep));
			//aPoints->push_back(osg::Vec3(m_dRadius*cos(theta), m_dRadius*sin(theta), m_dLength*i/m_iStep));
			osg::Vec3 p4(matrix[0]*m_dRadius*cos(theta) + matrix[1]*m_dRadius*sin(theta) + matrix[2]*m_dLength*(i-1)/m_iStep  + matrix[3],
				matrix[4]*m_dRadius*cos(theta) + matrix[5]*m_dRadius*sin(theta) + matrix[6]*m_dLength*(i-1)/m_iStep  + matrix[7],
				matrix[8]*m_dRadius*cos(theta) + matrix[9]*m_dRadius*sin(theta) + matrix[10]*m_dLength*(i-1)/m_iStep + matrix[11]);

			osg::Vec3 p5(matrix[0]*m_dRadius*cos(theta) + matrix[1]*m_dRadius*sin(theta) + matrix[2]*m_dLength*i/m_iStep  + matrix[3],
				matrix[4]*m_dRadius*cos(theta) + matrix[5]*m_dRadius*sin(theta) + matrix[6]*m_dLength*i/m_iStep  + matrix[7],
				matrix[8]*m_dRadius*cos(theta) + matrix[9]*m_dRadius*sin(theta) + matrix[10]*m_dLength*i/m_iStep + matrix[11]);

			osg::Vec3 nor1 = CalcTriNormal(p3,p4,p5);
			aPoints->push_back(p3);
			aPoints->push_back(p4);
			aPoints->push_back(p5);
			aNormals->push_back(nor1);
			aNormals->push_back(nor1);
			aNormals->push_back(nor1);
		}

	}

	aGeometry->setVertexArray(aPoints);//将坐标设置到几何体节点中
	aGeometry->setNormalArray(aNormals, osg::Array::BIND_PER_VERTEX);//添加法线到几何体中

	osg::ref_ptr<osg::DrawArrays> Primitives1 = new osg::DrawArrays(GL_TRIANGLES, 0, aNormals->size());
	aGeometry->addPrimitiveSet(Primitives1.get());

	//关闭显示列表，开启顶点缓存对象
	aGeometry->setUseVertexBufferObjects(true);

	//定义一个几何组结点，并把几何体结点加入到几何组结点当中
	osg::ref_ptr<osg::Geode> geode=new osg::Geode();
	//geode->addDrawable(geom.get());

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHT1, osg::StateAttribute::ON);

	osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel; //用于选择光照模型（如全局光和双面光照）
	lightModel->setTwoSided(true);
	stateset->setAttributeAndModes(lightModel.get() );

	osg::ref_ptr<osg::Material> mat = new osg::Material;

	mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.3,0.3,0.3,1.0f));
	mat->setShininess(osg::Material::FRONT_AND_BACK, 100.0f );
	//2016.06.01. 通常，对环境光与漫反射光的反射程度决定了材料的颜色，并且它们应该值相同。
	stateset->setAttributeAndModes(mat.get(),osg::StateAttribute::ON);

	//stateset->setAttributeAndModes(mat,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
    //stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);  //一定要加上这句话，否则网格内部对象看不见

	aGeometry->setStateSet(stateset);
	geode->addDrawable(aGeometry.get());

	osgUtil::Optimizer optimizer;
	optimizer.optimize(aGeometry);
	optimizer.reset();

	return geode;
}

PipeArc::PipeArc(double rotateAngle,double angle,double radius0,double radius1,int step)
	: m_dRotateAngle(rotateAngle)
	, m_dAngle(angle)
	, m_dRadius0(radius0)  // 弯管中心到圆弧圆心距离
	, m_dRadius1(radius1)  // 弯管半径
	, m_iStep(step)
{
	m_dRadius1 -= Pipe_Shrink;
	m_Points = new osg::Vec3Array();
}
PipeArc::~PipeArc()
{

}
// 计算三角面片法向量
osg::Vec3 CalcTriNormal(const osg::Vec3& p0,const osg::Vec3& p1,const osg::Vec3& p2)
{
	Point3D p00(p0.x(),p0.y(),p0.z());
	Point3D p11(p1.x(),p1.y(),p1.z());
	Point3D p22(p2.x(),p2.y(),p2.z());
	Point3D norm;

	Point3D xy = p11 - p00;
	Point3D xz = p22 - p00;

	VectorCross(xy, xz, norm);
	if (norm.Magnitude() == 0.0)
		return osg::Vec3(0.0, 0.0, 0.0);

	norm.Normalize();

	return osg::Vec3(norm.x(),norm.y(),norm.z());
}

osg::ref_ptr<osg::Geode> PipeArc::Create(osg::Vec3 pos,osg::Vec3 nor)
{
	osg::ref_ptr<osg::Geode> aGeode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> aGeometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> aPoints = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> aNormals = new osg::Vec3Array();

	Point3D center(pos.x(),pos.y(),pos.z());

	Point3D dir(nor.x(),nor.y(),nor.z());
	Point3D z_axis(0.0,0.0,1.0);
	double delta;
	Point3D rotate;
	double matrix[16] = {1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0};

	Point3D tempDir = dir;
	tempDir.Normalize();
	VectorCross(tempDir,z_axis,rotate);
	// double length = sqrt(VectorDot(z_axis,tempDir));
	// delta = sqrt(VectorDot(z_axis,tempDir));
	delta = VectorDot(z_axis,tempDir);
	delta = acos(delta);
	rotate.Normalize();

	//rotate.Normalize();
	double u = rotate.x();
	double v = rotate.y();
	double w = rotate.z();

	matrix[0] = cos(delta) + (u * u) * (1 - cos(delta));
	matrix[1] = u * v * (1 - cos(delta)) + w * sin(delta);
	matrix[2] = u * w * (1 - cos(delta)) - v * sin(delta);
	matrix[3] = center.x();

	matrix[4] = u * v * (1 - cos(delta)) - w * sin(delta);
	matrix[5] = cos(delta) + v * v * (1 - cos(delta));
	matrix[6] = w * v * (1 - cos(delta)) + u * sin(delta);
	matrix[7] = center.y();

	matrix[8] = u * w * (1 - cos(delta)) + v * sin(delta);
	matrix[9] = v * w * (1 - cos(delta)) - u * sin(delta);
	matrix[10] = cos(delta) + w * w * (1 - cos(delta));
	matrix[11] = center.z();

	for (int i = 1; i <= m_iStep; ++i)
	{
		double fai0;
		double fai1;
		if(nor == osg::Vec3(0.0,1.0,0.0))
		{
			fai0 = m_dRotateAngle+m_dAngle*(i-1)/m_iStep;
			fai1 = m_dRotateAngle+m_dAngle*i/m_iStep;
		}
		else if(nor == osg::Vec3(0.0,-1.0,0.0))
		{
			fai0 = m_dRotateAngle+m_dAngle*(i-1)/m_iStep+PI;
			fai1 = m_dRotateAngle+m_dAngle*i/m_iStep+PI;
		}

		for (int j = 1; j <= ARC_STEP;++j)
		{
			double theta = 2.0*PI*(j-1)/ARC_STEP;
			osg::Vec3 p0((m_dRadius0+m_dRadius1*cos(theta))*cos(fai0)*matrix[0]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai0)*matrix[1]+ m_dRadius1*sin(theta)*matrix[2]+matrix[3],
				(m_dRadius0+m_dRadius1*cos(theta))*cos(fai0)*matrix[4]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai0)*matrix[5]+ m_dRadius1*sin(theta)*matrix[6]+matrix[7],
				(m_dRadius0+m_dRadius1*cos(theta))*cos(fai0)*matrix[8]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai0)*matrix[9]+ m_dRadius1*sin(theta)*matrix[10]+matrix[11]);

			theta = 2.0*PI*j/ARC_STEP;
			osg::Vec3 p1((m_dRadius0+m_dRadius1*cos(theta))*cos(fai0)*matrix[0]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai0)*matrix[1]+m_dRadius1*sin(theta)*matrix[2]+matrix[3],
				(m_dRadius0+m_dRadius1*cos(theta))*cos(fai0)*matrix[4]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai0)*matrix[5]+m_dRadius1*sin(theta)*matrix[6]+matrix[7],
				(m_dRadius0+m_dRadius1*cos(theta))*cos(fai0)*matrix[8]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai0)*matrix[9]+m_dRadius1*sin(theta)*matrix[10]+matrix[11]);

			theta = 2.0*PI*(j-1)/ARC_STEP;
			osg::Vec3 p2((m_dRadius0+m_dRadius1*cos(theta))*cos(fai1)*matrix[0]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai1)*matrix[1]+ m_dRadius1*sin(theta)*matrix[2]+matrix[3],
				(m_dRadius0+m_dRadius1*cos(theta))*cos(fai1)*matrix[4]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai1)*matrix[5]+ m_dRadius1*sin(theta)*matrix[6]+matrix[7],
				(m_dRadius0+m_dRadius1*cos(theta))*cos(fai1)*matrix[8]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai1)*matrix[9]+ m_dRadius1*sin(theta)*matrix[10]+matrix[11]);
			osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);

			aPoints->push_back(p0);
			aPoints->push_back(p1);
			aPoints->push_back(p2);
			aNormals->push_back(nor0);
			aNormals->push_back(nor0);
			aNormals->push_back(nor0);

			osg::Vec3 p3((m_dRadius0+m_dRadius1*cos(theta))*cos(fai1)*matrix[0]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai1)*matrix[1]+m_dRadius1*sin(theta)*matrix[2]+matrix[3],
				(m_dRadius0+m_dRadius1*cos(theta))*cos(fai1)*matrix[4]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai1)*matrix[5]+m_dRadius1*sin(theta)*matrix[6]+matrix[7],
				(m_dRadius0+m_dRadius1*cos(theta))*cos(fai1)*matrix[8]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai1)*matrix[9]+m_dRadius1*sin(theta)*matrix[10]+matrix[11]);
			theta = 2.0*PI*j/ARC_STEP;
			osg::Vec3 p4((m_dRadius0+m_dRadius1*cos(theta))*cos(fai0)*matrix[0]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai0)*matrix[1]+ m_dRadius1*sin(theta)*matrix[2]+matrix[3],
				(m_dRadius0+m_dRadius1*cos(theta))*cos(fai0)*matrix[4]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai0)*matrix[5]+ m_dRadius1*sin(theta)*matrix[6]+matrix[7],
				(m_dRadius0+m_dRadius1*cos(theta))*cos(fai0)*matrix[8]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai0)*matrix[9]+ m_dRadius1*sin(theta)*matrix[10]+matrix[11]);
			osg::Vec3 p5((m_dRadius0+m_dRadius1*cos(theta))*cos(fai1)*matrix[0]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai1)*matrix[1]+ m_dRadius1*sin(theta)*matrix[2] + matrix[3],
				(m_dRadius0+m_dRadius1*cos(theta))*cos(fai1)*matrix[4]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai1)*matrix[5]+ m_dRadius1*sin(theta)*matrix[6] + matrix[7],
				(m_dRadius0+m_dRadius1*cos(theta))*cos(fai1)*matrix[8]+(m_dRadius0+m_dRadius1*cos(theta))*sin(fai1)*matrix[9]+ m_dRadius1*sin(theta)*matrix[10] + matrix[11]);

			osg::Vec3 nor1 = CalcTriNormal(p3,p4,p5);
			aPoints->push_back(p3);
			aPoints->push_back(p4);
			aPoints->push_back(p5);
			aNormals->push_back(nor1);
			aNormals->push_back(nor1);
			aNormals->push_back(nor1);
		}
	}

	aGeometry->setVertexArray(aPoints);//将坐标设置到几何体节点中
	aGeometry->setNormalArray(aNormals, osg::Array::BIND_PER_VERTEX);//添加法线到几何体中

#if 0
    static int i = 0;
    QString name("D:\\testdata\\arc");
    name.append(QString::number(i));
    name.append(".stl");
    i++;
    SaveStlASCII((char*)name.toStdString().c_str(),aPoints,aNormals,false);

#endif

	osg::ref_ptr<osg::DrawArrays> Primitives1 = new osg::DrawArrays(GL_TRIANGLES, 0, aNormals->size());
	aGeometry->addPrimitiveSet(Primitives1.get());

	//关闭显示列表，开启顶点缓存对象
	aGeometry->setUseVertexBufferObjects(true);

	//定义一个几何组结点，并把几何体结点加入到几何组结点当中
	osg::ref_ptr<osg::Geode> geode=new osg::Geode();
	//geode->addDrawable(geom.get());

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHT1, osg::StateAttribute::ON);

	osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel; //用于选择光照模型（如全局光和双面光照）
	lightModel->setTwoSided(true);
	stateset->setAttributeAndModes(lightModel.get() );

	osg::ref_ptr<osg::Material> mat = new osg::Material;

	//2016.06.01. 通常，对环境光与漫反射光的反射程度决定了材料的颜色，并且它们应该值相同。
	mat->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	mat->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	mat->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(0.3,0.3,0.3,1.0f));
	mat->setShininess( osg::Material::FRONT_AND_BACK, 100.0f );
	stateset->setAttributeAndModes(mat.get(),osg::StateAttribute::ON);

	aGeometry->setStateSet(stateset);
	geode->addDrawable(aGeometry.get());

	osgUtil::Optimizer optimizer;
	optimizer.optimize(aGeometry);
	optimizer.reset();

	return geode;
}

osg::ref_ptr<osg::Group> PipeArc::CreateNew()
{
	osg::ref_ptr<osg::Geode> aGeode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> aGeometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> aPoints = new osg::Vec3Array();

	for (int i = 1; i <= m_iStep; ++i)
	{
		double fai0 = m_dAngle*(i-1)/m_iStep;
		double fai1 = m_dAngle*i/m_iStep;
		for (int j = 1; j <= ARC_STEP;++j)
		{
			double theta = 2.0*PI*(j-1)/ARC_STEP;
			aPoints->push_back(osg::Vec3((m_dRadius0+m_dRadius1*cos(theta))*cos(fai0), (m_dRadius0+m_dRadius1*cos(theta))*sin(fai0), m_dRadius1*sin(theta)));

			theta = 2.0*PI*j/ARC_STEP;
			aPoints->push_back(osg::Vec3((m_dRadius0+m_dRadius1*cos(theta))*cos(fai0), (m_dRadius0+m_dRadius1*cos(theta))*sin(fai0), m_dRadius1*sin(theta)));
			theta = 2.0*PI*(j-1)/ARC_STEP;

			aPoints->push_back(osg::Vec3((m_dRadius0+m_dRadius1*cos(theta))*cos(fai1), (m_dRadius0+m_dRadius1*cos(theta))*sin(fai1), m_dRadius1*sin(theta)));
			aPoints->push_back(osg::Vec3((m_dRadius0+m_dRadius1*cos(theta))*cos(fai1), (m_dRadius0+m_dRadius1*cos(theta))*sin(fai1), m_dRadius1*sin(theta)));
			theta = 2.0*PI*j/ARC_STEP;
			aPoints->push_back(osg::Vec3((m_dRadius0+m_dRadius1*cos(theta))*cos(fai0), (m_dRadius0+m_dRadius1*cos(theta))*sin(fai0), m_dRadius1*sin(theta)));
			aPoints->push_back(osg::Vec3((m_dRadius0+m_dRadius1*cos(theta))*cos(fai1), (m_dRadius0+m_dRadius1*cos(theta))*sin(fai1), m_dRadius1*sin(theta)));
		}
	}

	//aGeometry->setVertexArray(aUVPoints);
	aGeometry->setVertexArray(aPoints);
	aGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,aPoints->size()));

	aGeode->addDrawable(aGeometry);

	//?set?material?for?the?surface
	osg::ref_ptr<osg::StateSet>	aStateSet = aGeode->getOrCreateStateSet();
	aStateSet->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	aStateSet->setMode( GL_LIGHTING, osg::StateAttribute::ON);
	aStateSet->setMode( GL_LIGHT1, osg::StateAttribute::ON);

	osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel; //用于选择光照模型（如全局光和双面光照）
	lightModel->setTwoSided(true);
	aStateSet->setAttributeAndModes(lightModel.get() );

	osg::ref_ptr<osg::Material>	aMaterial = new osg::Material();

	aMaterial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	aMaterial->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	aMaterial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	aMaterial->setShininess(osg::Material::FRONT_AND_BACK, 100.0f);
	aStateSet->setAttributeAndModes(aMaterial.get(),osg::StateAttribute::ON);

	aStateSet->setAttribute(aMaterial);

	return	aGeode;
}

WheelModel::WheelModel(double hight0,double hight1,double modelRadius,double pipeRadius,
	double innerRadius,int step,double length,double wide,bool right,bool onlyLeft)
	: m_dHight0(hight0)
	, m_dHight1(hight1)
	, m_dRadius0(modelRadius)
	, m_dRadius1(pipeRadius)
	, m_dRadius2(innerRadius)
	, m_iStep(step)
	, m_dLength(length)
	, m_dWide(wide)
	, m_bRight(right)
	, m_bOnlyLeft(onlyLeft)
{
	m_dWide = 2.0*m_dRadius1;
}

WheelModel::~WheelModel()
{

}

osg::ref_ptr<osg::Geode> WheelModel::CreateRabbet(Vec3 pos,Vec3 nor)
{
	osg::ref_ptr<osg::Geode> aGeode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> aGeometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> aPoints = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> aNormals = new osg::Vec3Array();

	Point3D center(pos.x(),pos.y(),pos.z());

	Point3D dir(nor.x(),nor.y(),nor.z());
	Point3D z_axis(0.0,0.0,1.0);
	double delta;
	Point3D rotate;
	double matrix[16] = {1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0};

	Point3D tempDir = dir;
	tempDir.Normalize();
	VectorCross(tempDir,z_axis,rotate);
	delta = VectorDot(z_axis,tempDir);
	delta = acos(delta);
	rotate.Normalize();

	rotate.Normalize();
	double u = rotate.x();
	double v = rotate.y();
	double w = rotate.z();

	matrix[0] = cos(delta) + (u * u) * (1 - cos(delta));
	matrix[1] = u * v * (1 - cos(delta)) + w * sin(delta);
	matrix[2] = u * w * (1 - cos(delta)) - v * sin(delta);
	matrix[3] = center.x();

	matrix[4] = u * v * (1 - cos(delta)) - w * sin(delta);
	matrix[5] = cos(delta) + v * v * (1 - cos(delta));
	matrix[6] = w * v * (1 - cos(delta)) + u * sin(delta);
	matrix[7] = center.y();

	matrix[8] = u * w * (1 - cos(delta)) + v * sin(delta);
	matrix[9] = v * w * (1 - cos(delta)) - u * sin(delta);
	matrix[10] = cos(delta) + w * w * (1 - cos(delta));
	matrix[11] = center.z();

#if 1

	for (int i = 1; i <= m_iStep; ++i)
	{
		double fai0;
		double fai1;
		if(nor == osg::Vec3(0.0,1.0,0.0))
		{
			fai0 = 2.0*PI*(i-1)/m_iStep;
			fai1 = 2.0*PI*i/m_iStep;
		}
		else if(nor == osg::Vec3(0.0,-1.0,0.0))
		{
			fai0 = 2.0*PI*(i-1)/m_iStep+PI;
			fai1 = 2.0*PI*i/m_iStep+PI;
		}

		for (int j = 1; j <= ARC_STEP;++j)
		{
			double theta = PI*(j-1)/ARC_STEP+0.5*PI;
			osg::Vec3 p0((m_dRadius0+m_dRadius1*cos(theta))*cos(fai0),(m_dRadius0+m_dRadius1*cos(theta))*sin(fai0),m_dRadius1*sin(theta));

			theta = PI*j/ARC_STEP+0.5*PI;
			osg::Vec3 p1((m_dRadius0+m_dRadius1*cos(theta))*cos(fai0),(m_dRadius0+m_dRadius1*cos(theta))*sin(fai0),m_dRadius1*sin(theta));

			theta = PI*(j-1)/ARC_STEP+0.5*PI;
			osg::Vec3 p2((m_dRadius0+m_dRadius1*cos(theta))*cos(fai1),(m_dRadius0+m_dRadius1*cos(theta))*sin(fai1), m_dRadius1*sin(theta));

			osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
			aPoints->push_back(p0);
			aPoints->push_back(p1);
			aPoints->push_back(p2);
			aNormals->push_back(nor0);
			aNormals->push_back(nor0);
			aNormals->push_back(nor0);

			osg::Vec3 p3((m_dRadius0+m_dRadius1*cos(theta))*cos(fai1),(m_dRadius0+m_dRadius1*cos(theta))*sin(fai1),m_dRadius1*sin(theta));
			theta = PI*j/ARC_STEP+0.5*PI;
			osg::Vec3 p4((m_dRadius0+m_dRadius1*cos(theta))*cos(fai0),(m_dRadius0+m_dRadius1*cos(theta))*sin(fai0),m_dRadius1*sin(theta));
			osg::Vec3 p5((m_dRadius0+m_dRadius1*cos(theta))*cos(fai1),(m_dRadius0+m_dRadius1*cos(theta))*sin(fai1),m_dRadius1*sin(theta));

			osg::Vec3 nor1 = CalcTriNormal(p3,p4,p5);
			aPoints->push_back(p3);
			aPoints->push_back(p4);
			aPoints->push_back(p5);
			aNormals->push_back(nor1);
			aNormals->push_back(nor1);
			aNormals->push_back(nor1);
		}
	}

	CreateUpAndDownCircle(matrix,aPoints,aNormals);
	CreateMidCylinder(matrix,aPoints,aNormals);

#endif

	if (m_dLength > 0.0)
	{    
		if (m_bRight)
		{
			CreateOuterRight(matrix,aPoints,aNormals);
		}
		else
		{
			if (m_bOnlyLeft)
			{
				CreateOuterLeft(matrix,aPoints,aNormals);
			}
			else
			{
				CreateOuterLeftNew(matrix,aPoints,aNormals);
			}

		}

	}

	for (size_t i = 0; i < aPoints->size()/3;++i)
	{
		osg::Vec3 p0 = (*aPoints)[3*i];
		double x = p0[0]*matrix[0]+p0[1]*matrix[1]+ p0[2]*matrix[2] +matrix[3];
		double y = p0[0]*matrix[4]+p0[1]*matrix[5]+ p0[2]*matrix[6] + matrix[7];
		double z = p0[0]*matrix[8]+p0[1]*matrix[9]+ p0[2]*matrix[10] + matrix[11];

		(*aPoints)[3*i].set(x, y, z);
		osg::Vec3 p00(x,y,z);

		osg::Vec3 p1 = (*aPoints)[3*i+1];
		x = p1[0]*matrix[0]+p1[1]*matrix[1]+ p1[2]*matrix[2] +matrix[3];
		y = p1[0]*matrix[4]+p1[1]*matrix[5]+ p1[2]*matrix[6] + matrix[7];
		z = p1[0]*matrix[8]+p1[1]*matrix[9]+ p1[2]*matrix[10] + matrix[11];

		(*aPoints)[3*i+1].set(x, y, z);
		osg::Vec3 p11(x,y,z);

		osg::Vec3 p2 = (*aPoints)[3*i+2];
		x = p2[0]*matrix[0]+p2[1]*matrix[1]+ p2[2]*matrix[2] +matrix[3];
		y = p2[0]*matrix[4]+p2[1]*matrix[5]+ p2[2]*matrix[6] + matrix[7];
		z = p2[0]*matrix[8]+p2[1]*matrix[9]+ p2[2]*matrix[10] + matrix[11];

		(*aPoints)[3*i+2].set(x, y, z);
		osg::Vec3 p22(x,y,z);

		osg::Vec3 nor = CalcTriNormal(p00,p11,p22);
		(*aNormals)[3*i].set(nor[0], nor[1], nor[2]);
		(*aNormals)[3*i+1].set(nor[0], nor[1], nor[2]);
		(*aNormals)[3*i+2].set(nor[0], nor[1], nor[2]);
	}

	aGeometry->setVertexArray(aPoints);//将坐标设置到几何体节点中
	aGeometry->setNormalArray(aNormals, osg::Array::BIND_PER_VERTEX);//添加法线到几何体中

	osg::ref_ptr<osg::DrawArrays> Primitives1 = new osg::DrawArrays(GL_TRIANGLES, 0, aNormals->size());
	aGeometry->addPrimitiveSet(Primitives1.get());

	//关闭显示列表，开启顶点缓存对象
	aGeometry->setUseVertexBufferObjects(true);

	//定义一个几何组结点，并把几何体结点加入到几何组结点当中
	osg::ref_ptr<osg::Geode> geode=new osg::Geode();
	//geode->addDrawable(geom.get());

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHT1, osg::StateAttribute::ON);

	osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel; //用于选择光照模型（如全局光和双面光照）
	lightModel->setTwoSided(true);
	stateset->setAttributeAndModes(lightModel.get() );

	osg::ref_ptr<osg::Material> mat = new osg::Material;

	//2016.06.01. 通常，对环境光与漫反射光的反射程度决定了材料的颜色，并且它们应该值相同。
	mat->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	mat->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	mat->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(0.3,0.3,0.3,1.0f));
	mat->setShininess( osg::Material::FRONT_AND_BACK, 100.0f );
	stateset->setAttributeAndModes(mat.get(),osg::StateAttribute::ON);

	m_pMaterial = mat;

	aGeometry->setStateSet(stateset);
	geode->addDrawable(aGeometry.get());

	osgUtil::Optimizer optimizer;
	optimizer.optimize(aGeometry);
	optimizer.reset();

	return geode;
}

// 创建上下圆环
void WheelModel::CreateUpAndDownCircle(double* matrix,osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals)
{
	// 创建上面圆环,3°圆弧对应一个三角形遍
	double delta = 6.0;
	int size = 360.0/delta;
	for (int i = 1;i <= size;++i)
	{
		double theta = 2.0*PI*(i-1)/size;
		osg::Vec3 p0(m_dRadius2*cos(theta),m_dRadius2*sin(theta),m_dHight0);
		osg::Vec3 p1(m_dRadius0*cos(theta),m_dRadius0*sin(theta),m_dHight0);

		theta = 2.0*PI*i/size;
		osg::Vec3 p2(m_dRadius0*cos(theta),m_dRadius0*sin(theta),m_dHight0);
		osg::Vec3 p3(m_dRadius2*cos(theta),m_dRadius2*sin(theta),m_dHight0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p3,p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aPoints->push_back(p0);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 创建下半圆周
	for (int i = 1;i <= size;++i)
	{
		double theta = 2.0*PI*(i-1)/size;
		osg::Vec3 p0(m_dRadius2*cos(theta),m_dRadius2*sin(theta),-m_dHight1);
		osg::Vec3 p1(m_dRadius0*cos(theta),m_dRadius0*sin(theta),-m_dHight1);

		theta = 2.0*PI*i/size;
		osg::Vec3 p2(m_dRadius0*cos(theta),m_dRadius0*sin(theta),-m_dHight1);
		osg::Vec3 p3(m_dRadius2*cos(theta),m_dRadius2*sin(theta),-m_dHight1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p0);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

}

// 创建中心圆柱及上下侧与凹槽链接的部分
void WheelModel::CreateMidCylinder(double* matrix,osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals)
{
	double delta = 6.0;
	int size = 360.0/delta;
	for (int i = 1;i <= size;++i)
	{
		double theta = 2.0*PI*(i-1)/size;
		osg::Vec3 p0(m_dRadius2*cos(theta),m_dRadius2*sin(theta),m_dHight0);
		osg::Vec3 p1(m_dRadius2*cos(theta),m_dRadius2*sin(theta),-m_dHight1);

		theta = 2.0*PI*i/size;
		osg::Vec3 p2(m_dRadius2*cos(theta),m_dRadius2*sin(theta),-m_dHight1);
		osg::Vec3 p3(m_dRadius2*cos(theta),m_dRadius2*sin(theta),m_dHight0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p0);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 上侧与凹槽链接的部分
	for (int i = 1;i <= size;++i)
	{
		double theta = 2.0*PI*(i-1)/size;
		osg::Vec3 p0(m_dRadius0*cos(theta),m_dRadius0*sin(theta),m_dHight0);
		osg::Vec3 p1(m_dRadius0*cos(theta),m_dRadius0*sin(theta),m_dRadius1);

		theta = 2.0*PI*i/size;
		osg::Vec3 p2(m_dRadius0*cos(theta),m_dRadius0*sin(theta),m_dRadius1);
		osg::Vec3 p3(m_dRadius0*cos(theta),m_dRadius0*sin(theta),m_dHight0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p0);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 下侧与凹槽链接的部分
	for (int i = 1;i <= size;++i)
	{
		double theta = 2.0*PI*(i-1)/size;
		osg::Vec3 p0(m_dRadius0*cos(theta),m_dRadius0*sin(theta),-m_dRadius1);
		osg::Vec3 p1(m_dRadius0*cos(theta),m_dRadius0*sin(theta),-m_dHight1);

		theta = 2.0*PI*i/size;
		osg::Vec3 p2(m_dRadius0*cos(theta),m_dRadius0*sin(theta),-m_dHight1);
		osg::Vec3 p3(m_dRadius0*cos(theta),m_dRadius0*sin(theta),-m_dRadius1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p0);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}
}

void WheelModel::CreateOuterRight(double* matrix,osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals)
{
	// 创建夹子上表面
	int size = 10;
	for (int i = 1;i <= size;++i)
	{
		double x0 = m_dRadius0 - m_dWide + m_dWide*(i-1)/size;
		double x1 = m_dRadius0 - m_dWide + m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,-m_dLength,m_dHight0);
		osg::Vec3 p1(x1,-m_dLength,m_dHight0);

		// 右侧两点
		osg::Vec3 p2(x0,0.0,m_dHight0);
		osg::Vec3 p3(x1,0.0,m_dHight0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 夹子下表面
	//leftUp(-m_dLength,-m_dRadius0+m_dWide,-m_dHight1); // 左上角点坐标
	//Vec3 leftDown(-m_dLength,-m_dRadius0,-m_dHight1);  //左下角坐标
	//Vec3 rightUp(0.0,-m_dRadius0+m_dWide,-m_dHight1);    // 右上角坐标
	//Vec3 rightDown(0.0,-m_dRadius0,-m_dHight1);  // 右下角坐标
	for (int i = 1;i <= size;++i)
	{
		double x0 = m_dRadius0 - m_dWide + m_dWide*(i-1)/size;
		double x1 = m_dRadius0 - m_dWide + m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,-m_dLength,-m_dHight1);
		osg::Vec3 p1(x1,-m_dLength,-m_dHight1);

		// 右侧两点
		osg::Vec3 p2(x0,0.0,-m_dHight1);
		osg::Vec3 p3(x1,0.0,-m_dHight1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 背面z
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = m_dHight0 - (m_dHight0 + m_dHight1)*(i-1)/size;
		double z1 = m_dHight0 - (m_dHight0 + m_dHight1)*i/size;
		// 左侧两点
		osg::Vec3 p0(m_dRadius0-m_dWide,-m_dLength,z0);
		osg::Vec3 p1(m_dRadius0-m_dWide,-m_dLength,z1);

		// 右侧两点
		osg::Vec3 p2(m_dRadius0-m_dWide,0.0,z0);
		osg::Vec3 p3(m_dRadius0-m_dWide,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = m_dHight0 - (m_dHight0-m_dRadius1)*(i-1)/size;
		double z1 = m_dHight0 - (m_dHight0-m_dRadius1)*i/size;
		// 左侧两点
		osg::Vec3 p0(m_dRadius0-m_dWide,-m_dLength,z0);
		osg::Vec3 p1(m_dRadius0-m_dWide,-m_dLength,z1);

		// 右侧两点
		osg::Vec3 p2(m_dRadius0,-m_dLength,z0);
		osg::Vec3 p3(m_dRadius0,-m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}


	// 左侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = -m_dRadius1 - (m_dHight1-m_dRadius1)*(i-1)/size;
		double z1 = -m_dRadius1 - (m_dHight1-m_dRadius1)*i/size;
		// 左侧两点
		osg::Vec3 p0(m_dRadius0-m_dWide,-m_dLength,z0);
		osg::Vec3 p1(m_dRadius0-m_dWide,-m_dLength,z1);

		// 右侧两点
		osg::Vec3 p2(m_dRadius0,-m_dLength,z0);
		osg::Vec3 p3(m_dRadius0,-m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double z0 = m_dRadius1*cos(theta);
		double x0 = m_dRadius0 - m_dRadius1*sin(theta);

		theta = PI*i/size;
		double z1 = m_dRadius1*cos(theta);
		double x1 = m_dRadius0 - m_dRadius1*sin(theta);
		// 前侧两点
		osg::Vec3 p0(x0,-m_dLength,z0);
		osg::Vec3 p1(x1,-m_dLength,z1);

		// 后侧两点
		osg::Vec3 p2(m_dRadius0 - m_dWide,-m_dLength,z0);
		osg::Vec3 p3(m_dRadius0 - m_dWide,-m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = m_dHight0 - (m_dHight0-m_dRadius1)*(i-1)/size;
		double z1 = m_dHight0 - (m_dHight0-m_dRadius1)*i/size;
		// 左侧两点
		osg::Vec3 p0(m_dRadius0-m_dWide,0.0,z0);
		osg::Vec3 p1(m_dRadius0-m_dWide,0.0,z1);

		// 右侧两点
		osg::Vec3 p2(m_dRadius0,0.0,z0);
		osg::Vec3 p3(m_dRadius0,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = -m_dRadius1 - (m_dHight1-m_dRadius1)*(i-1)/size;
		double z1 = -m_dRadius1 - (m_dHight1-m_dRadius1)*i/size;
		// 左侧两点
		osg::Vec3 p0(m_dRadius0-m_dWide,0.0,z0);
		osg::Vec3 p1(m_dRadius0-m_dWide,0.0,z1);

		// 右侧两点
		osg::Vec3 p2(m_dRadius0,0.0,z0);
		osg::Vec3 p3(m_dRadius0,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double z0 = m_dRadius1*cos(theta);
		double x0 = m_dRadius0 - m_dRadius1*sin(theta);

		theta = PI*i/size;
		double z1 = m_dRadius1*cos(theta);
		double x1 = m_dRadius0 - m_dRadius1*sin(theta);
		// 前侧两点
		osg::Vec3 p0(x0,0.0,z0);
		osg::Vec3 p1(x1,0.0,z1);

		// 后侧两点
		osg::Vec3 p2(m_dRadius0 - m_dWide,0.0,z0);
		osg::Vec3 p3(m_dRadius0 - m_dWide,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方没有圆弧的部分上半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = m_dHight0 - (m_dHight0-m_dRadius1)*(i-1)/size;
		double z1 = m_dHight0 - (m_dHight0-m_dRadius1)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(m_dRadius0,-m_dLength,z0);
		osg::Vec3 p1(m_dRadius0,-m_dLength,z1);
		// 前面右侧两点
		osg::Vec3 p2(m_dRadius0,0.0,z0);
		osg::Vec3 p3(m_dRadius0,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方没有圆弧的部分下半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = -m_dRadius1 - (m_dHight1-m_dRadius1)*(i-1)/size;
		double z1 = -m_dRadius1 - (m_dHight1-m_dRadius1)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(m_dRadius0,-m_dLength,z0);
		osg::Vec3 p1(m_dRadius0,-m_dLength,z1);
		// 前面右侧两点
		osg::Vec3 p2(m_dRadius0,0.0,z0);
		osg::Vec3 p3(m_dRadius0,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double z0 = m_dRadius1*cos(theta);
		double x0 = m_dRadius0 - m_dRadius1*sin(theta);

		theta = PI*i/size;
		double z1 = m_dRadius1*cos(theta);
		double x1 = m_dRadius0 - m_dRadius1*sin(theta);

		// 前面左侧两点
		osg::Vec3 p0(x0,-m_dLength,z0);
		osg::Vec3 p1(x1,-m_dLength,z1);
		// 前面右侧两点
		osg::Vec3 p2(x0,0.0,z0);
		osg::Vec3 p3(x1,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

}

// 用于左边加工的轮模，轮模开口朝右，初始位置在右边，但是在里侧，使用时机头转180度，就转到左侧
void WheelModel::CreateOuterLeftNew(double* matrix,osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals)
{
	// 创建夹子上表面
	int size = 10;
	for (int i = 1;i <= size;++i)
	{
		double x0 = m_dRadius0 - m_dWide + m_dWide*(i-1)/size;
		double x1 = m_dRadius0 - m_dWide + m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,0.0,m_dHight0);
		osg::Vec3 p1(x1,0.0,m_dHight0);

		// 右侧两点
		osg::Vec3 p2(x0,m_dLength,m_dHight0);
		osg::Vec3 p3(x1,m_dLength,m_dHight0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 夹子下表面
	//leftUp(-m_dLength,-m_dRadius0+m_dWide,-m_dHight1); // 左上角点坐标
	//Vec3 leftDown(-m_dLength,-m_dRadius0,-m_dHight1);  //左下角坐标
	//Vec3 rightUp(0.0,-m_dRadius0+m_dWide,-m_dHight1);    // 右上角坐标
	//Vec3 rightDown(0.0,-m_dRadius0,-m_dHight1);  // 右下角坐标
	for (int i = 1;i <= size;++i)
	{
		double x0 = m_dRadius0 - m_dWide + m_dWide*(i-1)/size;
		double x1 = m_dRadius0 - m_dWide + m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,0.0,-m_dHight1);
		osg::Vec3 p1(x1,0.0,-m_dHight1);

		// 右侧两点
		osg::Vec3 p2(x0,m_dLength,-m_dHight1);
		osg::Vec3 p3(x1,m_dLength,-m_dHight1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 背面z
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = m_dHight0 - (m_dHight0 + m_dHight1)*(i-1)/size;
		double z1 = m_dHight0 - (m_dHight0 + m_dHight1)*i/size;
		//// 左侧两点
		osg::Vec3 p0(m_dRadius0-m_dWide,0.0,z0);
		osg::Vec3 p1(m_dRadius0-m_dWide,0.0,z1);

		// 右侧两点
		osg::Vec3 p2(m_dRadius0-m_dWide,m_dLength,z0);
		osg::Vec3 p3(m_dRadius0-m_dWide,m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = m_dHight0 - (m_dHight0-m_dRadius1)*(i-1)/size;
		double z1 = m_dHight0 - (m_dHight0-m_dRadius1)*i/size;
		// 左侧两点
		osg::Vec3 p0(m_dRadius0-m_dWide,0.0,z0);
		osg::Vec3 p1(m_dRadius0-m_dWide,0.0,z1);

		// 右侧两点
		osg::Vec3 p2(m_dRadius0,0.0,z0);
		osg::Vec3 p3(m_dRadius0,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}


	// 左侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = -m_dRadius1 - (m_dHight1-m_dRadius1)*(i-1)/size;
		double z1 = -m_dRadius1 - (m_dHight1-m_dRadius1)*i/size;
		// 左侧两点
		osg::Vec3 p0(m_dRadius0-m_dWide,0.0,z0);
		osg::Vec3 p1(m_dRadius0-m_dWide,0.0,z1);

		// 右侧两点
		osg::Vec3 p2(m_dRadius0,0.0,z0);
		osg::Vec3 p3(m_dRadius0,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double z0 = m_dRadius1*cos(theta);
		double x0 = m_dRadius0 - m_dRadius1*sin(theta);

		theta = PI*i/size;
		double z1 = m_dRadius1*cos(theta);
		double x1 = m_dRadius0 - m_dRadius1*sin(theta);
		// 前侧两点
		osg::Vec3 p0(x0,0.0,z0);
		osg::Vec3 p1(x1,0.0,z1);

		// 后侧两点
		osg::Vec3 p2(m_dRadius0 - m_dWide,0.0,z0);
		osg::Vec3 p3(m_dRadius0 - m_dWide,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = m_dHight0 - (m_dHight0-m_dRadius1)*(i-1)/size;
		double z1 = m_dHight0 - (m_dHight0-m_dRadius1)*i/size;
		// 左侧两点
		osg::Vec3 p0(m_dRadius0-m_dWide,m_dLength,z0);
		osg::Vec3 p1(m_dRadius0-m_dWide,m_dLength,z1);

		// 右侧两点
		osg::Vec3 p2(m_dRadius0,m_dLength,z0);
		osg::Vec3 p3(m_dRadius0,m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = -m_dRadius1 - (m_dHight1-m_dRadius1)*(i-1)/size;
		double z1 = -m_dRadius1 - (m_dHight1-m_dRadius1)*i/size;
		// 左侧两点
		osg::Vec3 p0(m_dRadius0-m_dWide,m_dLength,z0);
		osg::Vec3 p1(m_dRadius0-m_dWide,m_dLength,z1);

		// 右侧两点
		osg::Vec3 p2(m_dRadius0,m_dLength,z0);
		osg::Vec3 p3(m_dRadius0,m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double z0 = m_dRadius1*cos(theta);
		double x0 = m_dRadius0 - m_dRadius1*sin(theta);

		theta = PI*i/size;
		double z1 = m_dRadius1*cos(theta);
		double x1 = m_dRadius0 - m_dRadius1*sin(theta);
		// 前侧两点
		osg::Vec3 p0(x0,m_dLength,z0);
		osg::Vec3 p1(x1,m_dLength,z1);

		// 后侧两点
		osg::Vec3 p2(m_dRadius0 - m_dWide,m_dLength,z0);
		osg::Vec3 p3(m_dRadius0 - m_dWide,m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方没有圆弧的部分上半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = m_dHight0 - (m_dHight0-m_dRadius1)*(i-1)/size;
		double z1 = m_dHight0 - (m_dHight0-m_dRadius1)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(m_dRadius0,0.0,z0);
		osg::Vec3 p1(m_dRadius0,0.0,z1);
		// 前面右侧两点
		osg::Vec3 p2(m_dRadius0,m_dLength,z0);
		osg::Vec3 p3(m_dRadius0,m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方没有圆弧的部分下半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = -m_dRadius1 - (m_dHight1-m_dRadius1)*(i-1)/size;
		double z1 = -m_dRadius1 - (m_dHight1-m_dRadius1)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(m_dRadius0,0.0,z0);
		osg::Vec3 p1(m_dRadius0,0.0,z1);
		// 前面右侧两点
		osg::Vec3 p2(m_dRadius0,m_dLength,z0);
		osg::Vec3 p3(m_dRadius0,m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double z0 = m_dRadius1*cos(theta);
		double x0 = m_dRadius0 - m_dRadius1*sin(theta);

		theta = PI*i/size;
		double z1 = m_dRadius1*cos(theta);
		double x1 = m_dRadius0 - m_dRadius1*sin(theta);

		// 前面左侧两点
		osg::Vec3 p0(x0,0.0,z0);
		osg::Vec3 p1(x1,0.0,z1);
		// 前面右侧两点
		osg::Vec3 p2(x0,m_dLength,z0);
		osg::Vec3 p3(x1,m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

}

void WheelModel::CreateOuterLeft(double* matrix,osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals)
{
	// 创建夹子上表面
	int size = 10;
	for (int i = 1;i <= size;++i)
	{
		double x0 = -m_dRadius0 + m_dWide - m_dWide*(i-1)/size;
		double x1 = -m_dRadius0 + m_dWide - m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,-m_dLength,m_dHight0);
		osg::Vec3 p1(x1,-m_dLength,m_dHight0);

		// 右侧两点
		osg::Vec3 p2(x0,0.0,m_dHight0);
		osg::Vec3 p3(x1,0.0,m_dHight0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 夹子下表面
	//leftUp(-m_dLength,-m_dRadius0+m_dWide,-m_dHight1); // 左上角点坐标
	//Vec3 leftDown(-m_dLength,-m_dRadius0,-m_dHight1);  //左下角坐标
	//Vec3 rightUp(0.0,-m_dRadius0+m_dWide,-m_dHight1);    // 右上角坐标
	//Vec3 rightDown(0.0,-m_dRadius0,-m_dHight1);  // 右下角坐标
	for (int i = 1;i <= size;++i)
	{
		double x0 = -m_dRadius0 + m_dWide - m_dWide*(i-1)/size;
		double x1 = -m_dRadius0 + m_dWide - m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,-m_dLength,-m_dHight1);
		osg::Vec3 p1(x1,-m_dLength,-m_dHight1);

		// 右侧两点
		osg::Vec3 p2(x0,0.0,-m_dHight1);
		osg::Vec3 p3(x1,0.0,-m_dHight1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 背面z
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = m_dHight0 - (m_dHight0 + m_dHight1)*(i-1)/size;
		double z1 = m_dHight0 - (m_dHight0 + m_dHight1)*i/size;
		// 左侧两点
		osg::Vec3 p0(-m_dRadius0+m_dWide,-m_dLength,z0);
		osg::Vec3 p1(-m_dRadius0+m_dWide,-m_dLength,z1);

		// 右侧两点
		osg::Vec3 p2(-m_dRadius0+m_dWide,0.0,z0);
		osg::Vec3 p3(-m_dRadius0+m_dWide,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = m_dHight0 - (m_dHight0-m_dRadius1)*(i-1)/size;
		double z1 = m_dHight0 - (m_dHight0-m_dRadius1)*i/size;
		// 左侧两点
		osg::Vec3 p0(-m_dRadius0+m_dWide,-m_dLength,z0);
		osg::Vec3 p1(-m_dRadius0+m_dWide,-m_dLength,z1);

		// 右侧两点
		osg::Vec3 p2(-m_dRadius0,-m_dLength,z0);
		osg::Vec3 p3(-m_dRadius0,-m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}


	// 左侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = -m_dRadius1 - (m_dHight1-m_dRadius1)*(i-1)/size;
		double z1 = -m_dRadius1 - (m_dHight1-m_dRadius1)*i/size;
		// 左侧两点
		osg::Vec3 p0(-m_dRadius0+m_dWide,-m_dLength,z0);
		osg::Vec3 p1(-m_dRadius0+m_dWide,-m_dLength,z1);

		// 右侧两点
		osg::Vec3 p2(-m_dRadius0,-m_dLength,z0);
		osg::Vec3 p3(-m_dRadius0,-m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}


	// 左侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double z0 = m_dRadius1*cos(theta);
		double x0 = -m_dRadius0 + m_dRadius1*sin(theta);

		theta = PI*i/size;
		double z1 = m_dRadius1*cos(theta);
		double x1 = -m_dRadius0 + m_dRadius1*sin(theta);
		// 前侧两点
		osg::Vec3 p0(x0,-m_dLength,z0);
		osg::Vec3 p1(x1,-m_dLength,z1);

		// 后侧两点
		//osg::Vec3 p2(m_dRadius0 - m_dWide,-m_dLength,z0);
		//osg::Vec3 p3(m_dRadius0 - m_dWide,-m_dLength,z1);
		osg::Vec3 p2(-m_dRadius0 + m_dWide,-m_dLength,z0);
		osg::Vec3 p3(-m_dRadius0 + m_dWide,-m_dLength,z1);

		osg::Vec3 nor0 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = m_dHight0 - (m_dHight0-m_dRadius1)*(i-1)/size;
		double z1 = m_dHight0 - (m_dHight0-m_dRadius1)*i/size;
		// 左侧两点
		osg::Vec3 p0(-m_dRadius0+m_dWide,0.0,z0);
		osg::Vec3 p1(-m_dRadius0+m_dWide,0.0,z1);

		// 右侧两点
		osg::Vec3 p2(-m_dRadius0,0.0,z0);
		osg::Vec3 p3(-m_dRadius0,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = -m_dRadius1 - (m_dHight1-m_dRadius1)*(i-1)/size;
		double z1 = -m_dRadius1 - (m_dHight1-m_dRadius1)*i/size;
		// 左侧两点
		osg::Vec3 p0(-m_dRadius0+m_dWide,0.0,z0);
		osg::Vec3 p1(-m_dRadius0+m_dWide,0.0,z1);

		// 右侧两点
		osg::Vec3 p2(-m_dRadius0,0.0,z0);
		osg::Vec3 p3(-m_dRadius0,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double z0 = m_dRadius1*cos(theta);
		double x0 = -m_dRadius0 + m_dRadius1*sin(theta);

		theta = PI*i/size;
		double z1 = m_dRadius1*cos(theta);
		double x1 = -m_dRadius0 + m_dRadius1*sin(theta);
		// 前侧两点
		osg::Vec3 p0(x0,0.0,z0);
		osg::Vec3 p1(x1,0.0,z1);

		// 后侧两点
		osg::Vec3 p2(-m_dRadius0 + m_dWide,0.0,z0);
		osg::Vec3 p3(-m_dRadius0 + m_dWide,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方没有圆弧的部分上半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = m_dHight0 - (m_dHight0-m_dRadius1)*(i-1)/size;
		double z1 = m_dHight0 - (m_dHight0-m_dRadius1)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(-m_dRadius0,-m_dLength,z0);
		osg::Vec3 p1(-m_dRadius0,-m_dLength,z1);
		// 前面右侧两点
		osg::Vec3 p2(-m_dRadius0,0.0,z0);
		osg::Vec3 p3(-m_dRadius0,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方没有圆弧的部分下半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double z0 = -m_dRadius1 - (m_dHight1-m_dRadius1)*(i-1)/size;
		double z1 = -m_dRadius1 - (m_dHight1-m_dRadius1)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(-m_dRadius0,-m_dLength,z0);
		osg::Vec3 p1(-m_dRadius0,-m_dLength,z1);
		// 前面右侧两点
		osg::Vec3 p2(-m_dRadius0,0.0,z0);
		osg::Vec3 p3(-m_dRadius0,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double z0 = m_dRadius1*cos(theta);
		double x0 = -m_dRadius0 + m_dRadius1*sin(theta);

		theta = PI*i/size;
		double z1 = m_dRadius1*cos(theta);
		double x1 = -m_dRadius0 + m_dRadius1*sin(theta);

		// 前面左侧两点
		osg::Vec3 p0(x0,-m_dLength,z0);
		osg::Vec3 p1(x1,-m_dLength,z1);
		// 前面右侧两点
		osg::Vec3 p2(x0,0.0,z0);
		osg::Vec3 p3(x1,0.0,z1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}
}

// 创建凸块
void WheelModel::CreateOuter(double* matrix,osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals)
{
	// 创建凸出块上表面
	//osg::Vec3 leftUp(-m_dLength,-m_dRadius0+m_dWide,m_dHight0); // 左上角点坐标
	//osg::Vec3 leftDown(-m_dLength,-m_dRadius0,m_dHight0);  //左下角坐标
	//osg::Vec3 rightUp(0.0,-m_dRadius0+m_dWide,m_dHight0);    // 右上角坐标
	//osg::Vec3 rightDown(0.0,-m_dRadius0,m_dHight0);  // 右下角坐标
	int size = 10;
	for (int i = 1;i <= size;++i)
	{
		double hight = -m_dRadius0+m_dWide-m_dWide*(i-1)/size;
		osg::Vec3 p0(-m_dLength,hight,m_dHight0);
		osg::Vec3 p1(0.0,hight,m_dHight0);

		hight = -m_dRadius0+m_dWide-m_dWide*i/size;
		osg::Vec3 p2(0.0,hight,m_dHight0);
		osg::Vec3 p3(-m_dLength,hight,m_dHight0);

		osg::Vec3 nor0 = CalcTriNormal(p3,p2,p1);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p0,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);

	}

	// 凸出块下表面
	//leftUp(-m_dLength,-m_dRadius0+m_dWide,-m_dHight1); // 左上角点坐标
	//Vec3 leftDown(-m_dLength,-m_dRadius0,-m_dHight1);  //左下角坐标
	//Vec3 rightUp(0.0,-m_dRadius0+m_dWide,-m_dHight1);    // 右上角坐标
	//Vec3 rightDown(0.0,-m_dRadius0,-m_dHight1);  // 右下角坐标
	for (int i = 1;i <= size;++i)
	{
		double hight = -m_dRadius0+m_dWide-m_dWide*(i-1)/size;
		osg::Vec3 p0(-m_dLength,hight,-m_dHight1);
		osg::Vec3 p1(0.0,hight,-m_dHight1);

		hight = -m_dRadius0+m_dWide-m_dWide*i/size;
		osg::Vec3 p2(0.0,hight,-m_dHight1);
		osg::Vec3 p3(-m_dLength,hight,-m_dHight1);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p3,p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aPoints->push_back(p0);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 背面z
	for (int i = 1;i <= size;++i)
	{
		double z = m_dHight0-(m_dHight0+m_dHight1)*(i-1)/size;
		osg::Vec3 p0(-m_dLength,-m_dRadius0+m_dWide,z);
		osg::Vec3 p1(0.0,-m_dRadius0+m_dWide,z);

		z = m_dHight0-(m_dHight0+m_dHight1)*i/size;
		osg::Vec3 p2(0.0,-m_dRadius0+m_dWide,z);
		osg::Vec3 p3(-m_dLength,-m_dRadius0+m_dWide,z);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p3,p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aPoints->push_back(p0);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		double x = -m_dLength;
		double theta = PI*(i-1)/size-0.5*PI;
		osg::Vec3 center(-m_dLength,-m_dRadius0,0.0);
		double y = -m_dRadius0+m_dRadius1*cos(theta);
		double z = m_dRadius2*sin(theta);

		osg::Vec3 p0(x,y,z);
		osg::Vec3 p1(x,-m_dRadius0+m_dWide,z);

		theta = PI*i/size-0.5*PI;
		y = -m_dRadius0+m_dRadius1*cos(theta);
		z = m_dRadius2*sin(theta);
		osg::Vec3 p2(x,-m_dRadius0+m_dWide,z);
		osg::Vec3 p3(x,y,z);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p3,p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aPoints->push_back(p0);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧
	for (int i = 1;i <= size;++i)
	{
		double x = 0.0;
		double theta = PI*(i-1)/size-0.5*PI;
		osg::Vec3 center(-m_dLength,-m_dRadius0,0.0);
		double y = -m_dRadius0+m_dRadius1*cos(theta);
		double z = m_dRadius2*sin(theta);

		osg::Vec3 p0(x,y,z);
		osg::Vec3 p1(x,-m_dRadius0+m_dWide,z);

		theta = PI*i/size-0.5*PI;
		y = -m_dRadius0+m_dRadius1*cos(theta);
		z = m_dRadius2*sin(theta);
		osg::Vec3 p2(x,-m_dRadius0+m_dWide,z);
		osg::Vec3 p3(x,y,z);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 上侧垂下的部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		double z = m_dHight0 - (i-1)*(m_dHight0-m_dRadius1)/size;
		osg::Vec3 p0(-m_dLength,-m_dRadius0,z);
		osg::Vec3 p1(0.0,-m_dRadius0,z);

		z = m_dHight0 - i*(m_dHight0-m_dRadius1)/size;
		osg::Vec3 p2(0.0,-m_dRadius0,z);
		osg::Vec3 p3(-m_dLength,-m_dRadius0,z);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	for (int i = 1;i <= size;++i)
	{
		double z = -m_dRadius1 - (i-1)*(m_dHight1-m_dRadius1)/size;
		osg::Vec3 p0(-m_dLength,-m_dRadius0,z);
		osg::Vec3 p1(0.0,-m_dRadius0,z);

		z = z = -m_dRadius1 - i*(m_dHight1-m_dRadius1)/size;
		osg::Vec3 p2(0.0,-m_dRadius0,z);
		osg::Vec3 p3(-m_dLength,-m_dRadius0,z);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧垂下部分
	for (int i = 1;i <= size;++i)
	{
		double z = m_dHight0 - (i-1)*(m_dHight0-m_dRadius1)/size;
		osg::Vec3 p0(-m_dLength,-m_dRadius0+m_dWide,z);
		osg::Vec3 p1(-m_dLength,-m_dRadius0,z);

		z = m_dHight0 - i*(m_dHight0-m_dRadius1)/size;
		osg::Vec3 p2(-m_dLength,-m_dRadius0,z);
		osg::Vec3 p3(-m_dLength,-m_dRadius0+m_dWide,z);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	for (int i = 1;i <= size;++i)
	{
		double z = -m_dRadius1 - (i-1)*(m_dHight1-m_dRadius1)/size;
		osg::Vec3 p0(-m_dLength,-m_dRadius0+m_dWide,z);
		osg::Vec3 p1(-m_dLength,-m_dRadius0,z);

		z = -m_dRadius1 - i*(m_dHight1-m_dRadius1)/size;
		osg::Vec3 p2(-m_dLength,-m_dRadius0,z);
		osg::Vec3 p3(-m_dLength,-m_dRadius0+m_dWide,z);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧下垂
	for (int i = 1;i <= size;++i)
	{
		double z = m_dHight0 - (i-1)*(m_dHight0-m_dRadius1)/size;
		osg::Vec3 p0(0.0,-m_dRadius0+m_dWide,z);
		osg::Vec3 p1(0.0,-m_dRadius0,z);

		z = m_dHight0 - i*(m_dHight0-m_dRadius1)/size;
		osg::Vec3 p2(0.0,-m_dRadius0,z);
		osg::Vec3 p3(0.0,-m_dRadius0+m_dWide,z);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	for (int i = 1;i <= size;++i)
	{
		double z = -m_dRadius1 - (i-1)*(m_dHight1-m_dRadius1)/size;
		osg::Vec3 p0(0.0,-m_dRadius0+m_dRadius1,z);
		osg::Vec3 p1(0.0,-m_dRadius0,z);

		z = -m_dRadius1 - i*(m_dHight1-m_dRadius1)/size;
		osg::Vec3 p2(0.0,-m_dRadius0,z);
		osg::Vec3 p3(0.0,-m_dRadius0+m_dRadius1,z);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 中间段凹槽
	for (int i = 1;i <= size;++i)
	{
		double x = 0.0;
		double theta = PI*(i-1)/size-0.5*PI;
		double y = -m_dRadius0+m_dRadius1*cos(theta);
		double z = m_dRadius2*sin(theta);

		osg::Vec3 p0(x,y,z);
		osg::Vec3 p1(-m_dLength,y,z);

		theta = PI*i/size-0.5*PI;
		y = -m_dRadius0+m_dRadius1*cos(theta);
		z = m_dRadius2*sin(theta);
		osg::Vec3 p2(-m_dLength,y,z);
		osg::Vec3 p3(x,y,z);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}
}


Clip::Clip(double hight0,double hight1,double radius,int step,double length,double wide,bool left,bool isAid)
	: m_dHight0(hight0)
	, m_dHight1(hight1)
	, m_dRadius(radius)               
	, m_iStep(step)
	, m_dLength(length)
	, m_dWide(wide)
	, m_bLeft(left)
    , m_bAid(isAid)  // 是否是辅夹
{
    //m_bAid = false;
}

Clip::~Clip()
{

}

// 创建夹子模型模型
// 以中间凹槽中心线为Z方向，水平向右为正方向
// 垂直向里为x正方向，竖直向上为Y方向
// 凹槽最左端中心为坐标原点
osg::ref_ptr<osg::Geode> Clip::Create(Vec3 pos,Vec3 nor)
{

	osg::ref_ptr<osg::Geode> aGeode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> aGeometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> aPoints = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> aNormals = new osg::Vec3Array();

	Point3D center(pos.x(),pos.y(),pos.z());

	Point3D dir(nor.x(),nor.y(),nor.z());
	Point3D z_axis(0.0,0.0,1.0);
	double delta;
	Point3D rotate;
	double matrix[16] = {1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0};

	Point3D tempDir = dir;
	tempDir.Normalize();
	VectorCross(tempDir,z_axis,rotate);
	delta = VectorDot(z_axis,tempDir);
	delta = acos(delta);
	rotate.Normalize();

	rotate.Normalize();
	double u = rotate.x();
	double v = rotate.y();
	double w = rotate.z();

	matrix[0] = cos(delta) + (u * u) * (1 - cos(delta));
	matrix[1] = u * v * (1 - cos(delta)) + w * sin(delta);
	matrix[2] = u * w * (1 - cos(delta)) - v * sin(delta);
	matrix[3] = center.x();

	matrix[4] = u * v * (1 - cos(delta)) - w * sin(delta);
	matrix[5] = cos(delta) + v * v * (1 - cos(delta));
	matrix[6] = w * v * (1 - cos(delta)) + u * sin(delta);
	matrix[7] = center.y();

	matrix[8] = u * w * (1 - cos(delta)) + v * sin(delta);
	matrix[9] = v * w * (1 - cos(delta)) - u * sin(delta);
	matrix[10] = cos(delta) + w * w * (1 - cos(delta));
	matrix[11] = center.z();

	if (!m_bAid && !m_bLeft)
	{
        // 主夹，开口朝左
		MouthToLeft(aPoints,aNormals);
	}
	else if(!m_bAid && m_bLeft)
	{
        // 主夹，开口朝右
		MouthToRight(aPoints,aNormals);
	}
    else if (m_bAid && m_bLeft)
    {
        // 辅夹，开口朝右
        AidMouthToRight(aPoints,aNormals);
    }
    else if (m_bAid && !m_bLeft)
    {
        // 辅夹，开口朝左
        AidMouthToLeft(aPoints,aNormals);
    }

	for (size_t i = 0; i < aPoints->size()/3;++i)
	{
		osg::Vec3 p0 = (*aPoints)[3*i];
		double x = p0[0]*matrix[0]+p0[1]*matrix[1]+ p0[2]*matrix[2] +matrix[3];
		double y = p0[0]*matrix[4]+p0[1]*matrix[5]+ p0[2]*matrix[6] + matrix[7];
		double z = p0[0]*matrix[8]+p0[1]*matrix[9]+ p0[2]*matrix[10] + matrix[11];

		(*aPoints)[3*i].set(x, y, z);
		osg::Vec3 p00(x,y,z);

		osg::Vec3 p1 = (*aPoints)[3*i+1];
		x = p1[0]*matrix[0]+p1[1]*matrix[1]+ p1[2]*matrix[2] +matrix[3];
		y = p1[0]*matrix[4]+p1[1]*matrix[5]+ p1[2]*matrix[6] + matrix[7];
		z = p1[0]*matrix[8]+p1[1]*matrix[9]+ p1[2]*matrix[10] + matrix[11];

		(*aPoints)[3*i+1].set(x, y, z);
		osg::Vec3 p11(x,y,z);

		osg::Vec3 p2 = (*aPoints)[3*i+2];
		x = p2[0]*matrix[0]+p2[1]*matrix[1]+ p2[2]*matrix[2] +matrix[3];
		y = p2[0]*matrix[4]+p2[1]*matrix[5]+ p2[2]*matrix[6] + matrix[7];
		z = p2[0]*matrix[8]+p2[1]*matrix[9]+ p2[2]*matrix[10] + matrix[11];

		(*aPoints)[3*i+2].set(x, y, z);
		osg::Vec3 p22(x,y,z);

		osg::Vec3 nor = CalcTriNormal(p00,p11,p22);
		(*aNormals)[3*i].set(nor[0], nor[1], nor[2]);
		(*aNormals)[3*i+1].set(nor[0], nor[1], nor[2]);
		(*aNormals)[3*i+2].set(nor[0], nor[1], nor[2]);
	}

	aGeometry->setVertexArray(aPoints);//将坐标设置到几何体节点中
	aGeometry->setNormalArray(aNormals, osg::Array::BIND_PER_VERTEX);//添加法线到几何体中

	osg::ref_ptr<osg::DrawArrays> Primitives1 = new osg::DrawArrays(GL_TRIANGLES, 0, aNormals->size());
	aGeometry->addPrimitiveSet(Primitives1.get());

	//关闭显示列表，开启顶点缓存对象
	aGeometry->setUseVertexBufferObjects(true);

	//定义一个几何组结点，并把几何体结点加入到几何组结点当中
	osg::ref_ptr<osg::Geode> geode=new osg::Geode();
	//geode->addDrawable(geom.get());

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHT1, osg::StateAttribute::ON);

	osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel; //用于选择光照模型（如全局光和双面光照）
	lightModel->setTwoSided(true);
	stateset->setAttributeAndModes(lightModel.get() );

	osg::ref_ptr<osg::Material> mat = new osg::Material;

	//2016.06.01. 通常，对环境光与漫反射光的反射程度决定了材料的颜色，并且它们应该值相同。
	mat->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	mat->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	mat->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(0.3,0.3,0.3,1.0f));
	mat->setShininess( osg::Material::FRONT_AND_BACK, 100.0f );
	stateset->setAttributeAndModes(mat.get(),osg::StateAttribute::ON);

	m_pMaterial = mat;

	aGeometry->setStateSet(stateset);
	geode->addDrawable(aGeometry.get());

	osgUtil::Optimizer optimizer;
	optimizer.optimize(aGeometry);
	optimizer.reset();

	return geode;
}


void Clip::MouthToLeft(osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals)
{
	// 创建夹子上表面
	int size = 10;
	for (int i = 1;i <= size;++i)
	{
		double x0 = m_dWide*(i-1)/size;
		double x1 = m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,m_dHight0,0.0);
		osg::Vec3 p1(x1,m_dHight0,0.0);

		// 右侧两点
		osg::Vec3 p2(x0,m_dHight0,m_dLength);
		osg::Vec3 p3(x1,m_dHight0,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 夹子下表面
	//leftUp(-m_dLength,-m_dRadius0+m_dWide,-m_dHight1); // 左上角点坐标
	//Vec3 leftDown(-m_dLength,-m_dRadius0,-m_dHight1);  //左下角坐标
	//Vec3 rightUp(0.0,-m_dRadius0+m_dWide,-m_dHight1);    // 右上角坐标
	//Vec3 rightDown(0.0,-m_dRadius0,-m_dHight1);  // 右下角坐标
	for (int i = 1;i <= size;++i)
	{
		double x0 = m_dWide*(i-1)/size;
		double x1 = m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,-m_dHight1,0.0);
		osg::Vec3 p1(x1,-m_dHight1,0.0);

		// 右侧两点
		osg::Vec3 p2(x0,-m_dHight1,m_dLength);
		osg::Vec3 p3(x1,-m_dHight1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 背面z
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0 + m_dHight1)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0 + m_dHight1)*i/size;
		// 左侧两点
		osg::Vec3 p0(m_dWide,y0,0.0);
		osg::Vec3 p1(m_dWide,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,m_dLength);
		osg::Vec3 p3(m_dWide,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0-m_dRadius)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,0.0);
		osg::Vec3 p3(m_dWide,y1,0.0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = -m_dRadius - (m_dHight1-m_dRadius)*(i-1)/size;
		double y1 = -m_dRadius - (m_dHight1-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,0.0);
		osg::Vec3 p3(m_dWide,y1,0.0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double x0 = m_dRadius*sin(theta);
		double y0 = m_dRadius*cos(theta);

		theta = PI*i/size;
		double x1 = m_dRadius*sin(theta);
		double y1 = m_dRadius*cos(theta);
		// 左侧两点
		osg::Vec3 p0(x0,y0,0.0);
		osg::Vec3 p1(x1,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,0.0);
		osg::Vec3 p3(m_dWide,y1,0.0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0-m_dRadius)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,m_dLength);
		osg::Vec3 p1(0.0,y1,m_dLength);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,m_dLength);
		osg::Vec3 p3(m_dWide,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = -m_dRadius - (m_dHight1-m_dRadius)*(i-1)/size;
		double y1 = -m_dRadius - (m_dHight1-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,m_dLength);
		osg::Vec3 p1(0.0,y1,m_dLength);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,m_dLength);
		osg::Vec3 p3(m_dWide,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double x0 = m_dRadius*sin(theta);
		double y0 = m_dRadius*cos(theta);

		theta = PI*i/size;
		double x1 = m_dRadius*sin(theta);
		double y1 = m_dRadius*cos(theta);
		// 左侧两点
		osg::Vec3 p0(x0,y0,m_dLength);
		osg::Vec3 p1(x1,y1,m_dLength);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,m_dLength);
		osg::Vec3 p3(m_dWide,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方没有圆弧的部分上半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0-m_dRadius)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0-m_dRadius)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);
		// 前面右侧两点
		osg::Vec3 p2(0.0,y0,m_dLength);
		osg::Vec3 p3(0.0,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方没有圆弧的部分下半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = -m_dRadius - (m_dHight1-m_dRadius)*(i-1)/size;
		double y1 = -m_dRadius - (m_dHight1-m_dRadius)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);
		// 前面右侧两点
		osg::Vec3 p2(0.0,y0,m_dLength);
		osg::Vec3 p3(0.0,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double x0 = m_dRadius*sin(theta);
		double y0 = m_dRadius*cos(theta);

		theta = PI*i/size;
		double x1 = m_dRadius*sin(theta);
		double y1 = m_dRadius*cos(theta);

		// 前面左侧两点
		osg::Vec3 p0(x0,y0,0.0);
		osg::Vec3 p1(x1,y1,0.0);
		// 前面右侧两点
		osg::Vec3 p2(x0,y0,m_dLength);
		osg::Vec3 p3(x1,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}
}

//凹槽朝右
void Clip::MouthToRight(osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals)
{
	// 创建夹子上表面
	int size = 10;
	for (int i = 1;i <= size;++i)
	{
		double x0 = m_dWide*(i-1)/size;
		double x1 = m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,m_dHight0,0.0);
		osg::Vec3 p1(x1,m_dHight0,0.0);

		// 右侧两点
		osg::Vec3 p2(x0,m_dHight0,m_dLength);
		osg::Vec3 p3(x1,m_dHight0,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 夹子下表面
	//leftUp(-m_dLength,-m_dRadius0+m_dWide,-m_dHight1); // 左上角点坐标
	//Vec3 leftDown(-m_dLength,-m_dRadius0,-m_dHight1);  //左下角坐标
	//Vec3 rightUp(0.0,-m_dRadius0+m_dWide,-m_dHight1);    // 右上角坐标
	//Vec3 rightDown(0.0,-m_dRadius0,-m_dHight1);  // 右下角坐标
	for (int i = 1;i <= size;++i)
	{
		double x0 = m_dWide*(i-1)/size;
		double x1 = m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,-m_dHight1,0.0);
		osg::Vec3 p1(x1,-m_dHight1,0.0);

		// 右侧两点
		osg::Vec3 p2(x0,-m_dHight1,m_dLength);
		osg::Vec3 p3(x1,-m_dHight1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 背面z
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0 + m_dHight1)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0 + m_dHight1)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(0.0,y0,m_dLength);
		osg::Vec3 p3(0.0,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0-m_dRadius)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,0.0);
		osg::Vec3 p3(m_dWide,y1,0.0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = -m_dRadius - (m_dHight1-m_dRadius)*(i-1)/size;
		double y1 = -m_dRadius - (m_dHight1-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,0.0);
		osg::Vec3 p3(m_dWide,y1,0.0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double x0 = m_dWide - m_dRadius*sin(theta);
		double y0 = m_dRadius*cos(theta);

		theta = PI*i/size;
		double x1 = m_dWide-m_dRadius*sin(theta);
		double y1 = m_dRadius*cos(theta);
		// 左侧两点
		osg::Vec3 p0(x0,y0,0.0);
		osg::Vec3 p1(x1,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(0.0,y0,0.0);
		osg::Vec3 p3(0.0,y1,0.0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0-m_dRadius)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,m_dLength);
		osg::Vec3 p1(0.0,y1,m_dLength);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,m_dLength);
		osg::Vec3 p3(m_dWide,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = -m_dRadius - (m_dHight1-m_dRadius)*(i-1)/size;
		double y1 = -m_dRadius - (m_dHight1-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,m_dLength);
		osg::Vec3 p1(0.0,y1,m_dLength);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,m_dLength);
		osg::Vec3 p3(m_dWide,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double x0 = m_dWide - m_dRadius*sin(theta);
		double y0 = m_dRadius*cos(theta);

		theta = PI*i/size;
		double x1 = m_dWide - m_dRadius*sin(theta);
		double y1 = m_dRadius*cos(theta);
		// 左侧两点
		osg::Vec3 p0(x0,y0,m_dLength);
		osg::Vec3 p1(x1,y1,m_dLength);

		// 右侧两点
		osg::Vec3 p2(0.0,y0,m_dLength);
		osg::Vec3 p3(0.0,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方没有圆弧的部分上半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0-m_dRadius)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0-m_dRadius)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(m_dWide,y0,0.0);
		osg::Vec3 p1(m_dWide,y1,0.0);
		// 前面右侧两点
		osg::Vec3 p2(m_dWide,y0,m_dLength);
		osg::Vec3 p3(m_dWide,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方没有圆弧的部分下半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = -m_dRadius - (m_dHight1-m_dRadius)*(i-1)/size;
		double y1 = -m_dRadius - (m_dHight1-m_dRadius)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(m_dWide,y0,0.0);
		osg::Vec3 p1(m_dWide,y1,0.0);
		// 前面右侧两点
		osg::Vec3 p2(m_dWide,y0,m_dLength);
		osg::Vec3 p3(m_dWide,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double x0 = m_dWide - m_dRadius*sin(theta);
		double y0 = m_dRadius*cos(theta);

		theta = PI*i/size;
		double x1 = m_dWide - m_dRadius*sin(theta);
		double y1 = m_dRadius*cos(theta);

		// 前面左侧两点
		osg::Vec3 p0(x0,y0,0.0);
		osg::Vec3 p1(x1,y1,0.0);
		// 前面右侧两点
		osg::Vec3 p2(x0,y0,m_dLength);
		osg::Vec3 p3(x1,y1,m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}
}


// 辅夹凹槽朝左
void Clip::AidMouthToLeft(osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals)
{
    	// 创建夹子上表面
	int size = 10;
	for (int i = 1;i <= size;++i)
	{
		double x0 = m_dWide*(i-1)/size;
		double x1 = m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,m_dHight0,0.0);
		osg::Vec3 p1(x1,m_dHight0,0.0);

		// 右侧两点
		osg::Vec3 p2(x0,m_dHight0,-m_dLength);
		osg::Vec3 p3(x1,m_dHight0,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 夹子下表面
	//leftUp(-m_dLength,-m_dRadius0+m_dWide,-m_dHight1); // 左上角点坐标
	//Vec3 leftDown(-m_dLength,-m_dRadius0,-m_dHight1);  //左下角坐标
	//Vec3 rightUp(0.0,-m_dRadius0+m_dWide,-m_dHight1);    // 右上角坐标
	//Vec3 rightDown(0.0,-m_dRadius0,-m_dHight1);  // 右下角坐标
	for (int i = 1;i <= size;++i)
	{
		double x0 = m_dWide*(i-1)/size;
		double x1 = m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,-m_dHight1,0.0);
		osg::Vec3 p1(x1,-m_dHight1,0.0);

		// 右侧两点
		osg::Vec3 p2(x0,-m_dHight1,-m_dLength);
		osg::Vec3 p3(x1,-m_dHight1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 背面z
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0 + m_dHight1)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0 + m_dHight1)*i/size;
		// 左侧两点
		osg::Vec3 p0(m_dWide,y0,0.0);
		osg::Vec3 p1(m_dWide,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,-m_dLength);
		osg::Vec3 p3(m_dWide,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0-m_dRadius)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,0.0);
		osg::Vec3 p3(m_dWide,y1,0.0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = -m_dRadius - (m_dHight1-m_dRadius)*(i-1)/size;
		double y1 = -m_dRadius - (m_dHight1-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,0.0);
		osg::Vec3 p3(m_dWide,y1,0.0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double x0 = m_dRadius*sin(theta);
		double y0 = m_dRadius*cos(theta);

		theta = PI*i/size;
		double x1 = m_dRadius*sin(theta);
		double y1 = m_dRadius*cos(theta);
		// 左侧两点
		osg::Vec3 p0(x0,y0,0.0);
		osg::Vec3 p1(x1,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,0.0);
		osg::Vec3 p3(m_dWide,y1,0.0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 后侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0-m_dRadius)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,-m_dLength);
		osg::Vec3 p1(0.0,y1,-m_dLength);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,-m_dLength);
		osg::Vec3 p3(m_dWide,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 后侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = -m_dRadius - (m_dHight1-m_dRadius)*(i-1)/size;
		double y1 = -m_dRadius - (m_dHight1-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,-m_dLength);
		osg::Vec3 p1(0.0,y1,-m_dLength);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,-m_dLength);
		osg::Vec3 p3(m_dWide,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 后侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double x0 = m_dRadius*sin(theta);
		double y0 = m_dRadius*cos(theta);

		theta = PI*i/size;
		double x1 = m_dRadius*sin(theta);
		double y1 = m_dRadius*cos(theta);
		// 左侧两点
		osg::Vec3 p0(x0,y0,-m_dLength);
		osg::Vec3 p1(x1,y1,-m_dLength);

		// 右侧两点
		osg::Vec3 p2(m_dWide,y0,-m_dLength);
		osg::Vec3 p3(m_dWide,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧上方没有圆弧的部分上半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0-m_dRadius)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0-m_dRadius)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);
		// 前面右侧两点
		osg::Vec3 p2(0.0,y0,-m_dLength);
		osg::Vec3 p3(0.0,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方没有圆弧的部分下半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = -m_dRadius - (m_dHight1-m_dRadius)*(i-1)/size;
		double y1 = -m_dRadius - (m_dHight1-m_dRadius)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);
		// 前面右侧两点
		osg::Vec3 p2(0.0,y0,-m_dLength);
		osg::Vec3 p3(0.0,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double x0 = m_dRadius*sin(theta);
		double y0 = m_dRadius*cos(theta);

		theta = PI*i/size;
		double x1 = m_dRadius*sin(theta);
		double y1 = m_dRadius*cos(theta);

		// 前面左侧两点
		osg::Vec3 p0(x0,y0,0.0);
		osg::Vec3 p1(x1,y1,0.0);
		// 前面右侧两点
		osg::Vec3 p2(x0,y0,-m_dLength);
		osg::Vec3 p3(x1,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}
}

// 辅夹凹槽朝右
void Clip::AidMouthToRight(osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals)
{
    // 创建夹子上表面
	int size = 10;
	for (int i = 1;i <= size;++i)
	{
		double x0 = -m_dWide*(i-1)/size;
		double x1 = -m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,m_dHight0,0.0);
		osg::Vec3 p1(x1,m_dHight0,0.0);

		// 右侧两点
		osg::Vec3 p2(x0,m_dHight0,-m_dLength);
		osg::Vec3 p3(x1,m_dHight0,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 夹子下表面
	//leftUp(-m_dLength,-m_dRadius0+m_dWide,-m_dHight1); // 左上角点坐标
	//Vec3 leftDown(-m_dLength,-m_dRadius0,-m_dHight1);  //左下角坐标
	//Vec3 rightUp(0.0,-m_dRadius0+m_dWide,-m_dHight1);    // 右上角坐标
	//Vec3 rightDown(0.0,-m_dRadius0,-m_dHight1);  // 右下角坐标
	for (int i = 1;i <= size;++i)
	{
		double x0 = -m_dWide*(i-1)/size;
		double x1 = -m_dWide*i/size;
		// 左侧两点
		osg::Vec3 p0(x0,-m_dHight1,0.0);
		osg::Vec3 p1(x1,-m_dHight1,0.0);

		// 右侧两点
		osg::Vec3 p2(x0,-m_dHight1,-m_dLength);
		osg::Vec3 p3(x1,-m_dHight1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 背面z
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0 + m_dHight1)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0 + m_dHight1)*i/size;
		// 左侧两点
		osg::Vec3 p0(-m_dWide,y0,0.0);
		osg::Vec3 p1(-m_dWide,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(-m_dWide,y0,-m_dLength);
		osg::Vec3 p3(-m_dWide,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p1,p2,p3);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前面竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0-m_dRadius)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(-m_dWide,y0,0.0);
		osg::Vec3 p3(-m_dWide,y1,0.0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前面竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = -m_dRadius - (m_dHight1-m_dRadius)*(i-1)/size;
		double y1 = -m_dRadius - (m_dHight1-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(-m_dWide,y0,0.0);
		osg::Vec3 p3(-m_dWide,y1,0.0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double x0 = -m_dRadius*sin(theta);
		double y0 = m_dRadius*cos(theta);

		theta = PI*i/size;
		double x1 = -m_dRadius*sin(theta);
		double y1 = m_dRadius*cos(theta);
		// 左侧两点
		osg::Vec3 p0(x0,y0,0.0);
		osg::Vec3 p1(x1,y1,0.0);

		// 右侧两点
		osg::Vec3 p2(-m_dWide,y0,0.0);
		osg::Vec3 p3(-m_dWide,y1,0.0);

		osg::Vec3 nor0 = CalcTriNormal(p0,p2,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p2);
		aPoints->push_back(p3);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p3,p1);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 后侧竖直方向没有圆弧的部分的上部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0-m_dRadius)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,-m_dLength);
		osg::Vec3 p1(0.0,y1,-m_dLength);

		// 右侧两点
		osg::Vec3 p2(-m_dWide,y0,-m_dLength);
		osg::Vec3 p3(-m_dWide,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧竖直方向没有圆弧的部分的下部分
	size = 10;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = -m_dRadius - (m_dHight1-m_dRadius)*(i-1)/size;
		double y1 = -m_dRadius - (m_dHight1-m_dRadius)*i/size;
		// 左侧两点
		osg::Vec3 p0(0.0,y0,-m_dLength);
		osg::Vec3 p1(0.0,y1,-m_dLength);

		// 右侧两点
		osg::Vec3 p2(-m_dWide,y0,-m_dLength);
		osg::Vec3 p3(-m_dWide,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 左侧竖直方向包含有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double x0 = - m_dRadius*sin(theta);
		double y0 = m_dRadius*cos(theta);

		theta = PI*i/size;
		double x1 = - m_dRadius*sin(theta);
		double y1 = m_dRadius*cos(theta);
		// 左侧两点
		osg::Vec3 p0(x0,y0,-m_dLength);
		osg::Vec3 p1(x1,y1,-m_dLength);

		// 右侧两点
		osg::Vec3 p2(-m_dWide,y0,-m_dLength);
		osg::Vec3 p3(-m_dWide,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p3,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p3);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p0,p1,p3);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 右侧上方没有圆弧的部分上半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = m_dHight0 - (m_dHight0-m_dRadius)*(i-1)/size;
		double y1 = m_dHight0 - (m_dHight0-m_dRadius)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);
		// 前面右侧两点
		osg::Vec3 p2(0.0,y0,-m_dLength);
		osg::Vec3 p3(0.0,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方没有圆弧的部分下半部分
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double y0 = -m_dRadius - (m_dHight1-m_dRadius)*(i-1)/size;
		double y1 = -m_dRadius - (m_dHight1-m_dRadius)*i/size;

		// 前面左侧两点
		osg::Vec3 p0(0.0,y0,0.0);
		osg::Vec3 p1(0.0,y1,0.0);
		// 前面右侧两点
		osg::Vec3 p2(0.0,y0,-m_dLength);
		osg::Vec3 p3(0.0,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}

	// 前侧上方有圆弧的部分
	size = 30;
	for (int i = 1;i <= size;++i)
	{
		// 从上到下
		double theta = PI*(i-1)/size;
		double x0 = - m_dRadius*sin(theta);
		double y0 = m_dRadius*cos(theta);

		theta = PI*i/size;
		double x1 = - m_dRadius*sin(theta);
		double y1 = m_dRadius*cos(theta);

		// 前面左侧两点
		osg::Vec3 p0(x0,y0,0.0);
		osg::Vec3 p1(x1,y1,0.0);
		// 前面右侧两点
		osg::Vec3 p2(x0,y0,-m_dLength);
		osg::Vec3 p3(x1,y1,-m_dLength);

		osg::Vec3 nor0 = CalcTriNormal(p0,p1,p2);
		aPoints->push_back(p0);
		aPoints->push_back(p1);
		aPoints->push_back(p2);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);
		aNormals->push_back(nor0);

		osg::Vec3 nor1 = CalcTriNormal(p2,p1,p3);
		aPoints->push_back(p2);
		aPoints->push_back(p1);
		aPoints->push_back(p3);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
		aNormals->push_back(nor1);
	}
}