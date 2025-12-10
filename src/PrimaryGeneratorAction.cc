#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4VPhysicalVolume.hh"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fGPS(0),fParticleGun(0)
{
    //fGPS  = new G4GeneralParticleSource();   
    fParticleGun = new G4ParticleGun(1);  
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    //delete fGPS;
    delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    G4ParticleDefinition* mu = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
    fParticleGun->SetParticleDefinition(mu);
    fParticleGun->SetParticleEnergy(4*GeV);

    G4ThreeVector pos;
    pos = G4ThreeVector(0, 0, 0.5*m);
    fParticleGun->SetParticlePosition(pos);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,-1));

    fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
