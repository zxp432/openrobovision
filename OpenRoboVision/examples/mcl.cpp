//
// OpenRoboVision
//
// Monte Carlo Lacalization
//
//
// robocraft.ru
//

#include "orv/orv.h"
#include "orv/vision/image.h"
#include "orv/robo/orv_math.h"
#include <vector>
#include <algorithm>

// image size
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

#define BACKGROUND_COLOR cv::Scalar(255, 255, 255, 0) // white

// window name
#define MCL_WINDOW_NAME "MCL"

// save result image
#define MCL_SAVE_IMAGE 0

// walls params
#define MCL_FIRST	100 // first point
#define MCL_WIDTH	400 // big wall length
#define MCL_HEIGHT	200 // wall height
#define MCL_WIDTH2	200 // width of "corridor"
#define MCL_HEIGHT2 100 // height of "corridor"

// particles count
#define MCL_PARTICLES_COUNT 600
// particles radius (for show)
#define DEFAULT_PARTICLE_R 3

// robot radius
#define ROBOT_R 15
// first robot pose
#define ROBOT_X (MCL_FIRST+MCL_HEIGHT2/2)
#define ROBOT_Y (MCL_FIRST+MCL_WIDTH2/4)
// first robot direction
#define ROBOT_BASE_HEDING 90.0f

// measurements count of robot's sensor
#define MEASUREMENT_COUNT 5

// max distance
#define MAX_DISTANCE WINDOW_WIDTH

// movement delta
#define MOVEMENT_DELTA 2

// measurement error
#define MEASUREMENT_ERROR 6.0f

namespace mcl
{
	cv::RNG rng; // random number generator

	// check is position located on map
	bool is_inside_map(cv::Point2f point, std::vector<cv::Point> &points);
	// get distance to whall
	float get_closest_wall_distance(cv::Point2f point, float heading, std::vector<cv::Point> &points, cv::Point2f &intersection);
	// calc whether a ray intersects a line segment
	float intersects_distance(cv::Point2f point, float heading, cv::Point point1, cv::Point point2, cv::Point2f &intersection);
	// distance probability
	float get_distance_probability(float measured_distance, float real_distance, float delta);
	// noraml distribution
	float normal_distribution(float x, float mean, float variance);

	// sensor for robot (measure distanse for walls - like LIDAR)
	class Sensor
	{
	public:
		Sensor():
		  size(MEASUREMENT_COUNT)
		{
			float n = 360.0f/ static_cast<float>(size);
			for(int i=0; i<size; i++)
			{
				directions[i] = n*i;
				data[i] = 0.;
			}
		}
		~Sensor(){}

		void measurement( cv::Point2f point, float heading, std::vector<cv::Point> &points )
		{
			for(int i=0; i<size; i++)
			{
				float angle = heading + directions[i] - ROBOT_BASE_HEDING;
				// get distance to whall
				data[i] = get_closest_wall_distance(point, angle, points, pcl[i]);
			}
		}
		int size;								// number of angles for measurement
		float data[MEASUREMENT_COUNT];			// measurement results
		float directions[MEASUREMENT_COUNT];	// angle direction for measurement
		cv::Point2f pcl[MEASUREMENT_COUNT];		// closest points
	};

	class Robot
	{
	public:
		Robot():
		  radius(ROBOT_R), heading(ROBOT_BASE_HEDING)
		{
			point = cv::Point(ROBOT_X, ROBOT_Y);
		}
		~Robot(){}

		void measurement(std::vector<cv::Point> &points)
		{
			sensor.measurement(point, heading, points);
		}

		cv::Point2f point;	// center
		int radius;			// radius
		float heading;		// heading
		Sensor sensor;		// robot's sensor
	};

	class Particle
	{
		public:
		Particle():	
			weight(0.), radius(DEFAULT_PARTICLE_R)
		{
			// set random position
			point = cv::Point( rng.next()%WINDOW_WIDTH, rng.next()%WINDOW_HEIGHT );
			heading = static_cast<float>(rng.next()%360);
		}
		~Particle(){}

		Particle& operator=(const Particle &p)
		{
			point = p.point;
			radius = p.radius;
			heading = p.heading;
			weight = p.weight;

			return *this;
		}

		cv::Point2f point;	// particle center
		int radius;			// radius
		float heading;		// heading
		float weight;		// wei
	};

