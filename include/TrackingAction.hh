#pragma once
#include "G4UserTrackingAction.hh"

class EventAction;

class TrackingAction : public G4UserTrackingAction {
public:
  explicit TrackingAction(EventAction* ea) : fEventAction(ea) {}
  void PostUserTrackingAction(const G4Track* track) override;

private:
  EventAction* fEventAction = nullptr;
};
