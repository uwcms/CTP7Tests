/*
 * \file L1TCTP7.cc
 *
 * \author P. Wittich
 *
 */

#include "CTP7Tests/CTP7DQM/interface/L1TCTP7.h"
#include "DataFormats/Provenance/interface/EventAuxiliary.h"

//DQMStore
#include "DQMServices/Core/interface/DQMStore.h"




using namespace edm;

const unsigned int NUMREGIONS = 396;

const unsigned int PHIBINS = 18;
const float PHIMIN = -0.5;
const float PHIMAX = 17.5;

// Ranks 6, 10 and 12 bits
const unsigned int R6BINS = 64;
const float R6MIN = -0.5;
const float R6MAX = 63.5;
const unsigned int R10BINS = 1024;
const float R10MIN = -0.5;
const float R10MAX = 1023.5;

const unsigned int ETABINS = 22;
const float ETAMIN = -0.5;
const float ETAMAX = 21.5;

const unsigned int EVBINS = 4000;
const float EVMIN = -0.5;
const float EVMAX = 3999.5;

const unsigned int PUMBINS = 22;
const float PUMMIN = -0.5;
const float PUMMAX = 21.5;

L1TCTP7::L1TCTP7(const ParameterSet & ps) :
   ctp7Source_L1CRCollection_( consumes<L1CaloRegionCollection>(ps.getParameter< InputTag >("ctp7Source") )),
   ctp7Source_L1CEMCollection_( consumes<L1CaloEmCollection>(ps.getParameter< InputTag >("ctp7Source") )),
   filterTriggerType_ (ps.getParameter< int >("filterTriggerType"))
{

  // verbosity switch
  verbose_ = ps.getUntrackedParameter < bool > ("verbose", false);

  if (verbose_)
    std::cout << "L1TCTP7: constructor...." << std::endl;


  dbe = NULL;
  if (ps.getUntrackedParameter < bool > ("DQMStore", false)) {
    dbe = Service < DQMStore > ().operator->();
    dbe->setVerbose(0);
  }

  outputFile_ =
      ps.getUntrackedParameter < std::string > ("outputFile", "");
  if (outputFile_.size() != 0) {
    std::
	cout << "L1T Monitoring histograms will be saved to " <<
	outputFile_.c_str() << std::endl;
  }

  bool disable =
      ps.getUntrackedParameter < bool > ("disableROOToutput", false);
  if (disable) {
    outputFile_ = "";
  }


  if (dbe != NULL) {
    dbe->setCurrentFolder("L1T/L1TCTP7");
  }


}

L1TCTP7::~L1TCTP7()
{
}

void L1TCTP7::beginJob(void)
{
  nev_ = 0;
}

