#pragma once

//#include "Novadef.h"
#include "math.h"

inline bool equivalent(float lhs,float rhs,float epsilon=1e-6)
{ 
	float delta = rhs-lhs; 
	return delta<0.0f?delta>=-epsilon:delta<=epsilon; 
}

class Point3D  //数值点结构体
{
public:

	double data[3]; //点坐标

	inline double& x() {return data[0];}
	inline double& y() {return data[1];}
	inline double& z() {return data[2];}
	inline double x() const { return data[0]; }
	inline double y() const { return data[1]; }
	inline double z() const { return data[2]; }

	// 点的初始化
	Point3D() {data[0] = 0.0; data[1] = 0.0; data[2] = 0.0;}
	Point3D(double x, double y, double z) {data[0] = x; data[1] = y; data[2] = z;}
	Point3D(const Point3D& rhs) {data[0] = rhs.x(); data[1] = rhs.y(); data[2] = rhs.z();}
	
	//判断是否为零点
	bool IsZero() const { return (equivalent(data[0], 0.0) && equivalent(data[1], 0.0) && equivalent(data[2], 0.0));}
	//置零
	void Clear() { data[0] = 0; data[1] = 0; data[2] = 0;}

	/*----------------------------矢量运算-----------------------------------*/
	//加法运算
	inline Point3D operator + (const Point3D& rhs) const;
	inline Point3D& operator += (const Point3D& rhs);

	//减法运算
	inline Point3D operator - () const;
	inline Point3D operator - ( const Point3D& rhs) const;
	inline Point3D& operator -= (const Point3D& rhs);

	//乘法运算
	inline Point3D operator * (double rhs) const;
	inline Point3D& operator *= (double rhs);

	//除法运算
	inline Point3D operator / (double rhs) const;
	inline Point3D& operator /= (double rhs)	;

	//点乘
	inline double operator * (const Point3D& rhs) const;

	//叉乘
	void Cross(const Point3D& rhs);
	Point3D Crossed(const Point3D& rhs) const;

	//叉乘
	void CrossCross(const Point3D& v1, const Point3D& v2);

	//! Computes the double vector product this ^ (V1 ^ V2).
	//! -   CrossCrossed creates a new unit vector.
	//! Exceptions
	//! Standard_ConstructionError if:
	//! -   V1 and V2 are parallel, or
	//! -   this unit vector and (V1 ^ V2) are parallel.
	//! This is because, in these conditions, the computed vector
	//! is null and cannot be normalized.
	Point3D CrossCrossed (const Point3D& V1, const Point3D& V2)  const;


	 /**
     * Get the perpendicular of this point to the line defined by rclBase and rclDir.
     * Note: Do not mix up this method with ProjToLine.
     */
	Point3D Perpedicular(const Point3D& rclBase, const Point3D& rclDir ) const; 



	//==重载操作符
	inline bool operator == (const Point3D& rhs) const {return data[0]==rhs.x() && data[1]==rhs.y() && data[2]==rhs.z();}

	inline bool operator != (const Point3D& rhs) const {return data[0]!=rhs.x() || data[1]!=rhs.y() || data[2]!=rhs.z();}

	//赋值运算
	inline Point3D& operator = (const Point3D& _v)
	{
		set(_v.x(), _v.y(), _v.z());
		return *this;
	}

	inline void set(double x, double y, double z)
	{
		data[0] = x; data[1] = y; data[2] = z;
	}

	//返回Magnitude
	inline double Magnitude() const
	{
	    return sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
	}

	inline void Normalize()
	{
		double length = sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
		if(length - 0.0 < 1e-12)
			return;

		data[0] = data[0] / length;
		data[1] = data[1] / length;
		data[2] = data[2] / length;
	}
	/*
	inline Point3D Normalize(const Point3D& dir) const
	{
		Point3D normalDir(dir.x(), dir.y(), dir.z());
		normalDir.Normalize();
		return normalDir;
	}
	*/
	inline double Length() const
	{
		return sqrt(data[0]*data[0] + data[1]*data[1] + data[2]*data[2]);
	}

	inline double Length2() const
	{
		return data[0]*data[0] + data[1]*data[1] + data[2]*data[2];
	}

	inline double dot(const Point3D& a_vector) const
	{
		return((data[0] * a_vector.data[0]) + (data[1] * a_vector.data[1]) + (data[2] * a_vector.data[2]));
	}

	//叉乘
	inline const Point3D operator ^ (const Point3D& rhs) const
	{
		return Point3D(data[1]*rhs.z() - data[2]*rhs.y(),
			data[2]*rhs.x() - data[0]*rhs.z() ,
			data[0]*rhs.y() - data[1]*rhs.x());
	}
};

inline Point3D Point3D::operator + (const Point3D& rhs) const
{
	return Point3D(data[0]+rhs.x(), data[1]+rhs.y(), data[2]+rhs.z());
}

inline Point3D& Point3D::operator += (const Point3D& rhs)
{
	data[0] += rhs.x();
	data[1] += rhs.y();
	data[2] += rhs.z();
	return *this;
}

inline Point3D Point3D::operator - () const	 
{
	return Point3D(-data[0], -data[1], -data[2]);
}

//后面加const，则对于const 对象也能使用-操作符，否则编译不通过
inline Point3D Point3D::operator - ( const Point3D& rhs) const	 
{
	return Point3D(data[0]-rhs.x(), data[1]-rhs.y(), data[2]-rhs.z());
}

inline Point3D& Point3D::operator -= (const Point3D& rhs)
{
	data[0] -= rhs.x();
	data[1] -= rhs.y();
	data[2] -= rhs.z();
	return *this;
}

inline Point3D Point3D::operator * (double rhs) const
{
	return Point3D(data[0]*rhs, data[1]*rhs, data[2]*rhs);
}

inline Point3D& Point3D::operator *= (double rhs)
{
	data[0] *= rhs;
	data[1] *= rhs;
	data[2] *= rhs;
	return *this;
}


inline Point3D Point3D::operator/( double rhs ) const
{
	if(!equivalent(rhs, 0.0))
		return Point3D(data[0]/rhs, data[1]/rhs, data[2]/rhs);

	return Point3D(0.0, 0.0, 0.0);
}


inline Point3D& Point3D::operator /= (double rhs)	
{
	if(!equivalent(rhs, 0.0))
	{
		data[0] /= rhs;
		data[1] /= rhs;
		data[2] /= rhs;
		return *this;
	}
	else
	{
		set(0.0f, 0.0f, 0.0f);
		return *this;
	}
}

inline double Point3D::operator * (const Point3D& rhs) const
{
	return data[0]*rhs.x() + data[1]*rhs.y() + data[2]*rhs.z();
}


//global function
inline Point3D operator * (double rhs, const Point3D& rvs)
{
	return Point3D(rvs.x() * rhs, rvs.y() * rhs, rvs.z() * rhs);
}

double VectorDot(const Point3D &a, const Point3D &b);

void VectorCross(const Point3D &a, const Point3D &b, Point3D &res);

