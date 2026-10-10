
#include "ExportStl.h"

#include "qfile.h"

#include "STL.h"
#include "PIPE.h"

bool SaveStlASCII(const char *fileName,const osg::ref_ptr<osg::Vec3Array>& aPoints,
    const osg::ref_ptr<osg::Vec3Array>& aNormals,bool flagPointNormal)
{
    bool suc = true;
    //char *savename = new char[100];
	char savename[100];
    sprintf(savename,"%s",fileName);

    // char *fileInf = new char[200];
	char fileInf[200];
    sprintf(fileInf,"solid %s.ast  %s",fileName,"by master");

    FILE *fp = fopen(savename,"w");
    fprintf(fp,"%s\n",fileInf);
    //delete []savename;

    for(int i=0;i<aPoints->size()/3;i++)
    {
        osg::Vec3 p0 = (*aPoints)[3*i];
        osg::Vec3 p1 = (*aPoints)[3*i+1];
        osg::Vec3 p2 = (*aPoints)[3*i+2];

        osg::Vec3 nor;
		if (flagPointNormal)
		{
			// normal 存储的是点的法向
			nor = (*aNormals)[3*i];
		}
		else
		{
			// normal 存储的是三角形的法向
			nor = (*aNormals)[i];
		}

        fprintf(fp,"facet normal %f %f %f\n",nor[0],nor[1],nor[2]);
        fprintf(fp,"outer loop\n");
        fprintf(fp,"vertex %f %f %f\n",p0[0],p0[1],p0[2]);
        fprintf(fp,"vertex %f %f %f\n",p1[0],p1[1],p1[2]);
        fprintf(fp,"vertex %f %f %f\n",p2[0],p2[1],p2[2]);
        fprintf(fp,"endloop\n");
        fprintf(fp,"endfacet\n");

    }
    sprintf(fileInf,"endsolid %s.ast  %s",fileName,"by master");
    fprintf(fp,"%s\n",fileInf);
    fclose(fp);

    //delete []fileInf;

    return suc;
}

bool SaveStlASCIINew(const char *fileName,const osg::ref_ptr<osg::Vec3Array>& aPoints,
	const std::vector<osg::Vec3>& aNormals,bool flagPointNormal)
{
	bool suc = true;
	//char *savename = new char[100];
	char savename[100];
	sprintf(savename,"%s",fileName);

	// char *fileInf = new char[200];
	char fileInf[200];
	sprintf(fileInf,"solid %s.ast  %s",fileName,"by master");

	FILE *fp = fopen(savename,"w");
	fprintf(fp,"%s\n",fileInf);
	//delete []savename;

	for(int i=0;i<aPoints->size()/3;i++)
	{
		osg::Vec3 p0 = (*aPoints)[3*i];
		osg::Vec3 p1 = (*aPoints)[3*i+1];
		osg::Vec3 p2 = (*aPoints)[3*i+2];

		osg::Vec3 nor;
		if (flagPointNormal)
		{
			// normal 存储的是点的法向
			nor = aNormals[3*i];
		}
		else
		{
			// normal 存储的是三角形的法向
			nor = aNormals[i];
		}

		fprintf(fp,"facet normal %f %f %f\n",nor[0],nor[1],nor[2]);
		fprintf(fp,"outer loop\n");
		fprintf(fp,"vertex %f %f %f\n",p0[0],p0[1],p0[2]);
		fprintf(fp,"vertex %f %f %f\n",p1[0],p1[1],p1[2]);
		fprintf(fp,"vertex %f %f %f\n",p2[0],p2[1],p2[2]);
		fprintf(fp,"endloop\n");
		fprintf(fp,"endfacet\n");

	}
	sprintf(fileInf,"endsolid %s.ast  %s",fileName,"by master");
	fprintf(fp,"%s\n",fileInf);
	fclose(fp);

	//delete []fileInf;

	return suc;
}

bool SaveModel(const char* fileName,STL* stl)
{
	osg::ref_ptr<osg::Geode> geode = (osg::Geode*)stl->GetSTLNode();
	if (geode!=nullptr)
	{
		osg::ref_ptr<osg::Geometry> geom = (osg::Geometry*)geode->getDrawable(0);

		if (geom!=nullptr)
		{
			osg::ref_ptr<osg::Vec3Array> vertices = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
			osg::ref_ptr<osg::Vec3Array> normals = dynamic_cast<osg::Vec3Array*>(geom->getNormalArray());

			return SaveStlASCII(fileName,vertices,normals,false);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	return true;
}

bool SavePipe(const char* fileName,PIPE* pipe)
{
	osg::ref_ptr<osg::Geode> geode = (osg::Geode*)pipe->GetPipeNode();
	if (geode!=nullptr)
	{
		osg::ref_ptr<osg::Geometry> geom = (osg::Geometry*)geode->getDrawable(0);

		if (geom!=nullptr)
		{
			osg::ref_ptr<osg::Vec3Array> vertices = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
			osg::ref_ptr<osg::Vec3Array> normals = dynamic_cast<osg::Vec3Array*>(geom->getNormalArray());

			return SaveStlASCII(fileName,vertices,normals,false);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}