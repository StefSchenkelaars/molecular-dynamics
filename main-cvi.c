/*
 * Molecular dynamics simulation LabWindows/CVI GUI program
 */
#include <ansi_c.h>
#include <cvirte.h>
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

#include "simulation.h"
#include "MD1_UI.h"

int hPanel;
FILE *logfile = NULL;
HANDLE hThread = 0;

/*
 * Purely GUI-related variables
 */

// Whether the simulation is running(1) or should be stopped(0)
int         running = 0;
// Whether to draw discs during simulation(1) or not(0)
int         do_draw_discs = 0;
// Initialization needed before run(1) or not(0)
char init_needed = 0;

unsigned int disc_size = 100; // in percent of potential minimum distance
unsigned int drawing_period = 100;


DWORD WINAPI gui_simulation_run(LPVOID lpParameter);


/*
 * The code
 */

// Program entry point: execution starts here
int main(int argc, char *argv[])
{
	if ((hPanel = LoadPanel (0, "MD1_UI.uir", PANEL)) < 0)
		return -1;
	
	// Open logfile
	if ( !(logfile=fopen("log.txt", "w")) ) {
		message("Error: couldn't open logfile 'log.txt'\n");
	}
	
	// Set the control values on screen to their defaults
	SetCtrlVal(hPanel, PANEL_NUM_DISCSIZE, disc_size);
	SetCtrlVal(hPanel, PANEL_NUM_STEPS, stepLimit);
	SetCtrlVal(hPanel, PANEL_NUM_DRAWPERIOD, drawing_period);
	SetCtrlVal(hPanel, PANEL_NUM_LOGPERIOD, stepAvg);
	SetCtrlVal(hPanel, PANEL_NUM_DELTAT, deltaT);
	SetCtrlVal(hPanel, PANEL_NUM_TEMP, temperature);
	SetCtrlVal(hPanel, PANEL_NUM_DENSITY, density);
	SetCtrlVal(hPanel, PANEL_NUM_SIZEX, initUcell.x);
	SetCtrlVal(hPanel, PANEL_NUM_SIZEY, initUcell.y);
	SetCtrlVal(hPanel, PANEL_TOG_DRAW, do_draw_discs);
	simulation_init();
	init_needed=0;

	// Run the program
	message("Press \"Run Simulation\" to run!\n");
	DisplayPanel (hPanel);
	RunUserInterface();
	// And clean it up again
	DiscardPanel (hPanel);
	
	if (logfile)
		fclose(logfile);
}

// Program close was requested
int CVICALLBACK end (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	if (event==EVENT_CLOSE) {
		QuitUserInterface(0);
	}
	return 0;
}

// We use one callback function for all value controls and use the control
// argument to distinguish between different controls.
int CVICALLBACK control_changed(int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if (event!=EVENT_COMMIT)
		return 0;

	switch (control) {
	case PANEL_NUM_DISCSIZE:
		GetCtrlVal(panel, control, &disc_size);
		break;

	case PANEL_NUM_STEPS:
		GetCtrlVal(panel, control, &stepLimit);
		break;

	case PANEL_NUM_DRAWPERIOD:
		GetCtrlVal(panel, control, &drawing_period);
		break;

	case PANEL_TOG_DRAW:
		GetCtrlVal(panel, control, &do_draw_discs);
		break;

	// The following make the simulation have to re-init

	case PANEL_NUM_LOGPERIOD:
		init_needed=1;
		GetCtrlVal(panel, control, &stepAvg);
		break;

	case PANEL_NUM_SIZEX:
		GetCtrlVal(panel, control, &initUcell.x);
		init_needed=1;
		break;
		
	case PANEL_NUM_SIZEY:
		GetCtrlVal(panel, control, &initUcell.y);
		init_needed=1;
		break;

	case PANEL_NUM_DELTAT:
		GetCtrlVal(panel, control, &deltaT);
		init_needed=1;
		break;

	case PANEL_NUM_TEMP:
		GetCtrlVal(panel, control, &temperature);
		init_needed=1;
		break;

	case PANEL_NUM_DENSITY:
		GetCtrlVal(panel, control, &density);
		init_needed=1;
		break;
	}

	return 0;
}

// We use one callback function for all buttons and use the control argument
// to distinguish between different buttons.
int CVICALLBACK button_pressed(int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if (event!=EVENT_COMMIT)
		return 0;

	switch(control) {
	case PANEL_BTN_DRAW:
		// Initialize if needed to make sure we draw most recent values
		if (init_needed) {
			simulation_init();
			init_needed=0;
		}
		discs_draw();
		break;

	case PANEL_BTN_CLEAR:
		discs_clear();
		break;

	case PANEL_BTN_RUN:
		GetCtrlVal(panel, control, &running);
		if (running) {
			// start simulation thread
			hThread = CreateThread(NULL, 0, &gui_simulation_run, NULL, 0, NULL);
		}
		else {
			// wait for the simulation thread to exit
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
		}
		break;

	case PANEL_BTN_RESET:
		simulation_init();
		init_needed=0;
		break;
	}

	return 0;
}

