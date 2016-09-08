#ifndef LASH_GAME_STANDARD_H_
#define LASH_GAME_STANDARD_H_

#define LASH_GAME_GRAVITY_EARTH 9.8
#define LASH_GAME_ATMOSPHERE_EARTH 101325

typedef struct lash_game_coords_t {
	unsigned int x;
	unsigned int y;
} lash_game_coords_t;

typedef struct lash_game_coords_t lash_game_coords_cartesian_t;

typedef struct lash_game_coords_float_t {
	double x;
	double y;
} lash_game_coords_float_t;

typedef unsigned long int lash_game_map_index_t;

typedef struct lash_game_line_t {
	double m;
	double c;
} lash_game_line_t;

enum lash_anglespan_compare {
	LASH_ANGLESPAN_NONE,
	LASH_ANGLESPAN_MIN,
	LASH_ANGLESPAN_MAX,
	LASH_ANGLESPAN_BOTH,
	LASH_ANGLESPAN_SPLIT
};

#ifdef __cplusplus 
extern "C" {
#endif
unsigned char lash_getQuadrant(const float rads);
unsigned char lash_getQuadrantFromCartesian(const int dx, const int dy);
float lash_getCartesianMagnitude(const int target_x, const int target_y, const int source_x, const int source_y);
int lash_cartesianToPolar(const int source_x, const int source_y, const int target_x, const int target_y, float *radians, float *radius, const int inv_x, const int inv_y);
int lash_polarToCartesian(const float radians, const float radius, const int source_x, const int source_y, int *target_x, int *target_y);
enum lash_anglespan_compare lash_compareAngleSpan(float old_rad_min, float old_rad_max, float new_rad_min, float new_rad_max);
int lash_getContainedAngle(const double x, const double y, const double m, const double w_container, const double h_container, const int buf_top, const int buf_right, const int buf_bottom, const int buf_left, double *angle_min, double *angle_max, const int include_dcoords, double *x_min, double *y_min, double *x_max, double *y_max);
int lash_getContainedAngleOnly(const double x, const double y, double m, const double w_container, const double h_container, const int buf_top, const int buf_right, const int buf_bottom, const int buf_left, double *angle_min, double *angle_max);
int lash_cartesianToIndex(unsigned long int *index, const unsigned int *w, const unsigned int *h, unsigned int *unit_size, const lash_game_coords_t *coords);
int lash_indexToCartesian(lash_game_coords_cartesian_t *coords, const unsigned int *w, const unsigned int *h, unsigned int *unit_size, const lash_game_map_index_t *index);
int lash_cartesianToFloat(lash_game_coords_float_t *coords_float, const unsigned int *w, const unsigned int *h, const lash_game_coords_cartesian_t *coords_cartesian);
int lash_getManhattanMagnitudeFromCartesian(const lash_game_coords_t targetcoords, const lash_game_coords_t sourcecoords);
int lash_linearIntersect(const lash_game_line_t a, const lash_game_line_t b, lash_game_coords_float_t *coords);
int lash_cartesianToLinear(const lash_game_coords_float_t src, const lash_game_coords_float_t target, lash_game_line_t *result);
float lash_distanceToRadians(const float distance, const float radius);
float lash_normalizeRadians(const float radians);
float lash_normalizeQuadrantRadians(float radians);
void lash_collisionSurfaceDeflectSimple(float *objvel, float *objrad, const float surfacerad, float bounce);
char lash_collisionCheckCircleLine(const lash_game_coords_float_t circle_centre, const float circle_radius, const lash_game_line_t slope, lash_game_coords_float_t *intersect_coords_1, lash_game_coords_float_t *intersect_coords_2);

#ifdef __cplusplus 
}
#endif

#endif // LASH_GAME_STANDARD_H_



/**
 * \file lash_game_standard.h
 * \brief LASH Game movement angle routines
 * 
 * \todo rename lash_game_map_index_t to lash_game_index_t, as it does not belong to map
 */
 
/**
 * \var enum lash_anglespan_compare
 * \brief Specifies what action should be taken to transform a compared anglespan
 * \sa lashCompareAngleSpan
 */
 
