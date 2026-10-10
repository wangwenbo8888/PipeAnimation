
#ifndef COLLISIONCALLBACK_H_
#define COLLISIONCALLBACK_H_

#include <osg/NodeCallback>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>

#include <osgCollisionDetection/CollisionInterface>

#include <QString>

class MainWindow;
class CollisionCallBack : public osg::NodeCallback
{
public:

    CollisionCallBack(MainWindow* w,std::string name,
		std::map<std::pair<QString,QString>,int>* pairs);

	virtual ~CollisionCallBack();

	static void SetPause(bool);

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

protected:

	bool IsSkip(const QString& n0,const QString& n1);


private:

	MainWindow* m_pWindow;
	std::string m_strName;
	osg::ref_ptr<osg::Group> m_pRoot;

	std::map<std::pair<QString,QString>,int>* m_MapCollisionPair;

	// 程序是否已经因为碰撞暂停了
	static bool m_bPaused;
};

#endif