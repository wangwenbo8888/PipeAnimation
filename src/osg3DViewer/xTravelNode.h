#ifndef _OSGVIEWER_TRAVELNODE_H_
#define _OSGVIEWER_TRAVELNODE_H_
#pragma once



#include <osg/Node>
#include <osgDB/ReadFile>
#include <iostream>
#include <osg/NodeVisitor>
using namespace std;

class ChildrenVisitor: public osg::NodeVisitor
{
public:
	ChildrenVisitor()
		:osg::NodeVisitor(TRAVERSE_ALL_CHILDREN), _indent(0) //TRAVERSE_NONE, 仅当前节点 //TRAVERSE_PARENTS, 向当前节点的父节点遍历 //TRAVERSE_ALL_CHILDREN, 向子节点遍历
	{}

	virtual void apply(osg::Node& node)
	{
		//for(int i = 0; i < _indent; i++)  cout << "    ";
		//cout << "[" << _indent << "]"<< node.libraryName()
		//	<< "::" << node.className() << endl;

		//_indent++;
		//traverse(node);
		//_indent--;

		//for(int i = 0; i < _indent; i++)  cout << "    ";
		//cout << "[" << _indent << "] "<< node.libraryName()
		//	<< "::" << node.className() << endl;
	}

	virtual void apply(osg::Geode& node)
	{
		/*for(int i = 0; i < _indent; i++)  cout << "    ";
		cout << "[" << _indent << "] "<< node.libraryName()
		<< "::" << node.className() << endl;

		_indent++;

		for(unsigned int n = 0; n < node.getNumDrawables(); n++)
		{
		osg::Drawable* draw = node.getDrawable(n);
		if(!draw)
		continue;
		for(int i = 0; i <  _indent; i++)  cout << "    ";
		cout << "[" << _indent << "]" << draw->libraryName() << "::" 
		<< draw->className() << endl;
		}

		traverse(node);
		_indent--;

		for(int i = 0; i < _indent; i++)  cout << "    ";
		cout << "[" << _indent << "]"<< node.libraryName()
		<< "::" << node.className() << endl;*/
	}
private:
	int _indent;
};


class ParentsVisitor: public osg::NodeVisitor
{
public:
	ParentsVisitor()
		:osg::NodeVisitor(TRAVERSE_PARENTS), _indent(0) //TRAVERSE_NONE, 仅当前节点 //TRAVERSE_PARENTS, 向当前节点的父节点遍历 //TRAVERSE_ALL_CHILDREN, 向子节点遍历
	{}

	virtual void apply(osg::Node& node)
	{
		/*for(int i = 0; i < _indent; i++)  cout << "    ";
		cout << "[" << _indent << "]"<< node.libraryName()
		<< "::" << node.className() << endl;

		_indent++;
		traverse(node);
		_indent--;

		for(int i = 0; i < _indent; i++)  cout << "    ";
		cout << "[" << _indent << "] "<< node.libraryName()
		<< "::" << node.className() << endl;*/
	}

	virtual void apply(osg::Geode& node)
	{
		/*for(int i = 0; i < _indent; i++)  cout << "    ";
		cout << "[" << _indent << "] "<< node.libraryName()
		<< "::" << node.className() << endl;

		_indent++;

		for(unsigned int n = 0; n < node.getNumDrawables(); n++)
		{
		osg::Drawable* draw = node.getDrawable(n);
		if(!draw)
		continue;
		for(int i = 0; i <  _indent; i++)  cout << "    ";
		cout << "[" << _indent << "]" << draw->libraryName() << "::" 
		<< draw->className() << endl;
		}

		traverse(node);
		_indent--;

		for(int i = 0; i < _indent; i++)  cout << "    ";
		cout << "[" << _indent << "]"<< node.libraryName()
		<< "::" << node.className() << endl;*/
	}
private:
	int _indent;
};

class CurNodeVisitor: public osg::NodeVisitor
{
public:
	CurNodeVisitor()
		:osg::NodeVisitor(TRAVERSE_NONE), _indent(0) //TRAVERSE_NONE, 仅当前节点 //TRAVERSE_PARENTS, 向当前节点的父节点遍历 //TRAVERSE_ALL_CHILDREN, 向子节点遍历
	{}

	virtual void apply(osg::Node& node)
	{
		/*for(int i = 0; i < _indent; i++)  cout << "    ";
		cout << "[" << _indent << "]"<< node.libraryName()
		<< "::" << node.className() << endl;

		_indent++;
		traverse(node);
		_indent--;

		for(int i = 0; i < _indent; i++)  cout << "    ";
		cout << "[" << _indent << "] "<< node.libraryName()
		<< "::" << node.className() << endl;*/
	}

	virtual void apply(osg::Geode& node)
	{
		/*for(int i = 0; i < _indent; i++)  cout << "    ";
		cout << "[" << _indent << "] "<< node.libraryName()
		<< "::" << node.className() << endl;

		_indent++;

		for(unsigned int n = 0; n < node.getNumDrawables(); n++)
		{
		osg::Drawable* draw = node.getDrawable(n);
		if(!draw)
		continue;
		for(int i = 0; i <  _indent; i++)  cout << "    ";
		cout << "[" << _indent << "]" << draw->libraryName() << "::" 
		<< draw->className() << endl;
		}

		traverse(node);
		_indent--;

		for(int i = 0; i < _indent; i++)  cout << "    ";
		cout << "[" << _indent << "]"<< node.libraryName()
		<< "::" << node.className() << endl;*/
	}
private:
	int _indent;
};



#endif



//int main(int argc, char** argv)
//{
//	osg::ArgumentParser  parser(&argc, argv);
//	osg::Node* root = osgDB::readNodeFiles(parser);
//
//	if(!root)
//	{
//		root = osgDB::readNodeFile("avatar.osg");
//	}
//
//	InfoVisitor infoVisitor;
//	if(root)
//	{
//		root->accept(infoVisitor);
//	}
//
//	system("pause");
//	return 0;
//}