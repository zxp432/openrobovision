//
// OpenRoboVision
//
// test math functions
//
// тестирование математических функций
//
//
// robocraft.ru
//

#include "gtest/gtest.h"

#include "orv/robo/orv_math.h"
using namespace orv::math;

TEST(Math, Generic)
{

	float pi = rad2deg(MATH_PI);
	EXPECT_EQ(180, pi);

	float g180 = deg2rad(180.0f);
	EXPECT_NEAR(MATH_PI, g180, MATH_TOLERANCE);

	float r = atan2(1.0f, 0.0f);
	float g = rad2deg( r );
	EXPECT_EQ(90, g);

	r = atan2(-1.0f, 0.0f);
	g = rad2deg( r );
	EXPECT_EQ(-90, g);

	r = atan2(0.0f, 1.0f);
	g = rad2deg( r );
	EXPECT_EQ(0, g);

	r = atan2(0.0f, 0.0f);
	g = rad2deg( r );
	EXPECT_EQ(0, g);
}

TEST(Vector3, Generic)
{
	Vector3 x0(1, 0, 0);
	Vector3 y0(0, 1, 0);
	float ang = x0.angle(y0);
	ang = RAD_TO_DEG(ang);
	EXPECT_EQ(90, ang);

	Vector3 x1(1, 0, 0);
	Vector3 y1(1, 0, 1);
	float ang1 = x1.angle(y1);
	ang1 = RAD_TO_DEG(ang1);
	EXPECT_EQ(45, ang1);

	/***
	#Maple
	x1 := Vector([1, 0, 0]);
	y1 := Vector([1, 0, 1]);
	with(LinearAlgebra):
	VectorAngle(x1, y1);
	# 1/4*Pi
	******/

	Vector3 z1 = x1*y1;
	float x1n = x1.norm();
	float y1n = y1.norm();
	float z1n = z1.norm();
	float ang2 = asin( z1.magnitude()/(x1.magnitude()*y1.magnitude()) );
	ang2 = RAD_TO_DEG(ang2);
	EXPECT_EQ(45, ang2);

	EXPECT_EQ(ang1, ang2);
}

TEST(Matrix, Generic)
{
	Matrix33 mat;
	mat.m[0][0]=1;
	mat.m[0][1]=2;
	mat.m[0][2]=3;

	mat.m[1][0]=4;
	mat.m[1][1]=5;
	mat.m[1][2]=6;

	mat.m[2][0]=7;
	mat.m[2][1]=8;
	mat.m[2][2]=9;
	//----------------------------------------------
	float tr = mat.trace();
	EXPECT_EQ(15, tr);
	//----------------------------------------------
	mat.transpose();
	EXPECT_EQ(3, mat.m[2][0]);
	EXPECT_EQ(6, mat.m[2][1]);
	EXPECT_EQ(9, mat.m[2][2]);

	mat.transpose();
	EXPECT_EQ(7, mat.m[2][0]);
	EXPECT_EQ(8, mat.m[2][1]);
	EXPECT_EQ(9, mat.m[2][2]);
	//----------------------------------------------
	mat.eye();
	EXPECT_EQ(1, mat.m[1][1]);
	EXPECT_EQ(0, mat.m[2][1]);
	EXPECT_EQ(1, mat.m[2][2]);
	//----------------------------------------------
	float det = mat.det();
	EXPECT_EQ(1, det);

	Matrix33 magic3(8, 1, 6, 3, 5, 7, 4, 9, 2);
	det = magic3.det();
	EXPECT_EQ(-360, det);

	Matrix44 magic4(16, 2, 3, 13, 5, 11, 10, 8, 9, 7, 6, 12, 4, 14, 15, 1);
	det = magic4.det();
	EXPECT_EQ(0, det);
	//----------------------------------------------
	Matrix44 mat4(11, 12, 13, 14, 21, 22, 23, 24, 31, 32, 33, 34, 41, 42, 43, 44);
	mat4.transpose();
	EXPECT_EQ(13, mat4.m[2][0]);
	EXPECT_EQ(23, mat4.m[2][1]);
	EXPECT_EQ(33, mat4.m[2][2]);
	EXPECT_EQ(43, mat4.m[2][3]);
}

