
#include "NodeCollisionDetect.h"

#include <osgUtil/Optimizer>
#include <osgUtil/IntersectVisitor>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/ComputeBoundsVisitor>

#include "CollisionDetect/MeshIntersect.h"
#include "GYDevillersTriangle.h"
//#include <>

#include <omp.h>

#include "STL.h"
#include "ExportStl.h"
#include "Novadef.h"

#include "log.h"

void GetAllLineSegment(QString name,osg::Geometry *pGeometry,osg::Matrixd &matParent,std::vector<VecArrayName>& vVertices)
{
	if (pGeometry == NULL)
	{
		return;
	}

	VecArrayName data;

	osg::ref_ptr<osg::Vec3Array> vertices = dynamic_cast<osg::Vec3Array*>(pGeometry->getVertexArray());
	if (vertices==NULL)
	{
		return;
	}

	data.array = new osg::Vec3Array();
	data.name = name;
	if (vertices.valid())
	{
		data.array->resize(vertices->size());

		#pragma omp parallel for
		for (int i = 0; i < vertices->size();++i)
		{
			(*data.array)[i]=vertices->at(i)*matParent;
		}

		vVertices.push_back(data);

		/*unsigned int uPriNum = pGeometry->getNumPrimitiveSets();
		for (unsigned int i = 0; i < uPriNum; i++)
		{
			const osg::PrimitiveSet *pPriSet = pGeometry->getPrimitiveSet(i);
			for( unsigned int j = 0; j < pPriSet->getNumIndices(); j++ )
			{
				unsigned int iIndex1 = pPriSet->index(j);
				osg::Vec3 p = vertices->at(iIndex1)*matParent;
			}
		}*/
	}
}

void GetAllLineSegmentAlter(QString name,osg::Geometry *pGeometry,osg::Matrixd &matParent,std::vector<VecArrayName>& vVertices,osg::Node *AlterNode)
{
	if (pGeometry == NULL)
	{
		return;
	}

	VecArrayName data;

	osg::ref_ptr<osg::Vec3Array> vertices = dynamic_cast<osg::Vec3Array*>(pGeometry->getVertexArray());
	data.array = new osg::Vec3Array();
	data.name = name;
	if (vertices.valid())
	{
		data.array->resize(vertices->size());

		#pragma omp parallel for
		for (int i = 0; i < vertices->size();++i)
		{
			(*data.array)[i] = vertices->at(i)*matParent;
		}

		vVertices.push_back(data);

		/*unsigned int uPriNum = pGeometry->getNumPrimitiveSets();
		for (unsigned int i = 0; i < uPriNum; i++)
		{
			const osg::PrimitiveSet *pPriSet = pGeometry->getPrimitiveSet(i);
			for( unsigned int j = 0; j < pPriSet->getNumIndices(); j++ )
			{
				unsigned int iIndex1 = pPriSet->index(j);
				osg::Vec3 p = vertices->at(iIndex1)*matParent;
			}
		}*/
	}
}

void GetAllLineSegment(QString name,osg::Node *pNode, osg::Matrixd &matParent,std::vector<VecArrayName>& vVertices)
{
	osg::Geode *pGeode = dynamic_cast<osg::Geode*>(pNode);
	if (pGeode)
	{
		unsigned int iDrawNum = pGeode->getNumDrawables();
		for(unsigned int i = 0; i < iDrawNum; i++ )
		{
			osg::Drawable *pDrawable = pGeode->getDrawable(i);
			GetAllLineSegment(QString(pGeode->getName().c_str()),pDrawable->asGeometry(),matParent,vVertices);
		}
	}
	else 
	{
		osg::MatrixTransform *pMatrix = (osg::MatrixTransform*)pNode;
		if (pMatrix)
		{
			osg::Matrixd mat = pMatrix->getMatrix();
			osg::Matrixd matNow = mat * matParent;
			unsigned int uNum = pMatrix->getNumChildren();
			for (unsigned int i = 0; i < uNum; i++)
			{
				osg::Node *pChild = pMatrix->getChild(i);
				GetAllLineSegment(QString("error"),pChild, matNow,vVertices);
			}
		}
		else
		{
			osg::Group *pGroup = dynamic_cast<osg::Group*>(pNode);
			if (pGroup)
			{
				unsigned int uNum = pGroup->getNumChildren();
				for (unsigned int i = 0; i < uNum; i++)
				{
					osg::Node *pChild = pGroup->getChild(i);
					GetAllLineSegment(QString("error"),pChild,matParent,vVertices);
				}
			}
		}
	}
}

