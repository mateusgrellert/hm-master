#include "TComComplexityManagement.h"
#include "TComCABACTables.h"
#include <cmath>

using namespace std;

Int64 TComComplexityBudgeter::countBudget[NUM_PSETS];
UInt new_hist[50][50];
vector<vector <cuStats> > TComComplexityBudgeter::history;
vector<vector <config> > TComComplexityBudgeter::configMap;
vector<Double> TComComplexityBudgeter::weightsVector;
UInt TComComplexityBudgeter::origGop;
UInt TComComplexityBudgeter::picWidth;
UInt TComComplexityBudgeter::picHeight;
UInt TComComplexityBudgeter::intraPeriod;
UInt TComComplexityBudgeter::maxCUDepth;
UInt TComComplexityBudgeter::maxTUDepth;
UInt TComComplexityBudgeter::maxNumRefPics;
UInt TComComplexityBudgeter::nEncoded;
UInt TComComplexityBudgeter::currPoc;
Int TComComplexityBudgeter::searchRange;
Bool TComComplexityBudgeter::hadME;
Bool TComComplexityBudgeter::en_FME;
Bool TComComplexityBudgeter::testAMP;
Bool TComComplexityBudgeter::activateControl;
Bool TComComplexityBudgeter::restore_AMVPInfo;
Double TComComplexityBudgeter::frameBudget;
std::ofstream TComComplexityBudgeter::budgetFile;
AMVPInfo* TComComplexityBudgeter::keep_AMVP;

Void TComComplexityBudgeter::init(UInt w, UInt h, UInt gopSize, UInt intraP){
   // vector<Double> directionVector;
    vector<cuStats> tempHistRow;
    vector<config> tempConfigRow;
    config conf;

    weightsVector.push_back(0); // first frame = I_Frame
    origGop = gopSize;
    intraPeriod = intraP;
    maxCUDepth = 4;
    maxTUDepth = 3;
    maxNumRefPics = 4;
    picWidth = w;
    picHeight = h;
    hadME = 1;
    en_FME = 1;
    testAMP = 1;
    activateControl = false;
    searchRange = 64;
    
    for(int i = 0; i < 50; i++)
        for(int j = 0; j < 50; j++)
                new_hist [i][j] = -1;
    
    for(int i = 0; i < (w >> 6) + 1; i++){
        tempHistRow.clear();
        tempConfigRow.clear();
        for(int j = 0; j < (h >> 6) + 1; j++){
              conf.clear();
   //       directionVector.clear();
            // for history table 7 directions plus no motion position = 8
    //        for(int k = 0; k < 9; k++){
     //           directionVector.push_back(0.0);
     //       }
            //tempHistRow.push_back(make_pair(-1, directionVector));
              
            tempHistRow.push_back(make_pair(-1, -1));

            // for config map
            for(int k = 0; k < 7; k++){
                conf.push_back(-1);
            }
            tempConfigRow.push_back(conf);
        }
        history.push_back(tempHistRow);
        configMap.push_back(tempConfigRow);
    }
}



void TComComplexityBudgeter::setDepthHistory(TComDataCU *&pcCU, UInt pu_idx){
      
    //TODO: implement CU extrapolation
    UInt x = pcCU->getCUPelX();
    UInt y = pcCU->getCUPelY();
    UInt d = pcCU->getDepth(pu_idx);
    Int xP, yP, nPSW, nPSH;
    
//    Double theta = -1;
//    Int idx;
//    Int vectorX = pcCU->getCUMvField(REF_PIC_LIST_0)->getMv(pu_idx).getHor() >> 2;
//    Int vectorY = pcCU->getCUMvField(REF_PIC_LIST_0)->getMv(pu_idx).getVer() >> 2;
    
    pcCU->getPartPosition(pu_idx, xP, yP, nPSW, nPSH);

   //history[x >> 6][y >> 6].first = d;
    new_hist[x >> 6][y >> 6] = d;
//    if((pcCU->isIntra(pu_idx)) or (pcCU->isSkipped(pu_idx))){
//        history[x >> 6][y >> 6].second[8] += ((nPSW*nPSH)/(64.0*64)); // no movement idx
//    }
//    else{
//        // test if PU is inside LCU no motion
//        if(vectorIsInCU(x,y,xP,yP,vectorX,vectorY))
//            history[x >> 6][y >> 6].second[8] += ((nPSW*nPSH)/(64.0*64)); // no movement idx
//        else{
//            theta = calcMotionAngle(xP, yP, vectorX + xP, vectorY + yP);
//            if(theta == 360) theta = 0;
//            idx = (Int) (floor(theta/45.0));
//            history[x >> 6][y >> 6].second[idx] += ((nPSW*nPSH)/(64.0*64));
//        }
//    }
    
}