// проверка умножения матриц
TEST(Matrix, Multiplication)
{
	// умножение матрицы на вектор
	/*
	v=[2;3;4]
	magic3 = magic(3)
	%magic3 =
    %	8     1     6
    %	3     5     7
    %	4     9     2
	magic3*v
	%ans =
    %	43
    %	49
    %	43
	*/
	Matrix33 magic3(8, 1, 6, 3, 5, 7, 4, 9, 2);
	Vector3 v3(2, 3, 4);
	Vector3 m3v3 = magic3*v3;
	EXPECT_EQ(43, m3v3.x);
	EXPECT_EQ(49, m3v3.y);
	EXPECT_EQ(43, m3v3.z);
	// проверка умножения матриц
	/*
	t = [1, 2, 3; 4, 5, 6; 7, 8, 9]
	e = eye(3)
	t*e
	*/
	Matrix33 t(1, 2, 3, 4, 5, 6, 7, 8, 9);
	Matrix33 e;
	e.eye();
	Matrix33 te = t*e;
	EXPECT_EQ(1, te.m[0][0]);
	EXPECT_EQ(2, te.m[0][1]);
	EXPECT_EQ(3, te.m[0][2]);
	EXPECT_EQ(9, te.m[2][2]);
	/*
	t*magic3
	%ans =
	%	26    38    26
	%	71    83    71
	%	116   128   116
	*/
	Matrix33 tm = t*magic3;
	EXPECT_EQ(26, tm.m[0][0]);
	EXPECT_EQ(38, tm.m[0][1]);
	EXPECT_EQ(26, tm.m[0][2]);
	EXPECT_EQ(71, tm.m[1][0]);
	EXPECT_EQ(83, tm.m[1][1]);
	EXPECT_EQ(71, tm.m[1][2]);
	EXPECT_EQ(116, tm.m[2][0]);
	EXPECT_EQ(128, tm.m[2][1]);
	EXPECT_EQ(116, tm.m[2][2]);
	//-------------------------
	Matrix44 t4(11, 12, 13, 14, 21, 22, 23, 24, 31, 32, 33, 34, 41, 42, 43, 44);
	Matrix33 e4;
	e4.eye();
	Matrix44 te4 = t4*e4;
	EXPECT_EQ(11, te4.m[0][0]);
	EXPECT_EQ(12, te4.m[0][1]);
	EXPECT_EQ(13, te4.m[0][2]);
	EXPECT_EQ(44, te4.m[3][3]);
	/*
	t4 = [11, 12, 13, 14; 21, 22, 23, 24; 31, 32, 33, 34; 41, 42, 43, 44]
	magic4 = magic(4)
	%magic4 =
    %	16     2     3    13
    %	5    11    10     8
    %	9     7     6    12
    %	4    14    15     1
	t4*magic4
	%ans =
    %    409         441         441         409
    %    749         781         781         749
    %   1089        1121        1121        1089
    %   1429        1461        1461        1429
	*/
	Matrix44 magic4(16, 2, 3, 13, 5, 11, 10, 8, 9, 7, 6, 12, 4, 14, 15, 1);
	Matrix44 t4m4 = t4*magic4;
	EXPECT_EQ(409, t4m4.m[0][0]);
	EXPECT_EQ(441, t4m4.m[0][1]);
	EXPECT_EQ(441, t4m4.m[0][2]);
	EXPECT_EQ(409, t4m4.m[0][3]);
	EXPECT_EQ(749, t4m4.m[1][0]);
	EXPECT_EQ(781, t4m4.m[1][1]);
	EXPECT_EQ(781, t4m4.m[1][2]);
	EXPECT_EQ(749, t4m4.m[1][3]);
	EXPECT_EQ(1089, t4m4.m[2][0]);
	EXPECT_EQ(1121, t4m4.m[2][1]);
	EXPECT_EQ(1121, t4m4.m[2][2]);
	EXPECT_EQ(1089, t4m4.m[2][3]);
	EXPECT_EQ(1429, t4m4.m[3][0]);
	EXPECT_EQ(1461, t4m4.m[3][1]);
	EXPECT_EQ(1461, t4m4.m[3][2]);
	EXPECT_EQ(1429, t4m4.m[3][3]);

	// на вектор
	/*
	v4=[2;3;4;5]
	magic4*v4
	%ans =
	%	115
	%	123
	%	123
	%	115
	*/
	Vector4 v4(2, 3, 4, 5);
	Vector4 m4v4 = magic4*v4;
	EXPECT_EQ(115, m4v4.x);
	EXPECT_EQ(123, m4v4.y);
	EXPECT_EQ(123, m4v4.z);
	EXPECT_EQ(115, m4v4.t);
}