/**
 * \fn int lash_getContainedAngle(int x, int y, double m, int w_container, int h_container, int buf_top, int buf_right, int buf_bottom, int buf_left, double *angle_min, double *angle_max, int include_dcoords, int *x_min, int *y_min, int *x_max, int *y_max)
 * \brief Determine movement angle contained within world bounds
 * \param x the x position in the world
 * \param y the y position in the world
 * \param m the movement magnitude to check against
 * \param w_container the width of the world
 * \param h_container the height of the world
 * \param buf_top max proximity to world top edge
 * \param buf_right max proximity to world right edge
 * \param buf_bottom max proximity to world bottom edge
 * \param buf_left max proximity to world left edge
 * \param *angle_min resulting minimum movement angle in radians
 * \param *angle_max resulting maximum movement angle in radians
 * \param include_dcoords whether or not to calculate the intersection between the extreme movement vectors and the game world edges
 * \param *x_min resulting intersection point x coordinate of game world for the minimum movement angle radian
 * \param *y_min resulting intersection point y coordinate of game world for the minimum movement angle radian
 * \param *x_max resulting intersection point x coordinate of game world for the maximum movement angle radian
 * \param *y_max resulting intersection point y coordinate of game world for the maximum movement angle radian
 *   
 * From a position point and a movement magnitude, this function yields an angle span within which movement of the given magnitude will be kept within the world bounds.
 * 
 * Optionally, the coordinates of the intersection points to the game world can also be calculated. In this case they will be stored in the variables passed in the last four parameters.
 * These variables will be left unchanged if 0 is passed for include_dcoords
 * 
 * \todo Return zero for both radians when movement in any case is out of world bounds (no vector is possible), or if position is out of bounds
 * 
 */
 
/**
 * \fn enum lash_anglespan_compare lash_compareAngleSpan(float old_rad_min, float old_rad_max, float new_rad_min, float new_rad_max)
 * \brief Contiguous angle spans from two angles
 * \param old_rad_min the old span minimum radian
 * \param old_rad_min the old span maxumum radian
 * \param old_rad_min the new span minimum radian
 * \param old_rad_min the new span maximum radian
 * 
 * Compares the span of two angles, and if they overlap returns which transformation should be made to reduce them to one contiguous span:
 * - If the new span encloses the MINIMUM radian of the old span, then the old minimum should be set to the new minimum
 * - If the new span encloses the MAXIMUM radian of the old span, then the old maximum should be set to the new maximum
 * - If the new span encloses the WHOLE of the old span, then the old span should be replaced by the new span
 * - If the new span is enclosed by the WHOLE of the old span, no change should occur
 * - If the new span is outside of the old span, no change should occur
 * 
 * \todo can never be LASH_ANGLESPAN_BOTH. BOTH should be enclosing the whole span, and should lead to deletion of the old span
 * 
 */

/**
 * \fn int lash_cartesianToIndex(unsigned long int *index, unsigned int *w, unsigned int *h, lash_game_coords_t *coords)
 * \brief Get the linear index of a cartesian coordinate pair
 * 
 * \param *index pointer to variable holding the resulting index
 * \param *w the width of the coordinate system
 * \param *h the height of the coordinate system
 * \param *coords the coordinate pair to calculate
 * 
 * If the coordinate pair is out of bounds, the variable the index parameter points to will be left unchanged, and the function will return 1
 * 
 * \see lashCartesianToIndex
 * 
 */

/**
 * \fn int lash_indexToCartesian(lash_game_coords_t *coords, unsigned int *w, unsigned int *h, unsigned long int *index)
 * \brief Get the coordinate pair from a linear index
 * 
 * \param *coords pointer to the struct the coordinate pair will be stored
 * \param *w the width of the coordinate system
 * \param *h the height of the coordinate system
 * \param *index to calculate
 * 
 * If the index is out of bounds, the variable the index parameter points to will be left unchanged, and the function will return 1
 * 
 * \see lashCartesianToIndex
 * 
 */

