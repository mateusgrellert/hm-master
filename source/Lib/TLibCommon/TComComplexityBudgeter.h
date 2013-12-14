/* 
 * File:   TComComplexityBudgeter.h
 * Author: mateusgrellert
 *
 * Created on November 27, 2012, 5:14 PM
 */

#ifndef TCOMCOMPLEXITYBUDGETER_H
#define	TCOMCOMPLEXITYBUDGETER_H

using namespace std;

typedef pair<UInt,vector<Double> > cuStats; //max depth, dominant direction vector 
typedef vector<UInt> config;

class TComComplexityBudgeter {
    
public:
    static vector<vector <cuStats> > history;
    static vector<vector <config> > configMap;

    static vector<Double> weightsVector;
    static Double frameBudget;
    static UInt origGop;
    static Int64 countBudget[4];
    
    static UInt picWidth;
    static UInt picHeight;
    static UInt intraPeriod;
    static UInt maxCUDepth;
    static UInt maxTUDepth;
    static UInt maxNumRefPics;
    static UInt nEncoded;
    static UInt currPoc;
    static Int searchRange;
    static Bool hadME;
    static Bool testAMP;
    static Bool restore_AMVPInfo;
    static ofstream budgetFile;
    
    static AMVPInfo *keep_AMVP;
    
    TComComplexityBudgeter();
    
    static Void init(UInt, UInt, UInt, UInt);
    static Bool isConstrained();
    
    static Void printBudgetStats();
    static Void resetBudgetStats();
    static Void uniformBudget();
    static Void bottomUpBudget();
    static Void topDownBudget();
    static Void knapSackBudget();
    static Void ICIPBudget();
    static Void distributeBudget();
    static Double updateEstimationAndStats(Double est, UInt old, UInt neww);

    static Void setDepthHistory(TComDataCU *&, UInt);
    static Void setVectorHistory(UInt, UInt, UInt);
    static Double calcWeight(UInt);
    static Void setFrameBudget(Double, UInt);
    static Void setEncodedCount(UInt);
    static Double estimateCycleCount(UInt , UInt);
    static Void updateConfig(TComDataCU*& cu);
    static Void resetConfig(TComDataCU*& cu);
    static Void setPoc(UInt p) {currPoc = p;};
    static Void setConfigMap(UInt i, UInt j, UInt prof);

    static Double calcMotionAngle(Int x1, Int y1, Int x2, Int y2);
    static Void calcDominantDirection();
    static Bool vectorIsInCU(Int cuX, Int cuY, Int puX, Int puY, Int vecX, Int vecY);


};


#endif	/* TCOMCOMPLEXITYBUDGETER_H */

