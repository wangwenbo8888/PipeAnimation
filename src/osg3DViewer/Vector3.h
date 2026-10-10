

#ifndef VECTOR3_H
#define VECTOR3_H

#include "JdcMath.h"



template <class Real>
class Vector3
{
public:
    // construction
    Vector3 ();  // uninitialized
    Vector3 (Real fX, Real fY, Real fZ);
    Vector3 (const Vector3& rkV);

    // coordinate access
    operator const Real* () const;
    operator Real* ();
    Real operator[] (int i) const;
    Real& operator[] (int i);
    Real X () const;
    Real& X ();
    Real Y () const;
    Real& Y ();
    Real Z () const;
    Real& Z ();

    // assignment
    Vector3& operator= (const Vector3& rkV);

    // comparison
    bool operator== (const Vector3& rkV) const;
    bool operator!= (const Vector3& rkV) const;
    bool operator<  (const Vector3& rkV) const;
    bool operator<= (const Vector3& rkV) const;
    bool operator>  (const Vector3& rkV) const;
    bool operator>= (const Vector3& rkV) const;

    // arithmetic operations
    Vector3 operator+ (const Vector3& rkV) const;
    Vector3 operator- (const Vector3& rkV) const;
    Vector3 operator* (Real fScalar) const;
    Vector3 operator/ (Real fScalar) const;
    Vector3 operator- () const;

    // arithmetic updates
    Vector3& operator+= (const Vector3& rkV);
    Vector3& operator-= (const Vector3& rkV);
    Vector3& operator*= (Real fScalar);
    Vector3& operator/= (Real fScalar);

    // vector operations
    Real Length () const;
    Real SquaredLength () const;
    Real Dot (const Vector3& rkV) const;
    Real Normalize ();

    // The cross products are computed using the right-handed rule.  Be aware
    // that some graphics APIs use a left-handed rule.  If you have to compute
    // a cross product with these functions and send the result to the API
    // that expects left-handed, you will need to change sign on the vector
    // (replace each component value c by -c).
    Vector3 Cross (const Vector3& rkV) const;
    Vector3 UnitCross (const Vector3& rkV) const;

    // Compute the barycentric coordinates of the point with respect to the
    // tetrahedron <V0,V1,V2,V3>, P = b0*V0 + b1*V1 + b2*V2 + b3*V3, where
    // b0 + b1 + b2 + b3 = 1.
    void GetBarycentrics (const Vector3<Real>& rkV0,
        const Vector3<Real>& rkV1, const Vector3<Real>& rkV2,
        const Vector3<Real>& rkV3, Real afBary[4]) const;

    // Gram-Schmidt orthonormalization.  Take linearly independent vectors
    // U, V, and W and compute an orthonormal set (unit length, mutually
    // perpendicular).
    static void Orthonormalize (Vector3& rkU, Vector3& rkV, Vector3& rkW);
    static void Orthonormalize (Vector3* akV);

    // Input W must be initialized to a nonzero vector, output is {U,V,W},
    // an orthonormal basis.  A hint is provided about whether or not W
    // is already unit length.
    static void GenerateOrthonormalBasis (Vector3& rkU, Vector3& rkV,
        Vector3& rkW, bool bUnitLengthW);

    // special vectors
    JDCENGINE static const Vector3 ZERO;
    JDCENGINE static const Vector3 UNIT_X;
    JDCENGINE static const Vector3 UNIT_Y;
    JDCENGINE static const Vector3 UNIT_Z;

private:
    // support for comparisons
    int CompareArrays (const Vector3& rkV) const;

	//使用数组而不使用x，y，z的原因在于：
	//在含有虚函数的类中，Real* ()将因为内存布局而引起错误
	//因为x，y，z形成了结构体，将随着对齐方式以及平台不同而不同
    Real m_afTuple[3];
};

template <class Real>
Vector3<Real> operator* (Real fScalar, const Vector3<Real>& rkV);

#include "Vector3.inl"

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;



#endif