	// check is position located on map
	bool is_inside_map(cv::Point2f point, std::vector<cv::Point> &points)
	{
		if(points.size()<2)
			return false;

		int size = points.size();

		// based on:
		// OLY - Point Inclusion in Polygon Test
		// W. Randolph Franklin (WRF)
		// http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html

		int i, j;
		bool is_in = false;

		for(i=0; i<size; i++)
		{
			j = (i + size - 1) % size;
			if( (points[i].y > point.y) != (points[j].y > point.y) &&
				(point.x < (points[j].x-points[i].x)*(point.y-points[i].y)/(points[j].y-points[i].y) + points[i].x) )
			{
				is_in = !is_in;
			}
		}
		return is_in;
	}

	// get distance to whall
	float get_closest_wall_distance(cv::Point2f point, float heading, std::vector<cv::Point> &points, cv::Point2f &intersection)
	{
		float closest_distance = MAX_DISTANCE;
		float distance = 0.;
		int size = points.size();

		for (int i = 0; i < size; i++)
		{
			cv::Point point1 = points[i];
			cv::Point point2 = points[(i + 1) % size];
			cv::Point2f inter;

			distance = intersects_distance(point, heading, point1, point2, inter);
			if (distance < 0)
			{
				continue;
			}
			if (distance < closest_distance)
			{
				closest_distance = distance;
				intersection = inter;
			}
		}

		return closest_distance;
	}

	// calc whether a ray intersects a line segment
	float intersects_distance(cv::Point2f point, float heading, cv::Point point1, cv::Point point2, cv::Point2f &intersection)
	{
		intersection = cv::Point2f(point.x, point.y);

		//
		// http://newsgroups.derkeiler.com/Archive/Comp/comp.graphics.algorithms/2006-08/msg00022.html
		//
		//
		// As mentioned by others, the 2D problem may be solved by
		// equating the parametric equations. The ray is P0+s*D0, where
		// P0 is the ray origin, D0 is a direction vector (not necessarily
		// unit length), and s >= 0. The segment is P1+t*D1, where P1
		// and P1+D1 are the endpoints, and 0 <= t <= 1. Equating, you
		// have P0+s*D0 = P1+t*D1. Define perp(x,y) = (y,-x). Then
		// if Dot(perp(D1),D0) is not zero,
		// s = Dot(perp(D1),P1-P0)/Dot(perp(D1),D0)
		// t = Dot(perp(D0),P1-P0)/Dot(perp(D1),D0)
		// The s and t value are where the *lines* containing the ray
		// and segment intersect. The ray and segment intersect as
		// long as s >= 0 and 0 <= t <= 1. If Dot(perp(D1),D0) is zero,
		// then the ray and segment are parallel. If they do not lie on
		// the same line, there is no intersection. If they do lie on the
		// same line, you have to test for overlap, which is a 1D problem.

		// create a pseudo cross product to determine whether the ray and the line segment are parallel
		float heading_rad = orv::math::deg2rad(heading);
		float cos_theta = cos( heading_rad );
		float sin_theta = sin(heading_rad);

		float delta_x = static_cast<float>(point2.x - point1.x);
		float delta_y = static_cast<float>(point2.y - point1.y);

		// are the lines parallel?
		if ( fabs(delta_y * cos_theta - delta_x * sin_theta) < MATH_TOLERANCE)
		{
			// the lines are parallel
			return -1;
		}

		// s = Dot(perp(D1),P1-P0)/Dot(perp(D1),D0)
		// t = Dot(perp(D0),P1-P0)/Dot(perp(D1),D0)

		float s = (delta_y * (point1.x - point.x) - delta_x * (point1.y - point.y)) /
			(delta_y * cos_theta - delta_x * sin_theta);

		float t = (sin_theta * (point1.x - point.x) - cos_theta * (point1.y - point.y)) /
			(delta_y * cos_theta - delta_x * sin_theta);

		intersection = cv::Point2f( (point.x + s * cos_theta), (point.y + s * sin_theta) );

		if (s < 0)
			return -1;
		if (t < 0 || t > 1)
			return -1;
		return s;
	}

