#ifndef __OSGWTOOLS_CREATE_PIPE_H__
#define __OSGWTOOLS_CREATE_PIPE_H__

#include <osg/Shape>
#include <osg/Group>
#include <osg/Material>
using namespace osg;
#define ARC_STEP    40
//#define Pi 3.141592653

osg::Vec3 CalcTriNormal(const osg::Vec3& p0,const osg::Vec3& p1,const osg::Vec3& p2);

class PipeStraight : public TriangleMesh
{
public:
	PipeStraight(double length,double radius,int step);
	~PipeStraight();

	osg::Geode* Create();
	osg::ref_ptr<osg::Geode> Create(Vec3 pos,Vec3 dir);
	bool WriteBinarySTL(const osg::Vec3Array& vs , const osg::Vec3Array& ns, const std::string& sFileName);
	osg::ref_ptr<osg::Vec3Array> GetPoints(){return m_Points;};


private:

	double m_dLength;
	double m_dRadius;
	int m_iStep;
	osg::ref_ptr<osg::Vec3Array> m_Points;

};

class PipeArc : public TriangleMesh
{
public:
	PipeArc(double rotateAngle,double angle,double radius0,double radius1,int step);
	~PipeArc();

	osg::ref_ptr<osg::Geode> Create(Vec3 pos,Vec3 dir);

	osg::ref_ptr<osg::Group> CreateNew();

	osg::ref_ptr<osg::Vec3Array> GetPoints(){return m_Points;};
private:

	double m_dRotateAngle;  // 弯管旋转角度
	double m_dAngle;        // 弯管角度
	double m_dRadius0;      // 圆弧半径
	double m_dRadius1;      // 圆管半径
	int m_iStep;
	osg::ref_ptr<osg::Vec3Array> m_Points;
};

// 创建轮模
class WheelModel : public TriangleMesh
{
public:
	WheelModel(double hight0,double hight1,double radius0,double radius1,
		double radius2,int step,double length,double wide = 0,bool right = true,bool onlyLeft = false);
	~WheelModel();

	// 创建轮模中间凹槽并生成模型
	osg::ref_ptr<osg::Geode> CreateRabbet(Vec3 pos,Vec3 dir);

	// 创建上下圆环
	void CreateUpAndDownCircle(double* matrix,osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals);

	// 创建中心圆柱
	void CreateMidCylinder(double* matrix,osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals);

	// 创建凸块
	void CreateOuter(double* matrix,osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals);

	void CreateOuterRight(double* matrix,osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals);

	void CreateOuterLeft(double* matrix,osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals);

	void CreateOuterLeftNew(double* matrix,osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals);

	osg::ref_ptr<osg::Vec3Array> GetPoints(){return m_Points;}

	osg::ref_ptr<osg::Material> GetMaterial(){return m_pMaterial;}
private:

	double m_dHight0;//中间凹槽y方向离轮模上端距离
	double m_dHight1;//下端
	double m_dRadius0;  // 轮模半径
	double m_dRadius1;  // 轮模凹槽半径，即弯管半径
	double m_dRadius2;  // 轮模中心孔半径
	int m_iStep;
	double m_dLength;   // 轮模突出的一块长度
	double m_dWide;     // 轮模突出的一块宽度
	bool m_bRight;    // 轮模突出部分位于右侧

	bool m_bOnlyLeft;   // 是否是纯左侧加工

	osg::ref_ptr<osg::Vec3Array> m_Points;
	osg::ref_ptr<osg::Material> m_pMaterial;

};

// 创建主夹，辅夹
class Clip : public TriangleMesh
{
public:
	Clip(double hight0,double hight1,double radius,int step,double length,double wide,bool left,bool isAid);
	~Clip();

	// 创建轮模中间凹槽并生成模型
	osg::ref_ptr<osg::Geode> Create(Vec3 pos,Vec3 dir);

	// 主夹凹槽朝左
	void MouthToLeft(osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals);

	//主夹凹槽朝右
	void MouthToRight(osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals);

    // 辅夹凹槽朝左
	void AidMouthToLeft(osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals);

	// 辅夹凹槽朝右
	void AidMouthToRight(osg::ref_ptr<osg::Vec3Array>& aPoints,osg::ref_ptr<osg::Vec3Array>& aNormals);

	osg::ref_ptr<osg::Vec3Array> GetPoints(){return m_Points;};

	osg::ref_ptr<osg::Material> GetMaterial(){return m_pMaterial;}
private:

	double m_dHight0;//中间凹槽y方向离轮模上端距离
	double m_dHight1;//下端
	double m_dRadius;  // 轮模凹槽半径，即弯管半径
	int m_iStep;
	double m_dLength;   // 长度,与凹槽长度相同
	double m_dWide;     // 宽度，垂直于凹槽方向
	bool m_bLeft;     // 开口朝向左侧
    bool m_bAid;      // 是否是辅夹
	osg::ref_ptr<osg::Vec3Array> m_Points;
	osg::ref_ptr<osg::Material> m_pMaterial;

};


#endif // __OSGWTOOLS_CREATE_PIPE_H__