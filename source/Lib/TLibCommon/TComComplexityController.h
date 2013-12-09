/* 
 * File:   TComComplexityController.h
 * Author: mateusgrellert
 *
 * Created on November 27, 2012, 5:14 PM
 */

#ifndef TCOMCOMPLEXITYCONTROLLER_H
#define	TCOMCOMPLEXITYCONTROLLER_H

class TComComplexityController {

public:
    
    static Double achievedComp, desiredComp;
    static Double kp, ki, kd;
    static Double nSteps;
    static Double error, prevError;
    static Double accumError;
    
    static Double procAvail;
    static UInt procFreq;
    static UInt fps;
    static UInt budgetAlg;
    
    static UInt nEncoded;
    static std::ofstream pidFile;

    TComComplexityController();
    
    static Void init(UInt);
    static Void setDesiredComplexity (Double);
    static Double getTargetComplexity();
    static Double calcAchievedComp();
    static Double calcTotalComp();
    static Double calcPID(UInt);
    static Double calcSimpleControl(UInt);
    static Double calcZieglerNichols();
    static Void openPidFile();

    static Void setProcFreq(UInt f) {procFreq = f;};
    static Void setProcAvail(Double a) {procAvail = a;};
    static Void setFPS(UInt f) {fps = f;};
    static Void setBudgetAlg(UInt b) {budgetAlg = b;};
};

#endif	/* TCOMCOMPLEXITYCONTROLLER_H */

