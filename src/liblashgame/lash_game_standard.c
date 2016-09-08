#include <stdlib.h>
#include <math.h>

#include "liblashgame/lash_game_standard.h"


#define LASH_M_PI_DOUBLE (M_PI * 2)

// http://stackoverflow.com/questions/4003232/how-to-code-a-modulo-operator-in-c-c-obj-c-that-handles-negative-numbers
// modulus must be handled differently for negatives 

unsigned char lash_getQuadrant(const float rads) {
	int mod, quad;
	mod = (int)floor(rads / M_PI_2);
	quad = mod % 4;
	
	if (mod < 0)
		if (quad != 0)
			quad += 4;
	
	return quad + 1;
}

unsigned char lash_getQuadrantFromCartesian(const int dx, const int dy) {
	if (dx > 0) {
		if (dy > 0) {
			return 1;
		} else {
			return 4;
		}
	} else {
		if (dy > 0) {
			return 2;
		} else {
			return 3;
		}
	}
	return 0;
}

 
enum lash_anglespan_compare lash_compareAngleSpan(float old_rad_min, float old_rad_max, float new_rad_min, float new_rad_max) {
	double full_circle = 2 * M_PI;
	
	if (old_rad_max > full_circle) {
		// if new angle is fully contained by the old, and the old angles are on each side of 2π
		if (new_rad_min + full_circle < old_rad_max && new_rad_min + full_circle > old_rad_min) {
			new_rad_min += full_circle;
			new_rad_max += full_circle;
		}
	} else if (new_rad_max > full_circle && old_rad_max <= full_circle && old_rad_min < full_circle) {
		// if the new angle are on each side of 2π, and the old min is inside the span
		//if (new_rad_min < old_rad_max + full_circle && new_rad_min > old_rad_min + full_circle) {
			old_rad_min += full_circle;
			old_rad_max += full_circle;
		//}	
	}
	
	enum lash_anglespan_compare result = LASH_ANGLESPAN_NONE;
	if (new_rad_max < old_rad_max && new_rad_min > old_rad_min) {
		result = LASH_ANGLESPAN_SPLIT;
	} else {
		if (new_rad_max >= old_rad_max && new_rad_min <= old_rad_max)
			result = LASH_ANGLESPAN_MAX;
		if (new_rad_min <= old_rad_min && new_rad_max >= old_rad_min)
			result = result == LASH_ANGLESPAN_MAX ? LASH_ANGLESPAN_BOTH : LASH_ANGLESPAN_MIN;
	}
	return result;
}

int lash_getContainedAngleOnly(const double x, const double y, const double m, const double w_container, const double h_container, const int buf_top, const int buf_right, const int buf_bottom, const int buf_left, double *angle_min, double *angle_max) {
	double n = 0.f;
	return lash_getContainedAngle(x, y, m, w_container, h_container, buf_top, buf_right, buf_bottom, buf_left, angle_min, angle_max, 0, &n, &n, &n, &n);
}