void GetAllLineSegmentAlter(QString name,osg::Node *pNode, osg::Matrixd &matParent,std::vector<VecArrayName>& vVertices,osg::Node *pAlterNode)
{
	osg::Geode *pGeode = dynamic_cast<osg::Geode*>(pNode);
	if (pGeode)
	{
		pGeode = dynamic_cast<osg::Geode*>(pAlterNode);//alter替换？
		unsigned int iDrawNum = pGeode->getNumDrawables();
		for(unsigned int i = 0; i < iDrawNum; i++ )
		{
			osg::Drawable *pDrawable = pGeode->getDrawable(i);
			GetAllLineSegmentAlter(QString(pGeode->getName().c_str()),pDrawable->asGeometry(),matParent,vVertices,pAlterNode);
		}
	}
	else 
	{
		osg::MatrixTransform *pMatrix = (osg::MatrixTransform*)pNode;
		if (pMatrix)
		{
			osg::Matrixd mat = pMatrix->getMatrix();
			osg::Matrixd matNow = mat * matParent;
			unsigned int uNum = pMatrix->getNumChildren();
			for (unsigned int i = 0; i < uNum; i++)
			{
				osg::Node *pChild = pMatrix->getChild(i);
				GetAllLineSegmentAlter(QString("error"),pChild, matNow,vVertices,pAlterNode);
			}
		}
		else
		{
			osg::Group *pGroup = dynamic_cast<osg::Group*>(pNode);
			if (pGroup)
			{
				unsigned int uNum = pGroup->getNumChildren();
				for (unsigned int i = 0; i < uNum; i++)
				{
					osg::Node *pChild = pGroup->getChild(i);
					GetAllLineSegmentAlter(QString("error"),pChild,matParent,vVertices,pAlterNode);
				}
			}
		}
	}
}

void GetAllPipeSegment(QString name,osg::Node *pNode, osg::Matrixd &matParent,std::vector<VecArrayName>& vVertices)
{
	osg::Geode *pGeode = dynamic_cast<osg::Geode*>(pNode);
	if (pGeode)
	{
		unsigned int iDrawNum = pGeode->getNumDrawables();
		for(unsigned int i = 0; i < iDrawNum; i++ )
		{
			osg::Drawable *pDrawable = pGeode->getDrawable(i);
			GetAllLineSegment(QString(pGeode->getName().c_str()),pDrawable->asGeometry(),matParent,vVertices);
		}
	}
	else 
	{
		osg::PositionAttitudeTransform *pMatrix = (osg::PositionAttitudeTransform*)pNode;
		if (pMatrix)
		{
			osg::Matrix matrix;
			osg::Matrixd matNow;
			matrix.makeScale(pMatrix->getScale());   //先缩放
			matNow = matrix;

			osg::Vec3d center = pMatrix->getPivotPoint();
			matrix.makeTranslate(-pMatrix->getPivotPoint()); // 移动到旋转中心点
			matNow = matNow*matrix;
			matrix.makeRotate(pMatrix->getAttitude());      // 旋转
			matNow = matNow*matrix;

			osg::Vec3d vec = pMatrix->getPosition();
			//if ((fabs(vec[0])>0.000001 
			//	&& fabs(vec[1])>0.00001)
			//	|| (fabs(center[1])!=0 && fabs(vec[1]-center[1])<0.00001))
			//{
			//	// 弯管不用平移
			//	vec = osg::Vec3d();
			//}

			if (fabs(center[0])<0.00001
				&& fabs(center[1])<0.00001
				&& fabs(center[2])<0.00001)
			{

			}
			else if (vec[2]<0.0)
			{
				vec = osg::Vec3d(0.0,0.0,vec[2]);
			}
			else
			{
				vec = osg::Vec3d(0.0,0.0,0.0);
			}

			matrix.makeTranslate(vec);        // 平移
			matNow = matNow*matrix;

			matrix.makeTranslate(pMatrix->getPivotPoint()); // 移动回去
			matNow = matNow*matrix;

			matNow = matNow*matParent;
			//matParent = matNow*matParent;
			unsigned int uNum = pMatrix->getNumChildren();
			for (unsigned int i = 0; i < uNum; i++)
			{
				osg::Node *pChild = pMatrix->getChild(i);
				GetAllPipeSegment(QString("error"),pChild, matNow,vVertices);
			}
		}
		else
		{
			osg::Group *pGroup = dynamic_cast<osg::Group*>(pNode);
			if (pGroup)
			{
				unsigned int uNum = pGroup->getNumChildren();
				for (unsigned int i = 0; i < uNum; i++)
				{
					osg::Node *pChild = pGroup->getChild(i);
					// GetAllLineSegment(QString("error"),pChild,matParent,vVertices);
				    GetAllPipeSegment(QString("error"),pChild,matParent,vVertices);
				}
			}
		}
	}
}