void TComComplexityBudgeter::updateConfig(TComDataCU*& cu){
    Int x = cu->getCUPelX() >> 6;
    Int y = cu->getCUPelY() >> 6;

    maxCUDepth = configMap[x][y][0];
    maxTUDepth = configMap[x][y][1];
    searchRange = configMap[x][y][2];
    maxNumRefPics = configMap[x][y][3];

    testAMP = configMap[x][y][4];
    hadME = configMap[x][y][5];
    en_FME = 1;
    // en_FME = configMap[x][y][6];
}

void TComComplexityBudgeter::resetConfig(TComDataCU*& cu){
    maxCUDepth = 4;
    maxTUDepth = 3;
    searchRange = 64;
    maxNumRefPics = 4;
    testAMP = 1;
    hadME = 1;
}

Bool TComComplexityBudgeter::isConstrained(){

    if(nEncoded % MANAGE_GOP < NUM_RD_FRAMES)
      return false;
    //if(nEncoded > 40 and nEncoded < 80)
   //     return false;
    
    return true;
}

UInt TComComplexityBudgeter::promote(UInt ctux, UInt ctuy){
    UInt new_pset = configMap[ctux][ctuy][6]+1; // upgrading pset
    assert(new_pset < 5);
    setConfigMap(ctux,ctuy,new_pset);
    return new_pset;
}

UInt TComComplexityBudgeter::demote(UInt ctux, UInt ctuy){
    UInt new_pset = configMap[ctux][ctuy][6]-1; // downgrading pset
    setConfigMap(ctux,ctuy,new_pset);
    return new_pset;
}

Void TComComplexityBudgeter::setMaxPS(){
        Double estCycleCount = 0.0;

 for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1)
                continue;
            setConfigMap(i,j,PS100);
            estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS100);
        }
    }
}

Void TComComplexityBudgeter::uniformBudget(){ 
    Int CUConfig = 0;
    Double estCycleCount = 0.0;
    UInt pset_idx = 4;
            
    while(pset_idx--){
        if (estimateCycleCount(3,pset_idx) <= frameBudget){
            CUConfig = pset_idx;
            break;
        }
    }
    
    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1)
                continue;
            setConfigMap(i,j,CUConfig);
            estCycleCount = updateEstimationAndStats(estCycleCount,-1,CUConfig);
        }
    }
}

Void TComComplexityBudgeter::topDownBudget(){
    Double estCycleCount = 0.0;
    
  //  Int curr_depth = 3;
    UInt new_pset,old_pset, n_demotions = 0;
    
    
    // start by assigning PS100 to all
    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                continue;

            setConfigMap(i,j,PS100);
            estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS100);
        }
    }
    
    n_demotions = 0;
    while(n_demotions < 4){ // maximum #iterations -- all PSETs are already set to PS20
        for (Int demote_depth = 0; demote_depth < 4 and estCycleCount > frameBudget; demote_depth++){
            for(int i = 0; i < history.size() and estCycleCount > frameBudget; i++){
                for(int j = 0; j < history[0].size() and estCycleCount > frameBudget; j++){
                    if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                        continue;

                    if (new_hist[i][j] == demote_depth){
                        old_pset = configMap[i][j][6];
                        new_pset = demote(i,j);
                        estCycleCount = updateEstimationAndStats(estCycleCount,old_pset,new_pset);
                    }
                }
            }          
        }
        n_demotions++;
    }
}

