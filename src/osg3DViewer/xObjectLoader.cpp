
/******************************************************************************
   osGraphX: a 3D file viewer
   Copyright(C) 2011-2012  xbee@xbee.net

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *******************************************************************************/
#include <QtCore/QFileInfo>

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osg/Node>
#include <osg/Geode>//Geode是专门管理可绘制几何体的总闸
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/LightModel>
#include <osg/Material>

#include "xObjectLoader.h"
#include "xLogHandler.h"
#include "JdcMath.h"

#include "log.h"

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

//---------------------------------------------------------------------------------

#define FWRITE(ptr, size, nmemb, stream) do { if (fwrite((ptr), (size), (nmemb), (stream)) != (nmemb)) return false; } while (0)

xObjectLoader::xObjectLoader(QObject *parent) : QObject(parent), m_bOptimize(false)
{
	m_vColor.push_back(QColor(255,251,205));
	m_vColor.push_back(QColor(255,105,180));
	m_vColor.push_back(QColor(46,139,87));
	m_vColor.push_back(QColor(0,191,255));
	m_vColor.push_back(QColor(133,153,211));
	m_vColor.push_back(QColor(184,105,11));
	m_vColor.push_back(QColor(192,192,192));
	m_vColor.push_back(QColor(255,251,205));
	m_vColor.push_back(QColor(251,240,191));
	m_vColor.push_back(QColor(255,255,172));
	m_vColor.push_back(QColor(128,255,128));
	m_vColor.push_back(QColor(0,255,0));
	m_vColor.push_back(QColor(46 ,139, 87));
	m_vColor.push_back(QColor(255,106,106));
	m_vColor.push_back(QColor(255,20,147));
	m_vColor.push_back(QColor(205,38,38));
	m_vColor.push_back(QColor(255,239,219));
	m_vColor.push_back(QColor(0,0,255));
}

osg::ref_ptr<osg::Node> xObjectLoader::createQuade(std::string strFileName)
{
	if (strFileName.empty() || strFileName.size()==0)
	{
		return NULL;
	}

	osg::ref_ptr<osg::Geometry> geom=new osg::Geometry();//定义一个几何体
	osg::ref_ptr<osg::LineWidth> lineSize=new osg::LineWidth();
	lineSize->setWidth(10.0);//设置线的宽度
	geom->getOrCreateStateSet()->setAttributeAndModes(lineSize.get(),osg::StateAttribute::ON);

	std::string filename = strFileName;
	std::fstream _in(filename.c_str(), std::ios_base::in);
	if (!_in)
		return NULL;

	Vertex  v, n;
	std::string line;
	std::string garbage;
	std::stringstream strstream;
	bool facet_normal(false);

	std::vector<Vertex> vers;
	std::vector<Vertex> nors;

	while(_in && !_in.eof()) 
	{
		// Get one line
		std::getline(_in, line);
		if (_in.bad())
			return false;

		// Normal found?
		if (line.find("normal") != std::string::npos)
		{
			strstream.str(line);
			strstream.clear();
			// facet
			strstream >> garbage >> garbage >> n._x >> n._y >> n._z;
			nors.push_back(n);
		}
		// Detected a triangle
		if ((line.find("outer") != std::string::npos) || (line.find("OUTER") != std::string::npos)) 
		{
			for (int i=0; i<3; ++i) 
			{
				// Get one vertex
				std::getline(_in, line);
				strstream.str(line);
				strstream.clear();
				strstream >> garbage >>v._x >> v._y >> v._z;
				vers.push_back(v);
			}

		}
	}

	//首先定义四个点
	osg::ref_ptr<osg::Vec3Array> vs=new osg::Vec3Array(nors.size()*3);//定义一个几何体坐标集合
	osg::ref_ptr<osg::Vec3Array> ns=new osg::Vec3Array(nors.size()*3);//定义了一个法线绑定到该四方体中
	for (int i = 0; i < nors.size();++i)
	{
		Vertex& v0 = vers[i*3];
		Vertex& v1 = vers[i*3+1];
		Vertex& v2 = vers[i*3+2];
		Vertex& nor = nors[i];
		//定义法线
		(*vs)[3*i].set(v0._x, v0._y, v0._z);
		(*vs)[3*i+1].set(v1._x, v1._y, v1._z);
		(*vs)[3*i+2].set(v2._x, v2._y, v2._z);

		(*ns)[3*i].set(nor._x, nor._y, nor._z);
		(*ns)[3*i+1].set(nor._x, nor._y, nor._z);
		(*ns)[3*i+2].set(nor._x, nor._y, nor._z);
	}
	//WriteBinarySTL(*vs.get(),*ns.get(),"D:/paper/pipe.stl");

	if (vs==NULL)
	{
		LogInfo("vs pointer null !");
		return NULL;
	}

	geom->setVertexArray(vs);//将坐标设置到几何体节点中
	geom->setNormalArray(ns, osg::Array::BIND_PER_VERTEX);//添加法线到几何体中

	osg::ref_ptr<osg::DrawArrays> Primitives1 = new osg::DrawArrays(GL_TRIANGLES, 0, 3*nors.size());
	geom->addPrimitiveSet(Primitives1.get());

	//关闭显示列表，开启顶点缓存对象
	geom->setUseVertexBufferObjects(true);

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
	m_pMaterial = new osg::Material;

	osg::ref_ptr<osg::Material> mat = new osg::Material;

	QColor tmp = GetUniqueColor();
	float r = tmp.red()/255.0;
	float g = tmp.green()/255.0;
	float b = tmp.blue()/255.0;
	m_color = osg::Vec4(r,g,b,1.0f);

	//2016.06.01. 通常，对环境光与漫反射光的反射程度决定了材料的颜色，并且它们应该值相同。
	m_pMaterial->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(r,g,b,1.0f));
	m_pMaterial->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(r,g,b,1.0f));
	m_pMaterial->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(0.3,0.3,0.3,1.0f));
	m_pMaterial->setShininess( osg::Material::FRONT_AND_BACK, 100.0f );
	stateset->setAttributeAndModes(m_pMaterial.get(),osg::StateAttribute::ON);

	geom->setStateSet(stateset);
	geode->addDrawable(geom.get());

	osgUtil::Optimizer optimizer;
	optimizer.optimize(geom);
	optimizer.reset();

	//m_geode = geode;
	return geode.get();
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

