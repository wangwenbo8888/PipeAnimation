
#include "CollisionCallBack.h"
#include "MainWindow.h"

#include <OpenThreads/Mutex>

#include "log.h"
#include "ExportStl.h"

OpenThreads::Mutex mutex;

// 程序是否已经因为碰撞暂停了
bool CollisionCallBack::m_bPaused = false;

// 实现：遍历场景图，计算最终变换矩阵
osg::Matrix getFinalTransform(const osg::Node *node)
{
    osg::Matrix finalTransform = osg::Matrix::identity();

    // 从当前节点开始向上遍历
    const osg::Node *currentNode = node;
    while (currentNode)
    {
        // 检查节点类型，并应用相应的变换
        if (const osg::MatrixTransform *matrixTransform = dynamic_cast<const osg::MatrixTransform*>(currentNode))
        {
            // 获取矩阵变换节点的矩阵
            finalTransform.postMult(matrixTransform->getMatrix());
        }
        else if (const osg::PositionAttitudeTransform *pat = dynamic_cast<const osg::PositionAttitudeTransform*>(currentNode))
        {
            // 获取位置变换节点的位置和姿态
            osg::Vec3 pos = pat->getPosition();
            osg::Quat att = pat->getAttitude();
            // 构建变换矩阵
            osg::Matrix m;
            m.makeTranslate(pos);
            m.preMultRotate(att);
            // 应用变换
            finalTransform.postMult(m);
        }
		//else if (const osg::Transform *transform = dynamic_cast<const osg::Transform*>(currentNode))
  //      {
  //          // 其他类型的变换节点，这里假设它们提供了局部变换矩阵
  //          osg::Matrix localTransform = transform->get;
  //          // 将当前节点的变换矩阵乘以前面的变换矩阵
  //          finalTransform.postMult(localTransform);
  //      }
		else
		{
			LogInfo(QString("Unknown node type !"));
		}

        // 获取父节点
		if (currentNode->getNumParents()>1)
		{
			LogInfo(QString("Parent node number is %1 !").arg(currentNode->getNumParents()));
			currentNode = currentNode->getParent(1); // 假设只有一个父节点
		}
		else
		{
			return finalTransform;
		}
    }

    return finalTransform;
}

CollisionCallBack::CollisionCallBack(MainWindow* w,std::string name,
	    std::map<std::pair<QString,QString>,int>* pairs)
	: m_pWindow(w)
    , m_strName(name)
	, m_MapCollisionPair(pairs)
{
	m_bPaused = false;
}

CollisionCallBack::~CollisionCallBack()
{
}

void CollisionCallBack::SetPause(bool b)
{
	m_bPaused = b;
}

bool CollisionCallBack::IsSkip(const QString& n0,const QString& n1)
{
	QString info("Collision obj ");
	info.append(n0);
	info.append(" and ");
	info.append(n1);

	LogInfo(info);

	if ((n0.contains("Straight") || n0.contains("Curve"))
		&& (n1.contains("Straight") || n1.contains("Curve"))
		|| (n1.contains(".stl") && n0.contains(".stl")))
	{

		return true;
	}

	return false;
	
	if (m_MapCollisionPair->find(std::make_pair(n0,n1))!=m_MapCollisionPair->end()
		|| m_MapCollisionPair->find(std::make_pair(n1,n0))!=m_MapCollisionPair->end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CollisionCallBack::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	//traverse(node,nv);
	//return;

	// osg::Matrixf matrix = getFinalTransform(node);
	osg::Matrix currentMatrix;
    currentMatrix = node->getWorldMatrices()[0];

	//float* elements = matrix.ptr();

	//LogInfo(QString("Matrix is %1 %2 %3 %4 !").arg(elements[0]).arg(elements[1]).arg(elements[2]).arg(elements[3]));
	//LogInfo(QString("Matrix is %1 %2 %3 %4 !").arg(elements[4]).arg(elements[5]).arg(elements[6]).arg(elements[7]));
	//LogInfo(QString("Matrix is %1 %2 %3 %4 !").arg(elements[8]).arg(elements[9]).arg(elements[10]).arg(elements[11]));
	//LogInfo(QString("Matrix is %1 %2 %3 %4 !").arg(elements[12]).arg(elements[13]).arg(elements[14]).arg(elements[15]))

	//创建矩阵
	//osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
	//osg::Matrix matrix;//  = trans->getMatrix();
	//mat.computeLocalToWorldMatrix(matrix,nv);

	LogInfo(QString("Collision model for %1 !").arg(node->getName().c_str()));

	//trans->setMatrix(matrix);
	//osg::Matrixf mf;
	//LogInfo(QString("Collision pipe for %1 !").arg(node->getName().c_str()));
	LogInfo(node->getName().c_str());

	// 已经暂停，不再检测碰撞 wangwenbo 20241005
	if (!m_bPaused)
	{
		bool haveCollision = osgCollisionDetection::CollisionInterface::Instence()->updateCollisionOject(m_strName, osg::Matrixf(currentMatrix), true);
		if (haveCollision)
		{
			// m_bPaused = true;
			// OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);
			std::string n0;
			std::string n1;
			//osgCollisionDetection::CollisionInterface::Instence()->getCollisionObjs(n0,n1);
			std::map<std::pair<std::string,std::string>,int> res = osgCollisionDetection::CollisionInterface::Instence()->getAllCollisionObjs();

			std::map<std::pair<std::string,std::string>,int>::iterator iter = res.begin();
			for (; iter != res.end();++iter)
			{
				n0 = iter->first.first;
				n1 = iter->first.second;
				if (IsSkip(QString(n0.c_str()),QString(n1.c_str())))
				{
				// 管子内部的碰撞忽略,继续检测
					m_bPaused = false;
				}
				else
				{
					LogInfo(QString("Collision obj for %1 %2 !").arg(n0.c_str()).arg(n1.c_str()));
					if (m_pWindow)
					{
						m_pWindow->collisionDetectedNew(n0,n1);
					}
					else
					{
						LogInfo(QString("MainWindow is null !"));
					}
				}	
			}

			res.clear();
			
		}
	}
	
    //继续遍历
    traverse(node,nv);
}