bool HitDetection(osg::ref_ptr<osg::Group> pipes,const std::vector<osg::ref_ptr<osg::Group>>& parts)
{
	unsigned int uNum = pipes->getNumChildren();
	unsigned int i = 0;
	for (i = 0; i < uNum-1; i++)
	{
		osg::Node *pNode1 = pipes->getChild(i);
		if (pNode1)
		{
			// 获取包围盒
			osg::ComputeBoundsVisitor boundvisitor ;  
			pNode1->accept(boundvisitor);  
			osg::BoundingBox& bb1 = boundvisitor.getBoundingBox(); 
			osg::ref_ptr<osgUtil::IntersectVisitor> iv = new osgUtil::IntersectVisitor();
			//GetAllLineSegment(pNode1, iv);
			//iv->addLineSegment(new osg::LineSegment(osg::Vec3(50.0,20.0, 0.0), osg::Vec3(50, 50,50)));
			unsigned int j = i + 1;
			for (; j < uNum; j++)
			{
				osg::Node *pNode2 = pipes->getChild(j);
				if (pNode2)
				{
					osg::ComputeBoundsVisitor boundvisitor2 ;  
					pNode2->accept(boundvisitor2);
					osg::BoundingBox& bb2 = boundvisitor2.getBoundingBox();
					if (bb1.intersects(bb2))
					{
						// 包围盒有相交
						// 判断两个第一个节点的所有线是否和第二个节点有相交
						pNode2->accept(*iv);
						if (iv->hits())
						{
							return true;
							//  有相交，下面的代码是获取相交点
							/*osgUtil::IntersectVisitor::LineSegmentHitListMap &mapList = iv->getSegHitList();
							osgUtil::IntersectVisitor::LineSegmentHitListMap::iterator mapIter = mapList.begin();
							for (mapIter = mapList.begin(); mapIter != mapList.end(); mapIter++)
							{
							osgUtil::IntersectVisitor::HitList hitList = mapIter->second;
							osgUtil::IntersectVisitor::HitList::iterator iter = hitList.begin();
							for (; iter != hitList.end(); iter++)
							{
							osg::Vec3 ptInter = iter->getWorldIntersectPoint();
							int cc = 0;
							}
							}*/
						}
						else
						{
							// 没有相交，用第二个节点的线段与第一个节点进行检查
							osg::ref_ptr<osgUtil::IntersectVisitor> iv2 = new osgUtil::IntersectVisitor();
							//GetAllLineSegment(pNode2, iv2);
							pNode1->accept(*iv2);
							if (iv2->hits())
							{
								// 有相交
								return true;
							}
						}

					}
				}
			}
		}
	}

	return false;
}

bool GetNodeInitPosMatrix(const QString& name,const std::vector<STL*>& stls,osg::Matrixd& matrix)
{
	for (int i = 0; i < stls.size();++i)
	{
		if (stls[i]->GetSTLName()==name)
		{
			matrix = stls[i]->GetPosMatrix()->getMatrix();
			osg::Vec3d center = stls[i]->GetCurModelCenter();

			osg::Matrixd temp;
			temp.makeTranslate(-center);
			//matrix = temp;

			return true;
		}
	}

	return false;
}