Void TComComplexityBudgeter::bottomUpBudget(){
    Double estCycleCount = 0.0;
    
  //  Int curr_depth = 3;
    UInt new_pset,old_pset, n_promotions = 0;
    
    
    // start by assigning PS20 to all
    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                continue;

            setConfigMap(i,j,PS20);
            estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS20);
        }
    }
    
    n_promotions = 0;
    while(n_promotions < 4){ // maximum #iterations -- all PSETs are already set to PS100
        for (Int promote_depth = 3; promote_depth >=0 and estCycleCount < frameBudget; promote_depth--){
            for(int i = 0; i < history.size() and estCycleCount < frameBudget; i++){
                for(int j = 0; j < history[0].size() and estCycleCount < frameBudget; j++){
                    if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                        continue;

                    if (new_hist[i][j] == promote_depth){
                        old_pset = configMap[i][j][6];
                        new_pset = promote(i,j);
                        estCycleCount = updateEstimationAndStats(estCycleCount,old_pset,new_pset);
                    }
                }
            }          
        }
        n_promotions++;
    }
    
    
}
        
Void TComComplexityBudgeter::knapSackBudget(){

    return;
}

Void TComComplexityBudgeter::leafPriorityBudget(){
    Double estCycleCount = 0.0;
    
    // first step - set HIGH and LOW configs and estimate cycle count

   for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                continue;
            
            if(new_hist[i][j] < 2){ //set LOWEST PSET!
                setConfigMap(i,j,PS20);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS20);
            }
            else if(new_hist[i][j] == 3){ //set HIGHEST PSET!
                setConfigMap(i,j, PS100);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS100);
            }
            else if(new_hist[i][j] == 2){ //set SECOND HIGHEST PSET!
                setConfigMap(i,j, PS80);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS80);
            }
              //reset history for future refs
//            for(int k = 0; k < 9; k++)
//                history[i][j].second[k] = 0.0;
        }

    }
    
  //  refine the top depths using bottom-uo
        UInt new_pset,old_pset, n_promotions = 0;

    while(n_promotions < 4){ // maximum #iterations -- all PSETs are already set to PS100
        for (Int promote_depth = 1; promote_depth >=0 and estCycleCount < frameBudget; promote_depth--){
            for(int i = 0; i < history.size() and estCycleCount < frameBudget; i++){
                for(int j = 0; j < history[0].size() and estCycleCount < frameBudget; j++){
                    if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                        continue;

                    if (new_hist[i][j] == promote_depth){
                        old_pset = configMap[i][j][6];
                        new_pset = promote(i,j);
                        estCycleCount = updateEstimationAndStats(estCycleCount,old_pset,new_pset);
                    }
                }
            }          
        }
        n_promotions++;
    }
    
              


    //cout << estCycleCount << "\t" << frameBudget << endl;
}

Void TComComplexityBudgeter::rootPriorityBudget(){
    Double estCycleCount = 0.0;
    
    // first step - set HIGH and LOW configs and estimate cycle count

   for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                continue;
            
            if(new_hist[i][j] > 1){ //set HIGHEST PSET!
                setConfigMap(i,j,PS100);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS100);
            }
            else if(new_hist[i][j] == 0){ //set LOWEST PSET!
                setConfigMap(i,j, PS20);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS20);
            }
            else if(new_hist[i][j] == 1){ //set LOWEST HIGHEST PSET!
                setConfigMap(i,j, PS40);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS40);
            }
              //reset history for future refs
//            for(int k = 0; k < 9; k++)
//                history[i][j].second[k] = 0.0;
        }

    }
    
  //  refine the bottom depths using top-down
        UInt new_pset,old_pset, n_demotions = 0;

    while(n_demotions < 4){ // maximum #iterations -- all PSETs are already set to PS20
        for (Int demote_depth = 2; demote_depth < 4 and estCycleCount > frameBudget; demote_depth++){
            for(int i = 0; i < history.size() and estCycleCount > frameBudget; i++){
                for(int j = 0; j < history[0].size() and estCycleCount > frameBudget; j++){
                    if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                        continue;

                    if (new_hist[i][j] == demote_depth){
                        old_pset = configMap[i][j][6];
                        new_pset = demote(i,j);
                        estCycleCount = updateEstimationAndStats(estCycleCount,old_pset,new_pset);
                    }
                }
            }          
        }
        n_demotions++;
    }
    
              


    //cout << estCycleCount << "\t" << frameBudget << endl;
}


