//GammaBari G4Week - By Corrado Altomare & Davide Serini :D
//Ex2-Physics By Corrado
//

#ifndef OneHit_h
#define OneHit_h 1

//Include Native G4 Classes
#include "G4VHit.hh"
#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"
#include "G4UnitsTable.hh"
#include "G4Track.hh"

class MyHit : public G4VHit
{
  public:

    MyHit();
    virtual ~MyHit();

    // methods from base class

    void SetPhotonCounter (G4int p) {fPhotonCounter = p;};
    G4int GetPhotonCounter() const { return fPhotonCounter;};

    void SetDetectorID(G4int id) {fDetectorID = id;};
    G4int GetDetectorID() const { return fDetectorID;};


  private:
 
    G4int fPhotonCounter;
    G4int fDetectorID;

  };

typedef G4THitsCollection<MyHit> HitCollection;


#endif
