#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"

#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
    // Detect interactive mode (if no arguments) and define UI session
    //
    G4UIExecutive* ui = 0;
    if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
    }

    // Choose the Random engine
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    G4long seed = time(nullptr);    
    G4Random::setTheSeed(seed);

    // Construct the default run manager
    //

    G4RunManager* runManager = new G4RunManager;


    // Set mandatory initialization classes
    //
    // Detector construction
    runManager->SetUserInitialization(new DetectorConstruction());

    // Physics list

    //auto physicsList = new PhysicsList;
    G4VModularPhysicsList* physicsList = new FTFP_BERT;
    physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
    auto opticalPhysics = new G4OpticalPhysics();
    physicsList->RegisterPhysics(opticalPhysics);
    runManager->SetUserInitialization(physicsList);  
    // User action initialization
    runManager->SetUserInitialization(new ActionInitialization());

    // Initialize visualization
    //
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // Process macro or start UI session
    //
    if ( ! ui ) { 
        // batch mode
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command+fileName);
    }
    else 
    { 
        // interactive mode
        UImanager->ApplyCommand("/control/execute macro/init_vis.mac");
        //UImanager->ApplyCommand("/control/execute gui.mac");
        //UImanager->ApplyCommand("/vis/open OGL 600x600-0+0");
        ui->SessionStart();
        delete ui;
    }

    delete visManager;
    delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
