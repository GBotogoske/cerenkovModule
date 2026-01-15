#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "MyHit.hh"
#include "MySensorHit.hh"

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

    HitCollection *GetHitsCollection(G4int HCID,const G4Event* event) const;
    SensorHitCollection *GetSensorHitsCollection(G4int HCID,const G4Event* event) const;
    G4int fHCID;
    G4int fHCID2;
    int feventID;

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
     void clearPhotonsAbs ()
    { 
        this->fNphotonsAbsWater=0; 
        return; 
    } 
    void addPhotonsAbs(G4int add)
    {
        this->fNphotonsAbsWater+=add; 
        return; 
    }
    void addWLS(G4int add)
    {
        this->fNphotonsWLS+=add; 
        return; 
    }
    void clearPhotonsWLS ()
    { 
        this->fNphotonsWLS=0; 
        return; 
    } 

    void insertPhoton(double typePhoton, double energy);

  private:

    bool fMuonEntered = false;
    G4ThreeVector fEntryPos;
    G4double fMuonEnergy;
    G4int fNphotons = 0;
    G4int fNphotonsAbsWater = 0;
    G4int fNphotonsWLS = 0;
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
