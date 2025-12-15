#pragma once

#include "G4UserSteppingAction.hh"
#include "EventAction.hh"

class SteppingAction : public G4UserSteppingAction
{

public:
    explicit SteppingAction(EventAction* eventAction);
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step *);

private:
    EventAction* fEventAction = nullptr;

};