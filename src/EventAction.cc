#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
: G4UserEventAction() , fMuonEntered(false), fHCID(-1), fHCID2(-1)
{
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{
    
}

void EventAction::SetMuonEntry(const G4ThreeVector &p, G4double energy)
{
    {
        if (!fMuonEntered) 
        { 
            fMuonEntered = true; 
            fEntryPos = p; 
            fMuonEnergy = energy;
        }
    }
}

HitCollection *EventAction::GetHitsCollection(G4int HCID, const G4Event *event) const
{
    auto hitsCollection = static_cast<HitCollection*>(event->GetHCofThisEvent()->GetHC(HCID));
    if ( ! hitsCollection ) {
        G4ExceptionDescription msg;
        msg << "Cannot access hitsCollection ID " << HCID;
        G4Exception("B4cEventAction::GetHitsCollection()",
        "MyCode0003", FatalException, msg);
    }
    return hitsCollection;
}

SensorHitCollection *EventAction::GetSensorHitsCollection(G4int HCID, const G4Event *event) const
{
    auto hitsCollection = static_cast<SensorHitCollection*>(event->GetHCofThisEvent()->GetHC(HCID));
    if ( ! hitsCollection ) {
        G4ExceptionDescription msg;
        msg << "Cannot access SensorhitsCollection ID " << HCID;
        G4Exception("B4cEventAction::GetHitsCollection()",
        "MyCode0003", FatalException, msg);
    }
    return hitsCollection;
}

void EventAction::insertPhoton(double typePhoton, double energy)
{
        const int eventID = feventID;
        auto am = G4AnalysisManager::Instance();
        am->FillNtupleIColumn(1,0, eventID);
        am->FillNtupleIColumn(1,1, typePhoton);
        am->FillNtupleDColumn(1,2, energy/CLHEP::MeV);
        am->AddNtupleRow(1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void EventAction::BeginOfEventAction(const G4Event* event)
{
    clearPhotons();
    clearPhotonsAbs();
    clearPhotonsWLS();
    fMuonEntered = false;
    feventID=event->GetEventID();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
   if (!fMuonEntered) return;

    auto am = G4AnalysisManager::Instance();
    if ( fHCID == -1 ) 
    {
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID("HitCollection");
    }

    const int eventID = event->GetEventID();
    auto HC = GetHitsCollection(fHCID, event);

    am->FillNtupleIColumn(0,0, eventID);
    am->FillNtupleDColumn(0,1, fEntryPos.x()/CLHEP::cm);
    am->FillNtupleDColumn(0,2, fEntryPos.y()/CLHEP::cm);
    am->FillNtupleDColumn(0,3, fEntryPos.z()/CLHEP::cm);
    am->FillNtupleDColumn(0,4, fMuonEnergy/CLHEP::MeV); // in Mev
    am->FillNtupleIColumn(0,5, fNphotons);
    am->FillNtupleIColumn(0,6, fNphotonsAbsWater);
    am->FillNtupleIColumn(0,7, fNphotonsWLS);

    int Pdetected = 0;
    if(HC->entries()>0)
    {
        Pdetected = (*HC)[0]->GetPhotonCounter();
    }
    am->FillNtupleIColumn(0,8, Pdetected);
    am->AddNtupleRow(0);
   
    if ( fHCID2 == -1 ) 
    {
        fHCID2 = G4SDManager::GetSDMpointer()->GetCollectionID("SensorHitCollection");
    }
    auto HC2 = GetSensorHitsCollection(fHCID2, event);
    int n2 = HC2->entries();
    for(int i =0 ; i<n2 ; i++)
    {
        am->FillNtupleIColumn(2,0, eventID);
        am->FillNtupleIColumn(2,1, (*HC2)[i]->GetPhotonType());
        am->FillNtupleDColumn(2,2, (*HC2)[i]->GetPhotonEnergy()/CLHEP::MeV);
        am->FillNtupleIColumn(2,3, (*HC2)[i]->GetDetectorID());
        am->AddNtupleRow(2);
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
