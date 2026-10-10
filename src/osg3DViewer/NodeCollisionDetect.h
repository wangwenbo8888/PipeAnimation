
#ifndef _NODE_COLLISION_DETECT_H_
#define _NODE_COLLISION_DETECT_H_

#include <osg/group>
#include <osg/ref_ptr>
#include "qstring.h"

#include "Novadef.h"

class STL;

// 测试各段管子和机台各部件是否有碰撞接口
bool HitDetection(osg::ref_ptr<osg::Group> pipes,const std::vector<osg::ref_ptr<osg::Group>>& parts);

bool HitDetectionNew(osg::ref_ptr<osg::Group> root,
					 osg::ref_ptr<osg::Group> pipes,
				     const std::vector<std::pair<STL*,STL*>>& parts,
	                 QString& node0,QString& node1,const std::vector<STL*>& stls);

bool HitDetection2(osg::ref_ptr<osg::Group> root,
				   osg::ref_ptr<osg::Group> SongLiao,
				   osg::ref_ptr<osg::Group> pipes,
				   const std::vector<std::pair<STL*,STL*>>& parts,
	               QString& node0,QString& type0,QString& node1,const std::vector<STL*>& stls);

void GetAllPipeSegment(QString name,osg::Node *pNode, osg::Matrixd &matParent,std::vector<VecArrayName>& vVertices);

void GetAllLineSegment(QString name,osg::Node *pNode, osg::Matrixd &matParent,std::vector<VecArrayName>& vVertices);

bool GetNodeInitPosMatrix(const QString& name,const std::vector<STL*>& stls,osg::Matrixd& matrix);

#endif _NODE_COLLISION_DETECT_H_ 