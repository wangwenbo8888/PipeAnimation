#include <osg/Node>
#include <osgDB/ReadFile>
#include <iostream>

using namespace std;


class findGeoNamedNode:
	public osg::NodeVisitor
{
public:
	findGeoNamedNode();
	findGeoNamedNode(const std::string name):
	osg::NodeVisitor(TRAVERSE_ALL_CHILDREN) //Set traverse mode
	{
		resultNode=NULL;
		this->name=name;
	}

	virtual void apply(osg::Node &searchNode)
	{
		if(searchNode.getName()==name)
		{
			osg::Geode* dynamicTry=dynamic_cast<osg::Geode*>(&searchNode);

			if(dynamicTry)
			{
				resultNode=dynamicTry;
			}
		}
		traverse(searchNode);
	}

	osg::Geode* getNode()
	{
		return resultNode;
	}
private:
	osg::Geode* resultNode;
	std::string name;
};