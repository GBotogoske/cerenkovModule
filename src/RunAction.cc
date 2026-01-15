#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeneralParticleSource.hh"
#include "G4AnalysisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(): G4UserRunAction(), fOutputFileName("/home/gabriel/Documents/cherenkov-module/data/Data") //, fPenCount(0)
{
    G4RunManager::GetRunManager()->SetPrintProgress(1);
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    G4cout << "Using " << analysisManager->GetType() << G4endl;
    analysisManager->SetNtupleMerging(true);
    analysisManager->CreateNtuple("Primary_Muon", "Primary_Muon");
    analysisManager->CreateNtupleIColumn(0,"eventID");
    analysisManager->CreateNtupleDColumn(0,"X");
    analysisManager->CreateNtupleDColumn(0,"Y");
    analysisManager->CreateNtupleDColumn(0,"Z");
    analysisManager->CreateNtupleDColumn(0,"KEnergy");
    analysisManager->CreateNtupleIColumn(0,"NPhotons");
    analysisManager->CreateNtupleIColumn(0,"NPhotonsWater");
    analysisManager->CreateNtupleIColumn(0,"NPhotonsWLS");
    analysisManager->CreateNtupleIColumn(0,"NPhotonsDetected");
    analysisManager->FinishNtuple(0);

    analysisManager->CreateNtuple("Photon", "Photon");
    analysisManager->CreateNtupleIColumn(1,"eventID");
    analysisManager->CreateNtupleIColumn(1,"photonType"); // 0 -cherenkov, 1-wls
    analysisManager->CreateNtupleDColumn(1,"energy");
    analysisManager->FinishNtuple(1);

    analysisManager->CreateNtuple("PhotonDetected", "PhotonDetected");
    analysisManager->CreateNtupleIColumn(2,"eventID");
    analysisManager->CreateNtupleIColumn(2,"photonType"); // 0 -cherenkov, 1-wls
    analysisManager->CreateNtupleDColumn(2,"energy");
    analysisManager->CreateNtupleIColumn(2,"detectorID");
    analysisManager->FinishNtuple(2);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
    delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*)
{
    G4String fileName   = fOutputFileName;
    G4String fileOutput = fileName;
    fileOutput += ".root";
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->OpenFile(fileOutput);    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run*)
{
    auto analysisManager = G4AnalysisManager::Instance();
    // save histograms & ntuple
    G4cout<<"[INFO]: Write the output file "<<fOutputFileName<<".root"<<G4endl;
    analysisManager->Write();
    analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