void L1TCTP7::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
  //Only histograms booking

  // get hold of back-end interface
  DQMStore *dbe = 0;
  dbe = Service < DQMStore > ().operator->();

  if (dbe) {
    dbe->setCurrentFolder("L1T/L1TCTP7");

    triggerType_ =
      dbe->book1D("TriggerType", "TriggerType", 17, -0.5, 16.5);
    // global regions
    ctp7RegionsNonZeroVsEvt_ =
	dbe->book2D("RctRegionsNonZeroVsEvt", "REGION PUM vs EVT", EVBINS, EVMIN,
                    EVMAX, NUMREGIONS,-0.5,NUMREGIONS-0.5);
   ctp7RegionsNonZeroBarrelVsEvt_ =
	dbe->book2D("RctRegionsNonZeroBarrelVsEvt", "REGION PUM vs EVT", EVBINS, EVMIN,
                    EVMAX, NUMREGIONS,-0.5,NUMREGIONS-0.5);
   ctp7RegionsNonZeroHFVsEvt_ =
	dbe->book2D("RctRegionsNonZeroHFVsEvt", "REGION PUM vs EVT", EVBINS, EVMIN,
                    EVMAX, NUMREGIONS,-0.5,NUMREGIONS-0.5);



    ctp7RegionsNormNonZeroVsEvt_ =
        dbe->book2D("RctRegionsNormNonZeroVsEvt", "REGION PUM vs EVT", EVBINS, EVMIN,
                    EVMAX, PUMBINS, PUMMIN, PUMMAX);

    ctp7RegionsAvgEtVsEvt_ = 
	dbe->book2D("RctRegionsAvgEtVsEvt", " AVERAGE REGION RANK vs EVT", EVBINS, EVMIN,
                    EVMAX, R10BINS, R10MIN, R10MAX);

    ctp7RegionsTotEtVsEvt_ =
        dbe->book2D("RctRegionsTotEtVsEvt", " TOTAL REGION RANK vs EVT", EVBINS, EVMIN,
                    EVMAX, 100,0,100000);

    ctp7RegionsOccVsEvt_ =
        dbe->book2D("RctRegionsOccVsEvt", " Occupancy vs EVT", EVBINS, EVMIN,
                    EVMAX, NUMREGIONS,-0.5,NUMREGIONS-0.5);

    ctp7RegionsEtMapVsEvt_ = 
        dbe->book2D("RctRegionsEtMapVsEvt", " EtMap per region ID vs EVT", EVBINS, EVMIN,
                    EVMAX, NUMREGIONS,-0.5,NUMREGIONS-0.5);

    ctp7RegionsHFPhiOccETVsEvt_ =
        dbe->book2D("RctRegionsHFPhiOccETVsEvt", "Phi of the HF clusters, energy in Z axis, vs EVT", EVBINS, EVMIN,
                    EVMAX, PHIBINS, PHIMIN, PHIMAX);

    ctp7RegionsHFPhiPlusOccETVsEvt_ =
        dbe->book2D("RctRegionsHFPhiPlusOccETVsEvt", "Phi of the HF clusters, energy in Z axis, vs EVT", EVBINS, EVMIN,
                    EVMAX, PHIBINS, PHIMIN, PHIMAX);

    ctp7RegionsHFPhiMinusOccETVsEvt_ =
        dbe->book2D("RctRegionsHFPhiMinusOccETVsEvt", "Phi of the HF clusters, energy in Z axis, vs EVT", EVBINS, EVMIN,
                    EVMAX, PHIBINS, PHIMIN, PHIMAX);

    ctp7RegionsMaxEtVsEvt_ =
        dbe->book2D("RctRegionsMaxEtVsEvt", " MAX REGION RANK vs EVT", EVBINS, EVMIN,
                    EVMAX, R10BINS, R10MIN, R10MAX);

    ctp7RegionsMaxEtHFVsEvt_ =
        dbe->book2D("RctRegionsMaxEtHFVsEvt", " MAX REGION RANK HF vs EVT", EVBINS, EVMIN,
                    EVMAX, R10BINS, R10MIN, R10MAX);

    ctp7RegionsMaxEtBarrelVsEvt_ =
        dbe->book2D("RctRegionsMaxEtBarrelVsEvt", " MAX REGION RANK gctETA=10||11 vs EVT", EVBINS, EVMIN,
                    EVMAX, R10BINS, R10MIN, R10MAX);

    ctp7EmTotEtVsEvt_ =
                dbe->book2D("RctEmIsoTotEtVsEvt", " TOTAL REGION RANK vs EVT", EVBINS, EVMIN,
                    EVMAX, R10BINS*2,R10MIN,R10MAX*2);

    ctp7RegionsTotalRegionEt_ =
        dbe->book1D("RctRegionsTotalRegionEt", "AVERAGE REGION RANK", 100,0,100000);

    ctp7EmAvgEtVsEvt_ =
		dbe->book2D("RctEmIsoAvgEtVsEvt", " AVERAGE REGION RANK vs EVT", EVBINS, EVMIN,
                    EVMAX, R10BINS, R10MIN, R10MAX);

    ctp7EmMaxEtVsEvt_ =
        dbe->book2D("RctEmIsoMaxEtVsEvt", " MAX REGION RANK vs EVT", EVBINS, EVMIN,
                    EVMAX, R10BINS, R10MIN, R10MAX);

    ctp7RegionsAverageRegionEt_ =
	dbe->book1D("RctRegionsAverageRegionEt", "AVERAGE REGION RANK", R10BINS, R10MIN, R10MAX);

    ctp7RegionsAvgEtVsEta_ =
	dbe->book2D("RctRegionsAvgEtVsEta", " AVERAGE REGION RANK vs ETA", ETABINS, ETAMIN,
                    ETAMAX, R10BINS, R10MIN, R10MAX);

    ctp7RegionsNonZero_ =
        dbe->book1D("RctRegionsNonZero", "REGION PUM", NUMREGIONS,-0.5,NUMREGIONS-0.5);

    ctp7RegionsNormNonZero_ =
	dbe->book1D("RctRegionsNormNonZero", "REGION PUM", PUMBINS, PUMMIN, PUMMAX);

    ctp7RegionsPumEta0_ =
	dbe->book2D("RctRegionsPumEta0", " PUM BIN", PUMBINS, PUMMIN,
		    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta1_ =
	dbe->book2D("RctRegionsPumEta1", " PUM BIN", PUMBINS, PUMMIN,
		    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta2_ =
        dbe->book2D("RctRegionsPumEta2", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta3_ =
        dbe->book2D("RctRegionsPumEta3", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta4_ =
        dbe->book2D("RctRegionsPumEta4", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta5_ =
        dbe->book2D("RctRegionsPumEta5", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta6_ =
        dbe->book2D("RctRegionsPumEta6", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta7_ =
        dbe->book2D("RctRegionsPumEta7", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta8_ =
        dbe->book2D("RctRegionsPumEta8", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta9_ =
        dbe->book2D("RctRegionsPumEta9", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta10_ =
	dbe->book2D("RctRegionsPumEta10", " PUM BIN", PUMBINS, PUMMIN,
		    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta11_ =
	dbe->book2D("RctRegionsPumEta11", " PUM BIN", PUMBINS, PUMMIN,
		    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta12_ =
        dbe->book2D("RctRegionsPumEta12", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta13_ =
        dbe->book2D("RctRegionsPumEta13", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta14_ =
        dbe->book2D("RctRegionsPumEta14", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta15_ =
        dbe->book2D("RctRegionsPumEta15", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta16_ =
        dbe->book2D("RctRegionsPumEta16", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta17_ =
        dbe->book2D("RctRegionsPumEta17", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta18_ =
        dbe->book2D("RctRegionsPumEta18", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta19_ =
        dbe->book2D("RctRegionsPumEta19", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta20_ =
        dbe->book2D("RctRegionsPumEta20", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);
    ctp7RegionsPumEta21_ =
        dbe->book2D("RctRegionsPumEta21", " PUM BIN", PUMBINS, PUMMIN,
                    PUMMAX, R10BINS, R10MIN, R10MAX);

    ctp7IsoEmEtEtaPhi_ =
	dbe->book2D("RctEmIsoEmEtEtaPhi", "ISO EM E_{T}", ETABINS, ETAMIN,
		    ETAMAX, PHIBINS, PHIMIN, PHIMAX);
    ctp7IsoEmOccEtaPhi_ =
	dbe->book2D("RctEmIsoEmOccEtaPhi", "ISO EM OCCUPANCY", ETABINS,
		    ETAMIN, ETAMAX, PHIBINS, PHIMIN, PHIMAX);
    ctp7NonIsoEmEtEtaPhi_ =
	dbe->book2D("RctEmNonIsoEmEtEtaPhi", "NON-ISO EM E_{T}", ETABINS,
		    ETAMIN, ETAMAX, PHIBINS, PHIMIN, PHIMAX);
    ctp7NonIsoEmOccEtaPhi_ =
	dbe->book2D("RctEmNonIsoEmOccEtaPhi", "NON-ISO EM OCCUPANCY",
		    ETABINS, ETAMIN, ETAMAX, PHIBINS, PHIMIN, PHIMAX);
    ctp7EmNonZeroVsEvt_ =
	dbe->book2D("RctEmIsoNonZeroVsEvt", "EM NonZero vs EVT", EVBINS, EVMIN,
                    EVMAX, NUMREGIONS,-0.5,NUMREGIONS-0.5);

    ctp7EmAvgEtVsEta_ =
	dbe->book2D("RctEmIsoAvgEtVsEta", " AVERAGE EM RANK vs ETA", ETABINS, ETAMIN,
                    ETAMAX, R10BINS, R10MIN, R10MAX);



    // global regions
    ctp7RegionsEtEtaPhi_ =
	dbe->book2D("RctRegionsEtEtaPhi", "REGION E_{T}", ETABINS, ETAMIN,
		    ETAMAX, PHIBINS, PHIMIN, PHIMAX);
    ctp7RegionsOccEtaPhi_ =
	dbe->book2D("RctRegionsOccEtaPhi", "REGION OCCUPANCY", ETABINS,
		    ETAMIN, ETAMAX, PHIBINS, PHIMIN, PHIMAX);

    ctp7OverFlowEtaPhi_ =
	dbe->book2D("RctBitOverFlowEtaPhi", "OVER FLOW OCCUPANCY", ETABINS,
		    ETAMIN, ETAMAX, PHIBINS, PHIMIN, PHIMAX);

    ctp7TauVetoEtaPhi_ =
	dbe->book2D("RctBitTauVetoEtaPhi", "TAU VETO OCCUPANCY", ETABINS,
		    ETAMIN, ETAMAX, PHIBINS, PHIMIN, PHIMAX);

    ctp7MipEtaPhi_ =
	dbe->book2D("RctBitMipEtaPhi", "MIP OCCUPANCY", ETABINS,
		    ETAMIN, ETAMAX, PHIBINS, PHIMIN, PHIMAX);

    ctp7QuietEtaPhi_ =
	dbe->book2D("RctBitQuietEtaPhi", "QUIET OCCUPANCY", ETABINS,
		    ETAMIN, ETAMAX, PHIBINS, PHIMIN, PHIMAX);

    ctp7HfPlusTauEtaPhi_ =
	dbe->book2D("RctBitHfPlusTauEtaPhi", "HF plus Tau OCCUPANCY", ETABINS,
		    ETAMIN, ETAMAX, PHIBINS, PHIMIN, PHIMAX);

    // local regions
/*
    const int nlocphibins = 2; 
    const float locphimin = -0.5;
    const float locphimax = 1.5;
    const int nlocetabins = 11;
    const float locetamin = -0.5;
    const float locetamax = 10.5;
    ctp7RegionsLocalEtEtaPhi_ =
	dbe->book2D("RctRegionsLocalEtEtaPhi", "REGION E_{T} (Local)", 
		    nlocetabins, locetamin, locetamax,
		    nlocphibins, locphimin, locphimax);
    ctp7RegionsLocalOccEtaPhi_ =
	dbe->book2D("RctRegionsLocalOccEtaPhi", "REGION OCCUPANCY (Local)", 
		    nlocetabins, locetamin, locetamax,
		    nlocphibins, locphimin, locphimax);
    ctp7TauVetoLocalEtaPhi_ =
	dbe->book2D("RctTauLocalVetoEtaPhi", "TAU VETO OCCUPANCY (Local)",
		    nlocetabins, locetamin, locetamax,
		    nlocphibins, locphimin, locphimax);
*/
    // rank histos
    ctp7RegionRank_ =
	dbe->book1D("RctRegionRank", "REGION RANK", R10BINS, R10MIN,
		    R10MAX);
    ctp7IsoEmRank_ =
	dbe->book1D("RctEmIsoEmRank", "ISO EM RANK", R6BINS, R6MIN, R6MAX);
    ctp7NonIsoEmRank_ =
	dbe->book1D("RctEmNonIsoEmRank", "NON-ISO EM RANK", R6BINS, R6MIN,
		    R6MAX);
    // hw coordinates
//    ctp7EmCardRegion_ = dbe->book1D("ctp7EmCardRegion", "Em Card * Region",
//				   256, -127.5, 127.5);

    // bx histos
    ctp7RegionBx_ = dbe->book1D("RctRegionBx", "Region BX", 256, -0.5, 4095.5);
    ctp7EmBx_ = dbe->book1D("RctEmBx", "EM BX", 256, -0.5, 4095.5);
 
  }
}

void L1TCTP7::endJob(void)
{
  if (verbose_)
    std::cout << "L1TCTP7: end job...." << std::endl;
  LogInfo("EndJob") << "analyzed " << nev_ << " events";

  if (outputFile_.size() != 0 && dbe)
    dbe->save(outputFile_);

  return;
}

void L1TCTP7::analyze(const Event & e, const EventSetup & c)
{
  if (verbose_) {
    std::cout << "L1TCTP7: analyze...." << std::endl;
  }

  // filter according trigger type
  //  enum ExperimentType {
  //        Undefined          =  0,
  //        PhysicsTrigger     =  1,
  //        CalibrationTrigger =  2,
  //        RandomTrigger      =  3,
  //        Reserved           =  4,
  //        TracedEvent        =  5,
  //        TestTrigger        =  6,
  //        ErrorTrigger       = 15

  // fill a histogram with the trigger type, for normalization fill also last bin
  // ErrorTrigger + 1
  double triggerType = static_cast<double> (e.experimentType()) + 0.001;
  double triggerTypeLast = static_cast<double> (edm::EventAuxiliary::ExperimentType::ErrorTrigger)
                          + 0.001;
  triggerType_->Fill(triggerType);
  triggerType_->Fill(triggerTypeLast + 1);

  // filter only if trigger type is greater than 0, negative values disable filtering
  if (filterTriggerType_ >= 0) {

      // now filter, for real data only
      if (e.isRealData()) {
          if (!(e.experimentType() == filterTriggerType_)) {

              edm::LogInfo("L1TCTP7") << "\n Event of TriggerType "
                      << e.experimentType() << " rejected" << std::endl;
              return;

          }
      }

  }

  // Get the RCT digis
  edm::Handle < L1CaloEmCollection > em;
  edm::Handle < L1CaloRegionCollection > rgn;

  bool doEm = true;
  bool doHd = true;

  e.getByToken(ctp7Source_L1CRCollection_,rgn);
 
  if (!rgn.isValid()) {
    edm::LogInfo("DataNotFound") << "can't find L1CaloRegionCollection";
    doHd = false;
  }

  if ( doHd ) {
    // Fill the RCT histograms
    double nonzeroregions = 0; // this is divided later
    double nonzeroregions_barrel = 0; // this is divided later
    double nonzeroregions_hf = 0; // this is divided later
    double totalregionet = 0;
    unsigned int maxregionet = 0;
    unsigned int maxregionet_hf = 0;
    unsigned int maxregionet_barrel = 0;

    int regionbin=0;

    // Regions
    for (L1CaloRegionCollection::const_iterator ireg = rgn->begin();
	 ireg != rgn->end(); ireg++) {
      regionbin++;
      if(ireg->et()>0)
      {

      ctp7RegionsEtMapVsEvt_->Fill(nev_,regionbin,ireg->et());
      ctp7RegionsOccVsEvt_->Fill(nev_,regionbin);

      nonzeroregions++;
      totalregionet += ireg->et();
      if(ireg->gctEta()==10||ireg->gctEta()==11){
        nonzeroregions_barrel++;   
	if(ireg->et()>maxregionet_barrel) maxregionet_barrel=ireg->et();     
      }
      if(ireg->gctEta()<4||ireg->gctEta()>17){
        nonzeroregions_hf++;
        if(ireg->et()>maxregionet_hf) maxregionet_hf=ireg->et();
        ctp7RegionsHFPhiOccETVsEvt_->Fill(nev_,ireg->gctPhi(),ireg->et());

        if(ireg->gctEta()<4) ctp7RegionsHFPhiMinusOccETVsEvt_->Fill(nev_,ireg->gctPhi(),ireg->et());

        if(ireg->gctEta()>17) ctp7RegionsHFPhiPlusOccETVsEvt_->Fill(nev_,ireg->gctPhi(),ireg->et());

      }

      if(ireg->et()>maxregionet) maxregionet=ireg->et();        

      ctp7RegionRank_->Fill(ireg->et());
      if(ireg->et()>5){
	ctp7RegionsOccEtaPhi_->Fill(ireg->gctEta(), ireg->gctPhi());
      }
      ctp7RegionsEtEtaPhi_->Fill(ireg->gctEta(), ireg->gctPhi(), ireg->et());
      ctp7RegionBx_->Fill(ireg->bx());
      }


    if(ireg->overFlow())  ctp7OverFlowEtaPhi_ ->Fill(ireg->gctEta(), ireg->gctPhi());
    if(ireg->tauVeto())   ctp7TauVetoEtaPhi_  ->Fill(ireg->gctEta(), ireg->gctPhi());
    if(ireg->mip())       ctp7MipEtaPhi_      ->Fill(ireg->gctEta(), ireg->gctPhi());
    if(ireg->quiet())     ctp7QuietEtaPhi_    ->Fill(ireg->gctEta(), ireg->gctPhi());
    if(ireg->fineGrain()) ctp7HfPlusTauEtaPhi_->Fill(ireg->gctEta(), ireg->gctPhi()); 
   
      regionbin++;
 
    }//end region loop
    //
    ctp7RegionsAverageRegionEt_->Fill(totalregionet*1.0/NUMREGIONS);
    ctp7RegionsAvgEtVsEvt_->Fill(nev_,totalregionet*1.0/NUMREGIONS);
    ctp7RegionsMaxEtVsEvt_->Fill(nev_,maxregionet);
    ctp7RegionsMaxEtHFVsEvt_->Fill(nev_,maxregionet_hf);
    ctp7RegionsMaxEtBarrelVsEvt_->Fill(nev_,maxregionet_barrel);

    ctp7RegionsTotEtVsEvt_->Fill(nev_,totalregionet);
    ctp7RegionsTotalRegionEt_->Fill(totalregionet);

    ctp7RegionsNormNonZero_->Fill(nonzeroregions*1.0/PUMBINS);
    ctp7RegionsNormNonZeroVsEvt_->Fill(nev_,(nonzeroregions*1.0)/(1.0*PUMBINS));

    ctp7RegionsNonZero_->Fill(nonzeroregions);
    ctp7RegionsNonZeroVsEvt_->Fill(nev_,nonzeroregions);
    ctp7RegionsNonZeroBarrelVsEvt_->Fill(nev_,nonzeroregions_barrel);
    ctp7RegionsNonZeroHFVsEvt_->Fill(nev_,nonzeroregions_hf);

    // Can we move this to a independent function, and/or clean up the loop (Maria)
    //second region loop necessary because pum found in prior loop  
    for (L1CaloRegionCollection::const_iterator ireg = rgn->begin();
         ireg != rgn->end(); ireg++) {
          ctp7RegionsAvgEtVsEta_->Fill(ireg->gctEta(),ireg->et());
      if (ireg->gctEta()==0){
          ctp7RegionsPumEta0_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==1) {
          ctp7RegionsPumEta1_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==2){ 
          ctp7RegionsPumEta2_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==3){ 
          ctp7RegionsPumEta3_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==4){ 
          ctp7RegionsPumEta4_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==5){ 
          ctp7RegionsPumEta5_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==6){ 
          ctp7RegionsPumEta6_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==7){ 
          ctp7RegionsPumEta7_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==8){ 
          ctp7RegionsPumEta8_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==9){ 
          ctp7RegionsPumEta9_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==10){ 
          ctp7RegionsPumEta10_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==11){ 
          ctp7RegionsPumEta11_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==12){ 
          ctp7RegionsPumEta12_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==13){ 
          ctp7RegionsPumEta13_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==14){ 
          ctp7RegionsPumEta14_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==15){ 
          ctp7RegionsPumEta15_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==16){ 
          ctp7RegionsPumEta16_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==17){ 
          ctp7RegionsPumEta17_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==18){ 
          ctp7RegionsPumEta18_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==19){ 
          ctp7RegionsPumEta19_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==20){ 
          ctp7RegionsPumEta20_->Fill(nonzeroregions/PUMBINS,ireg->et());}
      else if (ireg->gctEta()==21){
          ctp7RegionsPumEta21_->Fill(nonzeroregions/PUMBINS,ireg->et());}
    }

  }//end doHd

  
  e.getByToken(ctp7Source_L1CEMCollection_,em);
  
  if (!em.isValid()) {
    edm::LogInfo("DataNotFound") << "can't find L1CaloEmCollection";
    doEm = false;
  }
  if ( ! doEm ) return;

    double nonzeroem = 0;
    double totalemet = 0;
    unsigned int maxemet = 0;

  // Isolated and non-isolated EM
  for (L1CaloEmCollection::const_iterator iem = em->begin();
       iem != em->end(); iem++) {
    
    if(iem->rank()>0) { nonzeroem++; totalemet+=iem->rank();}
    if(iem->rank()>maxemet) maxemet=iem->rank(); 

    if (iem->isolated()) {
      if(iem->rank()>0)
      {
      ctp7IsoEmRank_->Fill(iem->rank());
      ctp7IsoEmEtEtaPhi_->Fill(iem->regionId().ieta(),
			      iem->regionId().iphi(), iem->rank());
      ctp7EmAvgEtVsEta_->Fill(iem->regionId().ieta(),iem->rank());

      if(iem->rank()>10){
	ctp7IsoEmOccEtaPhi_->Fill(iem->regionId().ieta(),
				 iem->regionId().iphi());
      }
      ctp7EmBx_->Fill(iem->bx());
      }
    }
    else {
      if(iem->rank()>0)
      { 
      ctp7NonIsoEmRank_->Fill(iem->rank());
      ctp7NonIsoEmEtEtaPhi_->Fill(iem->regionId().ieta(),
				 iem->regionId().iphi(), iem->rank());
      if(iem->rank()>10){
	ctp7NonIsoEmOccEtaPhi_->Fill(iem->regionId().ieta(),
				    iem->regionId().iphi());
      }
      ctp7EmBx_->Fill(iem->bx());
      }
    }

  }

    ctp7EmAvgEtVsEvt_->Fill(nev_,totalemet/NUMREGIONS);
    ctp7EmMaxEtVsEvt_->Fill(nev_,maxemet);
    ctp7EmTotEtVsEvt_->Fill(nev_,totalemet);
    ctp7EmNonZeroVsEvt_->Fill(nev_,nonzeroem);


  nev_++;

}
