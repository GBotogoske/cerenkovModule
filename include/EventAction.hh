#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

/// Event action class
///

class EventAction : public G4UserEventAction
{
  public:
    EventAction();
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void SetMuonEntry(const G4ThreeVector& p, G4double energy); 
    bool MuonEntered() const { return fMuonEntered; }
    G4ThreeVector EntryPos() const { return fEntryPos; }
    void clearPhotons ()
    { 
        this->fNphotons=0; 
        return; 
    } 
    void addPhotons(G4int add)
    {
        this->fNphotons+=add; 
        return; 
    }

  private:

    bool fMuonEntered = false;
    G4ThreeVector fEntryPos;
    G4double fMuonEnergy;
    G4int fNphotons = 0;
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
