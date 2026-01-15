#include "TrackingAction.hh"
#include "EventAction.hh"

#include "G4OpticalPhoton.hh"
#include "G4VProcess.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"


void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
    if (!fEventAction) return;

    if (track->GetDefinition() != G4OpticalPhoton::Definition()) return;

    auto* creator = track->GetCreatorProcess();
    if (creator && creator->GetProcessName() == "OpWLS") 
    {
        fEventAction->addWLS(1);
        fEventAction->insertPhoton(1,track->GetTotalEnergy());
    }
    if (creator && creator->GetProcessName() == "Cerenkov") 
    {
        fEventAction->insertPhoton(0,track->GetTotalEnergy());
    }
}


void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
    if (!fEventAction) return;

    if (track->GetDefinition() != G4OpticalPhoton::Definition()) return;

    auto* creator = track->GetCreatorProcess();
    if (!creator || (creator->GetProcessName() != "Cerenkov" && creator->GetProcessName() != "OpWLS")) return;

    const auto* step = track->GetStep();
    if (!step) return;

    const auto* post = step->GetPostStepPoint();
    const auto* proc = post ? post->GetProcessDefinedStep() : nullptr;
    if (!proc || proc->GetProcessName() != "OpAbsorption") return;

    const auto* pre = step->GetPreStepPoint();
    const auto* prePV = pre ? pre->GetPhysicalVolume() : nullptr;
    if (!prePV || prePV->GetName() != "Water") return;  

    fEventAction->addPhotonsAbs(1);

}
