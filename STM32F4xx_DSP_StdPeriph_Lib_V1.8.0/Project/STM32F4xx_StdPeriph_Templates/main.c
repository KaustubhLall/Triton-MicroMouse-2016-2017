#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "stm32f4xx.h"
#include "arm_math.h"
//__________________________________
/* true when we need to fill the cell with all walls */
int deadOn = 0;
/* previous to keep track which cells to fill */
int previousX = 0;
int previousY = 0;
/* for explore */
int exploreSteps = 0;
struct location {

  int x;
  int y;
};

/* cell to store the information of a cell in the 2d matrix */
struct cell {

  int wall;
  int distance;
  int visited;
};

/* this is the element for the array to be returned for speed run */
/* straight is the straight direction, and inst is the instruction when empty */
/* walls have been sighted */
struct instruction {

  int straight;
  char inst[50];
};

/* size of the maze */
const int SIZE = 16;

/* even maze */
const int CENTER_MIN = SIZE / 2 - 1;
const int CENTER_MAX = SIZE / 2;
const int NUM_CENTERS = 4;

/* starting cell */
const int START_Y = 0;
const int START_X = 15;

/* walls representations */
const int WALLS = 4;
const int TOP_WALL = 1;
const int RIGHT_WALL = 2;
const int BOTTOM_WALL = 4;
const int LEFT_WALL = 8;
const int ALL_WALLS = 15;
const int walls[WALLS] = {1, 2, 4, 8};

/* walls */
const int DEAD_WALLS = 3;
const int DEAD_WALLS_NEIGHBOR = 2;

/* directions representations */
const int DIRECTIONS = 4;
const int NORTH = 0;
const int EAST = 1;
const int SOUTH = 2;
const int WEST = 3;
const char *directions[DIRECTIONS] = {"North", "East", "South", "West"};
const char directionSymbols[DIRECTIONS] = {'^', '>', 'v', '<'};

/* maximum number of elements in the stack */
const int MAX_STACK = 257;

/* maximum number of steps to explore after reaching center */
const int MAX_EXPLORE = 60;

/* total number of cells in the maze */
const int TOTAL_CELLS = 257;

/* Variables for initializations */
/* the current direction and position of the mouse */
int currentDirection = NORTH;
struct location currentLocation = {START_X, START_Y};