	// distance probability
	float get_distance_probability(float measured_distance, float real_distance, float delta)
	{
		float probability = 1.0f;
		if (real_distance > MAX_DISTANCE)
			real_distance = MAX_DISTANCE;

		float distance_diff = real_distance - measured_distance; 
	//	if(distance_diff<delta)
	//		return probability;

	//	if (real_distance > MAX_DISTANCE)
	//		return 1.0f - normal_distribution(MEASUREMENT_ERROR, MEASUREMENT_ERROR, delta);

		float lower = normal_distribution(measured_distance - delta, real_distance, delta);
		float upper = normal_distribution(measured_distance + delta, real_distance, delta);
		return upper - lower;
	}

	// noraml distribution
	float normal_distribution(float x, float mean, float variance)
	{
		// http://mathworld.wolfram.com/NormalDistribution.html
		// http://mathworld.wolfram.com/DistributionFunction.html
		return 0.5f * ( 1.0f + orv::math::erf((x - mean)/(sqrt(variance) * orv::math::Sqrt2)) );
	}
};

// set walls points
void set_walls(std::vector<cv::Point> &points);
// draw walls (lines from points)
void draw_walls(orv::Image &image, std::vector<cv::Point> &points, cv::Scalar color=cv::Scalar(0), int thickness=4);
// init particles
void init_particles(std::vector<mcl::Particle> &particles, int count=MCL_PARTICLES_COUNT);
// draw particles
void draw_particles(orv::Image &image, std::vector<mcl::Particle> &particles, cv::Scalar color=cv::Scalar(0), int thickness=1);
// draw robot
void draw_robot(orv::Image &image, mcl::Robot &robot, cv::Scalar color=CV_RGB(0,0, 255), int thickness=1, int thickness_dir=3);
// draw robot's measurement
void draw_robot_measurement(orv::Image &image, mcl::Robot &robot, cv::Scalar color=CV_RGB(255, 0, 0), int thickness=1);
// move robot and particles
void move_robot(int delta_path, int delta_angle, mcl::Robot &robot, std::vector<cv::Point> &points, std::vector<mcl::Particle> &particles);
// update state
void mcl_update(std::vector<cv::Point> &points, std::vector<mcl::Particle> &particles, mcl::Robot &robot);
// calc particle's weight
float mcl_calc_particle_weight(mcl::Particle particle, mcl::Robot &robot, std::vector<cv::Point> &points);
// function for sort particles by weight
bool sort_particles_by_weight(mcl::Particle a, mcl::Particle b);
// measurement_error
float measurement_error(float max_error = MEASUREMENT_ERROR);

int main(int argc, char* argv[])
{
	printf("[i] Monte Carlo Localization example.\n");
	printf("[i] Usage:\n");
	printf("[i] \t press A-D for change robot's heading.\n");
	printf("[i] \t press W-S for move robot.\n");
	printf("[i] \t press ESC for quit.\n");

	// create image
	orv::Image image(WINDOW_WIDTH, WINDOW_HEIGHT, CV_8UC3);
	// window
	std::string window_name(MCL_WINDOW_NAME);
//	cv::namedWindow(window_name);

	// set white background
	image.setTo(BACKGROUND_COLOR);

	// set wall points
	std::vector<cv::Point> points;
	set_walls(points);

	// draw walls
	draw_walls(image, points);

	// init and draw particles
	std::vector<mcl::Particle> particles;
	init_particles(particles);
	draw_particles(image, particles);

	// draw robot
	mcl::Robot robot;
	draw_robot(image, robot);
	// draw robot's measurement
	robot.measurement(points);
	draw_robot_measurement(image, robot);

	int step = 0;
	// main cycle
	while(1)
	{
		image.show(window_name);
#if MCL_SAVE_IMAGE
		std::stringstream ss;
		ss<<"mcl_"<<step<<".bmp";
		image.save(ss.str());
#endif

		char key = cv::waitKey(0);
		if(key==27) //ESC
			break;

		// move robot by AWSD keys
		int dr = 0, da = 0; // delta movements (forward delta and angle delta)
		switch(key)
		{
		case 'a': 
		case 'A': 
			//if( mcl::is_inside_map( cv::Point(robot.point.x-robot.radius-MOVEMENT_DELTA, robot.point.y), points) ) // check for walls
				da = -MOVEMENT_DELTA; //robot.point.x -= MOVEMENT_DELTA;
			break;
		case 'd': 
		case 'D': 
			//if( mcl::is_inside_map( cv::Point(robot.point.x+robot.radius+MOVEMENT_DELTA, robot.point.y), points) ) // walls
				da = MOVEMENT_DELTA; //robot.point.x += MOVEMENT_DELTA;
			break;
		case 'w': 
		case 'W': 
			//if( mcl::is_inside_map( cv::Point(robot.point.x, robot.point.y-robot.radius-MOVEMENT_DELTA), points) ) // walls
				dr = MOVEMENT_DELTA; //robot.point.y -= MOVEMENT_DELTA;
			break;
		case 's': 
		case 'S': 
			//if( mcl::is_inside_map( cv::Point(robot.point.x, robot.point.y+robot.radius+MOVEMENT_DELTA), points) ) // walls
				dr = -MOVEMENT_DELTA; //robot.point.y += MOVEMENT_DELTA;
			break;
		default:
			break;
		}
		// move robot and particles
		move_robot(dr, da, robot, points, particles);
		// make robot measurement
		robot.measurement(points);

		//--------------------------------
		// update state
		mcl_update(points, particles, robot);
		//--------------------------------
		// DRAW
		// set white background
		image.setTo(BACKGROUND_COLOR);
		// draw walls
		draw_walls(image, points);
		// draw particles
		draw_particles(image, particles);
		// draw robot
		draw_robot(image, robot);
		// draw robot's measurement
		draw_robot_measurement(image, robot);
		//--------------------------------
		++step;
		printf("[i][MCL] Step: %04d\n", step);
	} // while(1)

	return 0;
}

