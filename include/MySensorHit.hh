//GammaBari G4Week - By Corrado Altomare & Davide Serini :D
//Ex2-Physics By Corrado
//

#ifndef SensorHit_h
#define SensorHit_h 1

//Include Native G4 Classes
#include "G4VHit.hh"
#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"
#include "G4UnitsTable.hh"
#include "G4Track.hh"

class MySensorHit : public G4VHit
{
  public:

    MySensorHit();
    virtual ~MySensorHit();

    // methods from base class

    void SetPhotonType (G4int p) {fPhotonType = p;};
    G4int GetPhotonType() const { return fPhotonType;};

    void SetDetectorID(G4int id) {fDetectorID = id;};
    G4int GetDetectorID() const { return fDetectorID;};

    void SetPhotonEnergy (G4double p) {fPhotonEnergy = p;};
    G4double GetPhotonEnergy() const { return fPhotonEnergy;};


  private:
 

    G4int fPhotonType;
    G4double fPhotonEnergy;
    G4int fDetectorID;

  };

typedef G4THitsCollection<MySensorHit> SensorHitCollection;


#endif