Void TComComplexityBudgeter::ICIPBudget(){
    Double estCycleCount = 0.0;
    UInt new_pset,old_pset, n_demotions = 0;

    // first step - set HIGH and LOW configs and estimate cycle count
    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1)
                        continue;
                   
            if(new_hist[i][j] == 0){ //set LOW budget!
                setConfigMap(i,j,PS20);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS20);
            }
            
            else if(new_hist[i][j] == 3){ //set HIGH budget!
                setConfigMap(i,j,PS100);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS100);
            }
            else{
                setConfigMap(i,j,PS60);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS60);
            }
        }
    }
    
           // second step - start demoting until available computation is reached
    
    UInt n_promotions = 0;
    
    n_promotions = 0;

    while(n_promotions < 4){ // maximum #iterations -- all PSETs are already set to PS100
        for (Int promote_depth = 2; promote_depth >=0 and estCycleCount < frameBudget; promote_depth--){
            for(int i = 0; i < history.size() and estCycleCount < frameBudget; i++){
                for(int j = 0; j < history[0].size() and estCycleCount < frameBudget; j++){
                    if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                        continue;

                    if (new_hist[i][j] == promote_depth){
                        old_pset = configMap[i][j][6];
                        if (old_pset < PS100){
                                new_pset = promote(i,j);
                                estCycleCount = updateEstimationAndStats(estCycleCount,old_pset,new_pset);
                        }
                    }
                }
            }          
        }
        n_promotions++;
    }

    // second step - start demoting until available computation is reached
    while(n_demotions < 4){ // maximum #iterations -- all PSETs are already set to PS20
        for (Int demote_depth = 1; demote_depth <= 3 and estCycleCount > frameBudget; demote_depth++){
            for(int i = 0; i < history.size() and estCycleCount > frameBudget; i++){
                for(int j = 0; j < history[0].size() and estCycleCount > frameBudget; j++){
                    if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                        continue;

                    if (new_hist[i][j] == demote_depth){
                        old_pset = configMap[i][j][6];
                        if (old_pset > PS20){
                                new_pset = demote(i,j);
                                estCycleCount = updateEstimationAndStats(estCycleCount,old_pset,new_pset);
                        }
                    }
                }
            }          
        }
        n_demotions++;
    }
       
}




// conf 0 = low, 1 = medL, 2 = medH, 3 = high

Double TComComplexityBudgeter::updateEstimationAndStats(Double est, Int old_pset, UInt new_pset){
    Double nCU = (picHeight*picWidth)/(64.0*64.0);
    UInt depth_map[] = {2,2,3,3,3}; //maximum depth allowed in each profile
   
    UInt d_old, d_new;
        
    if (old_pset != -1)
        d_old = depth_map[old_pset];
    d_new = depth_map[new_pset];

    if (old_pset != -1)
        countBudget[old_pset]--;
    countBudget[new_pset]++;
       
    if (old_pset != -1)
        est -= estimateCycleCount(d_old,old_pset)/nCU;
    est += estimateCycleCount(d_new,new_pset)/nCU;
    
    return est;
}

