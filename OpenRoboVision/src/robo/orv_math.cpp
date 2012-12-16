//
// OpenRoboVision
//
// own math type
//
// собственные математические типы
//
//
// robocraft.ru
//

#include "orv/robo/orv_math.h"

namespace orv
{
	namespace math
	{

		// Calculate the conjugate of a quaternion.
		Quaternion quatconj(const Quaternion& q)
		{
			return Quaternion( -q.x, -q.y, -q.z, q.w );
		}

		// Calculate the norm of a quaternion.
		float quatnorm(const Quaternion& q)
		{
			return (q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
		}

		// Calculate the modulus of a quaternion.
		float quatmod(const Quaternion& q)
		{
			return sqrt(quatnorm( q ));
		}

		// Normalize a quaternion.
		Quaternion quatnormalize(const Quaternion& q)
		{
			float mod = quatmod( q );
			return Quaternion( q.x/mod, q.y/mod, q.z/mod, q.w/mod );
		}

		// Calculate the inverse of a quaternion.
		Quaternion quatinv(const Quaternion& q)
		{
			float norm = quatnorm( q );
			Quaternion qinv = quatconj( q );
			qinv.x /= norm;
			qinv.y /= norm;
			qinv.z /= norm;
			qinv.w /= norm;
			return qinv;
		}

		// Calculate the product of two quaternions. 
		Quaternion quatmultiply(const Quaternion& q, const Quaternion& r)
		{
			float r0 = r.w;
			float r1 = r.x;
			float r2 = r.y;
			float r3 = r.z;

			float q0 = q.w;
			float q1 = q.x;
			float q2 = q.y;
			float q3 = q.z;

			float t0 = r0*q0 - r1*q1 - r2*q2 - r3*q3;
			float t1 = r0*q1 + r1*q0 - r2*q3 + r3*q2;
			float t2 = r0*q2 + r1*q3 + r2*q0 - r3*q1;
			float t3 = r0*q3 - r1*q2 + r2*q1 + r3*q0;

			return Quaternion( t1, t2, t3, t0 );

		}

		// Divide a quaternion by another quaternion.
		Quaternion quatdivide(const Quaternion& q, const Quaternion& r)
		{
			float r0 = r.w;
			float r1 = r.x;
			float r2 = r.y;
			float r3 = r.z;

			float q0 = q.w;
			float q1 = q.x;
			float q2 = q.y;
			float q3 = q.z;

			float rnorm = quatnorm( r );

			float t0 = (r0*q0 + r1*q1 + r2*q2 + r3*q3)/rnorm;
			float t1 = (r0*q1 - r1*q0 - r2*q3 + r3*q2)/rnorm;
			float t2 = (r0*q2 + r1*q3 - r2*q0 - r3*q1)/rnorm;
			float t3 = (r0*q3 - r1*q2 + r2*q1 - r3*q0)/rnorm;

			return Quaternion( t1, t2, t3, t0 );
		}

		// Convert quaternion to direction cosine matrix. 
		Matrix33 quat2dcm(const Quaternion& q)
		{
			Quaternion qin = quatnormalize( q );

			float q0 = qin.w;
			float q1 = qin.x;
			float q2 = qin.y;
			float q3 = qin.z;
			float q02 = q0*q0;
			float q12 = q1*q1;
			float q22 = q2*q2;
			float q32 = q3*q3;

			return Matrix33( (q02+q12-q22-q32),	2*(q1*q2+q0*q3),   2*(q1*q3-q0*q2),
				2*(q1*q2-q0*q3),   (q02-q12+q22-q32), 2*(q2*q3+q0*q1),
				2*(q1*q3+q0*q2),   2*(q2*q3-q0*q1),   (q02-q12-q22+q32) );

		}

		// Rotate a vector by a quaternion. 
		Vector3 quatrotate(const Quaternion& q, const Vector3& v)
		{
			Matrix33 dcm = quat2dcm( q );
			return dcm*v;
		}

		// Obsolete conversion for quaternion to Euler angles.
		EulerAngles quat2euler(const Quaternion& q)
		{
			Quaternion qin = quatnormalize( q );

			float q0 = qin.w;
			float q1 = qin.x;
			float q2 = qin.y;
			float q3 = qin.z;
			float q02 = q0*q0;
			float q12 = q1*q1;
			float q22 = q2*q2;
			float q32 = q3*q3;

			float phi = atan2(2*(q2*q3+q0*q1), (q02-q12-q22+q32));	// atan(DCM(2,3), DCM(3,3))
			float theta = asin(-2*(q1*q3-q0*q2)) ;					// asin(-DCM(1,3))
			float psi = atan2(2*(q1*q2+q0*q3), (q02+q12-q22-q32));	// atan(DCM(1,2), DCM(1,1))

			return EulerAngles(phi, theta, psi);
		}

		float deg2rad(const float val)
		{
			return val*DEGTORAD;
		}

		float rad2deg(const float val)
		{
			return val*RADTODEG;
		}

		// Signum function
		int sign(int x)
		{
			if( 0 == x )
				return 0;
			return x/abs(x);
		}

		float sign(float x)
		{
			if( fabs(x) < MATH_TOLERANCE )
				return 0.0f;
			return x/fabs(x);
		}

		// Numeric approximation of the error function.
		// http://en.wikipedia.org/wiki/Error_function
		static float erf_a =  8.0f * (PI - 3.0f) / (3.0f * PI * (4.0f - PI));

		// Error function
		float erf(float x)
		{
			float x2 = x*x;
			return sign(x) * sqrt(1.0f - exp(-x2 * (4.0f / PI + erf_a * x2) / (1.0f + erf_a * x2)));
		}

		// Normal probability density function
		float normpdf(float x, float mu, float sigma)
		{
			//          1                    -(x-mu)^2
			// y = -----------------* exp( ------------- )
			//      sqrt(2*Pi)*sigma         2*sigma^2
			//
			//, where 
			// mu - mean 
			// sigma - standard deviation

			float y = (1.0f/( sqrt(2.0f*(float)MATH_PI) * sigma ))*exp( -(x-mu)*(x-mu)/(2.0f*sigma*sigma) );
			return y;
		}

	}; //namespace math
}; //namespace orv
