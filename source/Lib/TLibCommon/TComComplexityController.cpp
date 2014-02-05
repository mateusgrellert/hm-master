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
Double TComComplexityController::fps;
UInt TComComplexityController::budgetAlg;
UInt TComComplexityController::nEncoded;
std::ofstream TComComplexityController::pidFile;

Void TComComplexityController::init(UInt n_encFrames){
    desiredComp = procFreq*procAvail/(fps); // gives number of cycles available per frame per frame
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
    nEncoded = n;

 // resetting desired in order to actually achieve 60% if possible
 //      Double totalComp = calcTotalComp();
 //   if(nEncoded <= NUM_RD_FRAMES){
 //       desiredComp = totalComp/(nEncoded-1)*PERCENTAGE_AVAILABLE;
 //   }
   // if (nEncoded <= 64)
  //      desiredComp = 50;
  //  else
   //     desiredComp = 25;
    
    error = desiredComp - achievedComp;
    
    accumError += error;           
    
    PIDOutput = kp*error + ki*accumError + kd*(error - prevError);

    pidFile << desiredComp << ";" << achievedComp << ";" << achievedComp + PIDOutput << endl;
    
    return (achievedComp + PIDOutput);
    //return (desiredComp + PIDOutput);
}

Void TComComplexityController::printAchievedComputation(){
    openPidFile();

    achievedComp = calcAchievedComp();
    pidFile << desiredComp << ";" << achievedComp << ";" << 0 << endl;

}

Double TComComplexityController::calcSimpleControl(UInt n){
        Double avgComp = calcTotalComp()/((n-1)*1.0);
        achievedComp = calcAchievedComp();
        openPidFile();

        pidFile <<desiredComp << ";" << achievedComp << ";" << (2*desiredComp - avgComp)  << endl;

        return (2*desiredComp - avgComp);
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
    if(!pidFile.is_open()){
        pidFile.open("controlOut.csv",ofstream::out);
        pidFile << "desiredComputation;achievedComputation;controlOutput\n";
    }
}