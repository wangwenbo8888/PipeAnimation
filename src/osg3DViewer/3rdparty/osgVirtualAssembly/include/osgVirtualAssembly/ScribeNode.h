#ifndef SCRIBENODE_H
#define SCRIBENODE_H

#include <osg/Group>

class ScribeNode : public osg::Group
{
public:
	ScribeNode();
	
	void setColor(osg::Vec4& color);
protected:
	~ScribeNode();
	
};

#endif // SCRIBENODE_H
