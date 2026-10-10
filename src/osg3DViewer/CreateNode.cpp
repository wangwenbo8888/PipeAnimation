
#include "CreateNode.h"


osg::ref_ptr<osg::Node> CloneStl(STL* stl,osg::Vec4 color,osg::ref_ptr<osg::Material>& material)
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();//定义一个几何体
	osg::ref_ptr<osg::LineWidth> lineSize=new osg::LineWidth();
	lineSize->setWidth(10.0);//设置线的宽度
	geom->getOrCreateStateSet()->setAttributeAndModes(lineSize.get(),osg::StateAttribute::ON);

	osg::Geometry *geometry = ((osg::Geode*)stl->GetSTLNode())->getDrawable(0)->asGeometry();
	//geometry->setDataVariance(osg::Object::DYNAMIC);
	// 顶点数据
	osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
	// 法向量
	osg::Vec3Array* normals = dynamic_cast<osg::Vec3Array*>(geometry->getNormalArray());
	//// 颜色
	//osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(geometry->getColorArray());

	//osg::Vec4& color = (*colors)[0];

	//首先定义四个点
	osg::ref_ptr<osg::Vec3Array> vs = new osg::Vec3Array(vertices->size());//定义一个几何体坐标集合
	osg::ref_ptr<osg::Vec3Array> ns = new osg::Vec3Array(normals->size());//定义了一个法线绑定到该四方体中

	for (size_t i = 0; i < vertices->size();++i)
	{
		osg::Vec3& v0 = (*vertices)[i];
		osg::Vec3& nor = (*normals)[i];
		//定义法线
		(*vs)[i].set(v0[0], v0[1], v0[2]);
		(*ns)[i].set(nor[0], nor[1], nor[2]);
	}

	//WriteBinarySTL(*vs.get(),*ns.get(),"D:/paper/pipe.stl");
	geom->setVertexArray(vs);//将坐标设置到几何体节点中
	geom->setNormalArray(ns, osg::Array::BIND_PER_VERTEX);//添加法线到几何体中

	osg::ref_ptr<osg::DrawArrays> Primitives1 = new osg::DrawArrays(GL_TRIANGLES, 0, normals->size());
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
	material = new osg::Material;

	osg::ref_ptr<osg::Material> mat = new osg::Material;

	//m_color = color;

	//2016.06.01. 通常，对环境光与漫反射光的反射程度决定了材料的颜色，并且它们应该值相同。
	material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(color[0],color[1],color[2],color[3]));
	material->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(color[0],color[1],color[2],color[3]));
	material->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(0.3,0.3,0.3,1.0f));
	material->setShininess( osg::Material::FRONT_AND_BACK, 100.0f );
	stateset->setAttributeAndModes(material.get(),osg::StateAttribute::ON);

	geom->setStateSet(stateset);
	geode->addDrawable(geom.get());

	osgUtil::Optimizer optimizer;
	optimizer.optimize(geom);
	optimizer.reset();

	//m_Node = geode.get();
	//replaceCurNode(m_Node);
	//m_geode = geode;
	return geode.get();
}