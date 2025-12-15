#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
: G4UserEventAction() , fMuonEntered(false)
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void EventAction::BeginOfEventAction(const G4Event* event)
{
     
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
   if (!fMuonEntered) return;

    auto am = G4AnalysisManager::Instance();
    const int eventID = event->GetEventID();

    am->FillNtupleIColumn(0,0, eventID);
    am->FillNtupleDColumn(0,1, fEntryPos.x()/CLHEP::cm);
    am->FillNtupleDColumn(0,2, fEntryPos.y()/CLHEP::cm);
    am->FillNtupleDColumn(0,3, fEntryPos.z()/CLHEP::cm);
    am->FillNtupleDColumn(0,4, fMuonEnergy/CLHEP::MeV);
    am->FillNtupleDColumn(0,5, fNphotons);
    am->AddNtupleRow();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