void gui_draw_begin(void)
{
	CanvasStartBatchDraw(hPanel, PANEL_CANVAS);
}

// Remove the discs from the canvas
void discs_clear(void)
{
	CanvasClear (hPanel, PANEL_CANVAS, VAL_ENTIRE_OBJECT);
}

// Draw the discs on the canvas
void discs_draw(void)
{
	int jj;
	int mulx=400, muly=400;
	double x, y, Lx, Ly;
	int size_x, size_y;
		

	SetCtrlAttribute (hPanel, PANEL_CANVAS, ATTR_PEN_COLOR, VAL_BLACK);
	SetCtrlAttribute (hPanel, PANEL_CANVAS, ATTR_PEN_FILL_COLOR, VAL_RED);
	
	Lx=get_x_region();
	Ly=get_y_region();
	
	// compute disc size; *2 because it's the diameter
	size_x = rMin/Lx * (float)mulx * (float)disc_size/100.0;
	size_y = rMin/Ly * (float)muly * (float)disc_size/100.0;
	
	// Draw the discs. Note that discs are drawn with their upper left corner
	// fixed. Also, the field is always drawn as square, even if Lx!=Ly so
	// circles in reality will be drawn as ovals.
	for (jj=0;jj<nMol;jj++) {
		x=get_x_coordinate(jj)+0.5*Lx; x=x/Lx; x=x*mulx;
		y=get_y_coordinate(jj)+0.5*Ly; y=y/Ly; y=y*muly;
 	  
		//message("x,y= %7.4f %7.4f\n", x,y);

		CanvasDrawOval (hPanel, PANEL_CANVAS,
			MakeRect(x, y, size_x, size_y),
			VAL_DRAW_FRAME_AND_INTERIOR);
	}

	// Make sure it's put on screen
	ProcessDrawEvents();
}

void gui_draw_end(void)
{
	CanvasEndBatchDraw(hPanel, PANEL_CANVAS);
}

DWORD WINAPI gui_simulation_run(LPVOID lpParameter)
{
	unsigned t;

	if (init_needed) {
		simulation_init();
		init_needed=0;
	}

	message("Starting simulation, %d steps\n", stepLimit);
	SetCtrlVal(hPanel, PANEL_BTN_RUN, 1);

	// Disable some GUI controls during simulation
	SetCtrlAttribute(hPanel, PANEL_NUM_LOGPERIOD, ATTR_DIMMED, 1); 
	SetCtrlAttribute(hPanel, PANEL_NUM_SIZEX, ATTR_DIMMED, 1); 
	SetCtrlAttribute(hPanel, PANEL_NUM_SIZEY, ATTR_DIMMED, 1); 
	SetCtrlAttribute(hPanel, PANEL_NUM_DELTAT, ATTR_DIMMED, 1); 
	SetCtrlAttribute(hPanel, PANEL_NUM_TEMP, ATTR_DIMMED, 1); 
	SetCtrlAttribute(hPanel, PANEL_NUM_DENSITY, ATTR_DIMMED, 1); 

	ProcessDrawEvents();

	simulation_run();

	// Now enable these controls again
	SetCtrlAttribute(hPanel, PANEL_NUM_LOGPERIOD, ATTR_DIMMED, 0); 
	SetCtrlAttribute(hPanel, PANEL_NUM_SIZEX, ATTR_DIMMED, 0); 
	SetCtrlAttribute(hPanel, PANEL_NUM_SIZEY, ATTR_DIMMED, 0); 
	SetCtrlAttribute(hPanel, PANEL_NUM_DELTAT, ATTR_DIMMED, 0); 
	SetCtrlAttribute(hPanel, PANEL_NUM_TEMP, ATTR_DIMMED, 0); 
	SetCtrlAttribute(hPanel, PANEL_NUM_DENSITY, ATTR_DIMMED, 0); 

	
	SetCtrlVal(hPanel, PANEL_BTN_RUN, 0);
	if (!running)
		message("Simulation aborted\n");
	
	running = 0;
	
	return 0;
}

/*// This is called every simulation step, it allows us to update the gui
// during simulation
void gui_simulation_step(void)
{
	if ( !(stepCount%25) ) ProcessSystemEvents();
}*/

/* A general function to print messages, can also used in simulation.c!
 * You don't need to understand how this works, you only need to know that you
 * can use something like
 * 	message("the value of counter i is now %d\n", i);
 * to put something on screen.
 */
void message(const char *msg, ...)
{
	char buf[4096];
	va_list ap;

	// Append message to text control and log file
	va_start(ap, msg);
	vsprintf(buf, msg, ap);
	SetCtrlVal(hPanel, PANEL_MESSAGES, buf);
	if (logfile) {
		vfprintf(logfile, msg, ap);
		fflush(logfile);
	}
	va_end(ap);
}

