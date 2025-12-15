#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
: G4UserEventAction() , fMuonEntered(false), fHCID(-1)
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void EventAction::BeginOfEventAction(const G4Event* event)
{
    clearPhotons();
    clearPhotonsAbs();
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
    am->FillNtupleDColumn(0,4, fMuonEnergy/CLHEP::MeV);
    am->FillNtupleDColumn(0,5, fNphotons);
    am->FillNtupleDColumn(0,6, fNphotonsAbsWater);
    int Pdetected = 0;
    if(HC->entries()>0)
    {
        Pdetected = (*HC)[0]->GetPhotonCounter();
    }
    am->FillNtupleDColumn(0,7, Pdetected);
    am->AddNtupleRow();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
