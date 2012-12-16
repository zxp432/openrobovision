//
// OpenRoboVision
//
// own math types
//
// собственные математические типы
//
//
// robocraft.ru
//

#ifndef _ORV_MATH_H_
#define _ORV_MATH_H_

#include <stdlib.h>
#include <math.h>

#ifndef MATH_PI
# define MATH_PI 3.1415926535897932384626433832795
#endif

#ifndef MATH_SQRT2
# define MATH_SQRT2 1.4142135623730950488016887242097
#endif 

#ifndef MATH_HALFPI
# define MATH_HALFPI 1.5707963267948966192313216916395
#endif

#ifndef MATH_TWOPI
# define MATH_TWOPI 6.283185307179586476925286766558
#endif

#ifndef PIOVER180
# define PIOVER180 0.017453292519943295769236907684883
#endif

#ifndef G180OVERPI
# define G180OVERPI 57.295779513082320876798154814114
#endif

#ifndef DEGTORAD
# define DEGTORAD (float)PIOVER180
#endif

#ifndef RADTODEG
# define RADTODEG (float)G180OVERPI
#endif

#ifndef DEG_TO_RAD
# define DEG_TO_RAD(x) ( (x) * (float)PIOVER180)
#endif

#ifndef RAD_TO_DEG
# define RAD_TO_DEG(x) ( (x) * (float)G180OVERPI)
#endif

#ifndef MATH_TOLERANCE
# define MATH_TOLERANCE 0.00001f
#endif

#ifndef MATH_MAX
# define MATH_MAX(x,y) (((x)>(y))?(x):(y))
#endif

#ifndef MATH_MIN
# define MATH_MIN(x,y) (((x)<(y))?(x):(y))
#endif

