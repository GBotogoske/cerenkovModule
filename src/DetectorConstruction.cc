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

#include "SensitiveDetector.hh"

#include "utils.hh"


// Option to switch on/off checking of volumes overlaps
G4bool checkOverlaps = true;

DetectorConstruction::DetectorConstruction():G4VUserDetectorConstruction()
{  
}

DetectorConstruction::~DetectorConstruction(){}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4NistManager *fNistManager = G4NistManager::Instance();
    fNistManager->SetVerbose(0);

    //MUNDO --> um bloco de ar 
    G4double world_sizeX = 1*m;
    G4double world_sizeY = 1*m;
    G4double world_sizeZ = 1*m;
    G4Material* air_mat = fNistManager->FindOrBuildMaterial("G4_AIR");
    auto mpt_air = new G4MaterialPropertiesTable();
    mpt_air->AddProperty("RINDEX", {1*eV,10*eV}, {1.00,1.00}, 2);
    air_mat->SetMaterialPropertiesTable(mpt_air);

    G4Box* solidWorld = new G4Box("World",0.5*world_sizeX, 0.5*world_sizeY, 0.5*world_sizeZ);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,air_mat,"World");
    G4VPhysicalVolume* physicalWorld = new G4PVPlacement(0,G4ThreeVector(),logicWorld,"World",0,false,0,checkOverlaps);

    //caixa de plastico
    G4double module_sizeX = 0.2*m;
    G4double module_sizeY = 0.2*m;
    G4double module_sizeZ = 0.2*m;
    G4double module_thick = 0.1*mm;
    G4Material* PVC_mat = fNistManager->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE");
    auto mpt_PVC = new G4MaterialPropertiesTable();
    mpt_PVC->AddProperty("RINDEX", {1*eV,10*eV}, {1.54,1.54}, 2);
    mpt_PVC->AddProperty("ABSLENGTH", {1*eV,10*eV}, {0.001*mm, 0.001*mm}, 2);
    PVC_mat->SetMaterialPropertiesTable(mpt_PVC);

    auto solidModule = new G4Box("ModuleE",0.5*module_sizeX, 0.5*module_sizeY, 0.5*module_sizeZ);
    G4LogicalVolume* logicModule = new G4LogicalVolume(solidModule,PVC_mat,"Module");
    G4VPhysicalVolume* physicalModule = new G4PVPlacement(0,G4ThreeVector(),logicModule,"Module",logicWorld,false,0,checkOverlaps); 

    //CASCA INTERNA DE PEN

    int n_pen = 3;
    double density_PEN = 1.3 *g/cm3;
    G4Material* PEN_mat = new G4Material("My_PEN",density_PEN,n_pen);
    G4Element* C = fNistManager->FindOrBuildElement("C");
    G4Element* H = fNistManager->FindOrBuildElement("H");
    G4Element* O = fNistManager->FindOrBuildElement("O");
    PEN_mat->AddElement(C,14);
    PEN_mat->AddElement(H,10);
    PEN_mat->AddElement(O,4);

    std::string file_PEN_ABS="../configuration/pen_absorption.json";
    std::vector<double> PEN_E_abs, PEN_abs;
    getValues(&PEN_E_abs,&PEN_abs,file_PEN_ABS,eV,mm,"E","l");
    std::string file_PEN_WLS="../configuration/pen_emissium.json";
    std::vector<double> PEN_E_WLS, PEN_WLS;
    getValues(&PEN_E_WLS,&PEN_WLS,file_PEN_WLS,eV,1,"E","l");

    auto mpt_PEN = new G4MaterialPropertiesTable();
    mpt_PEN->AddProperty("RINDEX",{0.1*eV,15*eV},{1.65,1.65} , 2);
    mpt_PEN->AddProperty("WLSABSLENGTH", PEN_E_abs, PEN_abs,PEN_E_abs.size());
    mpt_PEN->AddProperty("WLSCOMPONENT", PEN_E_WLS, PEN_WLS, PEN_E_WLS.size());
    mpt_PEN->AddProperty("RAYLEIGH", {0.1*eV,15*eV} , {150*um, 150*um}, 2);
    mpt_PEN->AddConstProperty("WLSTIMECONSTANT",25.0*ns);
    mpt_PEN->AddConstProperty("WLSMEANNUMBERPHOTONS",0.4);
    PEN_mat->SetMaterialPropertiesTable(mpt_PEN);
    PEN_mat->GetIonisation()->SetBirksConstant(0.015*cm/MeV);
    PEN_mat->GetIonisation()->SetMeanExcitationEnergy(64.7*eV);

    G4double PEN_thick = 0.01*cm;
    G4double PEN_sizeX = module_sizeX-2*module_thick;
    G4double PEN_sizeY = module_sizeY-2*module_thick;
    G4double PEN_sizeZ = module_sizeZ-2*module_thick;
    auto solidPEN = new G4Box("PEN",0.5*PEN_sizeX, 0.5*PEN_sizeY, 0.5*PEN_sizeZ);
    G4LogicalVolume* logicPEN = new G4LogicalVolume(solidPEN,PEN_mat,"PEN");
    G4VPhysicalVolume* physicalPEN = new G4PVPlacement(0,G4ThreeVector(),logicPEN,"PEN",logicModule,false,0,checkOverlaps); 

    //DEPOIS DEFINIR AS OPTICAL SURFACE ENTRE PEN E TEFLON ( simular tyvek e vikuiti )

    //VOLUME DE AGUA
    
    G4Material* H20_mat = fNistManager->FindOrBuildMaterial("G4_WATER");
    auto mpt_H20 = new G4MaterialPropertiesTable();
    mpt_H20->AddProperty("RINDEX", {1*eV,10*eV}, {1.33,1.33}, 2);
    mpt_H20->AddProperty("ABSLENGTH", {1*eV,10*eV}, {10*m, 10*m}, 2);
    H20_mat->SetMaterialPropertiesTable(mpt_H20);

    G4double water_sizeX = PEN_sizeX-2*PEN_thick;
    G4double water_sizeY = PEN_sizeY-2*PEN_thick;
    G4double water_sizeZ = PEN_sizeZ-2*PEN_thick;
    auto solidWater = new G4Box("Water",0.5*water_sizeX, 0.5*water_sizeY, 0.5*water_sizeZ);
    G4LogicalVolume* logicWater = new G4LogicalVolume(solidWater,H20_mat,"Water");
    G4VPhysicalVolume* physicalWater = new G4PVPlacement(0,G4ThreeVector(),logicWater,"Water",logicPEN,false,0,checkOverlaps); 

    //Sensor Otico
    G4Material* Si_mat = fNistManager->FindOrBuildMaterial("G4_Si");
    G4MaterialPropertiesTable* mpt_Si = new G4MaterialPropertiesTable();
    mpt_Si->AddProperty("RINDEX",{1*eV,10*eV},{1.5,1.5},2);
    mpt_Si->AddProperty("ABSLENGTH",{1*eV,10*eV},{0.0001*mm,0.0001*mm},2);
    Si_mat->SetMaterialPropertiesTable(mpt_Si);

    double sensor_thick = 0.8*mm;
    double sensor_side = 4*cm;
    double sensor_position = water_sizeY/2-sensor_thick/2;
    auto solidSensor = new G4Box("Sensor",0.5*sensor_thick, 0.5*sensor_side, 0.5*sensor_side);
    G4LogicalVolume* logicSensor = new G4LogicalVolume(solidSensor,Si_mat,"Sensor");
    G4VPhysicalVolume* physicalSensor = new G4PVPlacement(0,G4ThreeVector(sensor_position,0,0),logicSensor,"Sensor",logicWater,false,0,checkOverlaps); 

    G4VisAttributes* visSi = new G4VisAttributes(G4Colour(0.0, 0.8, 0.05)); 
    visSi->SetForceSolid(true); 
    logicSensor->SetVisAttributes(visSi);


    //Escrever a logica do sensor
    G4SDManager *SD_manager = G4SDManager::GetSDMpointer();
    G4String SDModuleName = "/SensitiveDetector";
    if(SD_manager->FindSensitiveDetector(SDModuleName,true))
    {
        delete(SD_manager->FindSensitiveDetector(SDModuleName,true));
    }
    SensitiveDetector *sensitiveModule = new SensitiveDetector(SDModuleName,"HitCollection");
    SD_manager->AddNewDetector(sensitiveModule);
    logicSensor->SetSensitiveDetector(sensitiveModule);
   



    return physicalWorld;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