// set walls points
void set_walls(std::vector<cv::Point> &points)
{
	//
	// walls
	//
	// ----------- W  -------------------->x
	// p0                     p1
	// *----------------------*
	// |                      |
	// *------*               |  H
	//        | h     w       |
	//        *---------------*
	//        p3              p2
	cv::Point p0, p1, p2, p3, p4, p5;
	int first = MCL_FIRST;
	int W = MCL_WIDTH;
	int H = MCL_HEIGHT;
	int w = MCL_WIDTH2;
	int h = MCL_HEIGHT2;
	p0 = cv::Point(first, first);
	p1 = cv::Point(first + W, first);
	p2 = cv::Point(first + W, first + H);
	p3 = cv::Point(first + w, first + H);
	p4 = cv::Point(first + w, first + h);
	p5 = cv::Point(first, first + h);
	points.reserve(6);
	points.push_back(p0);
	points.push_back(p1);
	points.push_back(p2);
	points.push_back(p3);
	points.push_back(p4);
	points.push_back(p5);
}

// draw walls (lines from points)
void draw_walls(orv::Image &image, std::vector<cv::Point> &points, cv::Scalar color, int thickness)
{
	if(points.size()<2)
		return;

	int size = points.size();
	int i=0;
	for(i=1; i<size; i++)
	{
		image.addline(points[i-1], points[i], color, thickness);
	}
	image.addline(points[i-1], points[0], color, thickness);
}

// init particles
void init_particles(std::vector<mcl::Particle> &particles, int count)
{
	if(count<=0)
		return;

	particles.reserve(count);
	for(int i=0; i<count; i++)
	{
		mcl::Particle particle;
		particles.push_back(particle);
	}
}

// draw particles
void draw_particles(orv::Image &image, std::vector<mcl::Particle> &particles, cv::Scalar color, int thickness)
{
	if(particles.size()<1)
		return;

	int size = particles.size();

	float x, y, k;
	for(int i=0; i<size; i++)
	{
		cv::Point center = particles[i].point;
		// draw particle
		image.addcircle( center, particles[i].radius, color, thickness);
		// draw particle's heading
		k =  orv::math::deg2rad(particles[i].heading);
		x = particles[i].radius*cos(k);
		y = x * tan(k);
		image.addline(center, cv::Point( center.x + static_cast<int>(y), center.y - static_cast<int>(x)), color, thickness);
	}
}

// draw robot
void draw_robot(orv::Image &image, mcl::Robot &robot, cv::Scalar color, int thickness, int thickness_dir)
{
	cv::Point center = robot.point;
	image.addcircle( center, robot.radius, color, thickness);
	// draw robot's heading
	float x, y, k;
	k =  orv::math::deg2rad(robot.heading);
	x = robot.radius*cos(k);
	y = x * tan(k);
	image.addline(center, cv::Point( center.x + static_cast<int>(y), center.y - static_cast<int>(x)), color, thickness_dir);
}