TEST(Quaternion, Basic)
{
	Quaternion q, qc, q2, q3;
	q.ident();

	Vector3 v(1, 0, 0);
	Vector3 v2 = q*v;
	EXPECT_EQ(1, v2.x);
	EXPECT_EQ(0, v2.y);
	EXPECT_EQ(0, v2.z);

	// v = QvQ*
	v2 = q*v;
	qc = q.getConjugate();
	v2 = qc*v2;
	EXPECT_EQ(1, v2.x);
	EXPECT_EQ(0, v2.y);
	EXPECT_EQ(0, v2.z);

	q.FromEuler(90, 0, 0); // yaw - OZ
	v2 = q*v;
	EXPECT_EQ(0, v2.x);
	EXPECT_NEAR(1, v2.y, MATH_TOLERANCE); //EXPECT_EQ(1, v2.y);
	EXPECT_EQ(0, v2.z);

	v2 = q*v;
	qc = q.getConjugate();
	v2 = qc*v2;
	EXPECT_NEAR(1, v2.x, MATH_TOLERANCE); //EXPECT_EQ(1, v2.x);
	EXPECT_EQ(0, v2.y);
	EXPECT_EQ(0, v2.z);

	q.ident();
	q.FromEuler(20, 30, 90);
	qc = q.getConjugate();
	q2 = q*qc;
	EXPECT_EQ(0, q2.x);
	EXPECT_EQ(0, q2.y);
	EXPECT_EQ(0, q2.z);
	EXPECT_NEAR(1, q2.w, MATH_TOLERANCE);

	q.ident();
	q.FromEuler(90, 0, 0);
	qc = q.inverted();
	q2.set(v.x, v.y, v.z, 0);
	// поворот вектора кватернионом: v' = q.v.q*
	q3 = q*q2*qc;
	EXPECT_EQ(0, q3.x);
	EXPECT_NEAR(1, q3.y, MATH_TOLERANCE);
	EXPECT_EQ(0, q3.z);
}

TEST(Quaternion, Multiplication)
{

	/*
	q = [1 0 0 1];
	q2 = [1 1 0 0];
	quatmultiply(q, q2)
	%ans =
    %	1     1     1     1
	*/
	Quaternion q(0, 0, 1, 1);
	Quaternion q2(1, 0, 0, 1);

	Quaternion qres = q*q2;
	EXPECT_EQ(1, qres.x);
	EXPECT_EQ(1, qres.y);
	EXPECT_EQ(1, qres.z);
	EXPECT_EQ(1, qres.w);

	/*
	q3 = [1 1 1 0];
	quatmultiply(q, q3)
	%ans =
    %	1     0     2     1
	*/
	Quaternion q3(1, 1, 0, 1);

	qres = q*q3;
	EXPECT_EQ(0, qres.x);
	EXPECT_EQ(2, qres.y);
	EXPECT_EQ(1, qres.z);
	EXPECT_EQ(1, qres.w);
}

TEST(normpdf, Generic)
{
	float y=0.0f;
	/*
	normpdf(1, 0, 1)
	%ans =
    %	0.2420
	*/
	y = normpdf(1.0f, 0, 1.0f);
	EXPECT_NEAR(0.242, y, 0.001f);
	/*
	normpdf(8, 0, 10)
	%ans =
    %	0.0290
	*/
	y = normpdf(8.0f, 0, 10.0f);
	EXPECT_NEAR(0.0290, y, 0.01f);

}

TEST(sign, Generic)
{
	float y = sign(100);
	EXPECT_NEAR(1, y, MATH_TOLERANCE);
	y = sign(-100);
	EXPECT_NEAR(-1, y, MATH_TOLERANCE);
	y = sign(0.1f);
	EXPECT_NEAR(1, y, MATH_TOLERANCE);
	y = sign(MATH_TOLERANCE/10.0f);
	EXPECT_NEAR(0, y, MATH_TOLERANCE);
}

TEST(erf, Generic)
{
	/*
	erf(1)
	%ans =
	%	0.8427
	*/
	float y = erf(1.0f);
	EXPECT_NEAR(0.8427, y, 0.001f);
	y = erf(10.0f);
	EXPECT_NEAR(1, y, 0.001f);
}