namespace orv
{
	namespace math
	{

// 2D-point
typedef struct Point2D
{
	int x, y;

	Point2D():
		x(0), y(0)
	{}
}Point2D;

// 2D-point
typedef struct Point2Df
{
	float x, y;

	Point2Df():
		x(0.0f), y(0.0f)
	{}
}Point2Df;

// 3D-point
typedef struct Point3D
{
	int x, y, z;

	Point3D():
		x(0), y(0), z(0)
	{}
}Point3D;

// 3D-point
typedef struct Point3Df
{
	float x, y, z;

	Point3Df():
		x(0.0f), y(0.0f), z(0.0f)
	{}
}Point3Df;

// for Matlab
// phi - over OX - roll
// theta - over OY - pitch
// psi - over OZ - yaw
typedef struct EulerAngles
{
	float phi, theta, psi;

	EulerAngles():
	phi(0.0f), theta(0.0f), psi(0.0f)
	{}

	EulerAngles(float _phi, float _theta, float _psi):
	phi(_phi), theta(_theta), psi(_psi)
	{}

	void set(float phi_, float theta_, float psi_)
	{
		phi = phi_; theta = theta_; psi = psi_;
	}

	void set(const EulerAngles& a)
	{
		phi = a.phi; theta = a.theta; psi = a.psi;
	}

	float roll() const { return phi; }
	float pitch() const { return theta; }
	float yaw() const { return psi; }

	float g_roll() const { return phi*RADTODEG; }
	float g_pitch() const { return theta*RADTODEG; }
	float g_yaw() const { return psi*RADTODEG; }

} EulerAngles;

// Vector
typedef struct Vector3
{
	float x, y, z;

	Vector3():
	x(0.0f), y(0.0f), z(0.0f)
	{}

	Vector3(float _x, float _y, float _z):
	x(_x), y(_y), z(_z)
	{}

	void set(float x_, float y_, float z_)
	{
		x = x_; y = y_; z = z_;
	}

	void set(const Vector3& v)
	{
		x = v.x; y = v.y; z = v.z;
	}

	void scale(float s)
	{
		x *= s; y *= s; z *= s;
	}

	// норма (norm)
	float norm() const
	{
		return (x*x + y*y + z*z);
	}

	// модуль (magnitude) - "длина"
	float magnitude() const
	{
		return sqrt( norm() );
	}

	void normalise()
	{
		float in = 1.0f / magnitude();
		x = x*in; 
		y = y*in;
		z = z*in;
	}

	Vector3 operator+(const Vector3& v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(const Vector3& v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(const float& s) const
	{
		return Vector3(x*s, y*s, z*s);
	}

	// векторное умножение векторов |c|=|a|*|b|*sin(a^b)
	// из определителя
	// | i  j  k|
	// |x1 y1 z1| = (y1*z2-z1*y2)i + (z1*x2-x1*z2)j + (x1*y2-y1*x2)k
	// |x2 y2 z2|
	Vector3 operator*(const Vector3& v) const
	{
		return Vector3( y*v.z - z*v.y, 
			z*v.x - x*v.z,
			x*v.y - y*v.x);
	}

	// скалярное умножение векторов |a|*|b|*cos(a^b)
	float scalar_mul(const Vector3& v) const
	{
		return ( x*v.x + y*v.y + z*v.z );
	}

	// угол между векторами
	// из скалярного произведения 
	float angle(const Vector3& v) const
	{
		float mul = scalar_mul(v);
		return acos( mul/(magnitude()*v.magnitude()) );
	}

	// сумма элементов
	float elem_summ() const
	{
		return (x + y + z);
	}

} Vector3;

// Vector (for work with 4x4 matrix)
typedef struct Vector4
{
	float x, y, z, t;

	Vector4():
	x(0.0f), y(0.0f), z(0.0f), t(0.0f)
	{}

	Vector4(float _x, float _y, float _z, float _t):
	x(_x), y(_y), z(_z), t(_t)
	{}

	void set(float x_, float y_, float z_, float t_)
	{
		x = x_; y = y_; z = z_; t = t_;
	}

	void set(const Vector4& v)
	{
		x = v.x; y = v.y; z = v.z; t = v.t;
	}

	// норма (norm)
	float norm() const
	{
		return (x*x + y*y + z*z + t*t);
	}

	// модуль (magnitude) - "длина"
	float magnitude() const
	{
		return sqrt( norm() );
	}

	void normalise()
	{
		float in = 1.0f / magnitude();
		x = x*in; 
		y = y*in;
		z = z*in;
		t = t*in;
	}

	Vector4 operator+(const Vector4& v) const
	{
		return Vector4(x + v.x, y + v.y, z + v.z, t + v.t);
	}

	Vector4 operator-(const Vector4& v) const
	{
		return Vector4(x - v.x, y - v.y, z - v.z, t - v.t);
	}

	Vector4 operator*(const float& s) const
	{
		return Vector4(x*s, y*s, z*s, t*s);
	}

	// скалярное умножение векторов |a|*|b|*cos(a^b)
	float scalar_mul(const Vector4& v) const
	{
		return ( x*v.x + y*v.y + z*v.z + t*v.t);
	}

	// угол между векторами
	// из скалярного произведения 
	float angle(const Vector4& v) const
	{
		float mul = scalar_mul(v);
		return acos( mul/(magnitude()*v.magnitude()) );
	}

	// сумма элементов
	float elemSumm() const
	{
		return (x + y + z + t);
	}

} Vector4;

// Matrix 3x3
typedef struct Matrix33
{
	float m[3][3];

	Matrix33()
	{
		m[0][0] = m[0][1] = m[0][2] = 0.0f;
		m[1][0] = m[1][1] = m[1][2] = 0.0f;
		m[2][0] = m[2][1] = m[2][2] = 0.0f;
	}

	Matrix33(float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22)
	{
		m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
		m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
		m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
	}

	// след матрицы
	float trace() const
	{
		return ( m[0][0] + m[1][1] + m[2][2] );
	}

	// единичная матрица
	void eye()
	{
		m[0][1] = m[0][2] = 0.0f;
		m[1][0] =           m[1][2] = 0.0f;
		m[2][0] = m[2][1]           = 0.0f;
		m[0][0] = m[1][1] = m[2][2] = 1.0f;
	}

	// детерминант
	float det() const
	{
		return ( m[0][0]*m[1][1]*m[2][2] + m[0][2]*m[1][0]*m[2][1] + m[0][1]*m[1][2]*m[2][0] -
			m[0][2]*m[1][1]*m[2][0] - m[0][0]*m[1][2]*m[2][1] - m[0][1]*m[1][0]*m[2][2] );
	}

	// транспонирование
	void transpose()
	{
		// меняем местами строчки и столбцы
		float temp;
		for(int i = 0; i < 3; i++)
		{
			for(int j = i; j < 3; j++)
			{
				temp = m[i][j];
				m[i][j] = m[j][i];
				m[j][i] = temp;
			}
		}
	}

	/*
	*   Positive angle: right hand
	*   Order of euler angles: psi, then theta, then phi (Z-Y-X)
	*   matrix row column ordering:
	*   [m00 m01 m02]
	*   [m10 m11 m12]
	*   [m20 m21 m22]*/
	void rotate(float psi, float theta, float phi)
	{
		/*
			# однородные матрицы элементарных поворотов
			Rpsi := Matrix( 4, 4, [[1, 0, 0, 0], [0, cos(psi), -sin(psi), 0], [0, sin(psi), cos(psi), 0], [0, 0, 0, 1]]);
			Rtheta := Matrix( 4, 4, [[cos(theta), 0, sin(theta), 0], [0, 1, 0, 0], [-sin(theta), 0, cos(theta), 0], [0, 0, 0, 1]]);
			Rphi := Matrix( 4, 4, [[cos(phi), -sin(phi), 0, 0], [sin(phi), cos(phi), 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]]);
			
			# матрица вращения
			R := Rphi.Rtheta.Rpsi;

			# R := "Matrix(4, 4, [[cos(theta)*cos(phi),-cos(psi)*sin(phi)+sin(psi)*sin(theta)*cos(phi),sin(psi)*sin(phi)+cos(psi)*sin(theta)*cos(phi),0],[cos(theta)*sin(phi),cos(psi)*cos(phi)+sin(psi)*sin(theta)*sin(phi),-sin(psi)*cos(phi)+cos(psi)*sin(theta)*sin(phi),0],[-sin(theta),sin(psi)*cos(theta),cos(psi)*cos(theta),0],[0,0,0,1]], datatype = anything, storage = rectangular, order = Fortran_order, shape = [])";
		*/

		// Assuming the angles are in grad

		phi = phi * (float)PIOVER180;
		theta = theta * (float)PIOVER180;
		psi = psi * (float)PIOVER180;

		// [R] = ([Rpsi][Rtheta])[Rphi]
		// Rpsi (rotate about Z) - Yaw
		// Rtheta (rotate about Y) - Pitch
		// Rphi (rotate about X) - Roll

		float cf = cos(phi);
		float sf = sin(phi);
		float ct = cos(theta);
		float st = sin(theta);
		float cp = cos(psi);
		float sp = sin(psi);

		m[0][0] = cf*ct;
		m[0][1] = cf*st*sp - sf*cp;
		m[0][2] = cf*st*cp + sf*sp;
		m[1][0] = sf*ct;
		m[1][1] = sf*st*sp + cf*cp;
		m[1][2] = sf*st*cp - cf*sp;
		m[2][0] = -st;
		m[2][1] = ct*sp;
		m[2][2] = ct*cp;
	}

	Matrix33 operator+(const Matrix33& mat) const
	{
		return Matrix33(m[0][0] + mat.m[0][0], m[0][1] + mat.m[0][1], m[0][2] + mat.m[0][2], 
						m[1][0] + mat.m[1][0], m[1][1] + mat.m[1][1], m[1][2] + mat.m[1][2], 
						m[2][0] + mat.m[2][0], m[2][1] + mat.m[2][1], m[2][2] + mat.m[2][2]);
	}

	Matrix33 operator-(const Matrix33& mat) const
	{
		return Matrix33(m[0][0] - mat.m[0][0], m[0][1] - mat.m[0][1], m[0][2] - mat.m[0][2], 
						m[1][0] - mat.m[1][0], m[1][1] - mat.m[1][1], m[1][2] - mat.m[1][2], 
						m[2][0] - mat.m[2][0], m[2][1] - mat.m[2][1], m[2][2] - mat.m[2][2]);
	}

	Matrix33 operator*(const float& s) const
	{
		return Matrix33(m[0][0] * s, m[0][1] * s, m[0][2] * s, 
						m[1][0] * s, m[1][1] * s, m[1][2] * s, 
						m[2][0] * s, m[2][1] * s, m[2][2] * s);
	}

	Vector3 operator*(const Vector3& v) const
	{
		return Vector3( m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
						m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
						m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z );
	}

	Matrix33 operator*(const Matrix33& mat) const
	{
		/*
		*   [m00 m01 m02]
		*   [m10 m11 m12]
		*   [m20 m21 m22]*/
		// строка на столбец
		return Matrix33( m[0][0]*mat.m[0][0] + m[0][1]*mat.m[1][0] + m[0][2]*mat.m[2][0],
						 m[0][0]*mat.m[0][1] + m[0][1]*mat.m[1][1] + m[0][2]*mat.m[2][1],
						 m[0][0]*mat.m[0][2] + m[0][1]*mat.m[1][2] + m[0][2]*mat.m[2][2],
						 //-
						 m[1][0]*mat.m[0][0] + m[1][1]*mat.m[1][0] + m[1][2]*mat.m[2][0],
						 m[1][0]*mat.m[0][1] + m[1][1]*mat.m[1][1] + m[1][2]*mat.m[2][1],
						 m[1][0]*mat.m[0][2] + m[1][1]*mat.m[1][2] + m[1][2]*mat.m[2][2],
						 //-
						 m[2][0]*mat.m[0][0] + m[2][1]*mat.m[1][0] + m[2][2]*mat.m[2][0],
						 m[2][0]*mat.m[0][1] + m[2][1]*mat.m[1][1] + m[2][2]*mat.m[2][1],
						 m[2][0]*mat.m[0][2] + m[2][1]*mat.m[1][2] + m[2][2]*mat.m[2][2] );
	}

	// сумма элементов
	float elemSumm() const
	{
		return (m[0][0] + m[0][1] + m[0][2] + 
				m[1][0] + m[1][1] + m[1][2] + 
				m[2][0] + m[2][1] + m[2][2]);
	}

	// получить углы из DCM
	// yaw - рыскание - phi - OZ
	// pitch - тангаж - theta - OY
	// roll - крен - psi - OX
	void GetEuler(float &yaw, float &pitch, float &roll)
	{
		yaw = atan2( m[1][2], m[2][2] );
		pitch = asin( - m[0][2] );
		roll = atan2( m[0][1], m[0][0] ); 

		roll = RAD_TO_DEG(roll);
		pitch = RAD_TO_DEG(pitch);
		yaw = RAD_TO_DEG(yaw);
	}

} Matrix33;

// Matrix 4x4
typedef struct Matrix44
{
	float m[4][4];

	Matrix44()
	{
		m[0][0] = m[0][1] = m[0][2] = m[0][3] = 0.0f;
		m[1][0] = m[1][1] = m[1][2] = m[1][3] = 0.0f;
		m[2][0] = m[2][1] = m[2][2] = m[2][3] = 0.0f;
		m[3][0] = m[3][1] = m[3][2] = m[3][3] = 0.0f;
	}

	Matrix44(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
		m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
		m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
		m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
	}

	Matrix44(Matrix33& m3)
	{
		m[0][0] = m3.m[0][0]; m[0][1] = m3.m[0][1]; m[0][2] = m3.m[0][2]; m[0][3] = 0.0f;
		m[1][0] = m3.m[1][0]; m[1][1] = m3.m[1][1]; m[1][2] = m3.m[1][2]; m[1][3] = 0.0f;
		m[2][0] = m3.m[2][0]; m[2][1] = m3.m[2][1]; m[2][2] = m3.m[2][2]; m[2][3] = 0.0f;
		m[3][0] = m[3][1] = m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	// след матрицы
	float trace() const
	{
		return ( m[0][0] + m[1][1] + m[2][2] + m[3][3] );
	}

	// единичная матрица
	float eye()
	{
		m[0][1] = m[0][2] = m[0][3] = 0.0f;
		m[1][0] =           m[1][2] = m[1][3] = 0.0f;
		m[2][0] = m[2][1] =           m[2][3] = 0.0f;
		m[3][0] = m[3][1] = m[3][2]           = 0.0f;
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
	}

	// детерминант
	float det() const
	{
		// разложение по первой сроке
		//
		//  n
		// ----
		// \         1+i
		// /    ( -1 )  * a  * M
		// ----            i1   i1
		// k=1
		//
		Matrix33 M01(m[1][1], m[1][2], m[1][3],
			         m[2][1], m[2][2], m[2][3], 
					 m[3][1], m[3][2], m[3][3]);
		Matrix33 M11(m[1][0], m[1][2], m[1][3],
			         m[2][0], m[2][2], m[2][3], 
					 m[3][0], m[3][2], m[3][3]);
		Matrix33 M21(m[1][0], m[1][1], m[1][3],
			         m[2][0], m[2][1], m[2][3], 
					 m[3][0], m[3][1], m[3][3]);
		Matrix33 M31(m[1][0], m[1][1], m[1][2],
			         m[2][0], m[2][1], m[2][2], 
					 m[3][0], m[3][1], m[3][2]);
		float det = m[0][0]*M01.det() - m[0][1]*M11.det() + m[0][2]*M21.det() - m[0][3]*M31.det();
		return det;
	}

	// транспонирование
	void transpose()
	{
		// меняем местами строчки и столбцы
		float temp;
		for(int i = 0; i < 4; i++)
		{
			for(int j = i; j < 4; j++)
			{
				temp = m[i][j];
				m[i][j] = m[j][i];
				m[j][i] = temp;
			}
		}
	}

	Matrix44 operator+(const Matrix44& mat) const
	{
		return Matrix44(m[0][0] + mat.m[0][0], m[0][1] + mat.m[0][1], m[0][2] + mat.m[0][2], m[0][3] - mat.m[0][3],
						m[1][0] + mat.m[1][0], m[1][1] + mat.m[1][1], m[1][2] + mat.m[1][2], m[1][3] - mat.m[1][3],
						m[2][0] + mat.m[2][0], m[2][1] + mat.m[2][1], m[2][2] + mat.m[2][2], m[2][3] - mat.m[2][3],
						m[3][0] + mat.m[3][0], m[3][1] + mat.m[3][1], m[3][2] + mat.m[3][2], m[3][3] - mat.m[3][3]);
	}

	Matrix44 operator-(const Matrix44& mat) const
	{
		return Matrix44(m[0][0] - mat.m[0][0], m[0][1] - mat.m[0][1], m[0][2] - mat.m[0][2], m[0][3] - mat.m[0][3],
						m[1][0] - mat.m[1][0], m[1][1] - mat.m[1][1], m[1][2] - mat.m[1][2], m[1][3] - mat.m[1][3],
						m[2][0] - mat.m[2][0], m[2][1] - mat.m[2][1], m[2][2] - mat.m[2][2], m[2][3] - mat.m[2][3],
						m[3][0] - mat.m[3][0], m[3][1] - mat.m[3][1], m[3][2] - mat.m[3][2], m[3][3] - mat.m[3][3]);
	}

	Matrix44 operator*(const float& s) const
	{
		return Matrix44(m[0][0] * s, m[0][1] * s, m[0][2] * s, m[0][3] * s,
						m[1][0] * s, m[1][1] * s, m[1][2] * s, m[1][3] * s,
						m[2][0] * s, m[2][1] * s, m[2][2] * s, m[2][3] * s,
						m[3][0] * s, m[3][1] * s, m[3][2] * s, m[3][3] * s);
	}

	Vector4 operator*(const Vector4& v) const
	{
		return Vector4( m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3]*v.t,
						m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3]*v.t,
						m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]*v.t,
						m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3]*v.t );
	}