Void TComComplexityBudgeter::setConfigMap(UInt i, UInt j, UInt pset){
    switch (pset){
        case PS100:
            configMap[i][j][0] = 4; // Max CU Depth
            configMap[i][j][1] = 3; // Max TU Depth
            configMap[i][j][2] = 64; // SR
            configMap[i][j][3] = 4; // Max Num Ref Pics
            configMap[i][j][4] = 1; // AMP
            configMap[i][j][5] = 1; // Had ME
            configMap[i][j][6] = 4; // PSET INDEX
            break;
        case PS80:                         
            configMap[i][j][0] = 4; // Max CU Depth
            configMap[i][j][1] = 3; // Max TU Depth
            configMap[i][j][2] = 32; // SR
            configMap[i][j][3] = 4; // Max Num Ref Pics
            configMap[i][j][4] = 1; // AMP
            configMap[i][j][5] = 1; // Had ME
            configMap[i][j][6] = 3; // PSET INDEX
            break;
        case PS60:
            configMap[i][j][0] = 4; // Max CU Depth
            configMap[i][j][1] = 1; // Max TU Depth
            configMap[i][j][2] = 32; // SR
            configMap[i][j][3] = 4; // Max Num Ref Pics
            configMap[i][j][4] = 0; // AMP
            configMap[i][j][5] = 1; // Had ME
            configMap[i][j][6] = 2; // PSET INDEX
            break;

        case PS40:
            configMap[i][j][0] = 3; // Max CU Depth
            configMap[i][j][1] = 1; // Max TU Depth
            configMap[i][j][2] = 16; // SR
            configMap[i][j][3] = 4; // Max Num Ref Pics
            configMap[i][j][4] = 0; // AMP
            configMap[i][j][5] = 0; // Had ME
            configMap[i][j][6] = 1; // PSET INDEX
            break;

        default:
            configMap[i][j][0] = 3; // Max CU Depth
            configMap[i][j][1] = 1; // Max TU Depth
            configMap[i][j][2] = 8; // SR
            configMap[i][j][3] = 1; // Max Num Ref Pics
            configMap[i][j][4] = 0; // AMP
            configMap[i][j][5] = 0; // Had ME
            configMap[i][j][6] = 0; // PSET INDEX
            break;

    }

}

Double TComComplexityBudgeter::estimateCycleCount(UInt d, UInt conf){
    Double count = 0;
    Double factor = 0.0;
    switch (conf){
        case PS100: factor = 1.0; break; //PS100
        case PS80: factor = 0.8; break; //PS80
        case PS60: factor = 0.6; break; //PS60
        case PS40: factor = 0.4; break; //PS40
        case PS20: factor = 0.2; break; //PS20
        default: factor = 1.0; break;
    }
    
    // for(int i = 0; i <= d; i++){
     for(int i = 0; i < 4; i++){
        count += (TComAnalytics::sadCount[i][0]) * CYCLES_SAD;
        count += (TComAnalytics::sseCount[i][0]) * CYCLES_SSE;
        count += (TComAnalytics::satdCount[i][0]) * CYCLES_SATD;
        count += (TComAnalytics::transfCount[i][0]) * CYCLES_TRANSF;
    }
    
    return (count*factor);
}

Void TComComplexityBudgeter::distributeBudget(){
    UInt alg = TComComplexityController::budgetAlg;
    resetBudgetStats();

    if (activateControl)
        switch(alg){
            case 0: uniformBudget(); break;
            case 1: topDownBudget(); break;
            case 2: bottomUpBudget(); break;
            case 3: leafPriorityBudget(); break; 
            case 4: rootPriorityBudget(); break; 
            case 5: ICIPBudget(); break;
            default: uniformBudget(); break;
        }
    else
        setMaxPS();
    
    printBudgetStats();

}

Void TComComplexityBudgeter::resetBudgetStats(){

    for (int i = 0; i < NUM_PSETS; i++)
         countBudget[i] = 0;
}

Void TComComplexityBudgeter::printBudgetStats(){
    
    if(!budgetFile.is_open()){
        budgetFile.open("budgetDist.csv",ofstream::out);
        budgetFile << "PS20;PS40;PS60;PS80;PS100" << endl;
    }

    Double total = 0.0;
    for (int i = 0; i < NUM_PSETS; i++)
        total += countBudget[i];
    
    for (int i = 0; i < NUM_PSETS; i++)
        budgetFile << (Double) countBudget[i]/total << ";";

    budgetFile << endl;
}

Void TComComplexityBudgeter::setFrameBudget(Double budget){
    frameBudget = budget*0.8;
    //cout << budget << "\t" << calcWeight(t_layer) << endl;
}

Void TComComplexityBudgeter::setEncodedCount(UInt n){
    nEncoded = n;
}

Double TComComplexityBudgeter::calcMotionAngle(Int x1, Int y1, Int x2, Int y2){
    Double theta;
    
    // calculate theta = arctan(co/ca)
    theta = atan(abs(y2-y1)/abs(x2-x1+0.01))*180/M_PI;

    if((x2 - x1) >= 0 and (y2 - y1) < 0){ // second quadrat
        theta = 180 - theta;
    }
    else if((x2 - x1) < 0 and (y2 - y1) <= 0){ // third quadrat
        theta = 270 - (theta - 90);
    }
    else if((x2 - x1) >= 0 and (y2 - y1) > 0){
        theta = 270 + (theta - 90);
    }
    
    return theta;
}

