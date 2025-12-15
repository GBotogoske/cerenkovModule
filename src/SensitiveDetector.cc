#include "SensitiveDetector.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4TouchableHistoryHandle.hh"
#include "G4TouchableHistory.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4Tubs.hh"
#include "G4String.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4GeometryTolerance.hh"
#include "TrackingAction.hh"
#include <math.h>

using namespace std;


SensitiveDetector::SensitiveDetector(const G4String &SDname,const G4String &HitCollectionName)
  : G4VSensitiveDetector(SDname), fHitCollection(NULL), fP(0)
{
    G4cout<<"Creating SD with name: "<<SDname<<G4endl;
    collectionName.insert(HitCollectionName);
}

SensitiveDetector::~SensitiveDetector(){}

void SensitiveDetector::Initialize(G4HCofThisEvent *HCE)
{
    ResetCounterStatus();
    fHitCollection = new HitCollection(GetName(),collectionName[0]);
    G4int HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); 
    HCE->AddHitsCollection(HCID, fHitCollection);
}


G4bool SensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *)
{

    G4String thisParticle = aStep->GetTrack()->GetParticleDefinition()->GetParticleName();
    G4String thisVolume   = aStep->GetTrack()->GetVolume()->GetName();

    if(thisParticle == G4OpticalPhoton::Definition()->GetParticleName())
    {
        auto* proc = aStep->GetPostStepPoint()->GetProcessDefinedStep();
        if (proc && proc->GetProcessName() == "OpAbsorption")
        {
            SensitiveDetector::SetCounterStatus(1);
        }
    }
    
    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
    //Fill the hits
    MyHit *aHit = new MyHit();
    G4int TotP = SensitiveDetector::GetCounterStatus();
  
    aHit->SetPhotonCounter(TotP);
    fHitCollection->insert(aHit);

    SensitiveDetector::PrintSDMemoryStatus();
    SensitiveDetector::CleanSDMemory();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SensitiveDetector::CleanSDMemory()
{
    SensitiveDetector::ResetCounterStatus();
}

void SensitiveDetector::PrintSDMemoryStatus()
{
    G4cout << "Foram detectados: " << SensitiveDetector::GetCounterStatus() << std::endl;
}