	Matrix44 operator*(const Matrix44& mat) const
	{
		/*
		[m00 m01 m02 m03]
		[m10 m11 m12 m13]
		[m20 m21 m22 m23]
		[m30 m31 m32 m33]*/
		// строка на столбец
		return Matrix44( m[0][0]*mat.m[0][0] + m[0][1]*mat.m[1][0] + m[0][2]*mat.m[2][0] + m[0][3]*mat.m[3][0],
						 m[0][0]*mat.m[0][1] + m[0][1]*mat.m[1][1] + m[0][2]*mat.m[2][1] + m[0][3]*mat.m[3][1],
						 m[0][0]*mat.m[0][2] + m[0][1]*mat.m[1][2] + m[0][2]*mat.m[2][2] + m[0][3]*mat.m[3][2],
						 m[0][0]*mat.m[0][3] + m[0][1]*mat.m[1][3] + m[0][2]*mat.m[2][3] + m[0][3]*mat.m[3][3],
						 //-
						 m[1][0]*mat.m[0][0] + m[1][1]*mat.m[1][0] + m[1][2]*mat.m[2][0] + m[1][3]*mat.m[3][0],
						 m[1][0]*mat.m[0][1] + m[1][1]*mat.m[1][1] + m[1][2]*mat.m[2][1] + m[1][3]*mat.m[3][1],
						 m[1][0]*mat.m[0][2] + m[1][1]*mat.m[1][2] + m[1][2]*mat.m[2][2] + m[1][3]*mat.m[3][2],
						 m[1][0]*mat.m[0][3] + m[1][1]*mat.m[1][3] + m[1][2]*mat.m[2][3] + m[1][3]*mat.m[3][3],
						 //-
						 m[2][0]*mat.m[0][0] + m[2][1]*mat.m[1][0] + m[2][2]*mat.m[2][0] + m[2][3]*mat.m[3][0],
						 m[2][0]*mat.m[0][1] + m[2][1]*mat.m[1][1] + m[2][2]*mat.m[2][1] + m[2][3]*mat.m[3][1],
						 m[2][0]*mat.m[0][2] + m[2][1]*mat.m[1][2] + m[2][2]*mat.m[2][2] + m[2][3]*mat.m[3][2],
						 m[2][0]*mat.m[0][3] + m[2][1]*mat.m[1][3] + m[2][2]*mat.m[2][3] + m[2][3]*mat.m[3][3],
						 //
						 m[3][0]*mat.m[0][0] + m[3][1]*mat.m[1][0] + m[3][2]*mat.m[2][0] + m[3][3]*mat.m[3][0],
						 m[3][0]*mat.m[0][1] + m[3][1]*mat.m[1][1] + m[3][2]*mat.m[2][1] + m[3][3]*mat.m[3][1],
						 m[3][0]*mat.m[0][2] + m[3][1]*mat.m[1][2] + m[3][2]*mat.m[2][2] + m[3][3]*mat.m[3][2],
						 m[3][0]*mat.m[0][3] + m[3][1]*mat.m[1][3] + m[3][2]*mat.m[2][3] + m[3][3]*mat.m[3][3] );
	}

