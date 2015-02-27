/*
 * Molecular Dynamics simulation definitions
 */
#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#include "in_vdefs.h"

/*
 * Simulation constants
 *
 * (some are declared using #define so they can be used to specify array sizes)
 * Default values are specified in simulation.c (except #defines)
 */

// Number of time steps to take
extern unsigned int n_t_steps;

extern VecI initUcell;
extern double deltaT, density, rCut, rMin, temperature, timeNow, uSum, velMag, vvSum;
extern int nMol, stepAvg, stepCount, stepLimit;
extern double virSum;


/*
 * Function prototypes
 */
double get_x_coordinate(int iMol);
double get_y_coordinate (int iMol);
double get_x_region(void);
double get_y_region(void);

void   simulation_init(void);
void   simulation_run(void);
void   simulation_step(void);

void   AccumProps (int icode);
void   PrintSummaryHeader(void);
void   PrintSummary(void);
void   PrintNameList(void);

/*
 * The following is defined in main-*.c
 */
extern int          running, do_draw_discs;
extern unsigned int disc_size, drawing_period;

void discs_clear(void);
void discs_draw(void);
void message(const char *msg, ...);
void gui_simulation_step(void);

void gui_draw_begin(void);
void gui_draw_end(void);

#endif /* __SIMULATION_H__ */