bool HitDetectionNew(osg::ref_ptr<osg::Group> root,
	osg::ref_ptr<osg::Group> pipes,
	const std::vector<std::pair<STL*,STL*>>& parts,
	QString& node0,QString& node1,
	const std::vector<STL*>& stls)
{

	time_t start = clock();

	std::vector<VecArrayName> vVertices;
	osg::Matrixd matrix;
	GetAllLineSegment(QString(root->getName().c_str()),root, matrix,vVertices);

	time_t end = clock() - start;

	for (int j = 0; j < parts.size(); ++j)
	{
		std::vector<VecArrayName> vVertices0;
		std::vector<VecArrayName> vVertices1;
		std::vector<VecArrayName> vVertices2;

		if (parts[j].first==NULL)
		{
			LogInfo("part null !"+ QString::number(j));
			continue;
		}

		const osg::ref_ptr<osg::Group>& part = parts[j].first->GetModelMatrix();
		const QString name0 = parts[j].first->GetSTLName();
		osg::Matrixd matrix0;
		osg::Matrixd matrix2;

		//GetNodeInitPosMatrix(name0,stls,matrix0);
		osg::ComputeBoundsVisitor boundvisitor ;
		part->accept(boundvisitor);  
		osg::BoundingBox& bb0 = boundvisitor.getBoundingBox(); 
		//osg::ref_ptr<osgUtil::IntersectVisitor> iv0 = new osgUtil::IntersectVisitor();
		//if (parts[j].second == NULL)
		//{
		GetAllLineSegment(QString(part->getName().c_str()),part, matrix0,vVertices0);
		//}
		//else if (parts[j].second != NULL)
		//{
		//	const osg::ref_ptr<osg::Group>& part0 = parts[j].second->GetModelMatrix();
		//	GetAllLineSegment(QString(part->getName().c_str()),part0, matrix2,vVertices0);
		//	const osg::ref_ptr<osg::Group>& part1 = parts[j].first->GetModelMatrix();
		//	GetAllLineSegment(QString(part->getName().c_str()),part1, matrix0,vVertices2);
		//}

		osg::Matrixd matrix1;
		osg::Node* node = (osg::Node*)pipes;
		osg::PositionAttitudeTransform *pMatrix = (osg::PositionAttitudeTransform*)node;
		if (pMatrix)
		{
			osg::Matrix matrix;
			matrix.makeScale(pMatrix->getScale());   //先移动
			matrix1 = matrix;
			matrix.makeTranslate(-pMatrix->getPivotPoint()); // 移动到旋转中心点
			matrix1 = matrix1*matrix;
			matrix.makeRotate(pMatrix->getAttitude());      // 旋转
			matrix1 = matrix1*matrix;

			//matrix.makeTranslate(pMatrix->getPosition());            // 缩放
			osg::Vec3d vec = pMatrix->getPosition();
			vec[0] = 0.0;
			vec[1] = 0.0;
			matrix.makeTranslate(vec);            // 平移
			matrix1 = matrix1*matrix;

			matrix.makeTranslate(pMatrix->getPivotPoint()); // 移动回去
			matrix1 = matrix1*matrix;
		}

		unsigned int uNum = pipes->getNumChildren();
		for (unsigned int i = 0; i < uNum; i++)
		{
			osg::Node *pNode1 = pipes->getChild(i);
			if (pNode1)
			{
				// 获取包围盒
				osg::ComputeBoundsVisitor boundvisitor ;  
				pNode1->accept(boundvisitor);  
				osg::BoundingBox& bb1 = boundvisitor.getBoundingBox(); 
				//osg::ref_ptr<osgUtil::IntersectVisitor> iv1 = new osgUtil::IntersectVisitor();

				GetAllPipeSegment(QString(pNode1->getName().c_str()),pNode1,matrix1,vVertices1);

				/*QString info("model index ");
				info.append(QString::number(j));
				info.append("min ");
				info.append(QString::number(bb0._min[0]));
				info.append(" ");
				info.append(QString::number(bb0._min[1]));
				info.append(" ");
				info.append(QString::number(bb0._min[2]));
				info.append(" ");
				LogInfo(info);

				info.clear();
				info.append("max ");
				info.append(QString::number(j));
				info.append(QString::number(bb0._max[0]));
				info.append(" ");
				info.append(QString::number(bb0._max[1]));
				info.append(" ");
				info.append(QString::number(bb0._max[2]));
				info.append(" ");
				LogInfo(info);

				info.clear();
				info.append("pipe number is ");
				info.append(QString::number(vVertices1.size()));
				LogInfo(info);*/

				if (bb0.intersects(bb1))
				{
					// 包围盒相交，才有可能相交
					for (size_t m = 0; m < vVertices0.size();++m)
					{
						for (size_t n = 0; n < vVertices1.size();++n)
						{
							const osg::ref_ptr<osg::Vec3Array>& array0 = vVertices0[m].array;
							const osg::ref_ptr<osg::Vec3Array>& array1 = vVertices1[n].array;
							if (IsGeometryIntersect(array0,vVertices0[m].name,array1,vVertices1[n].name))
							{
								node0 = vVertices0[m].name;
								node1 = vVertices1[n].name;

								LogInfo(node0 + node1 + "collision !");

								// 回退的管子不做碰撞检测
								if (node0.contains("back",Qt::CaseSensitive) || node1.contains("back",Qt::CaseSensitive))
								{
									continue;
								}

								//MyTriangle *tri0 = new MyTriangle;
								//MyTriangle *tri1 = new MyTriangle;
								//tri0->Vertex_1[0] = (*array0)[0][0];	
								//tri0->Vertex_1[1] = (*array0)[0][1];	
								//tri0->Vertex_1[2] = (*array0)[0][2];	

								//tri0->Vertex_2[0] = (*array0)[1][0];	
								//tri0->Vertex_2[1] = (*array0)[1][1];	
								//tri0->Vertex_2[2] = (*array0)[1][2];	

								//tri0->Vertex_3[0] = (*array0)[2][0];	
								//tri0->Vertex_3[1] = (*array0)[2][1];	
								//tri0->Vertex_3[2] = (*array0)[2][2];

								//tri1->Vertex_1[0] = (*array1)[0][0];	
								//tri1->Vertex_1[1] = (*array1)[0][1];	
								//tri1->Vertex_1[2] = (*array1)[0][2];	

								//tri1->Vertex_2[0] = (*array1)[1][0];	
								//tri1->Vertex_2[1] = (*array1)[1][1];	
								//tri1->Vertex_2[2] = (*array1)[1][2];	

								//tri1->Vertex_3[0] = (*array1)[2][0];	
								//tri1->Vertex_3[1] = (*array1)[2][1];	
								//tri1->Vertex_3[2] = (*array1)[2][2];

								


								//bool result = isTriangleTntersect(tri0,tri1);

								//return result;

								return true;
							}
						}
					}

				}

				vVertices1.clear();
			}
		}
	}

	return false;
}

