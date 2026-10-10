
#ifndef EXPORTSTL_H_
#define EXPORTSTL_H_

#include <osg/Group>

class STL;
class PIPE;

bool SaveStlASCII(const char * fileName,const osg::ref_ptr<osg::Vec3Array>& aPoints,
    const osg::ref_ptr<osg::Vec3Array>& aNormals,bool flagPointNormal=true);

bool SaveStlASCIINew(const char *fileName,const osg::ref_ptr<osg::Vec3Array>& aPoints,
	const std::vector<osg::Vec3>& aNormals,bool flagPointNormal = true);

bool SaveModel(const char* fileName,STL* stl);

bool SavePipe(const char* fileName,PIPE* pipe);

#endif // EXPORTSTL_H_