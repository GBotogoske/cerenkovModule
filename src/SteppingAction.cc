
#include "SteppingAction.hh"
#include "RunAction.hh"

#include "G4Track.hh"
#include "G4EventManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4StepPoint.hh"
#include "G4AnalysisManager.hh"
#include "G4UnitsTable.hh"
#include "G4OpticalPhoton.hh"
#include "G4VProcess.hh"

SteppingAction::SteppingAction(EventAction* eventAction): fEventAction(eventAction) {}
SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step *aStep)
{
    auto track = aStep->GetTrack();

    // só múon primário
    const int pdg = track->GetDefinition()->GetPDGEncoding();
    if (track->GetParentID() != 0) return;
    if (std::abs(pdg) != 13) return;
    
    auto pre  = aStep->GetPreStepPoint();
    auto post = aStep->GetPostStepPoint();

    auto prePV  = pre->GetPhysicalVolume();
    auto postPV = post->GetPhysicalVolume();
    if (!prePV || !postPV) return;

    // crossing boundary

    if (post->GetStepStatus() == fGeomBoundary)
    {
        const auto preName  = prePV->GetName();
        const auto postName = postPV->GetName();

        // ajuste "WaterVolume" pro nome real do seu volume de água
        if (preName != "Water" && postName == "Water") 
        {
            // posição exatamente na entrada (no boundary)
            const G4ThreeVector p = post->GetPosition();
            G4double energy = pre->GetKineticEnergy();
            fEventAction->SetMuonEntry(p,energy);
        }
    }

    //contar o numero de photons cherenkov na agua
    if (prePV->GetName() == "Water")
    {
        G4int nC = 0;
        const auto* secs = aStep->GetSecondaryInCurrentStep();
        for (const auto* s : *secs) 
        {
            if (s->GetDefinition() != G4OpticalPhoton::Definition()) continue;
            auto* proc = s->GetCreatorProcess();
            if (proc && proc->GetProcessName() == "Cerenkov") nC++;
        }

        if (nC > 0) fEventAction->addPhotons(nC);
    } 
    
    
  
}