bool HitDetection2(osg::ref_ptr<osg::Group> root,
	osg::ref_ptr<osg::Group> SongLiao,
	osg::ref_ptr<osg::Group> pipes,
	const std::vector<std::pair<STL*,STL*>>& parts,
	QString& node0,QString& type0,QString& node1,
	const std::vector<STL*>& stls)
{
	//time_t start = clock();

	std::vector<VecArrayName> vVertices;
	osg::Matrixd matrix;
	GetAllLineSegment(QString(root->getName().c_str()),root, matrix,vVertices);

	//time_t end = clock() - start;

	std::vector<VecArrayName> vVerticesSongLiao;
	osg::Matrixd matrixS;
	GetAllLineSegment(QString(SongLiao->getName().c_str()),SongLiao, matrixS,vVerticesSongLiao);

	//LogInfo(QString::number(end));

	if (vVertices.empty())
	{
		return false;
	}

	std::vector<VecArrayName> vTempVertices;
	for (size_t m = 0; m < parts.size();++m)
	{
		if (parts[m].first==NULL)
		{
			continue;
		}

		QString name = parts[m].first->GetSTLName();
		for (size_t n = 0; n < vVertices.size();++n)
		{
            if (vVertices[n].name==name && parts[m].first->GetIsShow())
			{
				vTempVertices.push_back(vVertices[n]);
				continue;
			}
		}
	}

	for (size_t j = 0; j < vTempVertices.size(); ++j)
	{
		std::vector<VecArrayName> vVertices1;
		//const osg::ref_ptr<osg::Group>& part = parts[j].first->GetModelMatrix();
		//const QString name0 = parts[j].first->GetSTLName();
		//osg::Matrixd matrix0;
		//osg::Matrixd matrix2;

		//GetNodeInitPosMatrix(name0,stls,matrix0);
		//osg::ComputeBoundsVisitor boundvisitor ;
		//part->accept(boundvisitor);  
		//osg::BoundingBox& bb0 = boundvisitor.getBoundingBox(); 
		//osg::ref_ptr<osgUtil::IntersectVisitor> iv0 = new osgUtil::IntersectVisitor();
		//if (parts[j].second == NULL)
		//{
		//GetAllLineSegment(QString(part->getName().c_str()),part, matrix0,vVertices0);
		//}
		//else if (parts[j].second != NULL)
		//{
		//	const osg::ref_ptr<osg::Group>& part0 = parts[j].second->GetModelMatrix();
		//	GetAllLineSegment(QString(part->getName().c_str()),part0, matrix2,vVertices0);
		//	const osg::ref_ptr<osg::Group>& part1 = parts[j].first->GetModelMatrix();
		//	GetAllLineSegment(QString(part->getName().c_str()),part1, matrix0,vVertices2);
		//}

		osg::Matrixd matrix1;
		osg::Node* node = (osg::Node*)pipes;
		osg::PositionAttitudeTransform *pMatrix = (osg::PositionAttitudeTransform*)node;
		if (pMatrix)
		{
			osg::Matrix matrix;
			matrix.makeScale(pMatrix->getScale());   //先移动
			matrix1 = matrix;
			matrix.makeTranslate(-pMatrix->getPivotPoint()); // 移动到旋转中心点
			matrix1 = matrix1*matrix;
			matrix.makeRotate(pMatrix->getAttitude());      // 旋转
			matrix1 = matrix1*matrix;

			//matrix.makeTranslate(pMatrix->getPosition());            // 缩放
			osg::Vec3d vec = pMatrix->getPosition();
			vec[0] = 0.0;
			vec[1] = 0.0;
			matrix.makeTranslate(vec);            // 平移
			matrix1 = matrix1*matrix;

			matrix.makeTranslate(pMatrix->getPivotPoint()); // 移动回去
			matrix1 = matrix1*matrix;
		}

		unsigned int uNum = pipes->getNumChildren();
		for (unsigned int i = 0; i < uNum; i++)
		{
			osg::Node *pNode1 = pipes->getChild(i);
			if (pNode1)
			{
				// 获取包围盒
				//osg::ComputeBoundsVisitor boundvisitor ;
				//pNode1->accept(boundvisitor);  
				//osg::BoundingBox& bb1 = boundvisitor.getBoundingBox(); 
				//osg::ref_ptr<osgUtil::IntersectVisitor> iv1 = new osgUtil::IntersectVisitor();

				GetAllPipeSegment(QString(pNode1->getName().c_str()),pNode1,matrix1,vVertices1);

				/*QString info("model index ");
				info.append(QString::number(j));
				info.append("min ");
				info.append(QString::number(bb0._min[0]));
				info.append(" ");
				info.append(QString::number(bb0._min[1]));
				info.append(" ");
				info.append(QString::number(bb0._min[2]));
				info.append(" ");
				LogInfo(info);

				info.clear();
				info.append("max ");
				info.append(QString::number(j));
				info.append(QString::number(bb0._max[0]));
				info.append(" ");
				info.append(QString::number(bb0._max[1]));
				info.append(" ");
				info.append(QString::number(bb0._max[2]));
				info.append(" ");
				LogInfo(info);

				info.clear();
				info.append("pipe number is ");
				info.append(QString::number(vVertices1.size()));
				LogInfo(info);*/

				//if (bb0.intersects(bb1))
				//{
				//	// 包围盒相交，才有可能相交
				//	for (int m = 0; m < vVertices0.size();++m)
				//	{
						for (size_t n = 0; n < vVertices1.size();++n)
						{
							const osg::ref_ptr<osg::Vec3Array>& array0 = vTempVertices[j].array;
							const osg::ref_ptr<osg::Vec3Array>& array1 = vVertices1[n].array;
							if (IsGeometryIntersect(array0,vTempVertices[j].name,array1,vVertices1[n].name))
							{
								node0 = vTempVertices[j].name;
								for (size_t m = 0; m < parts.size();++m)
								{
									if (parts[m].first->GetSTLName()==node0)
									{
										type0 = parts[m].first->GetType();
										break;
									}
								}

								if (type0.toStdString().empty())
								{
									type0 = node0;
								}

								node1 = vVertices1[n].name;


								LogInfo(node0 + node1 + "collision !");

								// 回退的管子不做碰撞检测
								if (node0.contains("back",Qt::CaseSensitive) || node1.contains("back",Qt::CaseSensitive))
								{
									continue;
								}

								//MyTriangle *tri0 = new MyTriangle;
								//MyTriangle *tri1 = new MyTriangle;
								//tri0->Vertex_1[0] = (*array0)[0][0];	
								//tri0->Vertex_1[1] = (*array0)[0][1];	
								//tri0->Vertex_1[2] = (*array0)[0][2];	

								//tri0->Vertex_2[0] = (*array0)[1][0];	
								//tri0->Vertex_2[1] = (*array0)[1][1];	
								//tri0->Vertex_2[2] = (*array0)[1][2];	

								//tri0->Vertex_3[0] = (*array0)[2][0];	
								//tri0->Vertex_3[1] = (*array0)[2][1];	
								//tri0->Vertex_3[2] = (*array0)[2][2];

								//tri1->Vertex_1[0] = (*array1)[0][0];	
								//tri1->Vertex_1[1] = (*array1)[0][1];	
								//tri1->Vertex_1[2] = (*array1)[0][2];	

								//tri1->Vertex_2[0] = (*array1)[1][0];	
								//tri1->Vertex_2[1] = (*array1)[1][1];	
								//tri1->Vertex_2[2] = (*array1)[1][2];	

								//tri1->Vertex_3[0] = (*array1)[2][0];	
								//tri1->Vertex_3[1] = (*array1)[2][1];	
								//tri1->Vertex_3[2] = (*array1)[2][2];

								//bool result = isTriangleTntersect(tri0,tri1);

								//return result;

								return true;
							}
					//	}
					//}

				}

				vVertices1.clear();
			}
		}
	}

	//return false;

	std::vector<VecArrayName> vRemainVertices;
	vector<bool> flags;
	flags.resize(vTempVertices.size(),false);
	for (size_t i = 0; i < vVerticesSongLiao.size();++i)
	{
		for (size_t j = 0; j < vTempVertices.size(); ++j)
		{
			if (vVerticesSongLiao[i].name==vTempVertices[j].name)
			{
				flags[j] = true;
				break;
			}
		}
	}

    // 不检测小车和机身的碰撞
    for (size_t j = 0; j < vTempVertices.size();++j)
    {
        if (vTempVertices[j].name=="jishen.stl")
        {
            flags[j] = true;
        }
    }

	for (size_t i = 0; i < flags.size();++i)
	{
		if (!flags[i])
		{
			vRemainVertices.push_back(vTempVertices[i]);
		}
	}

	for (size_t i = 0; i < vVerticesSongLiao.size();++i)
	{
		for (size_t j = 0; j < vRemainVertices.size();++j)
		{
			const osg::ref_ptr<osg::Vec3Array>& array0 = vVerticesSongLiao[i].array;
			const osg::ref_ptr<osg::Vec3Array>& array1 = vRemainVertices[j].array;
			if (IsGeometryIntersect(array0,vRemainVertices[j].name,array1,vVerticesSongLiao[i].name))
			{
				node0 = vRemainVertices[j].name;
				for (size_t m = 0; m < parts.size();++m)
				{
					if (parts[m].first->GetSTLName()==node0)
					{
						type0 = parts[m].first->GetType();
						break;
					}
				}

				node1 = vVerticesSongLiao[i].name;

				return true;
			}
		}
	}

	return false;
}