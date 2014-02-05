/* 
 * File:   TComComplexityManagement.h
 * Author: mateusgrellert
 *
 * Created on December 6, 2012, 6:15 PM
 */

#ifndef TCOMCOMPLEXITYMANAGEMENT_H
#define	TCOMCOMPLEXITYMANAGEMENT_H

#include "MyTypedef.h"
#include "TComAnalytics.h"
#include "TComComplexityController.h"
#include "TComComplexityBudgeter.h"


#define EN_COMPLEXITY_MANAGING 0
#define NUM_RD_FRAMES 2

#define CYCLES_ADD 1
#define CYCLES_SUB 1
#define CYCLES_MULT 4

#define CYCLES_SAD 64
#define CYCLES_SSE 128
#define CYCLES_SATD 128
#define CYCLES_TRANSF 256 // 64x64 transform


#define MANAGE_GOP 30

#define KU 0.06
#define TU 0.4
#define NUM_STEPS_ZIEGLER 30 // for ziegler-nichols method

#define KP (0.6*KU)
#define KI (2*KP/TU)
//#define KI (2*KP/TU/4)
#define KD (KP*TU/8.0)


#endif	/* TCOMCOMPLEXITYMANAGEMENT_H */

