#include "TComComplexityManagement.h"

Double TComComplexityController::achievedComp;
Double TComComplexityController::desiredComp;
Double TComComplexityController::error;
Double TComComplexityController::prevError;
Double TComComplexityController::accumError;
Double TComComplexityController::kd;
Double TComComplexityController::ki;
Double TComComplexityController::kp;
Double TComComplexityController::nSteps;
Double TComComplexityController::procAvail;
UInt TComComplexityController::procFreq;
UInt TComComplexityController::fps;
UInt TComComplexityController::nEncoded;
std::ofstream TComComplexityController::pidFile;

Void TComComplexityController::init(UInt n_encFrames){
    desiredComp = procFreq*procAvail/fps; // gives number of cycles available per frame per frame
    kp = KP;
    ki = KI;
    kd = KD;
    nEncoded = n_encFrames;
    nSteps = NUM_STEPS_ZIEGLER;
    error = 0.0;
    prevError = 0.0;
    accumError = 0.0;
}

Double TComComplexityController::calcPID(UInt n){
    Double PIDOutput;
    openPidFile();
    achievedComp = calcAchievedComp();
    
    prevError = error;
    
 // resetting desired in order to actually achieve 60% if possible
 //      Double totalComp = calcTotalComp();
    nEncoded = n;
 //   if(nEncoded <= NUM_RD_FRAMES){
 //       desiredComp = totalComp/(nEncoded-1)*PERCENTAGE_AVAILABLE;
 //   }
    
    error = desiredComp - achievedComp;
    
    accumError += error;           
    
    PIDOutput = kp*error + ki*accumError + kd*(error - prevError);

    pidFile << achievedComp << ";" << achievedComp + PIDOutput << ";" << desiredComp << endl;
    
    return (achievedComp + PIDOutput);
    //return (desiredComp + PIDOutput);
}

Double TComComplexityController::calcAchievedComp(){
    Double n_cycles = 0.0; // in 10^6 cycles
        // REMINDER - opCount is in 10^6
    for(int i = 0; i < g_uiMaxCUDepth; i++){
        n_cycles += TComAnalytics::sadCount[i][0]*CYCLES_SAD;
        n_cycles += TComAnalytics::sseCount[i][0]*CYCLES_SSE;
        n_cycles += TComAnalytics::satdCount[i][0]*CYCLES_SATD;
        n_cycles += TComAnalytics::transfCount[i][0]*CYCLES_TRANSF;
    }
    
    return n_cycles;
}

Double TComComplexityController::calcTotalComp(){
    Double n_cycles = 0.0;
        // REMINDER - opCount is in 10^6
    for(int i = 0; i < g_uiMaxCUDepth; i++){
        n_cycles += TComAnalytics::sadCount[i][1]*CYCLES_SAD;
        n_cycles += TComAnalytics::sseCount[i][1]*CYCLES_SSE;
        n_cycles += TComAnalytics::satdCount[i][1]*CYCLES_SATD;
        n_cycles += TComAnalytics::transfCount[i][1]*CYCLES_TRANSF;
    }
    
    return n_cycles;
}

Void TComComplexityController::openPidFile(){ 
    if(pidFile == NULL){
        pidFile.open("pidOut.csv",ofstream::out);
        pidFile << "achievedComputation;pidOutput;desiredComputation\n";
    }
}