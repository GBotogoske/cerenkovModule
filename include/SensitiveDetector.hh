#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1

#include "DetectorConstruction.hh"
#include "G4VSensitiveDetector.hh"
#include "G4TouchableHistory.hh"
#include "G4OpBoundaryProcess.hh"
#include <vector>
#include "MyHit.hh"

class G4Step;
class G4HCofThisEvent;

class SensitiveDetector : public G4VSensitiveDetector
{
    public:
    
        SensitiveDetector(const G4String &SDname, const G4String &HitCollectionName);

        virtual ~SensitiveDetector();

        void Initialize(G4HCofThisEvent *HCE);
        G4bool ProcessHits(G4Step *step, G4TouchableHistory *ROhist);
        void EndOfEvent(G4HCofThisEvent *HCE);

        // // Photon Counter
        void SetCounterStatus(G4int p) {fP +=p;};
        void ResetCounterStatus() {fP = 0;};
        G4int GetCounterStatus() const {return fP;};

        // // // // // //
        void CleanSDMemory();
        void PrintSDMemoryStatus();

    private:
        HitCollection *fHitCollection;
        G4int   fP;

};

#endif