int lash_getContainedAngle(const double x, const double y, const double m, const double w_container, const double h_container, const int buf_top, const int buf_right, const int buf_bottom, const int buf_left, double *angle_min, double *angle_max, const int include_dcoords, double *x_min, double *y_min, double *x_max, double *y_max) {
	// any left
	if (x - m < buf_left) {
		// topleft
		if (y - m < buf_top) {
			double min_mod = M_PI;
			double max_mod = 2 * M_PI;
			*angle_min = min_mod + acos((x - buf_left) / m);
			*angle_max = max_mod + asin((y - buf_top) / m);
			if (include_dcoords) {
				/* *x_min = buf_left;
				*y_min = y + floor(sin(*angle_min - min_mod) * m);
				*x_max = x + floor(cos(*angle_max - max_mod) * m);
				*y_max = buf_top; */
				*x_min = buf_left;
				*y_min = y + (sin(*angle_min - min_mod) * m);
				*x_max = x + (cos(*angle_max - max_mod) * m);
				*y_max = buf_top;
			}
			return 0;
		// bottomleft
		} else if (y + m > h_container - buf_bottom) {
			double min_mod = 3 * M_PI_2;
			double max_mod = 5 * M_PI_2;
			*angle_min = min_mod + acos((x - buf_left) / m);
			*angle_max = max_mod + asin((h_container - buf_bottom - y) / m);
			if (include_dcoords) {
				/* *x_min = x + floor(sin(*angle_min - min_mod) * m);
				*y_min = h_container - buf_bottom;
				*x_max = buf_left;
				*y_max = y - floor(cos(*angle_max - max_mod) * m); */
				*x_min = x + (sin(*angle_min - min_mod) * m);
				*y_min = h_container - buf_bottom;
				*x_max = buf_left;
				*y_max = y - (cos(*angle_max - max_mod) * m); 
			}
			return 0;
		} else {
		// left
			double tmp_angle = acos((x - buf_left) / m);
			*angle_min = M_PI + tmp_angle;
			*angle_max = (3 * M_PI) - tmp_angle;
			if (include_dcoords) {
				//int tmp_side = floor(sin(tmp_angle) * m);
				int tmp_side = (sin(tmp_angle) * m);
				*x_min = buf_left;
				*y_min = y + tmp_side;
				*x_max = buf_left;
				*y_max = y - tmp_side;
			}
			return 0;
		}
	// any right
	} else if (x + m > w_container - buf_right) {
		// top right
		if (y - m < buf_top) {
			double min_mod = M_PI_2;
			double max_mod = 3 * M_PI_2;
			*angle_min = min_mod + acos((y - buf_top) / m);
			*angle_max = max_mod + asin((w_container - buf_right - x) / m);
			if (include_dcoords) {
				/* *x_min = x - floor(sin(*angle_min - min_mod) * m);
				*y_min = buf_top;
				*x_max = w_container - buf_left;
				*y_max = y + floor(cos(*angle_max - max_mod) * m); */
				*x_min = x - (sin(*angle_min - min_mod) * m);
				*y_min = buf_top;
				*x_max = w_container - buf_left;
				*y_max = y + (cos(*angle_max - max_mod) * m);
			}
			return 0;
		// bottom right
		} else if (y + m > h_container - buf_bottom) {
			//double min_mod = 0;
			double max_mod = M_PI;
			*angle_min = acos((w_container - buf_right - x) / m);
			*angle_max = max_mod + asin((h_container - buf_bottom - y) / m);
			if (include_dcoords) {
				/* *x_min = w_container - buf_left;
				*y_min = y - floor(sin(*angle_min) * m);
				*x_max = x - floor(cos(*angle_max - max_mod) * m);
				*y_max = h_container - buf_bottom;*/
				*x_min = w_container - buf_left;
				*y_min = y - (sin(*angle_min) * m);
				*x_max = x - (cos(*angle_max - max_mod) * m);
				*y_max = h_container - buf_bottom;
			}
			return 0;
		// right
		} else {
			//angle min has no modifier
			//double tmp_angle = acos((h_container - buf_right - x) / m);
			*angle_min = acos((w_container - buf_right - x) / m);
			*angle_max = (2 * M_PI) - *angle_min;
			if (include_dcoords) {
				//int tmp_side = floor(sin(*angle_min) * m);
				int tmp_side = (sin(*angle_min) * m);
				*x_min = w_container - buf_right;
				*y_min = y - tmp_side;
				*x_max = w_container - buf_right;
				*y_max = y + tmp_side;
				
			}
			return 0;
		}
	// top (only)
	} else if (y - m < buf_top) {
		double tmp_angle = acos((y - buf_top) / m);
		*angle_min = M_PI_2 + tmp_angle;
		*angle_max = (5 * M_PI_2) - tmp_angle;
		if (include_dcoords) {
			//int tmp_side = floor(sin(tmp_angle) * m);
			int tmp_side = (sin(tmp_angle) * m);
			*x_min = x - tmp_side;
			*y_min = buf_top;
			*x_max = x + tmp_side;
			*y_max = buf_top;
		}
		return 0;
	// bottom (only)
	} else if (y + m > h_container - buf_bottom) {
		double tmp_angle = acos((h_container - buf_bottom - y) / m);
		*angle_min = (3 * M_PI_2) + tmp_angle;
		*angle_max = (7 * M_PI_2) - tmp_angle;
		if (include_dcoords) {
			//int tmp_side = floor(sin(tmp_angle) * m);
			int tmp_side = (sin(tmp_angle) * m);
			*x_min = x + tmp_side;
			*y_min = h_container - buf_bottom;
			*x_max = x - tmp_side;
			*y_max = h_container - buf_bottom;
		}
		return 0;
	}
	
	// default; no limit
	*angle_min = 0.f;
	*angle_max = 2 * M_PI;
	if (include_dcoords) {
		*x_min = h_container - buf_right;
		*y_min = y;
		*x_max = h_container - buf_right;
		*y_max = y;
	}
	return 0;
}

float lash_getCartesianMagnitude(const int target_x, const int  target_y, const int source_x, const int source_y) {
	return sqrt(pow(target_x - source_x, 2) + pow(target_y - source_y, 2));
}

// self.h = abs((targetindex % map.w) - (index % map.w)) + abs(math.floor(targetindex / map.w) - math.floor(index / map.w))
int lash_getManhattanMagnitudeFromCartesian(const lash_game_coords_t targetcoords, const lash_game_coords_t sourcecoords) {
	//return abs((target_idx % *w) - (source_idx % *w)) + abs(floor(target_idx / *w) - floor(source_idx / *w));
	return abs(targetcoords.y - sourcecoords.y) + abs(targetcoords.x - sourcecoords.x);
}