/* maze */
struct cell theMaze[SIZE][SIZE] = {
  {{9,14,0}, {1,13,0}, {1,12,0}, {1,11,0}, {1,10,0}, {1,9,0}, {1,8,0}, {1,7,0}, {1,7,0}, {1,8,0}, {1,9,0}, {1,10,0}, {1,11,0}, {1,12,0}, {1,13,0}, {3,14,0}},
  {{8,13,0}, {0,12,0}, {0,11,0}, {0,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {0,10,0}, {0,11,0}, {0,12,0}, {2,13,0}},
  {{8,12,0}, {0,11,0}, {0,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {0,10,0}, {0,11,0}, {2,12,0}},
  {{8,11,0}, {0,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {0,10,0}, {2,11,0}},
  {{8,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {2,10,0}},
  {{8,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,2,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {2,9,0}},
  {{8,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,2,0}, {0,1,0}, {0,1,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {2,8,0}},
  {{8,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,2,0}, {0,1,0}, {0,0,0}, {0,0,0}, {0,1,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {2,7,0}},
  {{8,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,2,0}, {0,1,0}, {0,0,0}, {0,0,0}, {0,1,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {2,7,0}},
  {{8,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,2,0}, {0,1,0}, {0,1,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {2,8,0}},
  {{8,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,2,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {2,9,0}},
  {{8,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {2,10,0}},
  {{8,11,0}, {0,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {0,10,0}, {2,11,0}},
  {{8,12,0}, {0,11,0}, {0,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {0,10,0}, {0,11,0}, {2,12,0}},
  {{8,13,0}, {0,12,0}, {0,11,0}, {0,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {0,10,0}, {0,11,0}, {0,12,0}, {2,13,0}},
  {{14,14,1}, {12,13,0}, {4,12,0}, {4,11,0}, {4,10,0}, {4,9,0}, {4,8,0}, {4,7,0}, {4,7,0}, {4,8,0}, {4,9,0}, {4,10,0}, {4,11,0}, {4,12,0}, {4,13,0}, {6,14,0}}
};



/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
int move(struct cell theMaze[SIZE][SIZE], struct location *currentLocation,        
int *currentDirection);                                                            
void evaluation(struct cell theMaze[SIZE][SIZE], int *deadOn, int *previousX,   
int *previousY, int wallInfo, struct location *currentLocation,                    
int *currentDirection);                                                            
void fillCenter(struct cell theMaze[SIZE][SIZE],                                   
struct location *currentLocation, int *currentDirection);                          
int explore(struct cell theMaze[SIZE][SIZE], struct location *currentLocation,  
int *currentDirection);                                                            
int speedRunPath(struct instruction myInstructions[TOTAL_CELLS]);
/*----------------------------------------------------------------------------*/


//__________________________________
static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;
volatile uint64_t Millis = 0;
volatile uint64_t t0 = 0, t = 0, dt = 0;
volatile uint16_t ADC_Value[20];
float32_t IR_L = 0;
float32_t IR_LM = 0;
float32_t IR_LF = 0;
float32_t IR_RF = 0;
float32_t IR_RM = 0;
float32_t IR_R = 0;
float32_t Lmem = 0.000f, Rmem = 0.000f;
int64_t   REnc_mem = 0,  LEnc_mem = 0;
uint8_t ErrorFlag = 1;
volatile uint8_t runFlag = 0;
typedef struct
{
		float32_t translational;
		float32_t	rotational;
}CoordinateVelocity;
/* Private function prototypes -----------------------------------------------*/
void InitializeAlles(void);
void HSI_SetSysClock(uint32_t, uint32_t, uint32_t, uint32_t);
void SysTick_Init(uint32_t);
void LEDInit(void);
void ButtonInit(void);
void EmitterInit(void);
void TIM3_Config(void);
void TIM4_Config(void);
void Encoder_Configration(void);
void ResetREnc(void);
void ResetLEnc(void);
void USART_Config(void);
void ADC_Config(void);
void Error_Alert(char []);
void blinkLED(void);
void CheckUpdate(void);
uint64_t millis(void);
uint64_t micros(void);
void IR_Scan(void);
void IRscan_L(void);
void IRscan_LM(void);
void IRscan_LF(void);
void IRscan_RF(void);
void IRscan_RM(void);
void IRscan_R(void);
void EncoderInit(void);
/*
int move(struct cell theMaze[SIZE][SIZE], struct location *currentLocation,
int *);
void evaluation(theMaze, &deadOn, &previousX, &previousY,
    virtualMaze[currentLocation.x][currentLocation.y].wall, &currentLocation,
    &currentDirection);*/
int32_t REnc(void);
int32_t LEnc(void); 
int32_t Iabs(int32_t);
float32_t Ifabs(float32_t);
float32_t Isqrt(float32_t);
float32_t Icos(float32_t);
float32_t Isin(float32_t);
float32_t Itan(float32_t);
void quickSort(int32_t[], int32_t, int32_t);
uint8_t equal(float32_t, float32_t, float32_t);
//********************************************
void curve(float32_t, float32_t);
uint8_t Lwall = 0, Rwall = 0, Fwall = 0;
uint8_t nLwall = 0, nRwall = 0, nFwall = 0;
void currCell(void);
void checkCell(void);
void IRscan(void);
void UpdateSensors(void);
//*********************************
void motorInit(void);
void motor(float32_t, float32_t);
void halt(void);
/* ---------------------------------------------------------------------------*/
void measuring(void);
void Testing(void);
void motor_test(void);
void Encoder_test(void);
//***********************************************
/* ---------------------------------------------------------------------------*/
//----------------------------------------------------------------------------------
float32_t LEnc_mm(void) {
		return (float32_t)LEnc() / 286.000f; 
}
float32_t REnc_mm(void) {
		return (float32_t)REnc() / 286.000f; 
}
float32_t Disp(void) {
		return (LEnc_mm() + REnc_mm()) / 2.000f;
}
float32_t Angle(void) {
		return (float32_t)(REnc() - LEnc()) * 90.000f/30000.000f;
}
//**********************************************************************************
typedef struct {
		float32_t P, I, D;
		float32_t Ep, Ei, Ed;
		float32_t Kp, Ki, Kd;
		float32_t integral, differential;
		float32_t pExp;
		float32_t pAct;
		uint64_t T;
}PID;
float32_t PTester[1000], ITester[1000], DTester[1000];
uint64_t Ttrac[1000];
uint32_t Tindex; 
//-------------------------------------------------
PID Angular;
void initAngular_PID(float32_t Kp, float32_t Ki, float32_t Kd, float32_t Exp, float32_t Act) {
		Angular.Kp = Kp;
		Angular.Ki = Ki;
		Angular.Kd = Kd;
		Angular.pExp = Exp;
		Angular.pAct = Act;
		Angular.integral = 0.000f;
		Angular.P = Kp * (Exp - Act); 
		Angular.I = 0.000f; 
		Angular.D = 0.000f;
		Angular.T = micros();
}
float32_t Angular_PID(float32_t Exp, float32_t Act) {
		float32_t dT = (float32_t)(micros() - Angular.T) / 1000000.000f;
		//-------P--------//
		Angular.Ep = Exp - Act;
		float32_t P = Angular.Kp * Angular.Ep;
		
		if(dT > 0.01000f) {
				Angular.pExp = Exp;
				Angular.pAct = Act;
				Angular.T = micros();
				return P + Angular.I + Angular.D;
		}
		//-------I--------//
		if(Angular.pExp != Exp) Angular.integral = 0.000f;
		Angular.integral += (((Exp - Act) + (Angular.pExp - Angular.pAct)) / 2.0000000000f) * dT ;
		float32_t I = Angular.Ki * Angular.integral;
		//-------D--------//
		float32_t D;
		if(equal(Act, Angular.pAct, 0.000001)) D = 0.000f;
		else {
			Angular.differential = (Angular.pAct - Act) / (dT * 1000.000f);
			D = Angular.differential * Angular.Kd;
		}
		//----update-----//
		Angular.pExp = Exp;
		Angular.pAct = Act;
		Angular.T = micros();
		//----Tester-----//
		//PTester[Tindex] = P; ITester[Tindex] = I; DTester[Tindex] = D; Ttrac[Tindex] = Angular.T;
		//Tindex++;
		//----return-----//
		Angular.P = P; Angular.I = I; Angular.D = D;
		return (P + I + D);
		//return P;
}
//-------------------------------------------------------
PID Angular2;
void initAngular_PID2(float32_t Kp, float32_t Ki, float32_t Kd, float32_t Exp, float32_t Act) {
		Angular2.Kp = Kp;
		Angular2.Ki = Ki;
		Angular2.Kd = Kd;
		Angular2.pExp = Exp;
		Angular2.pAct = Act;
		Angular2.integral = 0.000f;
		Angular2.P = Kp * (Exp - Act); 
		Angular2.I = 0.000f; 
		Angular2.D = 0.000f;
		Angular2.T = micros();
}
float32_t Angular_PID2(float32_t Exp, float32_t Act) {
		float32_t dT = (float32_t)(micros() - Angular2.T) / 1000000.000f;
		//-------P--------//
		Angular2.Ep = Exp - Act;
		float32_t P = Angular2.Kp * Angular2.Ep;
		
		if(dT > 0.01000f) {
				Angular2.pExp = Exp;
				Angular2.pAct = Act;
				Angular2.T = micros();
				return P + Angular2.I + Angular2.D;
		}
		//-------I--------//
		if(Angular2.pExp != Exp) Angular2.integral = 0.000f;
		Angular2.integral += (((Exp - Act) + (Angular2.pExp - Angular2.pAct)) / 2.0000000000f) * dT ;
		float32_t I = Angular2.Ki * Angular2.integral;
		//-------D--------//
		float32_t D;
		if(equal(Act, Angular2.pAct, 0.000001)) D = 0.000f;
		else {
			Angular2.differential = (Angular2.pAct - Act) / (dT * 1000.000f);
			D = Angular2.differential * Angular2.Kd;
		}
		//----update-----//
		Angular2.pExp = Exp;
		Angular2.pAct = Act;
		Angular2.T = micros();
		//----Tester-----//
		//PTester[Tindex] = P; ITester[Tindex] = I; DTester[Tindex] = D; Ttrac[Tindex] = Angular2.T;
		//Tindex++;
		//----return-----//
		Angular2.P = P; Angular2.I = I; Angular2.D = D;
		return (P + I + D);
		//return P;
}
//-------------------------------------------------------
PID Linear;
void initLinear_PID(float32_t Kp, float32_t Ki, float32_t Kd, float32_t Exp, float32_t Act) {
		Linear.Kp = Kp;
		Linear.Ki = Ki;
		Linear.Kd = Kd;
		Linear.pExp = Exp;
		Linear.pAct = Act;
		Linear.integral = 0.000f;
		Linear.P = Kp * (Exp - Act); 
		Linear.I = 0.000f; 
		Linear.D = 0.000f;
		Linear.T = micros();
}
float32_t Linear_PID(float32_t Exp, float32_t Act) {
		float32_t dT = (float32_t)(micros() - Linear.T) / 1000000.000f;
		//-------P--------//
		Linear.Ep = Exp - Act;
		if(Linear.Ep > 80.000f) Linear.Ep = 80.000f;
		float32_t P = Linear.Kp * Linear.Ep;
		if(dT > 0.01000f) {
				Linear.pExp = Exp;
				Linear.pAct = Act;
				Linear.T = micros();
				return P + Linear.I + Linear.D;
		}
		//-------I--------//
		if(Linear.Ep <= 80.000f) { 
			if(Linear.pExp != Exp) Linear.integral = 0.000f;
			Linear.integral += (((Exp - Act) + (Linear.pExp - Linear.pAct)) / 2.0000000000f) * dT ;
		}
		float32_t I = Linear.Ki * Linear.integral;
		//-------D--------//
		float32_t D;
		if(equal(Act, Linear.pAct, 0.000001)) D = 0.000f;
		else {
			Linear.differential = (Act - Linear.pAct) / (dT * 1000.000f);
			D = Linear.differential * Linear.Kd;
		}
		//----update-----//
		Linear.pExp = Exp;
		Linear.pAct = Act;
		Linear.T = micros();
		//----return-----//
		Linear.P = P; Linear.I = I; Linear.D = D;
		return (P + I + D);
		//return P;
}
//------------------------------------------------------
void TestLinear_PID(void) {
		float32_t ExpAngle = Angle();
		float32_t AngPID_val = 0.000f;
		float32_t TerminalLoc;
		float32_t LinearPID_val = 0.000f;
		while(1) {
			
		initAngular_PID(3.33f, 0.350f, 35.333f, ExpAngle, Angle());
		t0 = millis();
		while(millis() - t0 < 200) {
				AngPID_val = Angular_PID(ExpAngle, Angle());
				curve(37, AngPID_val);
		}			
		initAngular_PID(2.00f, 1.350f, 19.533f, ExpAngle, Angle());
		initLinear_PID(0.543f, 0.075f, 15.53000f, TerminalLoc, Disp());
		TerminalLoc = Disp() + 160.000f;
		t0 = millis();
		while(millis() - t0 < 250) {
				LinearPID_val = Linear_PID(TerminalLoc, Disp());
				AngPID_val = Angular_PID(ExpAngle, Angle());
				curve(LinearPID_val, AngPID_val);
		}
		
		
		ExpAngle += 93.000f;
		initAngular_PID(0.50f, 0.93f, 10.733f, ExpAngle, Angle());
		t0 = millis();
		while(millis() - t0 < 2000) {
				AngPID_val = Angular_PID(ExpAngle, Angle());
				curve(0, AngPID_val);
		}			
		TerminalLoc = Disp() + 120.000f;
		t0 = millis();
		while(millis() - t0 < 250) {
				LinearPID_val = Linear_PID(TerminalLoc, Disp());
				AngPID_val = Angular_PID(ExpAngle, Angle());
				curve(LinearPID_val, AngPID_val);
		}
		ExpAngle += 93.0000f;
		t0 = millis();
		while(millis() - t0 < 2000) {
				AngPID_val = Angular_PID(ExpAngle, Angle());
				curve(0, AngPID_val);
		}		
		
	  }
		halt();
}
float32_t LinearPID_val = 0.000f;
float32_t LinearRrefscale = 0.000f;
float32_t TerminalLoc = 0.000f;
float32_t Loc = 0.000f;
float32_t refExp = 0.000f;
float32_t AngPID_val = 0.000f;
float32_t L_Err = 0.000f, R_Err = 0.000f;
float32_t Lref = 277.000f, Rref = 288.000f;
float32_t LRref_scale = 0.0123f;
uint8_t Ltmp = 0, Rtmp = 0;
char Flag;
uint32_t dirRef = 0;
void initSprintA(void) {
		Flag = 'A';
		Lref = 273.000f, Rref = 283.000f;
		LRref_scale = 0.0123f;
		Ltmp = 0; Rtmp = 0;
	  refExp = Angle();
		initAngular_PID(3.31f, 0.0000f, 73.000f, refExp, Angle());
		initLinear_PID(3.943f, 0.015f, 43.53000f, 0 , 0);
}
void SprintA(float32_t speed) {
		if(IR_L + IR_R < 430.000f) {
						  AngPID_val = Angular_PID(refExp, Angle());
							LinearRrefscale = (430.000f - (IR_L + IR_R))*0.02f;
						  if(LinearRrefscale > 0.000f) {
									LinearPID_val = Linear_PID(0, LinearRrefscale);	
							}
							else LinearPID_val = 5.000f;
				      curve(LinearPID_val, AngPID_val);
		}
		else {
					if(IR_LF < Lref) {
							L_Err = (Lref - IR_LF) * LRref_scale;
							Ltmp = 1;
							refExp -= L_Err;
					}
					else {	
							if(Ltmp) {
								refExp = Angle();
									Ltmp = 0;
							}
					}
					if(IR_RF < Rref) 
					{
							R_Err = (Rref - IR_RF) * LRref_scale;
							Rtmp = 1;
							refExp += R_Err;
					}
					else {	
							if(Rtmp) {
								refExp = Angle();
									Rtmp = 0;
							}
					}
					if(Ifabs(refExp - Angle()) >=  10.000f) Angular.Kd = 0.000f;
					AngPID_val = Angular_PID(refExp, Angle());
					curve(speed, AngPID_val);
		}
}
float32_t Bref = 0.000f;
void initSprintB(void) {
		Flag = 'B';
		Lref = 273.500f, Rref = 283.500f;
		LRref_scale = 0.0055f;
		Ltmp = 0; Rtmp = 0;
		Bref = Angle();
	  refExp = Bref;
		initAngular_PID(2.5f, 0.00000f, 10.000f, refExp, Angle());
		initLinear_PID(2.943f, 0.015f, 7.00000f, 0 , 0);
}
void SprintB(void) {
	
			if(IR_L + IR_R < 400.000f) {
						  AngPID_val = Angular_PID(refExp, Angle());
							LinearRrefscale = (400.000f - (IR_L + IR_R))*0.02f;
						  if(LinearRrefscale > 0.000f) {
									LinearPID_val = Linear_PID(0, LinearRrefscale);	
							}
							else LinearPID_val = 5.000f;
				      curve(LinearPID_val, AngPID_val);
			}
			else {
					if(IR_LF < Lref) {
							L_Err = (Lref - IR_LF) * LRref_scale;
							Ltmp = 1;
							refExp -= L_Err;
					}
					else {	
							if(Ltmp) {
								refExp = Bref;
									Ltmp = 0;
							}
					}
					if(IR_RF < Rref) 
					{
							R_Err = (Rref - IR_RF) * LRref_scale;
							Rtmp = 1;
							refExp += R_Err;
					}
					else {	
							if(Rtmp) {
								refExp = Bref;
									Rtmp = 0;
							}
					}
					if(Ifabs(refExp - Angle()) >=  10.000f) Angular.Kd = 0.000f;
					AngPID_val = Angular_PID(refExp, Angle());
					curve(17, AngPID_val);
			}
}
void initLAlign() {
		Lref = 207.000f, Rref = 180.000f;
		LRref_scale = 3.500f;
}
void LwallAlign() {
					if(IR_LM < Lref) L_Err = (Lref - IR_LM) * LRref_scale;
					if(IR_RM < Rref) R_Err = (Rref - IR_RM) * LRref_scale;
					curve(5.3, (-L_Err + R_Err) * 0.310f);
}
//*******************************************************
//-------------------------------------------------------
uint8_t Fcnt = 0, Lcnt = 0, Rcnt = 0;
uint8_t interrP() {
		if(Fwall) Fcnt ++;
		else Fcnt = 0;
		if(!Lwall) {
			//IRscan_LM(); 
			//checkCell();
			//if(!nLwall) 
			Lcnt ++;
		}
		else Lcnt = 0;
		if(!Rwall) {
			//IRscan_RM();
			//checkCell();
			//if(!nRwall) 
			Rcnt ++;
		}
		else Rcnt = 0;
		if(Fcnt >= 5 || Lcnt >= 3 || Rcnt >= 3) return 1;	
		return 0;
}
//**********************************************************************************
void sBreak() {
						t0 = millis();
						initAngular_PID(436.50f, 0.000f, 7300.533f, refExp, Angle());//35 700
						while(millis() - t0 < 700) {
							AngPID_val = Angular_PID(refExp, Angle());
							curve(0, AngPID_val);
						}
}
float32_t T90 = 99.3000f;
void sLeftTurn() {
						dirRef += 3;
						refExp = Angle();
						initAngular_PID(2.70f, 0.00f, 17.533f, refExp, Angle());
						IRscan_LM(); checkCell();
						while(!nLwall) {
							IRscan_LM(); checkCell();
							AngPID_val = Angular_PID(refExp, Angle());
							curve(-5.000f, AngPID_val);
						}
						halt(); Delay(350);
						refExp = Angle();
						TerminalLoc = Disp() + 148.000f;
						initLinear_PID(0.173f, 0.0375f, 0.053000f, TerminalLoc, Disp());
						t0 = millis();
						while(millis() - t0 < 300) {
								LinearPID_val = Linear_PID(TerminalLoc, Disp());
								AngPID_val = Angular_PID(refExp, Angle());
								curve(LinearPID_val, AngPID_val);
						}
						refExp += T90;
						initAngular_PID(3.95f, 0.03f, 20.733f, refExp, Angle());
						t0 = millis();
						while(millis() - t0 < 500) {
								AngPID_val = Angular_PID(refExp, Angle());
								if(Ifabs(AngPID_val) > 20.000f) AngPID_val = AngPID_val > 0.000f ? 25.000f : -25.000f; 
								curve(0, AngPID_val);
						}			
}
void sRightTurn() {
						dirRef += 1;
						refExp = Angle();
						initAngular_PID(2.70f, 0.00f, 17.533f, refExp, Angle());
						IRscan_RM(); checkCell();
						while(!nRwall) {
							IRscan_RM(); checkCell();
							AngPID_val = Angular_PID(refExp, Angle());
							curve(-5.000f, AngPID_val);
						}
						halt(); Delay(350);
						refExp = Angle();
						TerminalLoc = Disp() + 148.000f;
						initLinear_PID(0.173f, 0.0375f, 0.053000f, TerminalLoc, Disp());
						t0 = millis();
						while(millis() - t0 < 300) {
								LinearPID_val = Linear_PID(TerminalLoc, Disp());
								AngPID_val = Angular_PID(refExp, Angle());
								curve(LinearPID_val, AngPID_val);
						}
						refExp -= T90;
						initAngular_PID(3.95f, 0.03f, 20.733f, refExp, Angle());
						t0 = millis();
						while(millis() - t0 < 500) {
								AngPID_val = Angular_PID(refExp, Angle());
								if(Ifabs(AngPID_val) > 20.000f) AngPID_val = AngPID_val > 0.000f ? 25.000f : -25.000f; 
								curve(0, AngPID_val);
						}			
}
//***********************************
int8_t finished = 0;
int8_t LOnOffp = 0, ROnOffp = 0;
int8_t stateLR = 1;
int8_t LRtmp = 0;
int8_t LR() {
	LRtmp++;
	if(LRtmp >= 10) {
			LRtmp = 0;
			if(millis() % 2) return -1;
			else return 1;
	}		
	if(stateLR == 1) stateLR = -1;
	else {if(stateLR == -1) stateLR = 1;}
	return stateLR;
}
int8_t stateLF = 0;
int8_t LFtmp = 0;
int8_t LF() {
	LFtmp++;
	if(LFtmp >= 10) {
			LFtmp = 0;
			if(millis() % 2) return -1;
			else return 1;
	}	
	if(stateLF == -1) stateLF = 0;
	else {if(stateLF == 0) stateLF = -1;}
	return stateLF;
}
int8_t stateRF = 0;
int8_t RFtmp = 0;
int8_t RF() {
	RFtmp++;
	if(RFtmp >= 10) {
			RFtmp = 0;
			if(millis() % 2) return -1;
			else return 1;
	}	
	if(stateRF == 1) stateRF = 0;
	else {if(stateRF == 0) stateRF = 1;}
	return stateRF;
}
uint16_t tmp = 0;
int8_t UpdateFloodfill(char mode) {
	if(mode == 'H') {
		IRscan_LM(); IRscan_RM(); IRscan_L(); IRscan_R(); 
		checkCell(); currCell();
		if(Fwall) {
			if(!nLwall) {
				if(!nRwall) return LR();
				else return -1;
			}			
			else {
				if(!nRwall) return 1;
				else return 2;
			}
		}
		else {
			if(!nLwall) {
				if(!nRwall) {if(tmp >= 5) {tmp = 0; return LR();} tmp++; return 0;}
				else return LF();
			}			
			else {
				if(!nRwall) return RF();
				else return 0;
			}
		}
	}
	if(mode == 'D') {
		UpdateSensors(); 
		
		if(Fwall) {
			if(!Lwall) {
				if(!Rwall) return LR();
				else return -1;
			}			
			else {
				if(!Rwall) return 1;
				else return 2;
			}
		}
		else {
			if(!Lwall) {
				if(!Rwall) {if(tmp >= 5) {tmp = 0; return LR();} tmp++; return 0;}
				else return LF();
			}			
			else {
				if(!Rwall) return RF();
				else return 0;
			}
		}
	}
	if(theMaze[currentLocation.x][currentLocation.y].distance == 0) finished = 1;
	uint16_t dir = move(theMaze, &currentLocation, &currentDirection);
	if(dirRef == 0) {
			if(dir == 0) return 0;
			if(dir == 1) return 1;
			if(dir == 2) return 2;
			if(dir == 3) return -1;
	}
 if(dirRef == 1) {
			if(dir == 0) return -1;
			if(dir == 1) return 0;
			if(dir == 2) return 1;
			if(dir == 3) return 2;
	}
 	if(dirRef == 2) {
			if(dir == 0) return 2;
			if(dir == 1) return -1;
			if(dir == 2) return 0;
			if(dir == 3) return 1;
	}
	if(dirRef == 3) {
			if(dir == 0) return 1;
			if(dir == 1) return 2;
			if(dir == 2) return -1;
			if(dir == 3) return 0;
	}
	return 0;
}
//**********************************************************************************
int8_t dir = 0;
void LeftTurn() {
						dirRef += 3;
						refExp = Angle();
						refExp += T90 + 0.199f;
						initAngular_PID(4.35f, 0.03f, 31.733f, refExp, Angle());
						t0 = millis();
						while(millis() - t0 < 500) {
								AngPID_val = Angular_PID(refExp, Angle());
								if(Ifabs(AngPID_val) > 25.000f) AngPID_val = AngPID_val > 0.000f ? 25.000f : -25.000f; 
								curve(0, AngPID_val);
						}	
}
void RightTurn() {
						dirRef += 1;
						refExp = Angle();
						refExp -= T90 + 0.199f;
						initAngular_PID(4.35f, 0.03f, 31.733f, refExp, Angle());
						t0 = millis();
						while(millis() - t0 < 500) {
								AngPID_val = Angular_PID(refExp, Angle());
								if(Ifabs(AngPID_val) > 25.000f) AngPID_val = AngPID_val > 0.000f ? 25.000f : -25.000f; 
								curve(0, AngPID_val);
						}	
}
uint8_t canSprint() {
		IRscan_LM(); IRscan_RM(); checkCell();
		UpdateSensors();
		if(Lwall && Rwall && nLwall && nRwall) return 1;
		return 0;
}
void FrontAlign() {
		uint8_t FAtmp = 0;
		UpdateSensors();
		refExp = Angle();
		initAngular_PID(2.70f, 0.00f, 17.533f, refExp, Angle());
		t0 = millis();
		while(FAtmp <= 3 && millis() - t0 < 1500) {
			 UpdateSensors();
			 if(IR_L + IR_R < 480.000f) {halt(); FAtmp++;}
			 else {
					AngPID_val = Angular_PID(refExp, Angle());
					curve(5, AngPID_val);
			 }
		}
		halt();
}
float32_t CellLength = 161.0000f;
float32_t BufferLength = 15.000f;
void Turn180() {
		dirRef += 2;
		FrontAlign();
		LeftTurn();
		FrontAlign();
		LeftTurn();
		initSprintB();
					Loc = Disp();
					while(Disp() - Loc < CellLength - BufferLength) {
							UpdateSensors();
							IRscan_L(); IRscan_R(); checkCell();
							SprintB();
					}
					while(millis() - t0 < 300) {
								LinearPID_val = Linear_PID(TerminalLoc, Disp());
								AngPID_val = Angular_PID(refExp, Angle());
								if(LinearPID_val > 0.000f) LinearPID_val = 3;
								curve(LinearPID_val, AngPID_val);
						}
					
}
void updateCell() {
		uint16_t upDir = 0;
		dirRef %= 4;
		if(nLwall) {
			 if(dirRef == 0) upDir += 8;
			 if(dirRef == 1) upDir += 1;
			 if(dirRef == 2) upDir += 2;
			 if(dirRef == 3) upDir += 4;
	 }
	 if(nRwall) {
			 if(dirRef == 0) upDir += 2;
			 if(dirRef == 1) upDir += 4;
			 if(dirRef == 2) upDir += 8;
			 if(dirRef == 3) upDir += 1;
	 }
	 if(Fwall) {
			 if(dirRef == 0) upDir += 1;
			 if(dirRef == 1) upDir += 2;
			 if(dirRef == 2) upDir += 4;
			 if(dirRef == 3) upDir += 8;
	 }
	 evaluation(theMaze, &deadOn, &previousX, &previousY, upDir, &currentLocation,&currentDirection);
}
void SearchingMode(void) {
			ResetLEnc();
			ResetREnc();
			uint8_t Scnt = 0;
			while(1) {
					if(finished == 1) break;
					if(canSprint()) Scnt ++;
					else Scnt = 0;
					if(Scnt >= 2) {
							UpdateFloodfill('D');
							break;
					}
					initSprintB();
					Loc = Disp();
					uint8_t fuck = 0;
					t0 = millis();
					while((Disp() - Loc < CellLength - BufferLength) && (millis() - t0 < 1500)) {
							UpdateSensors();
							IRscan_L(); IRscan_R(); checkCell();
							SprintB();
							if(nLwall) LOnOffp ++;
							else LOnOffp --;
							if(nRwall) ROnOffp ++;
							else ROnOffp --;
					}
					if(millis() - t0 >= 1500) fuck = 1;
					if(LOnOffp > 0) nLwall = 1;
					else nLwall = 0;
					if(ROnOffp > 0) nRwall = 1;
					else nRwall = 0;
					//updateCell();
					dir = UpdateFloodfill('H');
					if(dir != 0) {
						TerminalLoc = Disp() + BufferLength;
						initAngular_PID(3.75f, 0.03f, 33.733f, refExp, Angle());
						initLinear_PID(90.03f, 0.0175f, 0.0000f, TerminalLoc, Disp());
					 if(!fuck) {
						t0 = millis();
						while(millis() - t0 < 300) {
								LinearPID_val = Linear_PID(TerminalLoc, Disp());
								AngPID_val = Angular_PID(refExp, Angle());
								if(LinearPID_val > 0.000f) LinearPID_val = 3;
								curve(LinearPID_val, AngPID_val);
						}
					 }
						UpdateSensors(); checkCell();
						if(nFwall) {
								FrontAlign();
						}
						if(dir == -1) LeftTurn();
						if(dir == 1) RightTurn();
						if(dir == 2) Turn180();
					}
					else {
						Loc = Disp();
						while(Disp() - Loc < BufferLength) {UpdateSensors(); SprintB();}
					}
			}
			initSprintA();
}
void straightPhaseFix() {
		halt(); blinkLED();
		IRscan_LM(); IRscan_RM();
		checkCell(); 
		if(!nLwall) {
				    refExp = Angle();
						initAngular_PID(2.70f, 0.00f, 17.533f, refExp, Angle());
						IRscan_LM(); checkCell();
						while(!nLwall) {
							IRscan_LM(); checkCell(); 
							AngPID_val = Angular_PID(refExp, Angle());
							//printf("\r%f %d\r\n",IR_LM, nLwall);
							curve(-5.000f, AngPID_val);
							//LED1_ON(); LED2_OFF(); LED3_OFF();
							//blinkLED();
						}
						refExp = Angle();
						TerminalLoc = Disp() + 113.000f;
						initLinear_PID(0.173f, 0.0375f, 0.053000f, TerminalLoc, Disp());
						t0 = millis();
						while(millis() - t0 < 1000) {
								LinearPID_val = Linear_PID(TerminalLoc, Disp());
								AngPID_val = Angular_PID(refExp, Angle());
								curve(LinearPID_val, AngPID_val);
						}
		}
		if(!nRwall) {
				refExp = Angle();
						initAngular_PID(2.70f, 0.00f, 17.533f, refExp, Angle());
						IRscan_RM(); checkCell();
						while(!nRwall) {
							IRscan_RM(); checkCell();
							AngPID_val = Angular_PID(refExp, Angle());
							curve(-5.000f, AngPID_val);
							LED3_ON(); LED2_OFF(); LED1_OFF();
						}
						refExp = Angle();
						TerminalLoc = Disp() + 113.000f;
						initLinear_PID(0.173f, 0.0375f, 0.053000f, TerminalLoc, Disp());
						t0 = millis();
						while(millis() - t0 < 1000) {
								LinearPID_val = Linear_PID(TerminalLoc, Disp());
								AngPID_val = Angular_PID(refExp, Angle());
								curve(LinearPID_val, AngPID_val);
						}
		}
}

//**********************************************************************************
float32_t CellcntSuper = 0.000f;
void SuperUpdate() {
		uint16_t CellCnt = (uint16_t)(CellcntSuper / CellLength );
		dirRef %= 4;
		uint8_t upDir = 0;
		switch(dirRef) {
			case 0 : upDir = 10;  break;
			case 1 : upDir = 5;  break;
			case 2 : upDir = 10; break;
			case 3 : upDir = 5; break;
		}
		for(uint16_t i = 0; i < CellCnt; i++) {		 
			evaluation(theMaze, &deadOn, &previousX, &previousY,
			upDir, &currentLocation,
			&currentDirection);
			move(theMaze, &currentLocation, &currentDirection);
		}
		IRscan_LM(); IRscan_RM(); UpdateSensors(); checkCell();
		updateCell();
}

/*int main(int argc, char* argv[]) {
  phaseOne();
  phaseTwo();
  phaseThree();
  phaseFour();
*/
int main(void) 
{
	  InitializeAlles();
		LED1_OFF(); LED2_OFF(); LED3_OFF();		
		//measuring();
		//Testing();
		//printf("\rEnter r or R to run motors\r\n");
		//while(!runFlag) ; 
		initSprintA();
		//initSprintB();
	  //move(theMaze, &currentLocation, &currentDirection);
		uint8_t fir = 1;
		while(!finished) {
				UpdateSensors();
				if(fir) {
						fir = 0;
						SearchingMode();
 				}
				if(interrP()) {
						//CellcntSuper = Disp();
						sBreak();
						//SuperUpdate();
						dir = UpdateFloodfill('H');
				
						//printf("\r%d\r\n", dir);
						//while(1) halt();
						if(dir == 1) {
							LED3_ON(); LED2_OFF(); LED1_OFF();
							sRightTurn();
							//while(1) halt();
						}
						if(dir == -1) {
							LED1_ON(); LED2_OFF(); LED3_OFF();
							sLeftTurn();
							//while(1) halt();
						}
						if(dir == 0) {LED2_ON(); LED1_OFF(); LED3_OFF(); straightPhaseFix();}
						if(dir == 2) {LED2_OFF(); LED1_OFF(); LED3_OFF(); Turn180(); }
						ResetLEnc();
						ResetREnc();
						SearchingMode();
				}
				else SprintA(37);
		}
		return 0;
}
//***********************************************************************************
uint32_t samp[10000];
uint32_t IR_sample(void) {
		
		EM_LF();
		int i = 0;
		t = 0;
		for(t0 = micros(), t = t0; micros() - t0 <= 500; ) {
				if(micros() - t >= 1) {
					samp[i++] = RE_LF();
					t = micros();
				}
		}			
		XEM_LF();
		Delay(100);
		return i;
}
void measuring(void) {
		//while(1) printf("\r%f\r\n",Disp());
		//while(1) printf("\r%f\r\n",(float32_t)REnc() / 286.100f);
		//while(1) printf("\r%f\r\n",Angle());
		while(1) {
				IRscan_L();
				IRscan_LF();
				IRscan_LM();
				IRscan_RM();
				IRscan_RF();
				IRscan_R();
				//printf("\r%0.3f %0.3f\r\n",IR_L, IR_R);
				printf("\r[L] %0.1f    \r",IR_L);
				printf("\r[LM] %0.1f    \r",IR_LM);
				printf("\r[LF] %0.1f    \r",IR_LF);
				printf("\r[RF] %0.1f    \r",IR_RF);
				printf("\r[RM] %0.1f    \r",IR_RM);
				printf("\r[R] %0.1f    \r\n",IR_R);
		}
		/*
	  halt();
		uint32_t val;
	  val = REnc() - LEnc();
		IRscan_LF();
		while(IR_LF > 261.000f) curve(0, 10);
		halt();
		val = REnc() - LEnc() - val;
		printf("\r%lld\r\n",val); // 0~2Pi vs 0~val : 2Pi / val
		
		while(1) printf("\r%f\r\n",(float32_t)(REnc()-LEnc())* 2 * Pi / val);*/
}
void Testing(void) {
		//TestLinear_PID();
}
//***********************************************************************************
uint32_t RE(uint8_t ch) {
		if(ch == 1) return RE_L();
		else if(ch == 2) return RE_LM();
		else if(ch == 3) return RE_LF();
		else if(ch == 4) return RE_RF();
		else if(ch == 5) return RE_RM();
		else if(ch == 6) return RE_R();
		return 0;
}
int32_t collecting_IR_data(uint8_t ch, uint16_t n) {
		int32_t Val[n];
		for(int i = 0; i < n; i++) Val[i] = (int32_t)RE(ch);
		quickSort(Val, 0, n-1);
		return Val[n/2];
} 

void checkCell(void){
		if(IR_LM < 300) nLwall = 1;
		else nLwall = 0;
		if(IR_RM < 300) nRwall = 1;
		else nRwall = 0;
		if(IR_L < 500 && IR_R < 500) nFwall = 1;
		else nFwall = 0;
}
void currCell(void) {
		if(IR_LF < 550) Lwall = 1;
		else Lwall = 0;
		if(IR_RF < 550) Rwall = 1;
		else Rwall = 0;
		if(IR_L < 400 && IR_R < 400) Fwall = 1;
		else Fwall = 0;
}
void UpdateSensors(void) {
		IRscan();
		currCell();
}
void IRscan(void) { 
		IRscan_L(); IRscan_R(); IRscan_LF(); IRscan_RF();
}
void IRscan_L() {		
		int32_t val;
		EM_L();
		Delay_us(300);
		val = collecting_IR_data(1, 20);
		XEM_L();
		IR_L = (10000*1.000f/Isqrt((float32_t)(val)));
		Delay_us(500);
}
void IRscan_LM() {
		int32_t val;
		EM_LM();
		Delay_us(300);
		val = collecting_IR_data(2, 20);
		XEM_LM();
		IR_LM = (10000*1.000f/Isqrt((float32_t)(val)));
		Delay_us(500);
}
void IRscan_LF() {
		int32_t val;
		EM_LF();
		Delay_us(300);
		val = collecting_IR_data(3, 20);
		XEM_LF();
	  IR_LF = (10000*1.000f/Isqrt((float32_t)(val)));
		Delay_us(500);
}
void IRscan_RF() {
		int32_t val;
		EM_RF();
		Delay_us(300);
		val = collecting_IR_data(4, 20);
		XEM_RF();
		IR_RF = ((10000*1.000f/Isqrt((float32_t)(val))));
		Delay_us(500);
}
void IRscan_RM() {
		int32_t val;
		EM_RM();
		Delay_us(300);
		val = collecting_IR_data(5, 20);
		XEM_RM();
		IR_RM = ((10000*1.000f/Isqrt((float32_t)(val))));
		Delay_us(500);
}
void IRscan_R() {
		int32_t val;
		EM_R();
		Delay_us(300);
		val = collecting_IR_data(6, 20);
		XEM_R();
		IR_R = (10000*1.000f/Isqrt((float32_t)(val)));
		Delay_us(500);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void motorInit() {
		LpwmA_CCR = 0;
		LpwmB_CCR = 0;
		RpwmA_CCR = 0;
		RpwmB_CCR = 0;
}
void motor(float32_t L, float32_t R) 
{
		uint32_t Lpwm, Rpwm;
		if(L != Lmem) {
			if(L >= 0.000f) 
			{
					if(L > 100.000f) L = 100.000f;
					Lpwm = (uint32_t)(L * 10.000f);
					if(Lpwm > 0) Lpwm --;
					LpwmB_CCR = Lpwm;
					LpwmA_CCR = 0;
			}
			else
			{
					if(L < -100.000f) L = -100.000f;
					Lpwm = (uint32_t)(L * -10.000f);
					if(Lpwm > 0) Lpwm --;
					LpwmB_CCR = 0;
					LpwmA_CCR = Lpwm;
			}
		}
		if(R != Rmem) {
			if(R >= 0.000f) 
			{
					if(R > 100.000f) R = 100.000f;
					Rpwm = (uint32_t)(R * 10.000f);
					if(Rpwm > 0) Rpwm --;
					RpwmB_CCR = Rpwm;
					RpwmA_CCR = 0;
			}
			else
			{
					if(R < -100.000f) R = -100.000f;
					Rpwm = (uint32_t)(R * -10.000f);
					if(Rpwm > 0) Rpwm --;
					RpwmB_CCR = 0;
					RpwmA_CCR = Rpwm;
			}
		}
		Lmem = L;
		Rmem = R;
}
/* MatLab
>> rref([R/2 R/2 v;-1/2 1/2 w])
				[  R/2, R/2, v] [wL]
				[ -1/2, 1/2, w]	[wR]
ans =
[ 1, 0, (v - R*w)/R] wL
[ 0, 1, (v + R*w)/R] wR
*/
void curve(float32_t v, float32_t w) {
		//motor((v/Radius-w)/LwDp,(v/Radius+w)/RwDp);
		motor((v-w),(v+w));
}
void halt() {
		motor(0,0);
}
void ResetREnc() {
		TIM2->CNT = 0;
		REnc_mem = 0;
}
void ResetLEnc() {
		TIM5->CNT = 0;
		LEnc_mem = 0;
}
void EncoderInit(void) {
		ResetLEnc();
		ResetREnc();
}
int32_t REnc(void) 
{
		if(Iabs(TIM2->CNT) >= 400000000) {
				REnc_mem += TIM2->CNT;
				ResetREnc();
		}
	  return TIM2->CNT + REnc_mem;
}
int32_t LEnc(void) 
{
  	if(Iabs(TIM5->CNT) >= 400000000) {
				LEnc_mem += TIM5->CNT;
				ResetLEnc();
		}
	  return TIM5->CNT + LEnc_mem;
}
void blinkLED(void) {
			LED1_ON();
			Delay(50);
		  LED1_OFF();
			LED2_ON();
			Delay(50);
			LED2_OFF();
		  LED3_ON();
		  Delay(50);
			LED3_OFF();
			Delay(50);
			LED3_ON();
			Delay(50);
		  LED3_OFF();
			LED2_ON();
			Delay(50);
			LED2_OFF();
		  LED1_ON();
		  Delay(50);
			LED1_OFF();
			Delay(50);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void motor_test() {
		int16_t i;
			for(i = 0; i <= 100; i++) {
				motor(i,i);
						printf("\r%d %d\r\n",LEnc(), REnc());
				Delay(10);
			}
			for(; i >= -100; i--) {
				motor(i,i);
						printf("\r%d %d\r\n",LEnc(), REnc());
				Delay(10);
			}
			for(; i <= 0; i++) {
				motor(i,i);
						printf("\r%d %d\r\n",LEnc(), REnc());
				Delay(10);
			}
}
void Encoder_test() {
		motor(100,-50);
		t0 = millis();
		while(millis() - t0 <= 5000) {
			printf("\r%d %d \r\n",LEnc(), REnc());
		}
		halt();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void swap(int32_t* a, int32_t* b)
{
    int32_t t = *a;
    *a = *b;
    *b = t;
}
int32_t partition (int32_t a[], int32_t low, int32_t high)
{
    int32_t pivot = a[high];   
    int32_t i = (low - 1);  
    for (uint32_t j = low; j <= high - 1; j++) {
        if (a[j] <= pivot) {
            i++; 
            swap(&a[i], &a[j]);
        }
    }
    swap(&a[i+1], &a[high]);
    return (i+1);
}
void quickSort(int32_t a[], int32_t low, int32_t high)
{
    if (low < high) {
        int p_i = partition(a, low, high);
        quickSort(a, low, p_i - 1);
        quickSort(a, p_i + 1, high);
    }
}
int32_t Iabs(int32_t x) {
	return x < 0 ? -x : x;
}
float32_t Ifabs(float32_t x) {
	return x < 0.00f ? -x : x;
}
float32_t Isqrt(float32_t x) 
{
		if(x == 0) return 0.00f;
		if(x < 0) return -1.00f;
		return __sqrtf(x);
}
float32_t Icos(float32_t x) {
		return arm_cos_f32(x);
}
float32_t Isin(float32_t x) {
		return arm_sin_f32(x);
}
float32_t Itan(float32_t x) {
		float32_t tmp = Icos(x);
		if(Icos(x) == 0) Error_Alert("tan");
		return Isin(x) / Icos(x);
}
uint8_t equal(float32_t A, float32_t B, float32_t errorRange) {
		if(Ifabs(A-B) < errorRange) return 1;
		else return 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
uint64_t millis(void)
{
	 return Millis;
}
uint64_t micros(void)
{
	 return Millis*1000 + 1000 - (uint64_t)((SysTick->VAL)/(SystemCoreClock / 1000000)); 
}
void Delay_us(__IO uint64_t Time) 
{
		uint64_t T_init = micros(); 
		while(micros() - T_init < Time); 
}
void Delay(__IO uint32_t nTime)
{ 
		uwTimingDelay = nTime;
		while(uwTimingDelay != 0);
}
void Millis_Increment() 
{
		Millis++;
}
void TimingDelay_Decrement(void)
{
		if (uwTimingDelay != 0x00)
		{ 
				uwTimingDelay--;
		}
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CheckUpdate()
{
		UpdateSensors();
		return;
}
void Error_Alert(char s[])
{
		printf("\rError!!!\r\n");
		printf("\r%s\r\n",s);
		while(ErrorFlag) blinkLED();
}
//*****************************************************************************
void InitializeAlles(void)
{
		//SystemInit();
		SystemCoreClockUpdate();
	  SysTick_Init(1000); // 1ms interrupt
		//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  LEDInit();
		ButtonInit();
		EmitterInit();
		//TIM3_Config();
		TIM4_Config();
		Encoder_Configration();
	  USART_Config();
		ADC_Config();
		motorInit();
		EncoderInit();
	  for(uint8_t i = 0; i < 3; i++)	blinkLED();
		printf("\r!!!!!Triton Drei ~ %d !\r\n",SystemCoreClock);
		if(Radius <= 0) Error_Alert("Radius");
		if(LwDp == 0 || RwDp == 0) Error_Alert("wDp");
		Delay(1000);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void HSI_SetSysClock(uint32_t m, uint32_t n, uint32_t p, uint32_t q) 
{ 
    __IO uint32_t HSIStartUpStatus = 0; 
	  RCC_DeInit(); 
	  RCC_HSICmd(ENABLE); 
	  HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY; 
    if (HSIStartUpStatus == RCC_CR_HSIRDY) { 
				RCC->APB1ENR |= RCC_APB1ENR_PWREN; 
				PWR->CR |= PWR_CR_VOS; 
	      RCC_HCLKConfig(RCC_SYSCLK_Div1); 
		    RCC_PCLK2Config(RCC_HCLK_Div2); 
	      RCC_PCLK1Config(RCC_HCLK_Div4); 
		    RCC_PLLConfig(RCC_PLLSource_HSI, m, n, p, q); 
		    RCC_PLLCmd(ENABLE); 
		    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {} 
        PWR->CR |= PWR_CR_ODEN; 
        while ((PWR->CSR & PWR_CSR_ODRDY) == 0) {} 
        PWR->CR |= PWR_CR_ODSWEN; 
        while ((PWR->CSR & PWR_CSR_ODSWRDY) == 0) {} 
        FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;  
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
        while (RCC_GetSYSCLKSource() != 0x08) {} 
	  } 
		else Error_Alert("HSI");  
}  
void SysTick_Init(uint32_t ticks)  
{       
    /* SystemFrequency / 1000    1ms 
       SystemFrequency / 100000  10us 
       SystemFrequency / 1000000 1us 
      */ 
    if (SysTick_Config(SystemCoreClock / ticks)) { 
		 /* Capture error */ 
			 Error_Alert("SysTick_Init"); 
    } 
}	
void LEDInit(void)
{
	  LED1RCC_AHB();
	  GPIO_InitTypeDef LED1;
		LED1.GPIO_Pin   = LED1_Pin;
		LED1.GPIO_Mode  = GPIO_Mode_OUT; 
		LED1.GPIO_OType = GPIO_OType_PP; 
		LED1.GPIO_PuPd  = GPIO_PuPd_NOPULL; 
		LED1.GPIO_Speed = GPIO_Speed_100MHz; 
		GPIO_Init(LED1_GPIOX, &LED1);
		LED2RCC_AHB();
	  GPIO_InitTypeDef LED2;
		LED2.GPIO_Pin   = LED2_Pin;
		LED2.GPIO_Mode  = GPIO_Mode_OUT; 
		LED2.GPIO_OType = GPIO_OType_PP; 
		LED2.GPIO_PuPd  = GPIO_PuPd_NOPULL; 
		LED2.GPIO_Speed = GPIO_Speed_100MHz; 
		GPIO_Init(LED2_GPIOX, &LED2);
		LED3RCC_AHB();
	  GPIO_InitTypeDef LED3;
		LED3.GPIO_Pin   = LED3_Pin;
		LED3.GPIO_Mode  = GPIO_Mode_OUT; 
		LED3.GPIO_OType = GPIO_OType_PP; 
		LED3.GPIO_PuPd  = GPIO_PuPd_NOPULL; 
		LED3.GPIO_Speed = GPIO_Speed_100MHz; 
		GPIO_Init(LED3_GPIOX, &LED3);
}
void ButtonInit(void)
{
	  BUTTONRCC_AHB();
		GPIO_InitTypeDef Buttom;
		Buttom.GPIO_Pin   = Button_Pin;
		Buttom.GPIO_Mode  = GPIO_Mode_IN;
		Buttom.GPIO_PuPd  = GPIO_PuPd_DOWN; 
		Buttom.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(Button_GPIOX, &Buttom);
}
void TIM3_Config(void) {
		uint32_t arr ;
		uint32_t psc ;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; 
		NVIC_InitTypeDef NVIC_InitStructure;  
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);   
		TIM_TimeBaseInitStructure.TIM_Period = arr;  
		TIM_TimeBaseInitStructure.TIM_Prescaler=psc; 
		TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
		TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;  
		TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure); 
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);  	 
		NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE; 
		NVIC_Init(&NVIC_InitStructure);
		TIM_Cmd(TIM3,ENABLE); 
}
void EmitterInit(void)
{
		EM1RCC();
	  GPIO_InitTypeDef EM1;
		EM1.GPIO_Pin   = EM1_Pin;
		EM1.GPIO_Mode  = GPIO_Mode_OUT; 
		EM1.GPIO_OType = GPIO_OType_PP; 
		EM1.GPIO_PuPd  = GPIO_PuPd_DOWN; 
		EM1.GPIO_Speed = GPIO_Speed_100MHz; 
		GPIO_Init(EM1_GPIOX, &EM1); 
		
		EM2RCC();
	  GPIO_InitTypeDef EM2;
		EM2.GPIO_Pin   = EM2_Pin;
		EM2.GPIO_Mode  = GPIO_Mode_OUT; 
		EM2.GPIO_OType = GPIO_OType_PP; 
		EM2.GPIO_PuPd  = GPIO_PuPd_DOWN; 
		EM2.GPIO_Speed = GPIO_Speed_100MHz; 
		GPIO_Init(EM2_GPIOX, &EM2);
	
		EM3RCC();
	  GPIO_InitTypeDef EM3;
		EM3.GPIO_Pin   = EM3_Pin;
		EM3.GPIO_Mode  = GPIO_Mode_OUT; 
		EM3.GPIO_OType = GPIO_OType_PP; 
		EM3.GPIO_PuPd  = GPIO_PuPd_DOWN; 
		EM3.GPIO_Speed = GPIO_Speed_100MHz; 
		GPIO_Init(EM3_GPIOX, &EM3);		
		
		EM4RCC();
	  GPIO_InitTypeDef EM4;
		EM4.GPIO_Pin   = EM4_Pin;
		EM4.GPIO_Mode  = GPIO_Mode_OUT; 
		EM4.GPIO_OType = GPIO_OType_PP; 
		EM4.GPIO_PuPd  = GPIO_PuPd_DOWN; 
		EM4.GPIO_Speed = GPIO_Speed_100MHz; 
		GPIO_Init(EM4_GPIOX, &EM4);
		
		EM5RCC();
	  GPIO_InitTypeDef EM5;
		EM5.GPIO_Pin   = EM5_Pin;
		EM5.GPIO_Mode  = GPIO_Mode_OUT; 
		EM5.GPIO_OType = GPIO_OType_PP; 
		EM5.GPIO_PuPd  = GPIO_PuPd_DOWN; 
		EM5.GPIO_Speed = GPIO_Speed_100MHz; 
		GPIO_Init(EM5_GPIOX, &EM5);
		
		EM6RCC();
	  GPIO_InitTypeDef EM6;
		EM6.GPIO_Pin   = EM6_Pin;
		EM6.GPIO_Mode  = GPIO_Mode_OUT; 
		EM6.GPIO_OType = GPIO_OType_PP; 
		EM6.GPIO_PuPd  = GPIO_PuPd_DOWN; 
		EM6.GPIO_Speed = GPIO_Speed_100MHz; 
		GPIO_Init(EM6_GPIOX, &EM6);
}
void TIM4_GPIO_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	  TIM4_RCC();
	  TIM4_GPIO_RCC();
		GPIO_InitStructure.GPIO_Pin = RpwmA | RpwmB | LpwmA | LpwmB;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
		GPIO_Init(GPIOB, &GPIO_InitStructure); 
		GPIO_PinAFConfig(GPIOB, RpwmAsource, GPIO_AF_TIM4);
		GPIO_PinAFConfig(GPIOB, RpwmBsource, GPIO_AF_TIM4); 
		GPIO_PinAFConfig(GPIOB, LpwmAsource, GPIO_AF_TIM4);
		GPIO_PinAFConfig(GPIOB, LpwmBsource, GPIO_AF_TIM4); 
}
void TIM4_Config(void) 
{
		uint16_t PrescalerValue = 0;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
		TIM4_GPIO_Config();
		PrescalerValue = (uint16_t) ((SystemCoreClock/2)/1000/21000) - 1;
		TIM_TimeBaseStructure.TIM_Period = 999;
		TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		// Channel1 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OC1Init(TIM4, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
		// Channel2 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OC2Init(TIM4, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
		// Channel3 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OC3Init(TIM4, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
		// Channel4 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OC4Init(TIM4, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
		TIM_ARRPreloadConfig(TIM4, ENABLE);
		TIM_Cmd(TIM4, ENABLE);
}
void Encoder_Configration(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;                           
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_TIM2);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2);
		TIM_SetAutoreload (TIM2, 0xffffffff);
		TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);
		TIM_Cmd(TIM2, ENABLE);

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);
		TIM_SetAutoreload (TIM5, 0xffffffff);
		TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);
		TIM_Cmd(TIM5, ENABLE);
}
void ADC_GPIO_Config(void)  
{  
		GPIO_InitTypeDef GPIO_InitStructure;    
		RCC_AHB1PeriphClockCmd(ADC_GPIO_CLK1, ENABLE);  
		GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; 
		GPIO_Init(ADC_GPIO_PORT1, &GPIO_InitStructure); 
	
	  RCC_AHB1PeriphClockCmd(ADC_GPIO_CLK2, ENABLE);  
		GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; 
		GPIO_Init(ADC_GPIO_PORT2, &GPIO_InitStructure);

		RCC_AHB1PeriphClockCmd(ADC_GPIO_CLK3, ENABLE);  
		GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; 
		GPIO_Init(ADC_GPIO_PORT3, &GPIO_InitStructure); 
	
		RCC_AHB1PeriphClockCmd(ADC_GPIO_CLK4, ENABLE);  
		GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; 
		GPIO_Init(ADC_GPIO_PORT4, &GPIO_InitStructure); 
		
		RCC_AHB1PeriphClockCmd(ADC_GPIO_CLK5, ENABLE);  
		GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; 
		GPIO_Init(ADC_GPIO_PORT5, &GPIO_InitStructure); 
		
		RCC_AHB1PeriphClockCmd(ADC_GPIO_CLK6, ENABLE);  
		GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; 
		GPIO_Init(ADC_GPIO_PORT6, &GPIO_InitStructure); 
} 
void ADC_Config(void) 
{  
	  ADC_GPIO_Config();
    DMA_InitTypeDef DMA_InitStructure;  
    ADC_InitTypeDef ADC_InitStructure;  
		ADC_CommonInitTypeDef ADC_CommonInitStructure;  
    RCC_APB2PeriphClockCmd(ADC_CLK , ENABLE);  
	  RCC_AHB1PeriphClockCmd(ADC_DMA_CLK, ENABLE); 
	  DMA_InitStructure.DMA_Channel = ADC_DMA_CHANNEL; 
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_DR_ADDR; 
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC_Value; 
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; 
    DMA_InitStructure.DMA_BufferSize = NofCHANEL; 
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	  //DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; 
    DMA_InitStructure.DMA_PeripheralDataSize = 
    DMA_PeripheralDataSize_HalfWord; 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; 
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; 
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull; 
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 
    DMA_Init(ADC_DMA_STREAM, &DMA_InitStructure); 
    DMA_Cmd(ADC_DMA_STREAM, ENABLE); 
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; 
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; 
    ADC_CommonInitStructure.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled; 
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles; //10 
    ADC_CommonInit(&ADC_CommonInitStructure); 
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; 
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;  
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 
    ADC_InitStructure.ADC_NbrOfConversion = NofCHANEL;
    ADC_Init(ADCx, &ADC_InitStructure); 
    ADC_RegularChannelConfig(ADCx, ADC_CHANNEL1, 1, ADC_SampleTime_xxxCycles); 
    ADC_RegularChannelConfig(ADCx, ADC_CHANNEL2, 2, ADC_SampleTime_xxxCycles);
    ADC_RegularChannelConfig(ADCx, ADC_CHANNEL3, 3, ADC_SampleTime_xxxCycles); 
  	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL4, 4, ADC_SampleTime_xxxCycles); 
		ADC_RegularChannelConfig(ADCx, ADC_CHANNEL5, 5, ADC_SampleTime_xxxCycles); 
		ADC_RegularChannelConfig(ADCx, ADC_CHANNEL6, 6, ADC_SampleTime_xxxCycles); 
    ADC_DMARequestAfterLastTransferCmd(ADCx, ENABLE); 
    ADC_DMACmd(ADCx, ENABLE); 
    ADC_Cmd(ADCx, ENABLE); 
    ADC_SoftwareStartConv(ADCx); 
} 
void USART_Config(void)  
{  
		GPIO_InitTypeDef GPIO_InitStructure;  
		USART_InitTypeDef USART_InitStructure;  
		RCC_AHB1PeriphClockCmd(USARTx_RX_GPIO_CLK|USARTx_TX_GPIO_CLK,ENABLE); 
		USARTx_CLOCKCMD(USARTx_CLK, ENABLE); 
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //50
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
		GPIO_InitStructure.GPIO_Pin =  USARTx_TX_PIN  ; 
		GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure); 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
		GPIO_InitStructure.GPIO_Pin =  USARTx_RX_PIN; 
		GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure); 
    GPIO_PinAFConfig(USARTx_RX_GPIO_PORT,USARTx_RX_SOURCE,USARTx_RX_AF); 
		GPIO_PinAFConfig(USARTx_TX_GPIO_PORT,USARTx_TX_SOURCE,USARTx_TX_AF);
		USART_InitStructure.USART_BaudRate = USARTx_BAUDRATE; 
		USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = 
    USART_HardwareFlowControl_None; 
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
    USART_Init(USARTx, &USART_InitStructure);
		
		/*
		USART_OverSampling8Cmd(USART1, ENABLE);
		USART_ClockInitTypeDef USART_ClockInitstructure;
		USART_ClockInitstructure.USART_Clock   = USART_Clock_Disable ;
		USART_ClockInitstructure.USART_CPOL    = USART_CPOL_High ;
		USART_ClockInitstructure.USART_LastBit = USART_LastBit_Disable;
		USART_ClockInitstructure.USART_CPHA    = USART_CPHA_1Edge;
		USART_ClockInit(USART1, &USART_ClockInitstructure);*/
		
		NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);		
	  USART_Cmd(USARTx, ENABLE); 
}
int fputc(int ch, FILE *f)  
{ 
    USART_SendData(USARTx, (uint8_t) ch);  
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) CheckUpdate();  
    return (ch); 
} 
int fgetc(FILE *f) 
{ 
    while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET) CheckUpdate(); 
    return (int)USART_ReceiveData(USARTx); 
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef struct {
		float32_t Ep, Ei, Ed;
		float32_t Kp, Ki, Kd;
		float32_t integral, I_threshold;
		float32_t Exp;
		uint64_t T;
}PID2;
//***********************************************************************
PID2 FB;
void initFrontBlock_PID(float32_t Kp, float32_t Ki, float32_t Kd,float32_t Ed, float32_t Exp, float32_t Act) {
		FB.Exp = Exp;
		float32_t Error = Act - Exp;
		FB.Kp = Kp; FB.Ki = Ki; FB.Kd = Kd; 
		FB.Ed = Ed; FB.Ei = Error; 
		FB.integral = 0; 
		FB.T = micros();
}
float32_t FrontBlock_PID(float32_t Ed, float32_t Act) {
		uint64_t dT = micros() - FB.T;
		float32_t Error = Act - FB.Exp;
		if(Error > 0.000f) Error *= 4.300f;
		if(Error > 61.000f ) Error = 61.000f; 
		FB.integral += FB.Ki * ((FB.Ei + Error) * dT / 2); 
		float32_t P, I, D;
		P = FB.Kp * Error;
		if(P >= 30.000f) P = 30.000f;
		I = FB.Ki * FB.integral / 100000000000.000f;
	  if(Ifabs(I) >= 50.000f) I = I >= 0.000f ? 50.000f : -50.000f;
	  D = FB.Kd * ((Ed - FB.Ed) / dT) / 1000.000f;
		//if(Ifabs(D) >= 80.000f) D = D >= 0.000f ? 80.000f : -100.000f;
		FB.Ed = Ed;
		FB.Ei = Error; 
		FB.T = micros();
		float32_t ans = P+I+D;
		if(ans > 50.000f) ans = 50.000f; 
		return ans;
}
//*****************************************************************************
PID2 FB2;
void initFrontBlock_PID2(float32_t Kp, float32_t Ki, float32_t Kd,float32_t Ed, float32_t Exp, float32_t Act) {
		FB2.Exp = Exp;
		float32_t Error = Act - Exp;
		FB2.Kp = Kp; FB2.Ki = Ki; FB2.Kd = Kd; 
		FB2.Ed = Ed; FB2.Ei = Error; 
		FB2.integral = 0; 
		FB2.T = micros();
}
float32_t FrontBlock_PID2(float32_t Ed, float32_t Act) {
		uint64_t dT = micros() - FB2.T;
		float32_t Error = Act - FB2.Exp;
		if(Error > 0.000f) Error *= 4.300f;
		if(Error > 61.000f ) Error = 61.000f; 
		FB2.integral += FB2.Ki * ((FB2.Ei + Error) * dT / 2); 
		float32_t P, I, D;
		P = FB2.Kp * Error;
		if(P >= 30.000f) P = 30.000f;
		I = FB2.Ki * FB2.integral / 100000000000.000f;
	  if(Ifabs(I) >= 50.000f) I = I >= 0.000f ? 50.000f : -50.000f;
	  D = FB2.Kd * ((Ed - FB2.Ed) / dT) / 1000.000f;
		//if(Ifabs(D) >= 80.000f) D = D >= 0.000f ? 80.000f : -100.000f;
		FB2.Ed = Ed;
		FB2.Ei = Error; 
		FB2.T = micros();
		float32_t ans = P+I+D;
		if(ans > 50.000f) ans = 50.000f; 
		return ans;
}
//***********************************************************************************************
void TestAngular_PID(void) {
	/*
		motor(-10,10);
		Delay(1000);
		float32_t ExpAngle = Angle();
		initAngular_PID(1.000f, 1.000f, 1.000f, ExpAngle, Angle());
		for(Tindex = 0; Tindex < 100; Angular_PID(ExpAngle, Angle()));
		Delay(1000);
		halt();
		for(uint16_t i = 0; i < Tindex; i++) {
				printf("\r[T = %lld]\r\n",Ttrac[i]); 
				printf("     [P = %f]\r\n",PTester[i]);
				printf("     [I = %f]\r\n",ITester[i]);
				printf("     [D = %f]\r\n",DTester[i]);
		}*/

		while(1) {
			    
		}			
						/*
					if(IR_LF < Lref){
							L_Err = (Lref - IR_LF) * LRref_scale;
						  if(IR_RF < Rref) {
									R_Err = (Rref - IR_RF) * LRref_scale;
									refExp = R_Err - L_Err + Angle(); 
							}								
							else refExp = -L_Err + Angle();
					}
					else {
							if(IR_RF < Rref) {
									R_Err = (Rref - IR_RF) * LRref_scale;
									refExp = R_Err + Angle();
							}								
							else ;
					}*/
		/*
		float32_t ExpAngle = Angle();
		float32_t AngPID_val = 0.000f;
		initAngular_PID(3.00f, 0.100f, 1.000f, ExpAngle, Angle());
		while(1) {
		t0 = millis();
		while(millis() - t0 < 800) {
				AngPID_val = Angular_PID(ExpAngle, Angle());
				//printf("\r%f\r\n",AngPID_val);
				Delay(3);
				curve(37, AngPID_val);
		}			
		halt();
		Delay(1000);
		
		ExpAngle -= 200.000f;
		t0 = millis();
		while(millis() - t0 < 1000) {
				AngPID_val = Angular_PID(ExpAngle, Angle());
				curve(0,AngPID_val);
		}		
		halt();
		}*/
}















//****************************************************************************
/* location to represent the position of the cells in the 2d matrix */

/* Helper functions start here */

int noWalls(int wallInfo, int currentDirection) {

  for (int i = 0; i < DIRECTIONS; i++) {

    if (i != currentDirection && i != (currentDirection + 2) % DIRECTIONS) {

      if ((walls[i] & wallInfo) == 0) {

        return 1;
      }
    }
  }

  return 0;  
}

void initializeArray(int outputArray[TOTAL_CELLS]) {

  for (int i = 0; i < TOTAL_CELLS; i++) {

    outputArray[i] = -1;
  }
}

void copyArray(int visitedNeighbors[DIRECTIONS],
int enterableNeighbors[DIRECTIONS]) {

  for (int i = 0; i < DIRECTIONS; i++) {

    visitedNeighbors[i] = enterableNeighbors[i];
  }
}

struct location popStack(struct location myStack[MAX_STACK], int* top) {

  struct location returnElement = myStack[*top];

  myStack[*top].x = 0;
  myStack[*top].y = 0;

  *top = *top - 1;

  return returnElement;
}

void pushStack(struct location myStack[MAX_STACK], int* top,
struct location newElement) {

  *top = *top + 1;

  myStack[*top].x = newElement.x;
  myStack[*top].y = newElement.y;
}

/*
 * Name: isCenter()
 * Parameters: x - this is the x coordinate to be checked.
 * y - this is the y coordinate to be checked.
 * Description: This function will check whether or not the given coordinates
 * combine is one of the centers.
 * Return: A boolean indicating that the given coordinates combine is one of the
 * centers.
 */
int isCenter(int x, int y) {

  /* check if it's one of the centers */
  if (x >= CENTER_MIN && x <= CENTER_MAX) {

    if (y >= CENTER_MIN && y <= CENTER_MAX) {

      return 1;
    }
  }

  /* otherwise, return 0 */
  return 0;
}

/*
 * Name: isOut()
 * Parameters: x - this is the x coordinate to be checked.
 * y - this is the y coordinate to be checked.
 * Description: This function will check whether or not the given coordinates
 * are out of bounds of the 2D array.
 * Return: A boolean indicating that the given coordinates are out of bounds.
 */
int isOut(int x, int y) {

  /* check if x and y is out of bounds of the maze array */
  if (x < 0 || x >= SIZE) {

    return 1;
  }

  else if (y < 0 || y >= SIZE) {

    return 1;
  }

  /* otherwise, return 0 */
  return 0;
}

/*
 * Name: numWalls()
 * Parameters: walls - this is the walls to be counted.
 * Description: This function will be able to count the number of walls from the
 * given walls.
 * Return: An int representing the number of walls around this cell.
 */
int numWalls(int walls) {

  /* the mask to get the first bit */
  int mask = 1;

  /* the counter of 1's */
  int count = 0;

  /* for loop to count the number of walls */
  for (int i = 0; i < DIRECTIONS; i++) {

    if ((walls >> i & mask) == 1) {

      count++;
    }
  }

  return count;
}

/*
 * Name: neighborsVisited()
 * Parameters: currentX - the x position of the current location.
 * currentY - they y position of the current location.
 * theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * enterableNeighbors - the array to be populated with distances if the cell
 * is enterable from the current location.
 * Description: This function will be able to determine which neighbors are
 * enterable and populate the given array with the distances. It will keep the
 * element as -1 if there is a wall.
 */
void neighborsVisited(int currentX, int currentY,
struct cell theMaze[SIZE][SIZE], int visitedNeighbors[DIRECTIONS]) {

  /* check all directions whether this neighbor is visited */
  for (int i = 0; i < DIRECTIONS; i++) {

    /* 
     * the i represents the current direction being examined, if there is no
     * wall then add the distance to the array
     */
    switch (i) {

      case NORTH:

        if (theMaze[currentX - 1][currentY].visited == 1 &&
        visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 1;
        }

        else if (visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 0;
        }

        break;

      case EAST:

        if (theMaze[currentX][currentY + 1].visited == 1 &&
        visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 1;
        }

        else if (visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 0;
        }

        break;

      case SOUTH:

        if (theMaze[currentX + 1][currentY].visited == 1 &&
        visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 1;
        }

        else if (visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 0;
        }

        break;

      case WEST:

        if (theMaze[currentX][currentY - 1].visited == 1 &&
        visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 1;
        }

        else if (visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 0;
        }

        break;
    }
  }
}

/*
 * Name: enterableCells()
 * Parameters: currentX - the x position of the current location.
 * currentY - they y position of the current location.
 * theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * enterableNeighbors - the array to be populated with distances if the cell
 * is enterable from the current location.
 * Description: This function will be able to determine which neighbors are
 * enterable and populate the given array with the distances. It will keep the
 * element as -1 if there is a wall.
 */
void enterableCells(int currentX, int currentY, struct cell theMaze[SIZE][SIZE],
int enterableNeighbors[DIRECTIONS]) {

  int currentWall = theMaze[currentX][currentY].wall;

  /* check all directions whether this neighbor is enterable */
  for (int i = 0; i < DIRECTIONS; i++) {

    /* 
     * the i represents the current direction being examined, if there is no
     * wall then add the distance to the array
     */
    switch (i) {

      case NORTH:

        if ((currentWall & TOP_WALL) == 0) {

          enterableNeighbors[i] = theMaze[currentX - 1][currentY].distance;
        }
        break;

      case EAST:

        if ((currentWall & RIGHT_WALL) == 0) {

          enterableNeighbors[i] = theMaze[currentX][currentY + 1].distance;
        }
        break;

      case SOUTH:

        if ((currentWall & BOTTOM_WALL) == 0) {

          enterableNeighbors[i] = theMaze[currentX + 1][currentY].distance;
        }
        break;

      case WEST:

        if ((currentWall & LEFT_WALL) == 0) {

          enterableNeighbors[i] = theMaze[currentX][currentY - 1].distance;
        }
        break;
    }
  }
}

/*
 * Name: findMaxDistance()
 * Parameters: enterableNeighbors() - this is the array that contains the
 * distances of the neighbors.
 * Description: This function will find the max distance to center, except if
 * the distance is -1, unenterable.
 * Return: an integer representing the smallest distance in given array.
 */
int findMaxDistance(int enterableNeighbors[DIRECTIONS]) {

  /* this will keep track of the smallest distance */
  int max = -1;

  /* check all distances */
  for (int i = 0; i < DIRECTIONS; i++) {

    int currentDistance = enterableNeighbors[i];

    if (currentDistance != -1 && currentDistance > max) {

      max = currentDistance;
    }
  }

  return max;
}

/*
 * Name: findMinDistance()
 * Parameters: enterableNeighbors() - this is the array that contains the
 * distances of the neighbors.
 * Description: This function will find the least distance to center, except if
 * the distance is -1, unenterable.
 * Return: an integer representing the smallest distance in given array.
 */
int findMinDistance(int enterableNeighbors[DIRECTIONS]) {

  /* this will keep track of the smallest distance */
  int min = SIZE * SIZE;

  /* check all distances */
  for (int i = 0; i < DIRECTIONS; i++) {

    int currentDistance = enterableNeighbors[i];

    if (currentDistance != -1 && currentDistance < min) {

      min = currentDistance;
    }
  }

  return min;
}

/*
 * Name: stepAtDirection()
 * Parameters: currentLocation - the current location of the mouse.
 * direction - the direction to take a step to.
 * Description: This function will update the current location to a neighbor for
 * a step given the specified direction.
 */
void stepAtDirection(struct location *currentLocation, int direction) {

    switch (direction) {

      case NORTH:
        currentLocation->x -= 1;
        break;
      
      case EAST:
        currentLocation->y +=1;
        break;

      case SOUTH:
        currentLocation->x += 1;
        break;

      case WEST:
        currentLocation->y -= 1;
        break;
    }
}

/*
 * Name: addWall()
 * Parameters: currentX - the x position of the neighbor.
 * currentY - the y position of the neighbor.
 * theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * wall - the side of the wall to add.
 * Description: This function will update the wall of the given location with
 * the side of the wall.
 */
void addWall(int currentX, int currentY, struct cell theMaze[SIZE][SIZE],
int wall) {

  /* add the wall at the given location */
  if (!isOut(currentX, currentY)) {

    int *tempWall = &(theMaze[currentX][currentY].wall);
    *tempWall = *tempWall | wall;
  }  
}

/*
 * Name: updateNeighborWall()
 * Parameters: currentX - the x position of the current location.
 * currentY - the y position of the current location.
 * theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * direction - the direction of the neighbor to add wall.
 * Description: This function will be able to add the corresponding wall of the
 * the neighbor with the corresponding direction.
 */
void updateNeighborWall(int currentX, int currentY,
struct cell theMaze[SIZE][SIZE], int direction) {

  /* update the wall of the neighbor */
  switch (direction) {

    /* north side neighbor */
    case NORTH:

      addWall(currentX - 1, currentY, theMaze, BOTTOM_WALL);
      break;

    /* east side neighbor */
    case EAST:

      addWall(currentX, currentY + 1, theMaze, LEFT_WALL);
      break;

    /* south side neighbor */
    case SOUTH:

      addWall(currentX + 1, currentY, theMaze, TOP_WALL);
      break;

    /* west side neighbor */
    case WEST:

      addWall(currentX, currentY - 1, theMaze, RIGHT_WALL);
      break;
  }
}

/*
 * Name: populateStack()
 * Parameters: currentX - the x position of the current cell.
 * currentY - the y position of the current cell.
 * direction - the direction of the neighbor to be examined.
 * myStack - the stack to populate.
 * Description: This function will be able to populate the stack with the given
 * neighbor that is not out of bounds.
 */
void populateStack(int currentX, int currentY, int direction,
struct location myStack[MAX_STACK], int *top) {

  switch (direction) {

    case NORTH:
      
      if (!isOut(currentX - 1, currentY)) {

        pushStack(myStack, top, (struct location) {currentX - 1, currentY});
      }
      break;

    case EAST:
      
      if (!isOut(currentX, currentY + 1)) {

        pushStack(myStack, top, (struct location) {currentX, currentY + 1});
      }
      break;

    case SOUTH:
      
      if (!isOut(currentX + 1, currentY)) {

        pushStack(myStack, top, (struct location) {currentX + 1, currentY});
      }
      break;

    case WEST:
      
      if (!isOut(currentX, currentY - 1)) {

        pushStack(myStack, top, (struct location) {currentX, currentY - 1});
      }
      break;
  }
}

/*
 * Name: printMaze()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * currentLocation - the current location of the mouse.
 * currentDirection - the current direction of the mouse.
 * Description: This function has the ability to print the maze using the ASCII
 * characters to represent the walls and the mouse with the given maze
 * representation as defined.
 */
void printMaze(struct cell theMaze[SIZE][SIZE], struct location currentLocation,
int currentDirection) {

  /* get the x and y of the mouse's location */
  int mouseX = currentLocation.x;
  int mouseY = currentLocation.y;

  /* print the walls on the top row */
  for (int i = 0; i < SIZE; i++) {

    printf(" ");
    printf("=");
  }

  /* the extra space at the end of top row */
  printf(" \n");

  /* print the left or bottom walls of the current cell if any */
  for (int i = 0; i < SIZE; i++) {

    /*
     * iterate to print left walls, if any, then print the car if at the current
     * cell
     */
    for (int j = 0; j < SIZE; j++) {

      /* get the current wall */
      int currentWall = theMaze[i][j].wall;

      /* check if there is a left wall, if so, print it */
      if ((currentWall & LEFT_WALL) != 0) {

        printf("|");
      }

      else {

        printf(" ");
      }

      /* check if the mouse is present at this cell, if so, print it */
      if (i == mouseX && j == mouseY) {

        printf("%c", directionSymbols[currentDirection]);
      }

      else {

        printf(" ");
      }
    }

    /* the wall at the very end, then new line */
    printf("|\n");

    /* iterate to print the bottom walls, if any */
    for (int j = 0; j < SIZE; j++) {

      /* get the current wall */
      int currentWall = theMaze[i][j].wall;

      /* check if there is a bottom wall, if so, print it */
      if ((currentWall & BOTTOM_WALL) != 0) {

        printf(" =");
      }

      else {

        printf("  ");
      }
    }

    /* the space at the very end, then new line */
    printf(" \n");
  }
}

/*
 * Name: printArray()
 * Parameters: theMaze - the 2D array to be printed.
 * Description: This function will print the given 2D array, where each element
 * is a cell, which represents walls and distance.
 */
void printArray(struct cell theMaze[SIZE][SIZE]) {

  for (int i = 0; i < SIZE; i++) {

    for (int j = 0; j < SIZE; j++) {

      // printf("(%2d,%2d,%d)", theMaze[i][j].wall, theMaze[i][j].distance,
      theMaze[i][j].visited;
    }

    // printf("\n");
  }
}

/*
 * Name: checkStatus()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * currentLocation - the current location of the mouse.
 * currentDirection - the current direction of the mouse.
 * Description: This function will be used to check the current state of the
 * maze and mouse for debugging purposes.
 */
void checkStatus(struct cell theMaze[SIZE][SIZE], struct location currentLocation,
int currentDirection) {

  /* check status of the maze and mouse */
  // printMaze(theMaze, currentLocation, currentDirection);
  /* printArray(theMaze); */
}

/*
 * Name: fillWalls()
 * Parameters: currentX - the x position of the location to be isolated.
 * currentY - the y position of the location to be isolated.
 * theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * Description: This function will be able to isolate the cell from other cells,
 * by filling it with walls on all sides because it is a dead end.
 */
void fillWalls(int currentX, int currentY, struct cell theMaze[SIZE][SIZE]) {

  int enterableNeighbors[DIRECTIONS] = {-1, -1, -1, -1};

  enterableCells(currentX, currentY, theMaze, enterableNeighbors);

  for (int i = 0; i < DIRECTIONS; i++) {

    if (enterableNeighbors[i] != -1) {

      updateNeighborWall(currentX, currentY, theMaze, i);
    }
  }

  theMaze[currentX][currentY].wall = ALL_WALLS;
}

/*----------------------------------------------------------------------------*/
/* Core functions start here */

void printEnter(int enter[DIRECTIONS], struct location myElement) {

  // printf("(%2d,%2d) Enterable: ", myElement.x, myElement.y);
  for (int i = 0; i < DIRECTIONS; i++) {

    // printf("%d, ", enter[i]);
  }

  // printf("\n");
}

/*
 * Name: updateDistances()
 * Parameters: currentLocation - the location of the current that is being
 * examined.
 * theMaze - the 2D array representing the maze, where each element is a cell
 * that has a wall and distance member.
 * neighbors - this is an array containing the neighbors that are enterable,
 * represented by a value of 1, 0 otherwise.
 * Description: This function will be able to update the distances of the cells
 * starting from the current cell and the neighbors that has new walls added to
 * them.
 */
void updateDistances(struct location currentLocation,
struct cell theMaze[SIZE][SIZE], int neighbors[DIRECTIONS]) {

  /* get the x and y position of the current location */
  int currentX = currentLocation.x;
  int currentY = currentLocation.y;

  /* stack to store the potentially changing cell's distance */
  struct location myStack[MAX_STACK] = {0};
  int top = -1;

  /* push the current cell */
  pushStack(myStack, &top, currentLocation);

  /* iterate through all directions */
  for (int i = 0; i < DIRECTIONS; i++) {

    // if this neighbor has been set to 1, then put it to stack
    if (neighbors[i] == 1) {

      /* push the neighbor if it's valid, if so, add it to stack */
      populateStack(currentX, currentY, i, myStack, &top);
    }
  }

  /* this will track the element being examined from the stack */
  struct location myElement;

  /* update while there is element in stack */
  while (top != -1) {

    /* get the top element */
    myElement = popStack(myStack, &top);

    /* the x and y positions of the current element */
    int tempX = myElement.x;
    int tempY = myElement.y;

    /* get the distance of the current element */
    int *currentDistance = &(theMaze[tempX][tempY].distance);

    /* array to keep track which neighbor is enterable (open) */
    int enterableNeighbors[DIRECTIONS] = {-1, -1, -1, -1};

    /* find out which neighbors can be entered (no wall in between) */
    enterableCells(tempX, tempY, theMaze, enterableNeighbors);
    /* printEnter(enterableNeighbors, myElement); */

    /* find the minimum distance among the neighbors */
    int minDistance = findMinDistance(enterableNeighbors);

    /*
     * the correct distance value of the current cell is minimum distance
     * neighbor + 1
     */
    int correctDistance = minDistance + 1;

    /* 
     * check if the distance of the current element is not correct and it is not
     * the center, if so, update the distance and push the open neighbors to the
     * stack
     */
    if (*currentDistance != correctDistance && *currentDistance != 0) {

      /* update the distance of the current element */
      *currentDistance = correctDistance;

      /* push all reachable neighbors to stack */
      for (int i = 0; i < DIRECTIONS; i++) {

        // if it's not -1, then it's reachable
        if (enterableNeighbors[i] != -1) {

          populateStack(tempX, tempY, i, myStack, &top);
        }
      }
    }
  }
}

/*
 * Name: evaluateCell()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * virtualMaze - the testing maze to see where are the walls.
 * currentLocation - the current location of the mouse.
 * deadOn - the status of a dead end to populate it to other cells.
 * Description: This function will first examine if there are new walls
 * discovered around the cell, it will update the cell's wall status
 * accordingly. Then, it wiill call the helper function to update the distances
 * starting from the current cell and the neighbor cells that have added new
 * walls. It will update the deadOn status when the cell is a dead end.
 */
void evaluateCell(struct cell theMaze[SIZE][SIZE], int actualWalls,
struct location currentLocation, int *deadOn) {

  /* get the x and y position of the current location */
  int currentX = currentLocation.x;
  int currentY = currentLocation.y;

  /* get the status of the wall at the current location */
  int *currentWalls = &(theMaze[currentX][currentY].wall);

  /* 
   * this will keep track which neighbors should be put into the stack for
   * distance update
   */
  int neighbors[DIRECTIONS] = {0};

  /* for loop to check if there is new wall discovered */
  for (int i = 0; i < DIRECTIONS; i++) {

    /* check if there is a wall at this direction */
    if ((actualWalls & walls[i]) != 0) {

      /* 
       * if there is, check if the wall not exist in theMaze, add it if that's
       * the case (new wall discovered)
       */
      if ((*currentWalls & walls[i]) == 0) {

        /* add the wall */
        *currentWalls = *currentWalls | walls[i];

        /* update the neighbor wall */
        updateNeighborWall(currentX, currentY, theMaze, i);

        /* set this neighbor to be put in stack for distance update */
        neighbors[i] = 1;
      }
    }
  }

  /*
   * update the distances starting from the current cell and neighbor cells if
   * applicable
   */
  updateDistances(currentLocation, theMaze, neighbors);

  /* check if it has three walls and not the start, if so, it's dead */
  if (numWalls(theMaze[currentX][currentY].wall) == DEAD_WALLS) {

    if (currentX != START_X || currentY != START_Y) {

      *deadOn = 1;
    }
  }

  else {

    *deadOn = 0;
  }
}

/*
 * Name: move()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * currentLocation - the current location of the mouse.
 * currentDirection - the current direction of the mouse.
 * Description: This function will be able to decide which way to turn and move.
 * It will first attempt to go straight to the cell that is in the current
 * direction with minimum distance. If not, it will turn the mouse in the order
 * of N, E, S, W that has minimum distance.
 */
int move(struct cell theMaze[SIZE][SIZE], struct location *currentLocation,
int *currentDirection) {

  /* get the x and y position of the current location */
  int currentX = currentLocation->x;
  int currentY = currentLocation->y;

  /* initially, assume neighbors are not enterable */
  int enterableNeighbors[DIRECTIONS] = {-1, -1, -1, -1};

  /* check which neighbors can be entered (no wall in between) */
  enterableCells(currentX, currentY, theMaze, enterableNeighbors);

  /* find the minimum distance */
  int minDistance = findMinDistance(enterableNeighbors);

  /* check if the next cell of current direction has minimum distance */
  if (enterableNeighbors[*currentDirection] == minDistance) {

    /* check the current direction then go straight */
    stepAtDirection(currentLocation, *currentDirection);

    /* mark this cell as visited */
    theMaze[currentLocation->x][currentLocation->y].visited = 0;

    /* TODO: Move mouse straight one step */
    return *currentDirection;
  }

  /* otherwise, find the earliest direction then turn that way and move */
  else {

    /* find the direction that has the minimum distance, then go there */
    int index = 0;
    for (; index < DIRECTIONS; index++) {

      if (enterableNeighbors[index] == minDistance) {

        break;
      }
    }

    /* take a step to that direction */
    stepAtDirection(currentLocation, index);

    /* update the direction */
    *currentDirection = index;

    /* mark this cell cell as visited */
    theMaze[currentLocation->x][currentLocation->y].visited = 1;

    /* TODO: turn to this direction, then take a step */
    return index;
  }
}

/*
 * Name: explore()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * currentLocation - the current location of the mouse.
 * currentDirection - the current direction of the mouse.
 * Description: This function will be able to decide which way to explore like
 * move. It will first attempt to go to the one that is not visited, then the
 * current direction, and finally the maximum distance in the order of N, E, S,
 * W.
 */
int explore(struct cell theMaze[SIZE][SIZE], struct location *currentLocation,
int *currentDirection) {

  /* get the x and y position of the current location */
  int currentX = currentLocation->x;
  int currentY = currentLocation->y;

  /* initially, assume neighbors are not enterable */
  int enterableNeighbors[DIRECTIONS] = {-1, -1, -1, -1};

  /* check which neighbors can be entered (no wall in between) */
  enterableCells(currentX, currentY, theMaze, enterableNeighbors);

  /* find the minimum distance */
  int maxDistance = findMaxDistance(enterableNeighbors);

  /* this will store which neighbor has been visited, initially 0 */
  int visitedNeighbors[DIRECTIONS] = {0};
  copyArray(visitedNeighbors, enterableNeighbors);

  /* find which neighbors are visited and not visited */
  neighborsVisited(currentX, currentY, theMaze, visitedNeighbors);

  /* prioritized unvisited cells */
  for (int i = 0; i < DIRECTIONS; i++) {

    if (visitedNeighbors[i] == 0) {

      /* take a step to that direction */
      stepAtDirection(currentLocation, i);

      /* update the direction */
      *currentDirection = i;

      /* mark this cell cell as visited */
      theMaze[currentLocation->x][currentLocation->y].visited = 1;

      return i;   
    }
  }

  /* check if the next cell of current direction has maximum distance */
  if (enterableNeighbors[*currentDirection] == maxDistance) {

    /* check the current direction then go straight */
    stepAtDirection(currentLocation, *currentDirection);

    /* mark this cell as visited */
    theMaze[currentLocation->x][currentLocation->y].visited = 1;

    /* TODO: Move mouse straight one step */
    return *currentDirection;
  }

  /* otherwise, find the earliest direction then turn that way and move */
  else {

    /* find the direction that has the maximum distance, then go there */
    int index = 0;
    for (; index < DIRECTIONS; index++) {

      if (enterableNeighbors[index] == maxDistance) {

        break;
      }
    }

    /* take a step to that direction */
    stepAtDirection(currentLocation, index);

    /* update the direction */
    *currentDirection = index;

    /* mark this cell cell as visited */
    theMaze[currentLocation->x][currentLocation->y].visited = 1;

    /* TODO: turn to this direction, then take a step */
    return index;
  }
}

/*
 * Name: fillCenter()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * currentLocation - the current location of the mouse.
 * currentDirection - the current direction of the mouse.
 * Description: This function will run through the center of the maze, but not
 * actually moving the mouse in order to fill the walls and update the
 * distances.
 */
void fillCenter(struct cell theMaze[SIZE][SIZE],
struct location *currentLocation, int *currentDirection) {

  /* walls */
  int T = TOP_WALL;
  int R = RIGHT_WALL;
  int B = BOTTOM_WALL;
  int L = LEFT_WALL;

  struct location centers[WALLS] = {{7, 7}, {7, 8}, {8, 7}, {8, 8}};
  int walls[WALLS] = {L|T, T|R, L|B, R|B};

  int currentCenter = 0;
  int tempBool = 0;

  for (int i = 0; i < WALLS; i++) {

    if (centers[i].x != currentLocation->x ||
    centers[i].y != currentLocation->y) {

      /*
       * evaluate the cell to see if there are new walls, then update the
       * distances accordingly
       */
      evaluateCell(theMaze, walls[i], centers[i], &tempBool);

      /* check the status through print outs */
      // checkStatus(theMaze, *currentLocation, *currentDirection);

      theMaze[centers[i].x][centers[i].y].visited = 1;
    }
  }

  *currentDirection = *currentDirection + 2 % DIRECTIONS;
}

/*
 * Name: floodMaze()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * Description: This function will be able to flood the distances of each cell
 * of the given maze with initial distances.
 */
void floodMaze(struct cell theMaze[SIZE][SIZE]) {

  /* constants for neighbors searching */
  /*
  const int NUM_NEIGHBORS = 4;
  const int TOP = 0;
  const int RIGHT = 1;
  const int BOTTOM = 2;
  const int LEFT = 3; 

  // define all of the centers
  location center1 = {CENTER_MIN, CENTER_MIN};
  location center2 = {CENTER_MIN, CENTER_MAX};
  location center3 = {CENTER_MAX, CENTER_MIN};
  location center4 = {CENTER_MAX, CENTER_MAX};

  // the queue for storing the neighbors to be updated
  queue<location> myQueue;

  // current distance starts with 0
  int currentDistance = 0;

  // number of cells for this distance 
  int count = NUM_CENTERS;

  int tempCount = 0;

  // push all of the centers to queue
  myQueue.push(center1);
  myQueue.push(center2);
  myQueue.push(center3);
  myQueue.push(center4);

  // while loop to update all of the cell's distaces
  while (!myQueue.empty()) {

    // get the next element from the queue
    location currentPoint = myQueue.front();
    myQueue.pop();

    // update the distance at this location with the current distance
    theMaze[currentPoint.x][currentPoint.y].distance = currentDistance;

    // get the x and y of the current location
    int currentX = currentPoint.x;
    int currentY = currentPoint.y;

    // temporary x and y for the neighbors
    int tempX = 0;
    int tempY = 0;

    // try all of the neighbors
    for (int i = 0; i < NUM_NEIGHBORS; i++) {

      switch (i) {

        // top neighbor
        case TOP:
          tempX = currentX - 1;
          tempY = currentY;
          break;

        // right neighbor
        case RIGHT:
          tempX = currentX;
          tempY = currentY + 1;
          break;

        // bottom neighbor
        case BOTTOM:
          tempX = currentX + 1;
          tempY = currentY;
          break;

        // left neighbor
        case LEFT:
          tempX = currentX;
          tempY = currentY - 1;
          break;
      }

      // check if it's not center or outside
      if (!isCenter(tempX, tempY) && !isOut(tempX, tempY)) {

        // get the distance of this neighbor cell
        int neighborDistance = theMaze[tempX][tempY].distance;

        // check if the distance is 0 and not pending to be changed -1
        if (neighborDistance == 0 && neighborDistance != -1) {

          // add this location to the queue
          myQueue.push({tempX, tempY});
          theMaze[tempX][tempY].distance = -1;
          tempCount++;
        }
      }
    }

    // decrement it to keep track of the remaining cell for currentDistance
    count--;

    // if count becomes 0, then time to reset it
    if (count == 0) {

      // assign tempCount to count, then reset tempCount
      count = tempCount;
      tempCount = 0;

      // increment the currentDistance
      currentDistance++;
    }
  }*/
}

void evaluation(struct cell theMaze[SIZE][SIZE], int *deadOn, int *previousX,
int *previousY, int wallInfo, struct location *currentLocation,
int *currentDirection) {

  if (*deadOn) {

    fillWalls(*previousX, *previousY, theMaze);
  }

  /*
    * evaluate the cell to see if there are new walls, then update the
    * distances accordingly
    */
  evaluateCell(theMaze, wallInfo, *currentLocation, deadOn);
  
  if (*deadOn) {

    *previousX = currentLocation->x;
    *previousY = currentLocation->y;
  }

  /* check the status through print outs */
  // checkStatus(theMaze, *currentLocation, *currentDirection);
}

int speedRunPath(struct instruction myInstructions[TOTAL_CELLS]) {

  /* get the directions first */
  int path[TOTAL_CELLS];
  int index = 0;
  initializeArray(path);
  currentLocation.x = START_X;
  currentLocation.y = START_Y;
  currentDirection = NORTH;

  while (theMaze[currentLocation.x][currentLocation.y].distance != 0) {

    /* move to a cell, smaller value */
    path[index++] = move(theMaze, &currentLocation, &currentDirection);

    /* this will evaluate the cell */
  //  evaluation(theMaze, &deadOn, &previousX, &previousY,
//    virtualMaze[currentLocation.x][currentLocation.y].wall, &currentLocation,
//    &currentDirection);
  }

  int count = 0;

  /* status of the location */
  struct location currentLocation = {START_X, START_Y};

  /* directions */
  int currentDirection = path[0];

  /* current index of the instruction array, then its element */
  index = 0;
  struct instruction *structure = &myInstructions[index];

  /* whether or not we are concatenating characters */
  int concatenating = 0;

  for (int i = 0; i < TOTAL_CELLS && path[i] != -1; i++) {

    currentDirection = path[i];

    stepAtDirection(&currentLocation, currentDirection);

    if (concatenating) {

      if (currentDirection == path[i - 1]) {

        concatenating = 0;
        index++;
        structure = &myInstructions[index];

        if (noWalls(theMaze[currentLocation.x][currentLocation.y].wall,
        currentDirection)) {

          strcpy(structure->inst, "");
          structure->straight = currentDirection;

          index++;
          structure = &myInstructions[index];
          
          count++;
        }
      }

      else {

        /* append a direction instruction */
        switch (currentDirection) {
          
          case 0:
            strcat(structure->inst, "N");
            break;

          case 1:
            strcat(structure->inst, "E");
            break;

          case 2:
            strcat(structure->inst, "S");
            break;

          case 3:
            strcat(structure->inst, "W");
            break;

        }

        if (path[i + 1] != -1 && path[i + 1] == currentDirection) {

          concatenating = 0;
          index++;
          structure = &myInstructions[index];
        }
      }
    }

    else if (noWalls(theMaze[currentLocation.x][currentLocation.y].wall,
    currentDirection)) {

      concatenating = 1;
      strcpy(structure->inst, "");
      structure->straight = currentDirection;

      count++;
    }
  }

  return count;
}

void phaseOne() {

  /* keep moving until 0 has been found */
  // printf("Phase 1: Find Center\n");
  while (theMaze[currentLocation.x][currentLocation.y].distance != 0) {

    /* move to a cell, smaller value */
    move(theMaze, &currentLocation, &currentDirection);

    /* this will evaluate the cell */
  //  evaluation(theMaze, &deadOn, &previousX, &previousY,
//    virtualMaze[currentLocation.x][currentLocation.y].wall, &currentLocation,
//    &currentDirection);
  }
  
  /* to fill the walls of the other center cells */
  fillCenter(theMaze, &currentLocation, &currentDirection);
}

void phaseTwo() {

  // printf("Phase 2: Explore\n");
  // checkStatus(theMaze, currentLocation, currentDirection);

  /* preparation */
  deadOn = 0;
  exploreSteps = 0;

  /* perform 60 steps */
  while (exploreSteps < MAX_EXPLORE) {

    explore(theMaze, &currentLocation , &currentDirection);

    /* this will evaluate the cell */
  //  evaluation(theMaze, &deadOn, &previousX, &previousY,
//    virtualMaze[currentLocation.x][currentLocation.y].wall, &currentLocation,
//    &currentDirection);

    exploreSteps++;
  }
}

void phaseThree() {

  // printf("Phase 3: Search Again\n");
  /* preparation */
  currentLocation.x = START_X;
  currentLocation.y = START_Y;
  currentDirection = NORTH;

  /* loop til we find the center */
  while (theMaze[currentLocation.x][currentLocation.y].distance != 0) {

    /* move to a cell, smaller value */
    move(theMaze, &currentLocation, &currentDirection);

    /* this will evaluate the cell */
  //  evaluation(theMaze, &deadOn, &previousX, &previousY,
//    virtualMaze[currentLocation.x][currentLocation.y].wall, &currentLocation,
//    &currentDirection);
  }
}

void phaseFour() {

  /* TODO: create the array for speed run */

  // printf("Phase 4: Speed Run\n");

  /* an array to pass in, then it will return the number of valid elements */
  struct instruction myInstructions[TOTAL_CELLS];
  int count = speedRunPath(myInstructions);
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */

void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif
//********************************************************************
/*
void Testing() {
			
			float32_t ENC_PID_INC = 0.000f;
		float32_t Halt_PID_INC = 0.000f;
		initEnc_PID(6.000f,0.000f,3000.000f,0.000,(float32_t)(LEnc()-REnc()));
		while(LEnc() + REnc() < 39000) {
					ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc()));
					curve(30, ENC_PID_INC);}
			while(1) halt();
			//motor(15,-15);
			//while(LEnc() - REnc() < 26000);
			//motor(-15,15);
			curve(11.8f,5.0f);
			while(REnc() - LEnc() < 26000);
			halt();
			while(1);*/
	/*
		halt();
		Delay(1500);
		float32_t FBPID_INC = 0.000f;
		float32_t FBPID_INC2 = 0.000f;
		IRscan_L();
		initFrontBlock_PID(0.353,0.00583,30.530,Enc_Ed,180, IR_L);
		IRscan_R();
		initFrontBlock_PID2(0.353,0.00583,30.530,Enc_Ed,200, IR_L);
		while(1) {
				IRscan_L();
				IRscan_R();
				FBPID_INC = FrontBlock_PID(Enc_Ed, IR_L);
				FBPID_INC2 = FrontBlock_PID2(Enc_Ed, IR_R);
				motor(0.000f + FBPID_INC, 0.000f + FBPID_INC2);		
		}*/
		/*LED1_OFF(); LED2_OFF(); LED3_OFF();
		while(1) {	
				UpdateSensors();
				if(Lwall) LED1_ON();
				else LED1_OFF();
			if(Fwall) LED2_ON();
				else LED2_OFF();
			if(Rwall) LED3_ON();
				else LED3_OFF();
		}*/
		/*33mm worked
		float32_t FBPID_INC = 0.000f;
	  IRscan_L();
		initFrontBlock_PID(3.53,0.0583,7.530,Enc_Ed,33, IR_L);
		while(1) {
				IRscan_L();
				FBPID_INC = FrontBlock_PID(Enc_Ed, IR_L);
				//printf("\r%f\r\n", FBPID_INC);
				motor(0.000f + FBPID_INC, 0.000f + FBPID_INC);

		}*/
	/*
		float32_t w = 10.000f;
		while(1) {
				t0 = millis();
				while(millis() - t0 < 300) {
						curve(30.000f,w);
				}
				w -= 3.000f;
				halt();
				Delay(3000);
		}*/
		/*Math functions
		printf("\rcos 30 = %f\r\n",Icos(Pi/6.00f));
		printf("\rsin 30 = %f\r\n",Isin(Pi/6.00f));
		printf("\rtan 30 = %f\r\n",Itan(Pi/2.00f));
		printf("\rabs(-1234.12) = %f\r\n",Ifabs(-1234.12f));
		printf("\rabs(-1200) = %d\r\n",Iabs(-1200));
		printf("\rsqrt(25) = %f\r\n",Isqrt(25));
		while(1);*/
		/*
	  printf("\rEnter r or R to run motors\r\n");
		while(!runFlag); 
		while(1) motor_test(); 
	*/
	/*
		printf("\rEnter r or R to run motors\r\n");
		while(!runFlag); 
		while(1) Encoder_test();
		*/
	  /*
		printf("\rEnter r or R to run motors\r\n");
		while(!runFlag); 
		AngularV_test();*/
		/*
		while(1) {
			printf("\r%d %d %d %d %d %d\r\n",RE_L(), RE_LM(), RE_LF(), RE_RF(), RE_RM(), RE_R());
		}*/
		/*
		initIR_Scan();
		while(1) 
		{
				//int n = IR_sample();
				//for(int i = 0; i < n; i++) printf("\r#%d=[ %d ]\r\n",i,samp[i]);
				IR_Scan();
				printf("\r%d %d %d %d %d %d\r\n",IR_L, IR_LM, IR_LF, IR_RF, IR_RM, IR_R);
		}*//*
		while(1) {	
				IRscan_L();
				//printf("\r%f\r\n",IR_L);
				IRscan_LM();
				//printf("\r%f\r\n",IR_LM);
				IRscan_LF();
				printf("\r%0.1f %0.1f %0.1f\r\n",IR_L, IR_LM, IR_LF);
		}*/
	  
		//while(1) printf("\r%d\r\n",RE_L());
	/*	
		uint32_t val;
		while(1)
		{
			
				EM_L();
				Delay_us(300);
				val = RE_L();
				XEM_L();
				Delay_us(500);
			  printf("\r%d ",val);
			
				//printf("\r%0.1f ",(1000*1.000f/Isqrt((float32_t)((val%3000))))-33.000f);
			
				EM_LM();
				Delay_us(300);
				val = RE_LM();
				XEM_LM();
				Delay_us(500);
				printf("\r%d ",val);
				//printf("\r%0.1f ",(1000*1.000f/Isqrt((float32_t)((val)))));
			
				EM_LF();
				Delay_us(300);
				val = RE_LF();
				XEM_LF();
				Delay_us(500);
				printf("\r%d ",val);
				//printf("\r%0.1f ",(1000*1.000f/Isqrt((float32_t)((val)))));
							
				EM_RF();
				Delay_us(300);
				val = RE_RF();
				XEM_RF();
				Delay_us(500);
				printf("\r%d ",val);
				//printf("\r%0.1f  \r\n",(1000*1.000f/Isqrt((float32_t)((val)))));
				//printf("\r%d\r\n",val);
			
				EM_RM();
				Delay_us(500);
				val = RE_RM();
				XEM_RM();
				Delay_us(600);
				printf("\r%d ",val);
				//printf("\r%0.1f \r\n ",(1000*1.000f/Isqrt((float32_t)((val)))));
				
				
				EM_R();
				Delay_us(300);
				val = RE_R();
				XEM_R();
				Delay_us(500);
				printf("\r%d \r\n",val);
				//printf("\r%d\r\n",val);
				//printf("\r%0.1f\r\n",(1000*1.000f/Isqrt((float32_t)((val%3000))))-33.000f);
				
				
		}
}*/
//************************
/*
typedef struct {
		float32_t Ep, Ei, Ed;
		float32_t Kp, Ki, Kd;
		float32_t integral, I_threshold;
		float32_t Exp;
		uint64_t T;
}PID;*/
//***********************************************************************
/*
PID FB;
void initFrontBlock_PID(float32_t Kp, float32_t Ki, float32_t Kd,float32_t Ed, float32_t Exp, float32_t Act) {
		FB.Exp = Exp;
		float32_t Error = Act - Exp;
		FB.Kp = Kp; FB.Ki = Ki; FB.Kd = Kd; 
		FB.Ed = Ed; FB.Ei = Error; 
		FB.integral = 0; 
		FB.T = micros();
}
float32_t FrontBlock_PID(float32_t Ed, float32_t Act) {
		uint64_t dT = micros() - FB.T;
		float32_t Error = Act - FB.Exp;
		if(Error > 0.000f) Error *= 4.300f;
		if(Error > 61.000f ) Error = 61.000f; 
		FB.integral += FB.Ki * ((FB.Ei + Error) * dT / 2); 
		float32_t P, I, D;
		P = FB.Kp * Error;
		if(P >= 30.000f) P = 30.000f;
		I = FB.Ki * FB.integral / 100000000000.000f;
	  if(Ifabs(I) >= 50.000f) I = I >= 0.000f ? 50.000f : -50.000f;
	  D = FB.Kd * ((Ed - FB.Ed) / dT) / 1000.000f;
		//if(Ifabs(D) >= 80.000f) D = D >= 0.000f ? 80.000f : -100.000f;
		FB.Ed = Ed;
		FB.Ei = Error; 
		FB.T = micros();
		float32_t ans = P+I+D;
		if(ans > 50.000f) ans = 50.000f; 
		return ans;
}*/
//*****************************************************************************
/*
PID FB2;
void initFrontBlock_PID2(float32_t Kp, float32_t Ki, float32_t Kd,float32_t Ed, float32_t Exp, float32_t Act) {
		FB2.Exp = Exp;
		float32_t Error = Act - Exp;
		FB2.Kp = Kp; FB2.Ki = Ki; FB2.Kd = Kd; 
		FB2.Ed = Ed; FB2.Ei = Error; 
		FB2.integral = 0; 
		FB2.T = micros();
}
float32_t FrontBlock_PID2(float32_t Ed, float32_t Act) {
		uint64_t dT = micros() - FB2.T;
		float32_t Error = Act - FB2.Exp;
		if(Error > 0.000f) Error *= 4.300f;
		if(Error > 61.000f ) Error = 61.000f; 
		FB2.integral += FB2.Ki * ((FB2.Ei + Error) * dT / 2); 
		float32_t P, I, D;
		P = FB2.Kp * Error;
		if(P >= 30.000f) P = 30.000f;
		I = FB2.Ki * FB2.integral / 100000000000.000f;
	  if(Ifabs(I) >= 50.000f) I = I >= 0.000f ? 50.000f : -50.000f;
	  D = FB2.Kd * ((Ed - FB2.Ed) / dT) / 1000.000f;
		//if(Ifabs(D) >= 80.000f) D = D >= 0.000f ? 80.000f : -100.000f;
		FB2.Ed = Ed;
		FB2.Ei = Error; 
		FB2.T = micros();
		float32_t ans = P+I+D;
		if(ans > 50.000f) ans = 50.000f; 
		return ans;
}*/
//*****************************************************************************
/*PID LB;
void initLeftBlock_PID(float32_t Kp, float32_t Ki, float32_t Kd,float32_t Ed, float32_t Exp, float32_t Act) {
		LB.Exp = Exp;
		float32_t Error = Act - Exp;
		LB.Kp = Kp; LB.Ki = Ki; LB.Kd = Kd; 
		LB.Ed = Ed; LB.Ei = Error; 
		LB.integral = 0; 
		LB.T = micros();
}
float32_t LeftBlock_PID(float32_t Ed, float32_t Act) {
		uint64_t dT = micros() - LB.T;
		float32_t P = 0.000f, I = 0.000f, D = 0.000f;
		float32_t Error = Act - LB.Exp;  
	  //Error = Error >= 0 ? Isqrt(Error) : -Isqrt(-Error);
		//if(Error > 0) Error /= 1.800f;
	  //if(Ifabs(Error) > 8.000f) Error = Error >= 0 ? 8.000f : -8.000f;
		if(Error > 0.000f) Error *= (29.000f/72.000f);
		else Error *= 1;
		P = LB.Kp * Error;
		I = LB.Ki * LB.integral / 100000000000.000f;
	  //if(Ifabs(I) >= 30.000f) I = I >= 0.000f ? 30.000f : -30.000f;
		LB.Ei = Error;
	  D = LB.Kd * ((Ed - LB.Ed) / dT) / 100.000f;
		//if(Ifabs(D) >= 80.000f) D = D >= 0.000f ? 80.000f : -100.000f;
		LB.Ed = Ed;
		float32_t ans = P + I + D;
		//if(ans > 50.000f) ans = 50.000f; 
		return ans;
}*/
//*****************************************************************************
/*
PID HA;
void initHAEnc_PID(float32_t Kp, float32_t Ki, float32_t Kd, float32_t Exp, float32_t Act) {
		HA.Exp = Exp;
		float32_t Error = Act - Exp;
		HA.Kp = Kp; HA.Ki = Ki; HA.Kd = Kd; 
		HA.Ed = Act; HA.Ei = Error; 
		HA.integral = 0; 
		ResetLEnc();
		ResetREnc();
		HA.T = micros();
}
float32_t HAEnc_PID(float32_t Act) {
		uint64_t dT = micros() - HA.T;
		float32_t P = 0.000f, I = 0.000f, D = 0.000f;
		float32_t Error = HA.Exp - Act;
		Error /= 1000.000f;
		float32_t Ed = Error;  
		P = HA.Kp * Error;
		I = HA.Ki * HA.integral / 100000000000.000f;
		HA.Ei = Error;
	  D = HA.Kd * ((Ed - HA.Ed) / dT);
		HA.Ed = Ed;
		return D;
}*/
//*****************************************************************************

//*****************************************************************************
/*
PID EN;
void initEnc_PID(float32_t Kp, float32_t Ki, float32_t Kd, float32_t Exp, float32_t Act) {
		EN.Exp = Exp;
		float32_t Error = Act - Exp;
		EN.Kp = Kp; EN.Ki = Ki; EN.Kd = Kd; 
		EN.Ed = Act; EN.Ei = Error; 
		EN.integral = 0; 
		ResetLEnc();
		ResetREnc();
		EN.T = micros();
}
float32_t Enc_PID(float32_t Act) {
		uint64_t dT = micros() - EN.T;
		float32_t P = 0.000f, I = 0.000f, D = 0.000f;
		float32_t Error = Act - EN.Exp;
		Error /= 1000.000f;
		float32_t Ed = Error;  
		P = EN.Kp * Error;
		I = EN.Ki * EN.integral / 100000000000.000f;
		EN.Ei = Error;
	  D = EN.Kd * ((Ed - EN.Ed) / dT);
		EN.Ed = Ed;
		return P + D;
}*/
//*****************************************************************************
/*
#define Enc_Ed (float32_t)((-LEnc() - REnc()) * 1000) 
#define LEnc_Ed (float32_t)(REnc() - LEnc()) / -2.000f
#define REnc_Ed (float32_t)(LEnc() - REnc()) / -2.000
int8_t FF(void) {
		//int32_t ans = rand() % 4;
		//ans--;
		//return ans;
		if(!Fwall) return 0;
		if(!Lwall) return -1;
		if(!Rwall) return 1;
		else return 2;
}*/
/*
int main(void) 
{
	  InitializeAlles();
    Delay(1000);
		//while(1) printf("\r%d\r\n",LEnc()+REnc());
		LED1_OFF(); LED2_ON(); LED3_OFF();
		//measuring();
		//Testing();
		//while(!Button()); 
	//printf("\rEnter r or R to run motors\r\n");
	//while(!runFlag) ; */
	/*	float32_t ENC_PID_INC = 0.000f;
		float32_t Halt_PID_INC = 0.000f;
		initEnc_PID(6.000f,0.000f,3600.000f,0.000,(float32_t)(LEnc()-REnc()));
		initHAEnc_PID(100000000.000f,0.000f,0.000f,0.000,0.000f);
		float32_t FBPID_INC = 0.000f;
		float32_t FBPID_INC2 = 0.000f;
		float32_t L_INC = 0.000f, R_INC = 0.000f;
		UpdateSensors();
		initFrontBlock_PID(0.653,0.00583,200.530,Enc_Ed,180, IR_L);
		initFrontBlock_PID2(0.653,0.00583,200.530,Enc_Ed,200, IR_R);
		uint8_t interruptL = 0, interruptR = 0, interruptF = 0;
		int32_t comp = 0, tmp = 0;
		while(1) {*/
			/*
			UpdateSensors();
			if(nFwall) interruptF++;
			else interruptF = 0;
			if(!nLwall) interruptL++;
			else interruptL = 0;
			if(!nRwall) interruptR++;
			else interruptR = 0;
			if(interruptF + interruptL + interruptR > 3) {
					t0 = millis();
					while(millis() - t0 < 1000) {
							ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc()));
							Halt_PID_INC = HAEnc_PID(LEnc()+REnc());
							curve(Halt_PID_INC,ENC_PID_INC);
					}
					if(interruptF >= interruptL && interruptF >= interruptR) {
							UpdateSensors();
							t0 = millis();
							while(millis() - t0 < 500) {
									UpdateSensors();
									FBPID_INC = FrontBlock_PID(Enc_Ed, IR_L);
									FBPID_INC2 = FrontBlock_PID2(Enc_Ed, IR_R);
									motor(0.000f + FBPID_INC, 0.000f + FBPID_INC2);	
							}
							UpdateSensors();
							if(FF() == -1) {
									motor(-15,15);
									while(REnc() - LEnc() < 27000);
									comp += REnc() - LEnc();
									tmp = LEnc() + REnc();
									while(LEnc() + REnc() - tmp < 45000) {
									ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
									curve(30, ENC_PID_INC);}
									interruptF = 0;
									interruptL = 0;
									interruptR = 0;
									while(1) halt();
							}
							//else if(FF() == 1) {
							else {
									motor(15,-15);
									while(LEnc() - REnc() < 26399);
									comp += LEnc() - REnc();
									tmp = LEnc() + REnc();
									while(LEnc() + REnc() - tmp < 49000) {
									ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
									curve(30, ENC_PID_INC);}
									interruptF = 0;
									interruptL = 0;
									interruptR = 0;
							}
					}
					else if(interruptL >= interruptR && interruptL >= interruptF) {
							UpdateSensors();
							while(!Lwall) {
									UpdateSensors();
									ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
									curve(-10, ENC_PID_INC);
							}
							tmp = LEnc() + REnc();
							while(LEnc() + REnc() - tmp < 39000) {
							ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
							curve(30, ENC_PID_INC);}
							motor(-15,15);
							while(REnc() - LEnc() < 26399);
							comp += REnc() - LEnc();
							tmp = LEnc() + REnc();
							while(LEnc() + REnc() - tmp < 49000) {
							ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
							curve(30, ENC_PID_INC);}
							interruptF = 0;
							interruptL = 0;
							interruptR = 0;
							
					}
					else if(interruptR >= interruptL && interruptR >= interruptF) {
							UpdateSensors();
							while(!Rwall) {
									UpdateSensors();
									ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
									curve(-10, ENC_PID_INC);
							}
							tmp = LEnc() + REnc();
							while(LEnc() + REnc() - tmp < 39000) {
							ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
							curve(30, ENC_PID_INC);}
							motor(15,-15);
							while(LEnc() - REnc() < 26399);
							comp += LEnc() - REnc();
							tmp = LEnc() + REnc();
							while(LEnc() + REnc() - tmp < 49000) {
							ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
							curve(30, ENC_PID_INC);}
							interruptF = 0;
							interruptL = 0;
							interruptR = 0;
					}
					else while(1) halt();
			}*/
				/*
					while(1) {
						UpdateSensors();
						FBPID_INC = FrontBlock_PID(Enc_Ed, IR_L);
						FBPID_INC2 = FrontBlock_PID2(Enc_Ed, IR_R);
						motor(0.000f + FBPID_INC, 0.000f + FBPID_INC2);
					}*/
			
			/*
			if(Fwall) {
						FBPID_INC = FrontBlock_PID(Enc_Ed, IR_L);
						FBPID_INC2 = FrontBlock_PID2(Enc_Ed, IR_R);
						motor(0.000f + FBPID_INC, 0.000f + FBPID_INC2);		
			}*/
			//UpdateSensors();
			/*
			if(nFwall) {interruptF++; halt(); }
			else interruptF = 0;
			if(!nLwall) {interruptL++; halt(); }
			else interruptL = 0;
			if(!nRwall) {interruptR++; halt(); }
			else interruptR = 0;
			if(interruptF + interruptL + interruptR > 6) {
					t0 = millis();
					while(millis() - t0 < 1000) {
							ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc()));
							Halt_PID_INC = HAEnc_PID(LEnc()+REnc());
							curve(Halt_PID_INC,ENC_PID_INC);
					}
					tmp = LEnc() + REnc();
									while(LEnc() + REnc() - tmp < 10000) {
									ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
									curve(10, ENC_PID_INC);}
					UpdateSensors();
					int g = FF();
					if(g == -1) 
					{
							motor(-10,10);
									while(REnc() - LEnc() < 26900);
						halt();
									comp += REnc() - LEnc();
									EN.Ed = LEnc()-REnc() + comp;
									tmp = LEnc() + REnc();
									while(LEnc() + REnc() - tmp < 53000) {
									ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
									curve(10, ENC_PID_INC);}
									halt();
									interruptF = 0;
									interruptL = 0;
									interruptR = 0;
					}
					if(g == 1) 
					{
							motor(10,-10);
									while(LEnc() - REnc() < 26900);
						halt();			
						comp += REnc() - LEnc();
									EN.Ed = LEnc()-REnc() + comp;
									tmp = LEnc() + REnc();
									while(LEnc() + REnc() - tmp < 53000) {
									ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
									curve(10, ENC_PID_INC);}
									halt();
									interruptF = 0;
									interruptL = 0;
									interruptR = 0;
					}
					if(g == 0) 
					{
									tmp = LEnc() + REnc();
									EN.Ed = LEnc()-REnc() + comp;
									while(LEnc() + REnc() - tmp < 53000) {
									ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
									curve(10, ENC_PID_INC);}
									halt();
									interruptF = 0;
									interruptL = 0;
									interruptR = 0;
					}
					if(g == 2) {
									motor(10,-10);
									while(LEnc() - REnc() < 53000);
						halt();
									comp += REnc() - LEnc();	
									EN.Ed = LEnc()-REnc() + comp;
					}
			}*//*
			UpdateSensors();
			if(IR_L + IR_R < 950) {
					t0 = millis();
							while(millis() - t0 < 500) {
									UpdateSensors();
									FBPID_INC = FrontBlock_PID(Enc_Ed, IR_L);
									FBPID_INC2 = FrontBlock_PID2(Enc_Ed, IR_R);
									motor(0.000f + FBPID_INC, 0.000f + FBPID_INC2);	
							}
							
					motor(20,0);
					//while(LEnc() - REnc() < 26900);
					while(IR_L < 300) UpdateSensors();
					halt();			
					comp += REnc() - LEnc();
					EN.Ed = LEnc()-REnc() + comp;
			}
			else {
				if(!Lwall) {
						motor(18,39);
						comp += REnc() - LEnc();
					EN.Ed = LEnc()-REnc() + comp;
				}
				else {
					if(IR_LF < 280.000f) L_INC = IR_LF - 280.000f;	
					if(IR_RF < 272.000f) R_INC = IR_RF - 272.000f;
					ENC_PID_INC = Enc_PID((float32_t)(LEnc()-REnc() + comp));
					curve(30, ENC_PID_INC + L_INC * 0.3300f - R_INC * 0.3300f);
					L_INC = 0;
					R_INC = 0;
				}
			}
		}*/
	/*
	  
	  IRscan_L();
		initFrontBlock_PID(3.53,0.0583,7.530,Enc_Ed,33, IR_L);
		float32_t LBPID_INC = 0.000f;
	  IRscan_LF();
	  initLeftBlock_PID(3.10f,0.00f,0.000f,LEnc_Ed,260.000f, IR_LF);
		while(1) {
				IRscan_L();
				IRscan_LF();
				if(1) {
					if(3) {//
						LBPID_INC = LeftBlock_PID(LEnc_Ed, IR_LF);
						//printf("\r%f |||||%f\r\n",IR_LF, LBPID_INC);
						curve(10,LBPID_INC);
					}
					else motor(10,10);
				}
				else  {	
					
					FBPID_INC = FrontBlock_PID(Enc_Ed, IR_L);
					//printf("\r%f\r\n", FBPID_INC);
					motor(0.000f + FBPID_INC, 0.000f + FBPID_INC);
				}//
		}*/
		//return 0;
//}


//**********************************************************************************
/*
float32_t w(int32_t dCNT, int64_t dt) {
		return (((float32_t)dCNT / 1024) * 2 * Pi) / ((float32_t)dt / 1000000 ); 
}
void AngularV_test() {		
		int32_t LCNT0, dLCNT;
		for(int i = 100; i >= 0; i -= 10) {
			motor((float32_t)i,0);
			Delay(1000);
			LCNT0 = LEnc();
			t0 = micros();
			while(LEnc() - LCNT0 <= 10240) printf("\r%d\r\n",LEnc() - LCNT0);
			dLCNT = Iabs(LEnc()-LCNT0);
			dt = micros() - t0;
			halt();
			printf("\rdLCNT = # %d\r\n", dLCNT);
			printf("\rdt = %lld us(microsec)\r\n", dt);
			printf("\rw = %f rad/s\r\n", w(dLCNT, dt));
			printf("\rpwm = %d\r\n",i);
			printf("\rw/pwm = %f",w(dLCNT, dt) / (float32_t)i);
			Delay(1000);
		}
		int32_t RCNT0, dRCNT;
		for(int i = 100; i >= 0; i -= 10) {
			motor(0,(float32_t)i);
			Delay(1000);
			RCNT0 = REnc();
			t0 = micros();
			while(Iabs(REnc()-RCNT0) <= 10240) ;
			dRCNT = Iabs(REnc()-RCNT0);
			dt = micros() - t0;
			halt();
			printf("\rdRCNT = # %d\r\n", dRCNT);
			printf("\rdt = %lld us(microsec)\r\n", dt);
			printf("\rw = %f rad/s\r\n", w(dRCNT, dt));
			printf("\rpwm = %d\r\n",i);
			printf("\rw/pwm = %f",w(dRCNT, dt) / (float32_t)i);
			Delay(1000);
		}
}
*/
/*

#define On_Span_L 300
#define Off_Span_L 500
#define On_Span_LM 300
#define Off_Span_LM 500
#define On_Span_LF 300
#define Off_Span_LF 500
#define On_Span_RF 300
#define Off_Span_RF 500
#define On_Span_RM 300
#define Off_Span_RM 500
#define On_Span_R 300
#define Off_Span_R 500
void IR_Scan(void);
void initIR_Scan(void);
uint64_t IR_t = 0;
uint16_t regulations = 0;
void initIR_Scan(void) {
		IR_t = micros(); 
		regulations = 0;
}
void IR_Scan(void) {
		if(regulations == 0) {
				if(micros() - IR_t < On_Span_L) EM_L();
				else {
						XEM_L();
						IR_L = RE_L();
						regulations++;
						IR_t = micros();
				}
		}
		else if(regulations == 1) {
				if(micros() - IR_t < Off_Span_L) XEM_L();
				else {
						regulations++;
						IR_t = micros();
				}
		}
		else if(regulations == 2) {
				if(micros() - IR_t < On_Span_LM) EM_LM();
				else {
						XEM_LM();
						IR_LM = RE_LM();
						regulations++;
						IR_t = micros();
				}
		}
		else if(regulations == 3) {
				if(micros() - IR_t < Off_Span_LM) XEM_LM();
				else {
						regulations++;
						IR_t = micros();
				}
		}
		else if(regulations == 4) {
				if(micros() - IR_t < On_Span_LF) EM_LF();
				else {
						XEM_LF();
						IR_LF = RE_LF();
						regulations++;
						IR_t = micros();
				}
		}
		else if(regulations == 5) {
				if(micros() - IR_t < Off_Span_LF) XEM_LF();
				else {
						regulations++;
						IR_t = micros();
				}
		}
		else if(regulations == 6) {
				if(micros() - IR_t < On_Span_R) EM_R();
				else {
						XEM_R();
						IR_R = RE_R();
						regulations++;
						IR_t = micros();
				}
		}
		else if(regulations == 7) {
				if(micros() - IR_t < Off_Span_R) XEM_R();
				else {
						regulations++;
						IR_t = micros();
				}
		}
		else if(regulations == 8) {
				if(micros() - IR_t < On_Span_RM) EM_RM();
				else {
						XEM_RM();
						IR_LM = RE_LM();
						regulations++;
						IR_t = micros();
				}
		}
		else if(regulations == 9) {
				if(micros() - IR_t < Off_Span_RM) XEM_RM();
				else {
						regulations++;
						IR_t = micros();
				}
		}
		else if(regulations == 10) {
				if(micros() - IR_t < On_Span_RF) EM_RF();
				else {
						XEM_RF();
						IR_RF = RE_RF();
						regulations++;
						IR_t = micros();
				}
		}
		else if(regulations == 11) {
				if(micros() - IR_t < Off_Span_RF) XEM_RF();
				else {
						regulations = 0;
						IR_t = micros();
				}
		}
}
*/
/*
uint16_t On_span = 200, Off_span = 200;
void IR_Scan(void)
{
	
		IR_L = RE_L();
	  IR_LM = RE_LM();
    IR_LF	= RE_LF();
		IR_RF = RE_RF();
		IR_RM = RE_RM();
		IR_R = RE_R();
		IR_L = 0;
	  IR_LM = 0;
    IR_LF	= 0;
		IR_RF = 0;
		IR_RM = 0;
		IR_R = 0;
		uint16_t Val;
		EM_L();
		Delay_us(On_span);
		Val = RE_L();
	  XEM_L();
	  if(IR_L < Val) IR_L = Val - IR_L;
		else IR_L = 0;
		Delay_us(Off_span);
		
		EM_LM();
		Delay_us(On_span);
		Val = RE_LM();
	  XEM_LM();
	  if(IR_LM < Val) IR_LM = Val - IR_LM;
		else IR_LM = 0;
		Delay_us(Off_span);
		
		EM_LF();
		Delay_us(On_span);
		Val = RE_LF();
	  XEM_LF();
	  if(IR_LF < Val) IR_LF = Val - IR_LF;
		else IR_LF = 0;
		Delay_us(Off_span);
		
		EM_RF();
		Delay_us(On_span);
		Val = RE_RF();
	  XEM_RF();
	  if(IR_RF < Val) IR_RF = Val - IR_RF;
		else IR_RF = 0;
		Delay_us(Off_span);
		
		EM_RM();
		Delay_us(On_span);
		Val = RE_RM();
	  XEM_RM();
	  if(IR_RM < Val) IR_RM = Val - IR_RM;
		else IR_RM = 0;
		Delay_us(Off_span);
		
	  EM_R();
		Delay_us(On_span);
		Val = RE_R();
	  XEM_R();
	  if(IR_R < Val) IR_R = Val - IR_R;
		else IR_R = 0;
		Delay_us(Off_span);
}*/
//**********************************************************************
//PID Controller
/*
typedef struct {
		float32_t p;
		float32_t i; 
		float32_t d;
}PID_Constant;
float32_t Kp;
float32_t Ki; 
float32_t Kd;
typedef struct {
		float32_t p;
		float32_t i; 
		float32_t d;
}PID_Error;
PID_Error e0;
uint64_t t0_pid = 0;
float32_t dt_pid = 0;
float32_t P,I,D;
void initPID(PID_Constant K, PID_Error e) {
		Kp = K.p / 100.00000f;
		Ki = K.i / 100.00000f;;
		Kd = K.d / 100.00000f;;
		e0.i = e.i;
		e0.d = e.d;
		I = 0;
		t0_pid = micros();
}
float32_t PID(PID_Error e, float32_t I_threshold) {
		dt_pid = (float32_t)(micros() - t0_pid) / 1000000;
		P = Kp * e.p;
		if(e.i < I_threshold) I += Ki * ((e0.i + e.i) * dt_pid / 2);
		D = Kd * ((e.d - e0.d) / dt_pid);
		e0 = e;
		t0_pid = micros();
		return P+I+D;
}*/
//*****************************************************************************
/*
void ExpLEDInit(void) 
{
		ExpLEDRCC_AHB();
		GPIO_InitTypeDef ExpLED;
			ExpLED.GPIO_Pin   = ExpLED_Pin;
			ExpLED.GPIO_Mode 	= GPIO_Mode_OUT;
			ExpLED.GPIO_OType = GPIO_OType_PP;
			ExpLED.GPIO_PuPd  = GPIO_PuPd_NOPULL;
			ExpLED.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(ExpLED_GPIOX, &ExpLED);
}
void ExpLED_TIM_GPIO_Config(void) 
{
		GPIO_InitTypeDef ExpLED;
		ExpLEDRCC_ABP();
		ExpLEDRCC_AHB(); 
		ExpLED.GPIO_Pin = ExpLED_Pin;
		ExpLED.GPIO_Mode = GPIO_Mode_AF;
		ExpLED.GPIO_Speed = GPIO_Speed_100MHz;
		ExpLED.GPIO_OType = GPIO_OType_PP;
		ExpLED.GPIO_PuPd = GPIO_PuPd_UP ;
		GPIO_Init(ExpLED_GPIOX, &ExpLED); 
		GPIO_PinAFConfig(ExpLED_GPIOX, ExpLED_PinSource, ExpLED_AF_TIMX); 
}
void ExpLED_TIM_Config(void) 
{
	  / * -----------------------------------------------------------------------
    TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles.
    
    In this example TIM3 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM3CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM3CLK = HCLK / 2 = SystemCoreClock /2
          
    To get TIM3 counter clock at 21 MHz, the prescaler is computed as follows:
       Prescaler = (TIM3CLK / TIM3 counter clock) - 1
       Prescaler = ((SystemCoreClock /2) /21 MHz) - 1
                                              
    To get TIM3 output clock at 30 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM3 counter clock / TIM3 output clock) - 1
           = 665
                  
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR)* 100 = 12.5%
    Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.    
		// *   
		uint16_t PrescalerValue = 0;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
		ExpLED_TIM_GPIO_Config();
		// Compute the prescaler value 
		//PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 21000000) - 1;
		PrescalerValue = (uint16_t) ((SystemCoreClock)/1000/21000) - 1;
		// Time base configuration 
		TIM_TimeBaseStructure.TIM_Period = 999;
		TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(ExpLED_TIMX, &TIM_TimeBaseStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    //ChannelX
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 0;
		ExpLED_TIM_OCXInit(ExpLED_TIMX, &TIM_OCInitStructure);
		ExpLED_TIM_OCXPreloadConfig(ExpLED_TIMX, TIM_OCPreload_Enable);
		TIM_ARRPreloadConfig(ExpLED_TIMX, ENABLE);
		TIM_Cmd(ExpLED_TIMX, ENABLE);
}
*/
/*
uint32_t testV;
uint64_t a[1000];
uint32_t i=0, s = 0;
void IR(void) 
{
		t = micros() % 150;
		if(t <= 60) GPIO_SetBits(OB_LED_GPIOX, OB_LED_Pin);
		else if(t <= 70)	testV = ADC_Value[0];
		else GPIO_ResetBits(OB_LED_GPIOX, OB_LED_Pin); 
		a[i++] = t;
	  if(t >= 140) s = 1;
		
}*/
		//double ans=0;
		/*
		while(1)
		{
			  IR();
			  //for(int k = 0; k < 1000; k++) ans *= (10.00+ans)/(1.00-ans)-100.00;
			  if(s == 1)
				{
					s = 0;
					printf("\r**********************************************\r\n");
					for(int j = 0; j < i; j++) printf("\r%lld\r\n", a[j]);
					printf("\r**********************************************\r\n");
					i = 0;
				}
		}*/
				/*
		t0 = micros();
		Delay_us(60);
		dt = micros() - t0;
		printf("\r%lld\r\n",dt);
		*/
				//while(1) 
				 //printf("\r%d %d %d %d %d %d\r\n",ADC_Value[0], ADC_Value[1], ADC_Value[2], ADC_Value[3], ADC_Value[4], ADC_Value[5]);
    /*				
		t0 = micros();
		for(uint32_t i = 0; i<1000; i++);
		t = micros()-t0;
		printf("\r%lld\r\n",t);
		t0 = micros();
		uint32_t i = 0;
		while((micros() - t0) <= 660) i++;
			printf("\rinst = %d\r\n",i);			
		*/
	
	
/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
