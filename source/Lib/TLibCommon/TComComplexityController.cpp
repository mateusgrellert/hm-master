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
UInt TComComplexityController::countToOFF;
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
    countToOFF = 0;
    prevError = 0.0;
    accumError = 0.0;
}

Double TComComplexityController::calcPID(UInt n, UInt t_layer){
    
    Double PIDOutput;
    openPidFile();
    achievedComp = calcAchievedComp();
    Double SP = desiredComp;
    prevError = error;
    nEncoded = n;

   
  // SP = desiredComp*(1.0-0.2*(int)(floor(nEncoded/32)));
   SP = desiredComp;
  //  else
   //     desiredComp = 25;
    

    
   // if (SP > achievedComp and TComComplexityBudgeter::activateControl==false){ // turn on only when needed
    if ((SP > achievedComp and TComComplexityBudgeter::activateControl==false) or (nEncoded >= 64 and nEncoded < 128)){ // turn on only when needed
        TComComplexityBudgeter::activateControl=false;
        countToOFF=0;
        
       pidFile << ";" << TComComplexityBudgeter::activateControl << ";" <<  SP << ";" << achievedComp << ";" << SP << endl;
       pidFile << TComComplexityBudgeter::currPoc;
        return SP;
    }
    else{
        if (SP > achievedComp)
                countToOFF+=1;
        if (countToOFF < 8)
                TComComplexityBudgeter::activateControl=true;
        else{
                TComComplexityBudgeter::activateControl=false;
                countToOFF = 0;
                error = 0;
                accumError = 0.0;
        }

        
        error = SP - achievedComp;
    
        accumError += error;           
    
        PIDOutput = kp*error + ki*accumError + kd*(error - prevError);
        Double finalOutput = calcWeight(t_layer)*(achievedComp + PIDOutput);
        
        if (finalOutput < 0) finalOutput = 0.0;

        
        pidFile << ";" << TComComplexityBudgeter::activateControl << ";" <<  SP << ";" << achievedComp << ";" << finalOutput  << endl;
        pidFile << TComComplexityBudgeter::currPoc;

        return finalOutput;
    }
    //return (desiredComp + PIDOutput);
}

Double TComComplexityController::calcWeight(UInt t_layer){


    Double weights8[] = {2.6,1.3,0.86,0.65,0.65,0.86,0.65,0.65};
    Double weights4[] = {0.6,0.9,0.6,1.8};

    //if (t_layer == 0) return (1+origGop*0.2);
   // return (1+origGop*0.2)/(t_layer)*1.0;

    if(TComComplexityBudgeter::origGop == 8)
        return weights8[(TComComplexityBudgeter::currPoc) % (TComComplexityBudgeter::origGop)];
    else
        return weights4[(TComComplexityBudgeter::currPoc) % (TComComplexityBudgeter::origGop)];
    //return (a1 + d*(5-t_layer));
}

Void TComComplexityController::printAchievedComputation(){
    openPidFile();

    achievedComp = calcAchievedComp();
    pidFile << "POC;" << desiredComp << ";" << achievedComp << ";" << 0 << endl;

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
        pidFile << "POC;Control ON/OFF;desiredComputation;achievedComputation;controlOutput\n";
    }
}