int lash_polarToCartesian(const float radians, const float radius, const int source_x, const int source_y, int *target_x, int *target_y) {
	*target_x = source_x + floor(cos(radians) * radius);
	*target_y = source_y + floor(sin(radians) * radius);
	return 0;
}

int lash_cartesianToPolar(const int source_x, const int source_y, const int target_x, const int target_y, float *radians, float *radius, const int inv_x, const int inv_y) {	
	
	float tmp_radians;
	
	int x = target_x - source_x;
	if (inv_x != 0)
		x *= -1;

	int y = target_y - source_y;
	if (inv_y != 0)
		y *= -1;
			
	*radius = sqrt(pow(x, 2) + pow(y, 2));
	tmp_radians = atan2((float)y, (float)x);
	if (tmp_radians < 0) 
		tmp_radians += 2 * M_PI;
		
	*radians = tmp_radians;
	
	return 0;
}

int lash_cartesianToIndex(unsigned long int *index, const unsigned int *w, const unsigned int *h, unsigned int *unit_size, const lash_game_coords_cartesian_t *coords) {
	
	if (coords->x > *w - 1 || coords->y > *h - 1)
		return 1;
	
	*index = coords->x;
	*index += floor(coords->y * *w);
	
	return 0;
}

int lash_indexToCartesian(lash_game_coords_cartesian_t *coords, const unsigned int *w, const unsigned int *h, unsigned int *unit_size, const lash_game_map_index_t *index) {
	
	if ((*index) >= *w * *h)
		return 1;
		
	if (*unit_size < 1)
		*unit_size = 1;
	
	coords->x = (unsigned int)((*index % *w) * *unit_size);
	coords->y = (unsigned int)(floor(*index / *w) * *unit_size);
	
	return 0;
}

int lash_cartesianToFloat(lash_game_coords_float_t *coords_float, const unsigned int *w, const unsigned int *h, const lash_game_coords_cartesian_t *coords_cartesian) {
	coords_float->x = (float)coords_cartesian->x / *w;
	coords_float->y = (float)coords_cartesian->y / *h;
	return 0;
}

int lash_linearIntersect(const lash_game_line_t a, const lash_game_line_t b, lash_game_coords_float_t *coords) {
	
	coords->x = (b.c - a.c) / (a.m - b.m);
	coords->y = (a.m * coords->x) + a.c;
	if (isinf(coords->x) || isinf(coords->y))
		return 1;
	return 0;
	
}

int lash_cartesianToLinear(const lash_game_coords_float_t src, const lash_game_coords_float_t target, lash_game_line_t *result) {
	if (target.x == src.x) {
		result->m = INFINITY;
		result->c = INFINITY;
		return 1;
	} else if (target.y == src.y) {
		result->m = NAN;
		result->c = NAN;
		return 1;
	} else {
		result->m = (target.y - src.y) / (float)(target.x - src.x);
		result->c = ((-result->m) * src.x) + src.y;
	}
	return 0;
}

float lash_distanceToRadians(const float distance, const float radius) {
	return LASH_M_PI_DOUBLE * (distance / (LASH_M_PI_DOUBLE * radius));
}

/// \todo check lashNormalize..Radians if they work with multiple negative rotations
float lash_normalizeRadians(float radians) {
	char rotations = floor(radians / (2 * M_PI));
	if (rotations != 0)
		radians -= LASH_M_PI_DOUBLE * rotations;
	else if (radians < 0)
		radians += LASH_M_PI_DOUBLE;
	return radians;
}

float lash_normalizeQuadrantRadians(float radians) {
	float rotations = floor(radians / M_PI_2);
	if (rotations != 0)
		radians -= (float)M_PI_2 * rotations;
	else if (radians < 0)
		radians += M_PI_2 + (float)(M_PI_2 * rotations);
	return radians;
}