bool xObjectLoader::WriteBinarySTL(const osg::Vec3Array& vs , const osg::Vec3Array& ns, const std::string& sFileName)
{
	//if (sFileName.empty())
	//	return false;

	FILE *f = NULL;
	errno_t eType = fopen_s(&f,sFileName.c_str(), "wb");
	if (0!=eType)
		return false;

	bool need_swap = !we_are_little_endian();
	char header[80];
	memset(header, ' ', 80);
	FWRITE(header, 80, 1, f);

	int nfaces = vs.size()/3;
	if (need_swap)
		swap_int(nfaces);
	FWRITE(&nfaces, 4, 1, f);

	//if (m_vMeshNormal.size() < 1)
	//	CalcMeshNormal();

	unsigned int iSize = nfaces;
	for (size_t i = 0; i < iSize; i++)
	{
		float fbuf[12];
		fbuf[0] = (float)(ns)[3*i].x();
		fbuf[1] = (float)(ns)[3*i].y();
		fbuf[2] = (float)(ns)[3*i].z();
		fbuf[3]  = (float)(vs)[3*i].x();
		fbuf[4]  = (float)(vs)[3*i].y();
		fbuf[5]  = (float)(vs)[3*i].z();
		fbuf[6]  = (float)(vs)[3*i+1].x();
		fbuf[7]  = (float)(vs)[3*i+1].y();
		fbuf[8]  = (float)(vs)[3*i+1].z();
		fbuf[9]  = (float)(vs)[3*i+2].x();
		fbuf[10] = (float)(vs)[3*i+2].y();
		fbuf[11] = (float)(vs)[3*i+2].z();
		if (need_swap) 
		{
			for (int j = 0; j < 12; j++)
				swap_float(fbuf[j]);
		}
		FWRITE(fbuf, 48, 1, f);
		unsigned char att[2] = { 0, 0 };
		FWRITE(att, 2, 1, f);
	}
	if (NULL!=f)
		fclose(f);

	return true;
}


osg::Vec4 xObjectLoader::GetLoadModelColor()
{
	return m_color;
}


void xObjectLoader::CreatAlterSTLNodeByFile(QString stlpath)
{

}

//osg::ref_ptr<osg::Node> xObjectLoader::GetCurGeode()
//{
//	return m_geode.get();
//}

QColor xObjectLoader::GetUniqueColor()
{
	static int curindex = 0;


	if(curindex > 17)
		curindex = 0;
	return m_vColor[curindex++];
}

void xObjectLoader::slotNewObjectToLoad(const QString &file)
{
    m_file = file;

    // go to file path (for relative included resources)
    chdir(QFileInfo(m_file).absolutePath().toAscii());

    // load the scene.
    xLogHandler::getInstance()->slotReportInfo(tr("Loading of %1 ...").arg(file));

	QByteArray cdata = file.toLocal8Bit();	
	std::string stdstrFileName = std::string(cdata);
	LogInfo("before File name ");
	LogInfo("File name " + QString(stdstrFileName.c_str()));
	if (stdstrFileName.empty() || stdstrFileName.size()==0)
	{
		LogInfo("Can not load file "+ file);
		return;
	}
	//std::string stdstrFileName = file.toStdString(); 
	osg::ref_ptr<osg::Node> ptrLoadedModel = createQuade(stdstrFileName);
	if (ptrLoadedModel==NULL)
	{
		LogInfo("create quade error  !");
		return;
	}

	m_ptrLoadedModels.push_back(ptrLoadedModel);
	//m_ptrLoadedModel = osgDB::readNodeFile(file.toStdString());

	//m_ptrLoadedModel = new osg::Node;

	QString  file_name;  
	QFileInfo fi;  
	fi = QFileInfo(file);  
	file_name = fi.fileName();  
	QByteArray ba = file_name.toLatin1(); 
	char *name = ba.data(); 
	ptrLoadedModel->setName(name);

    if (!ptrLoadedModel)
    {
        QString mess = tr("Error loading file %1").arg(file);
        xLogHandler::getInstance()->slotReportError(mess);
    }

    // optimize the scene graph, remove redundant nodes and state etc.
    if (m_bOptimize)
    {
        xLogHandler::getInstance()->slotReportInfo(tr("Optimization of the graph ..."));
        osgUtil::Optimizer optimizer;
        optimizer.optimize(ptrLoadedModel.get());
    }
	m_ptrLoadedModel = ptrLoadedModel;
	m_file_name = file_name;
   // emit sigNewObjectToView(ptrLoadedModel,file_name);
}
