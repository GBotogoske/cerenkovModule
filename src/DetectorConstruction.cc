#include "DetectorConstruction.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"  

#include "G4Material.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4EllipticalTube.hh"
#include "G4ExtrudedSolid.hh"
#include "G4AssemblyVolume.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"
#include "G4VisAttributes.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4NistManager.hh"
#include <math.h>


//nota de coordadenas (x,y,z) --> (direcao da tela, direcao do campo, direcao transversal da tela)

// Option to switch on/off checking of volumes overlaps
G4bool checkOverlaps = true;

DetectorConstruction::DetectorConstruction():G4VUserDetectorConstruction()
{  
}

DetectorConstruction::~DetectorConstruction(){}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* DetectorConstruction::Construct()
{
    
    return nullptr;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