// calculates at the point of impact. 
/// \todo resultquad calculation should resolve to being able to use resultquad for the objrad calculation below
void lash_collisionSurfaceDeflectSimple(float *objvel, float *objrad, const float surfacerad, float bounce) {
	
	
	if (bounce < 0.f)
		bounce = 0.0;
	else if (bounce > 1.f)
		bounce = 1.f;
		
	float resultquad; // the angle of impact
	float direction; // the difference of objrad and surfacerad
	float xvel, yvel; // the resulting velocities after bounce is calculated
	float objradnormal, surfaceradnormal, objradquad, surfaceradquad, radmod;
	char surfacequadrant, objquadrant; // quadrant the surface and object angles belong to
	
	objradnormal = lash_normalizeRadians(*objrad);
	surfaceradnormal = lash_normalizeRadians(surfacerad);
	
	radmod = 0.f; // bounce angle
	
	surfacequadrant = floor(surfaceradnormal / M_PI_2); 
	objquadrant = floor(objradnormal / M_PI_2);

	objradquad = objradnormal - (objquadrant * (float)M_PI_2); // object rads normalized within quadrant
	surfaceradquad = surfaceradnormal - (surfacequadrant * (float)M_PI_2); // surface rads normalized within quadrant
	
	if (objquadrant % 2 != surfacequadrant % 2)
		direction = surfaceradquad + (M_PI_2 - objradquad);
	else
		direction = surfaceradquad - objradquad;
	
	
	if (direction < 0.f)
		resultquad = M_PI + direction;
	else //if (direction < M_PI_2)
		resultquad = M_PI - direction;
	//else
		//resultquad = direction;


	if (direction < 0.f)
		xvel = cos(-direction) * *objvel;
	else
		xvel = cos(direction) * *objvel;
	
	yvel = sin(direction) * *objvel * bounce;

	if (resultquad != M_PI_2)
		radmod = atan(yvel / xvel);
		
	if (radmod < 0.f)
		radmod = -radmod;
	
	if (direction < 0.f)
		*objrad = *objrad + direction - radmod;
	else if (direction > M_PI_2)
		*objrad = *objrad - resultquad - radmod;
	else
		*objrad = *objrad + direction + radmod;
		
	*objvel = sqrt(pow(xvel, 2) + pow(yvel, 2));
		
}


// if want intersect coords, pass both as pointers, if NULL the coords are not resolved
char lash_collisionCheckCircleLine(const lash_game_coords_float_t circle_centre, const float circle_radius, const lash_game_line_t slope, lash_game_coords_float_t *intersect_coords_1, lash_game_coords_float_t *intersect_coords_2) {
	
	float a, b, c, discriminant;
	char intersections = 0;
	
	// quadratic equation to resolve intersections from circle and line equation (substitute the line into circle, expand and collect equal terms)
	// binomial: (m²+1)x²+2(mc−mq−p)x+(q²−r²+p²−2cq+c²)=0.
	// discriminant: -2(mc−mq−p) +/- sqrt(pow(2(mc-mq-p), 2) - (4 * ((m*m)+1) * ((p*p)-(r*r)+(q*q)-(2*c*q)+(c*c))))
	// taken from http://math.stackexchange.com/questions/228841/how-do-i-calculate-the-intersections-of-a-straight-line-and-a-circle
	a = (slope.m * slope.m) + 1;
	b = 2 * ((slope.m * slope.c) - (slope.m * circle_centre.y) - circle_centre.x);
	c = (circle_centre.x * circle_centre.x) + (circle_centre.y * circle_centre.y) - (2 * slope.c * circle_centre.y) + (slope.c * slope.c) - (circle_radius * circle_radius);
	
	discriminant = (b * b) - (4 * a * c);
	if (discriminant > 0)
		intersections = 2;
	else if (discriminant == 0)
		intersections = 1;
	
	if (intersections == 0) {
		intersect_coords_1 = NULL;
		intersect_coords_2 = NULL;
	} else if (intersect_coords_1 != NULL && intersect_coords_2 != NULL) {
		intersect_coords_1->x = (-b - sqrt(discriminant)) / (2 * a);
		intersect_coords_1->y = (slope.m * intersect_coords_1->x) + slope.c;
		if (intersections  == 2) {
			intersect_coords_2->x = (-b + sqrt(discriminant)) / (2 * a);
			intersect_coords_2->y = (slope.m * intersect_coords_2->x) + slope.c;
		}
	}
		
	return intersections;
}

/*
/// \todo check if this is faster with circle/line intersect check
void lash_collisionCheckCircleLine(const lash_game_coords_float_t c_coords, const float c_r, const lash_game_coords_float_t l_coords_1, const lash_game_coords_float_t l_coords_2, float *intersect_perp) {
	float l_reverse_vx, l_reverse_vy;
	lash_game_line_t l_slope, l_reverse_slope;
	lash_game_coords_float_t intersect_coords;
	lash_cartesianToLinear(l_coords_1, l_coords_2, &l_slope);
	reverse_vy = l_coords_1.x - l_coords_2.x;
	reverse_vx = l_coords_1.y - l_coords_2.y;
	l_reverse_slope.m = (reverse_vy / reverse_vx) * -1;
	l_reverse_slope.c = c_coords.y - (l_reverse_slope.m * c_coords.x);
	lash_linearIntersect(l_slope, l_reverse_slope, &intersect_coords);
}
*/

/*
int lash_isBigEndian() {
	if (_lash_endian == -1) {
		union {
			uint32_t i;
			char[4] c;
		} bint = (0x01020304);
		if (bint.c[0] == 1)
			_lash_endian = 1;
		else
			_lash_endian = 0;
	}
	return _lash_endian;
}
*/