//Void TComComplexityBudgeter::calcDominantDirection(){
//    Double temp_max;
//    Int domDir = 8;
//    for(int i = 0; i < history.size() -1; i++){
//        for(int j = 0; j < history[0].size() -1; j++){
//            temp_max = 0;
//            for(int k = 0; k < 9; k++){
//                // 1 - calculate dominant direction
//                if(temp_max < history[i][j].second[k]){
//                    temp_max = history[i][j].second[k];
//                    domDir = k;
//                }
//                // 2 - switch depths
//                switch(domDir){
//                    case 0:
//                        if(i+1 < picWidth)
//                            history[i+1][j].first = max(new_hist[i][j], history[i+1][j].first);
//                        break;
//                    case 1:
//                        if(i+1 < picWidth)
//                            history[i+1][j].first = max(new_hist[i][j], history[i+1][j].first);
//                        if((i+1 < picWidth) and (j-1  >= 0))
//                            history[i+1][j-1].first = max(new_hist[i][j], history[i+1][j-1].first);
//                        if(j-1 >= 0)
//                            history[i][j-1].first = max(new_hist[i][j], history[i][j-1].first);
//                        break;
//                    case 2:
//                        if(j-1 >= 0)
//                            history[i][j-1].first = max(new_hist[i][j], history[i][j-1].first);
//                        break;
//                    case 3:
//                        if(j-1 >= 0)
//                            history[i][j-1].first = max(new_hist[i][j], history[i][j-1].first);
//                        if((i-1  >= 0) and (j-1  >= 0))
//                            history[i-1][j-1].first = max(new_hist[i][j], history[i-1][j-1].first);
//                        if(i-1  >= 0)
//                            history[i-1][j].first = max(new_hist[i][j], history[i-1][j].first);
//                        break;
//                    case 4:
//                        if(i-1 >= 0)
//                            history[i-1][j].first = max(new_hist[i][j], history[i-1][j].first);
//                        break;
//                    case 5:
//                        if(i-1 >= 0)
//                            history[i-1][j].first = max(new_hist[i][j], history[i-1][j].first);
//                        if((i-1  >= 0) and (j+1 < picHeight))
//                            history[i-1][j+1].first = max(new_hist[i][j], history[i-1][j+1].first);
//                        if(j+1 < picHeight)
//                            history[i][j+1].first = max(new_hist[i][j], history[i][j+1].first);
//                        break;
//                    case 6:
//                        if(j+1 < picHeight)
//                            history[i][j+1].first = max(new_hist[i][j], history[i][j+1].first);
//                        break;
//                    case 7:
//                        if(j+1 < picHeight)
//                            history[i][j+1].first = max(new_hist[i][j], history[i][j+1].first);
//                        if((i+1 < picWidth) and (j+1 < picHeight))
//                            history[i+1][j+1].first = max(new_hist[i][j], history[i+1][j+1].first);
//                        if(i+1 < picWidth)
//                            history[i+1][j].first = max(new_hist[i][j], history[i+1][j].first);
//                        break;
//                    default:
//                        break;
//                }
//            }
//        }
//    }
//}

Bool TComComplexityBudgeter::vectorIsInCU(Int cuX, Int cuY, Int puX, Int puY, Int vecX, Int vecY){
    
    if(vecX == 0 and vecY == 0)
        return true;
    
    if(vecX <= 0){ // left
        if (vecY <= 0) // above left
            return (( vecX <= (puX - cuX)) and ( vecY <= (puY - cuY)));
        else // below left
            return (( vecX <= (puX - cuX)) and ( vecY <= (64 - puY + cuY)));
    }
    else{ // right
        if (vecY <= 0) // above right
            return (( vecX <= (64 - puX + cuX)) and ( vecY <= (puY - cuY)));
        else // below right
            return (( vecX <= (64 - puX + cuX)) and ( vecY <= (64 - puY + cuY)));
    }
}