	// сумма элементов
	float elemSumm() const
	{
		return (m[0][0] + m[0][1] + m[0][2] + m[0][3] +
				m[1][0] + m[1][1] + m[1][2] + m[1][3] + 
				m[2][0] + m[2][1] + m[2][2] + m[2][3] +
				m[3][0] + m[3][1] + m[3][2] + m[3][3]);
	}

} Matrix44;

// Кватернионы в программировании игр.
// http://wat.gamedev.ru/articles/quaternions

// Quaternion
typedef struct Quaternion
{
	float x, y, z;	// вектор
	float w;		// скаляр

	Quaternion():
	x(0.0f), y(0.0f), z(0.0f), w(1.0f)
	{}

	Quaternion(float _x, float _y, float _z, float _w):
	x(_x), y(_y), z(_z), w(_w)
	{}

	void set(float x_, float y_, float z_, float w_)
	{
		x = x_; y = y_; z = z_; w = w_;
	}

	void set(const Quaternion& q)
	{
		x = q.x; y = q.y; z = q.z; w = q.w;
	}

	// сопряжение ( conjugate )
	void conjugate()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	// получить сопряжённый кватернион
	Quaternion getConjugate() const
	{
		return Quaternion(-x, -y, -z, w);
	}

	// тождественный кватернион: q[0, 0, 0, 1]
	// описывает нулевой поворот и не изменяет другой кватернион при умножении.
	void ident()
	{
		x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f;
	}

	void scale(float s)
	{
		x *= s; y *= s; z *= s; w *= s;
	}

	float norm() const
	{
		return (x*x + y*y + z*z + w*w);
	}

	// модуль (magnitude) - "длина" кватерниона
	float magnitude() const
	{
		return sqrt( norm() );
	}

	void normalise()
	{
		float in = 1.0f / magnitude();
		x = x*in; 
		y = y*in;
		z = z*in;
		w = w*in;
	}

	// получить инверсный (inverse) кватернион
	void invert()
	{
		float in = 1.0f / norm();
		x = -x*in;
		y = -y*in;
		z = -z*in;
		w = w*in;
	}

	Quaternion inverted() const
	{
		float in = 1.0f / norm();
		return Quaternion( -x*in, -y*in, -z*in, w*in );
	}

	Quaternion& operator=(const Quaternion &q)
	{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;

		return *this;
	}

	Quaternion operator+(const Quaternion& q) const
	{
		return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
	}

	Quaternion operator-(const Quaternion& q) const
	{
		return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
	}

	Quaternion operator*(const float& s) const
	{
		return Quaternion(x*s, y*s, z*s, w*s);
	}

	// Multiplying q1 with q2 applies the rotation q2 to q1
	// ! Quaternion-multiplication is NOT commutative: q1 * q2 is not the same as q2 * q1 !
	Quaternion operator*(const Quaternion& q) const
	{
		// the constructor takes its arguments as (x, y, z, w)
		return Quaternion(w*q.x + x*q.w + y*q.z - z*q.y,
			w*q.y + y*q.w + z*q.x - x*q.z,
			w*q.z + z*q.w + x*q.y - y*q.x,
			w*q.w - x*q.x - y*q.y - z*q.z);
		// 16 multiplications    12 addidtions
	}

	const Quaternion& operator+=(const Quaternion& q)
	{
		x += q.x; y += q.y; z += q.z; w += q.w;
		return *this;
	}

	const Quaternion& operator-=(const Quaternion& q)
	{
		x -= q.x; y -= q.y; z -= q.z; w -= q.w;
		return *this;
	}

	const Quaternion& operator*=(const Quaternion& q)
	{
		set((*this)*q);
	}

	// rotate vector by quaternion
	Vector3 rotate(const Vector3& v) const 
	{
		Quaternion q(v.x * w + v.z * y - v.y * z,
			v.y * w + v.x * z - v.z * x,
			v.z * w + v.y * x - v.x * y,
			v.x * x + v.y * y + v.z * z);

		return Vector3(w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y + y * q.w + z * q.x - x * q.z,
			w * q.z + z * q.w + x * q.y - y * q.x)*( 1.0f/norm() );
	}

	// Multiplying a quaternion q with a vector v applies the q-rotation to v
	Vector3 operator* (const Vector3 &vec) const
	{
		return rotate(vec);
	}

	// return rotation angle from -PI to PI 
	float getRotation() const
	{
		if( w < 0 )
			return 2.0f*atan2(-sqrt( x*x + y*y + z*z ), -w );
		else
			return 2.0f*atan2( sqrt( x*x + y*y + z*z ),  w );
	}

	//  quaternion from axis angle representation
	void fromAxisAngle(const Vector3& axis, const float& angle)
	{
		Vector3 v(axis);
		v.normalise();
		float half_angle = angle*0.5f;
		float sin_a = sin(half_angle);
		set(v.x*sin_a, v.y*sin_a, v.z*sin_a, cos(half_angle));
	}

	// convert a quaternion to axis angle representation, 
	// preserve the axis direction and angle from -PI to +PI
	void toAxisAngle(Vector3& axis, float& angle) const
	{
		float vl = sqrt( x*x + y*y + z*z );
		if( vl > MATH_TOLERANCE )
		{
			float ivl = 1.0f/vl;
			axis.set( x*ivl, y*ivl, z*ivl );
			if( w < 0 )
				angle = 2.0f*atan2(-vl, -w); //-PI,0 
			else
				angle = 2.0f*atan2( vl,  w); //0,PI 
		}
		else
		{
			axis = Vector3(0, 0, 0);
			angle = 0;
		}
	}

	// Convert to Axis/Angles
	void getAxisAngle(Vector3 &axis, float &angle) const
	{
		float in = 1 / magnitude();
		axis.x = x * in;
		axis.y = y * in;
		axis.z = z * in;
		angle = acos(w) * 2.0f;
	}


	// Convert from Euler Angles (values in angles)
	// Qyaw*Qpitch*Qroll (Z-Y-X) (psi-theta-phi)
	void FromEuler(float yaw, float pitch, float roll)
	{
		// Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll
		// and multiply those together.
		// the calculation below does the same, just shorter

		float p = pitch * (float)PIOVER180 * 0.5f;
		float y = yaw * (float)PIOVER180 * 0.5f;
		float r = roll * (float)PIOVER180 * 0.5f;

		float sinp = sin(p);
		float siny = sin(y);
		float sinr = sin(r);
		float cosp = cos(p);
		float cosy = cos(y);
		float cosr = cos(r);

		this->x = sinr * cosp * cosy - cosr * sinp * siny;
		this->y = cosr * sinp * cosy + sinr * cosp * siny;
		this->z = cosr * cosp * siny - sinr * sinp * cosy;
		this->w = cosr * cosp * cosy + sinr * sinp * siny;

		normalise();
	}

	// Get Euler angles from quaternion
	void GetEuler(float &yaw, float &pitch, float &roll) const
	{
		float sqw = w*w;
		float sqx = x*x;
		float sqy = y*y;
		float sqz = z*z;

		float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor

	#if 1
		roll = atan2( 2*(w*x + y*z), (sqw-sqx-sqy+sqz) );
		//pitch = asin( 2*(w*y-z*x) );
		yaw = atan2( 2*(w*z + x*y), (sqw+sqx-sqy-sqz) );
		float t = 2*(x*z-w*y);
		float t2 = sqrt( fabs(1.0f - t*t) );
		if( fabs(1.0f - t*t) < MATH_TOLERANCE )
			t2 = 0;
		if(t*t>1){
			if(2*(w*y-z*x)>1){ // north pole
				pitch = (float)MATH_PI*0.5f;
			}
			else if(2*(w*y-z*x)<-1){ // south pole
				pitch = - (float)MATH_PI*0.5f;
			}
		}
		else
			pitch = -atan2(t, t2);
	#else
		roll = atan2( 2*(w*x + y*z), 1-2*(sqx + sqy) );
		pitch = asin( 2*(w*y-z*x) );
		yaw = atan2( 2*(w*z + x*y), 1-2*(sqy + sqz) );
	#endif

		roll = RAD_TO_DEG(roll);
		pitch = RAD_TO_DEG(pitch);
		yaw = RAD_TO_DEG(yaw);
	}

	// Convert to Matrix
	Matrix44 getMatrix() const
	{
		float x2 = x * x;
		float y2 = y * y;
		float z2 = z * z;
		float xy = x * y;
		float xz = x * z;
		float yz = y * z;
		float wx = w * x;
		float wy = w * y;
		float wz = w * z;

		// This calculation would be a lot more complicated for non-unit length quaternions
		// Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
		//   OpenGL
		/*
		m1 m5 m09 m13
		m2 m6 m10 m14
		m3 m7 m11 m15
		m4 m8 m12 m16 */
		return Matrix44( 1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
			2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
			2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Convert quaternion to direction cosine matrix. 
	Matrix33 getDCM() const
	{
		Quaternion q (x, y, z, w);
		q.normalise();

		float x2 = q.x * q.x;
		float y2 = q.y * q.y;
		float z2 = q.z * q.z;
		float w2 = q.w * q.w;
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float yz = q.y * q.z;
		float wx = q.w * q.x;
		float wy = q.w * q.y;
		float wz = q.w * q.z;

		//
		// P' = q.P.(q*)
		// DCM = transpose(P')
		//
		return Matrix33( w2+x2-y2-z2, 2*(xy+wz), 2*(xz-wy),
			             2*(xy-wz), w2-x2+y2-z2, 2*(yz+wx),
						 2*(xz+wy), 2*(yz-wx), w2-x2-y2+z2 );
	}


} Quaternion;


//
// constants
//
const float PI = (float)MATH_PI;
const float Sqrt2 = (float)MATH_SQRT2;

// Matlab methods

Quaternion quatconj(const Quaternion& q);
float quatnorm(const Quaternion& q);
float quatmod(const Quaternion& q);
Quaternion quatnormalize(const Quaternion& q);
Quaternion quatinv(const Quaternion& q);
Quaternion quatmultiply(const Quaternion& q, const Quaternion& r);
Quaternion quatdivide(const Quaternion& q, const Quaternion& r);
Matrix33 quat2dcm(const Quaternion& q);
Vector3 quatrotate(const Quaternion& q, const Vector3& v);
EulerAngles quat2euler(const Quaternion& q);

float deg2rad(const float val);
float rad2deg(const float val);

// Signum function
int sign(int x);
float sign(float x);

// Error function
float erf(float x);

// Normal probability density function
float normpdf(float x, float mu=0.0f, float sigma=1.0f);

	}; //namespace math
}; //namespace orv

#endif //#ifndef _ORV_MATH_H_
