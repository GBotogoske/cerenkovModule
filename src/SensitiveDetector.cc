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


SensitiveDetector::SensitiveDetector(const G4String &SDname,const G4String &HitCollectionName,const G4String &SensorHitCollectionName)
  : G4VSensitiveDetector(SDname), fHitCollection(NULL), fP(0), fSensorHitCollection(NULL)
{
    G4cout<<"Creating SD with name: "<<SDname<<G4endl;
    collectionName.insert(HitCollectionName);
    collectionName.insert(SensorHitCollectionName);
}

SensitiveDetector::~SensitiveDetector(){}

void SensitiveDetector::Initialize(G4HCofThisEvent *HCE)
{
    ResetCounterStatus();
    fHitCollection = new HitCollection(GetName(),collectionName[0]);
    G4int HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); 
    HCE->AddHitsCollection(HCID, fHitCollection);

    fSensorHitCollection = new SensorHitCollection(GetName(),collectionName[1]);
    G4int HCID2 = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[1]); 
    HCE->AddHitsCollection(HCID2, fSensorHitCollection);
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
            const auto& sipm_spectrum = SiPMSpectrum::get();
            std::vector<G4double> eff;
            std::vector<G4double> E;
            double reflectance = sipm_spectrum.getReflectivy();
            int n;
                 
            eff = sipm_spectrum.get_effVIS();
            E = sipm_spectrum.get_EVIS();
            n = sipm_spectrum.getNVIS();
            G4double p;

            auto Ephoton = aStep->GetTrack()->GetTotalEnergy();
            if(Ephoton<=E[0])
            {
                p=eff[0];
            }
            else if(Ephoton>=E[n-1])
            {
                p=eff[n-1]; 
            }
            else
            {
                for (int i = 0; i < n - 1; ++i) 
                {
                    if (Ephoton >= E[i] && Ephoton <= E[i + 1])
                    {
                        G4double x0 = E[i];
                        G4double x1 = E[i + 1];
                        G4double y0 = eff[i];
                        G4double y1 = eff[i + 1];
                        p = y0 + (y1 - y0) * (Ephoton - x0) / (x1 - x0);
                    }
                }
            }
            G4double r = G4UniformRand();  
            if(r<p)
            {
                SensitiveDetector::SetCounterStatus(1);


                MySensorHit* thisHit = new MySensorHit();
                auto creation_process = aStep->GetTrack()->GetCreatorProcess()->GetProcessName();
                int thisprocess = -1;
                if(creation_process=="Cerenkov")
                {
                    thisprocess=0;
                }
                else if(creation_process=="OpWLS")
                {
                    thisprocess=1;
                }
                else
                {
                    thisprocess=-1;
                }
                thisHit->SetDetectorID(0); // alterar depois
                thisHit->SetPhotonType(thisprocess);
                thisHit->SetPhotonEnergy(Ephoton);
                
                fSensorHitCollection->insert(thisHit);
            }
            
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
    //G4cout << "Foram detectados: " << SensitiveDetector::GetCounterStatus() << std::endl;
}
