/*
 * Molecular dynamics simulation
 *
 * pr_02_1 - all pairs, two dimensions
 *
 *
 * (C)2004	D. C. Rapaport
 *	 This software is copyright material accompanying the book
 *	 "The Art of Molecular Dynamics Simulation", 2nd edition,
 *	 by D. C. Rapaport, published by Cambridge University Press (2004).
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "in_vdefs.h"
#include "in_mddefs.h"
#include "random.h"

#include "simulation.h"

typedef struct {
	VecR r, rv, ra;
} Mol;


// These variables are input to the simulation
VecI initUcell = { 20, 20 };
double deltaT = 0.005;
double density = 0.8;
double temperature = 1.0;
int stepAvg = 100, stepLimit = 1000;


// The following variables are computed during simulation
Prop kinEnergy, totEnergy;
Prop pressure;
Prop pressure_xx, pressure_xy, pressure_yx, pressure_yy;
double timeNow;
Mol *mol = NULL;
int nMol;
VecR region, vSum;
int stepCount;
double rCut, rMin, uCut;
double uSum, velMag, vvSum;
double virSum;
Ten2R2 tvirSum;


// Variables related to timing
time_t time_computations;


// Local function definitions
void ComputeForces (void);
void LeapfrogStep (int part);
void ApplyBoundaryCond (void);
void InitCoords (void);
void InitVels (void);
void InitAccels (void);
void EvalProps (void);
void write_velocities(const char *filename);
void write_distances(const char *filename);

double get_x_coordinate(int iMol)
{
	 return mol[iMol].r.x;	
}

double get_y_coordinate (int iMol)
{
	 return mol[iMol].r.y;	
}

double get_x_region(void)
{
	return region.x;
}		
		
double get_y_region(void)
{
	return region.y;
}		

void simulation_init(void)
{
	message("-----------------------------------------------------------------------\n");
	message("Initializing simulation\n");
	
	// Display simulation parameters
	PrintNameList();
	
	// Initialize random number generator
	InitRand(0); // 0 to use time as random seed

	// Calculate parameters
	rMin = pow (2., 1./6.);
	// rCut = 2.5;
	rCut = rMin;
	//uCut = 4. * (pow(rCut, 1./12.) - pow(rCut, 1./6.));
	uCut = 4. * (pow(rCut, -12.) - pow(rCut, -6.));
	message("Ucut = %8.4f\n", uCut);
	VSCopy (region, 1. / sqrt (density), initUcell);
	nMol = VProd (initUcell);
	velMag = sqrt (n_dimensions * (1. - 1. / nMol) * temperature);

	// Initialize data structures
	if (mol) free(mol);
	AllocMem (mol, nMol, Mol);
	stepCount = 0;
	InitCoords ();
	InitVels ();
	InitAccels ();
	AccumProps (0);
}

void simulation_run(void)
{
	unsigned int step;
	
	PrintSummaryHeader();
	
	// Reset time counters
	time_computations = 0;

	// Run simulation steps. This just continues where the previous simulation
	// left off, use simulation_init() to restart from the initial condition.
	running=1;
	for (step=0; step<stepLimit && running; step++) {
		simulation_step();
		
		// Update display every drawing_period steps
		if ( do_draw_discs && drawing_period && ( (step%drawing_period) == 0 ) ) {
			gui_draw_begin();
    		discs_clear();
			discs_draw();
			gui_draw_end();
		}
		// average reporting
		if ( stepAvg && (((step+1)%stepAvg) == 0) ) {
			AccumProps(2);	// Accumulate averages
			PrintSummary();	// Print averages
			AccumProps(0);	// Clear averages
		}
		// give the gui time to do something during run, if needed
		//gui_simulation_step();
	}
	
	// Print time counters
	message("Computations took %.4f s\n", time_computations/(float)CLOCKS_PER_SEC );
	
	// Finally write velocities and distances to a text file
	write_velocities("velocities.txt");
	write_distances("distances.txt");
}

void simulation_step(void)
{
	// Setup time counters for measuring this step's computation time
	time_t clock0, clocklast;
	clock0 = clock();
	
	// Do the real simulation step
	stepCount++;
	timeNow = stepCount * deltaT;
	LeapfrogStep (1);
	ApplyBoundaryCond ();
	ComputeForces ();
	LeapfrogStep (2);
	EvalProps ();
	AccumProps (1);
	
	// Update time counters
	clocklast = clock();
	time_computations += clocklast - clock0;
}

void ComputeForces ()
{
	VecR dr, fc;
	double fcVal, rr, rrCut, rri, rri3;
	int j1, j2, n;

	rrCut = Sqr (rCut);
	DO_MOL VZero (mol[n].ra);
	uSum = 0.;
	virSum = 0.;
	TZero (tvirSum);
	for (j1 = 0; j1 < nMol - 1; j1 ++) {
		for (j2 = j1 + 1; j2 < nMol; j2 ++) {
			VSub (dr, mol[j1].r, mol[j2].r);
			VWrapAll (dr);
			rr = VLenSq (dr);
			if (rr < rrCut) {
				rri = 1. / rr;
				rri3 = Cube (rri);
				fcVal = 48. * rri3 * (rri3 - 0.5) * rri;
				VSCopy (fc, fcVal, dr);
				VVAdd (mol[j1].ra, fc);
				VVSub (mol[j2].ra, fc);
				uSum += 4. * rri3 * (rri3 - 1.) - uCut;
				virSum += fcVal * rr;
				TVVAddDyad (tvirSum, dr, fc);
			}
		}
	}
}


void LeapfrogStep (int part)
{
	int n;

	if (part == 1) {
		DO_MOL {
			VVSAdd (mol[n].rv, 0.5 * deltaT, mol[n].ra);
			VVSAdd (mol[n].r, deltaT, mol[n].rv);
		}
	} else {
		DO_MOL VVSAdd (mol[n].rv, 0.5 * deltaT, mol[n].ra);
	}
}


void ApplyBoundaryCond (void)
{
	int n;

	DO_MOL VWrapAll (mol[n].r);
}


void InitCoords (void)
{
	VecR c, gap;
	int n, nx, ny;

	VDiv (gap, region, initUcell);
	message("Initial distance between particles: %f\n", gap);
	message("Box size: %f %f \n",region.x, region.y );
											
	n = 0;
	for (ny = 0; ny < initUcell.y; ny ++) {
		for (nx = 0; nx < initUcell.x; nx ++) {
			VSet (c, nx + 0.5, ny + 0.5);
			VMul (c, c, gap);
			VVSAdd (c, -0.5, region);
			mol[n].r = c;
			++ n;
		}
	}
}


void InitVels (void)
{
	int n;

	VZero (vSum);
	DO_MOL {
		VRand (&mol[n].rv);
		VScale (mol[n].rv, velMag);
		VVAdd (vSum, mol[n].rv);
	}
	DO_MOL VVSAdd (mol[n].rv, - 1. / nMol, vSum);
}


void InitAccels (void)
{
	int n;

	DO_MOL VZero (mol[n].ra);
}


void EvalProps ()
{
	double vv, vvMax;
	int n;
	Ten2R2 tvvSum;

	VZero (vSum);
	vvSum = 0.;
	TZero (tvvSum);
	DO_MOL {
		VVAdd (vSum, mol[n].rv);
		vv = VLenSq (mol[n].rv);
		vvSum += vv;
		TVAddDyad (tvvSum, mol[n].rv);
	}
	kinEnergy.val = 0.5 * vvSum / nMol;
	totEnergy.val = kinEnergy.val + uSum / nMol;
	pressure.val = density * (vvSum + virSum) / (nMol * n_dimensions);
	pressure_xx.val = density * (tvvSum.xx + tvirSum.xx) / nMol;
	pressure_xy.val = density * (tvvSum.xy + tvirSum.xy) / nMol;
	pressure_yx.val = density * (tvvSum.yx + tvirSum.yx) / nMol;
	pressure_yy.val = density * (tvvSum.yy + tvirSum.yy) / nMol;
}


void AccumProps (int icode)
{
	if (icode == 0) {
		PropZero (totEnergy);
		PropZero (kinEnergy);
		PropZero (pressure);
		PropZero (pressure_xx);
		PropZero (pressure_xy);
		PropZero (pressure_yx);
		PropZero (pressure_yy);
	} else if (icode == 1) {
		PropAccum (totEnergy);
		PropAccum (kinEnergy);
		PropAccum (pressure);
		PropAccum (pressure_xx);
		PropAccum (pressure_xy);
		PropAccum (pressure_yx);
		PropAccum (pressure_yy);
	} else if (icode == 2) {
		PropAvg (totEnergy, stepAvg);
		PropAvg (kinEnergy, stepAvg);
		PropAvg (pressure, stepAvg);
		PropAvg (pressure_xx, stepAvg);
		PropAvg (pressure_xy, stepAvg);
		PropAvg (pressure_yx, stepAvg);
		PropAvg (pressure_yy, stepAvg);
	}
}


void PrintSummaryHeader(void)
{
	message(" Step   Time    Sum(v)  Etot            Ekin            Pressure        Pressure_xx     Pressure_xy     Pressure_yx     Pressure_yy\n");
	
}

void PrintSummary(void)
{
	message("%5d %8.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f\n",
		 stepCount, timeNow, VCSum (vSum) / nMol, PropEst (totEnergy),
		 PropEst (kinEnergy), PropEst (pressure),
		 PropEst (pressure_xx), PropEst (pressure_xy), PropEst (pressure_yx),
		 PropEst (pressure_yy));
}

void PrintNameList(void)
{
	message("            lattice size (initUcell) = %3d X %3d\n", initUcell.x, initUcell.y);
	message("  # of integration steps (stepLimit) = %5d\n", stepLimit);
	message("             time step size (deltaT) = %.6f\n", deltaT);
	message("             average every (stepAvg) = %4d\n", stepAvg);
	message("update visual every (drawing_period) = %4d\n", drawing_period);
	message("           temperature (temperature) = %.6f\n", temperature);
	message("                   density (density) = %.6f\n", density);
}

void write_velocities(const char *filename)
{
	FILE *f;
	unsigned int n;
	
	f=fopen(filename, "w");
	if (!f) {
		message("Error: could not write velocities to %s.\n", filename);
		return;
	}
	
	fprintf(f,"  v.x     v.y     |v|\n");

	DO_MOL {
		fprintf(f,"%7.4f %7.4f %7.4f\n",mol[n].rv.x,mol[n].rv.y,VLen(mol[n].rv));
	}

	fclose(f);
}

void write_distances(const char *filename)
{
	FILE *f;
	VecR dr;
	double rr, rrCut;
	int i, j;

	f=fopen(filename, "w");
	if (!f) {
		message("Error: could not write distances to %s.\n", filename);
		return;
	}

	rrCut = Sqr (0.5 * Min(region.x, region.y));
	for (i = 0; i < nMol; i++) {
		for (j = 0; j < nMol; j++) {
			if (j == i) continue;
			VSub(dr, mol[i].r, mol[j].r);
			VWrapAll(dr);
			rr = VLenSq(dr);
			if (rr < rrCut) {
				fprintf(f,"%7.4f\n", sqrt(rr));
			}
		}
	}

	fclose(f);
}
