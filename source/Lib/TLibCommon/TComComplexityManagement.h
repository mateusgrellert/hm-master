/* 
 * File:   TComComplexityManagement.h
 * Author: mateusgrellert
 *
 * Created on December 6, 2012, 6:15 PM
 */

#ifndef TCOMCOMPLEXITYMANAGEMENT_H
#define	TCOMCOMPLEXITYMANAGEMENT_H


#define EN_COMPLEXITY_MANAGING 1
#define NUM_RD_FRAMES 2

#define NUM_PSETS 5
#define PS100 4
#define PS80 3
#define PS60 2
#define PS40 1
#define PS20 0

#define CYCLES_ADD 1
#define CYCLES_SUB 1
#define CYCLES_MULT 4

#define CYCLES_SAD 64
#define CYCLES_SSE 256
#define CYCLES_SATD 256
#define CYCLES_TRANSF 544 // 64x64 transform


#define MANAGE_GOP 30

#define KU 0.06
#define TU 0.4
#define NUM_STEPS_ZIEGLER 30 // for ziegler-nichols method

#define KP (0.6*KU)
#define KI (2*KP/TU)
//#define KI (2*KP/TU/4)
#define KD (KP*TU/8.0)


#include "MyTypedef.h"
#include "TComAnalytics.h"
#include "TComComplexityController.h"
#include "TComComplexityBudgeter.h"


#endif	/* TCOMCOMPLEXITYMANAGEMENT_H */