// draw robot's measurement
void draw_robot_measurement(orv::Image &image, mcl::Robot &robot, cv::Scalar color, int thickness)
{
	for(int i=0; i<robot.sensor.size; i++)
	{
		image.addline(robot.point, robot.sensor.pcl[i], color, thickness);
	}
}

// move robot and particles
void move_robot(int delta_path, int delta_angle, mcl::Robot &robot, std::vector<cv::Point> &points, std::vector<mcl::Particle> &particles)
{
	robot.heading += delta_angle;

	float robot_heading_rad = orv::math::deg2rad(robot.heading + ROBOT_BASE_HEDING);
	float dx = -( (delta_path*cos(robot_heading_rad)) );
	float dy = -( (delta_path*sin(robot_heading_rad)) );

	// if step go out of map - dont do it
	if( !mcl::is_inside_map( cv::Point2f(robot.point.x+robot.radius+dx, robot.point.y+dy), points) || 
		!mcl::is_inside_map( cv::Point2f(robot.point.x-robot.radius+dx, robot.point.y+dy), points) ||
		!mcl::is_inside_map( cv::Point2f(robot.point.x+dx, robot.point.y+robot.radius+dy), points) ||
		!mcl::is_inside_map( cv::Point2f(robot.point.x+dx, robot.point.y-robot.radius+dy), points))
		return;

	// calc movement
	robot.point.x += dx;
	robot.point.y += dy;

	printf("[i][move_robot] robot.heading= %0.2f (%0.2f %0.2f)\n", robot.heading, robot.point.x, robot.point.y);

	// apply motion for all particles
	int size = particles.size();
	for(int i=0; i<size; i++)
	{
		particles[i].heading += delta_angle;
		float particle_heading_rad = orv::math::deg2rad(particles[i].heading + ROBOT_BASE_HEDING);
		particles[i].point.x += -( (delta_path*cos(particle_heading_rad)) );
		particles[i].point.y += -( (delta_path*sin(particle_heading_rad)) );
	}
}

// update state
void mcl_update(std::vector<cv::Point> &points, std::vector<mcl::Particle> &particles, mcl::Robot &robot)
{
	if(points.size()<2 || particles.size()<1)
		return;

	int particles_size = particles.size();

	float weights_sum = 0.0f;
	float max_weights = 0.0f;
	int i,j;
	for(i=0; i<particles_size; i++)
	{
		// is particle is out of the map?
		while( !mcl::is_inside_map(particles[i].point, points) )
		{
			// replace it with a new random particle
			particles[i] = mcl::Particle();
		}
		// calc particle weight
		float weight = mcl_calc_particle_weight(particles[i], robot, points);

		particles[i].weight = weight;
		weights_sum += weight;

		if(weight>max_weights)
			max_weights = weight;
	}
	printf("[i][mcl_update] weights_sum=%0.2f (max: %0.2f)\n", weights_sum, max_weights);

	// sort by weight
	std::sort(particles.begin(), particles.end(), sort_particles_by_weight);

	// second cycle
	// use best matching particles (first half of sorted list) as base for over
	int half_particles_size = particles_size/2;
	for(i=0, j=half_particles_size; i<half_particles_size; i++, j++)
	{
	//	if(mcl::rng.next()%particles_size>1)
	//	{
			particles[j] = particles[i];
			particles[j].point.x += measurement_error();
			particles[j].point.y += measurement_error();
			particles[j].heading += measurement_error();
	//	}
	//	else
	//		particles[j] = mcl::Particle();
	}
}

// calc particle's weight
float mcl_calc_particle_weight(mcl::Particle particle, mcl::Robot &robot, std::vector<cv::Point> &points)
{
	float weight = 0.0;
	mcl::Sensor particle_sensor;
	particle_sensor.measurement(particle.point, particle.heading, points);
	for (int i = 0; i<robot.sensor.size; i++)
	{
		weight += mcl::get_distance_probability(robot.sensor.data[i], particle_sensor.data[i], MEASUREMENT_ERROR);
	}
	return weight;
}

// function for sort particles by weight
bool sort_particles_by_weight(mcl::Particle a, mcl::Particle b) 
{ 
	return (a.weight > b.weight);
}

// measurement_error
float measurement_error(float max_error)
{
	float val = static_cast<float>( mcl::rng.next()%(int)(max_error) );
	if(mcl::rng.next()%100 < 50)
		val = -val;
	return val;
}
