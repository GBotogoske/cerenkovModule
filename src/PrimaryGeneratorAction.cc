#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include <cmath>

// --- helpers -------------------------------------------------------------

static G4double SamplePowerLaw(G4double Emin, G4double Emax, G4double gamma)
{
    // sample from pdf ~ E^{-gamma} on [Emin, Emax]
    const G4double u = G4UniformRand();

    if (std::abs(gamma - 1.0) < 1e-12) {
        // special case: pdf ~ 1/E
        return Emin * std::pow(Emax / Emin, u);
    }

    const G4double a = std::pow(Emin, 1.0 - gamma);
    const G4double b = std::pow(Emax, 1.0 - gamma);
    return std::pow(a + u * (b - a), 1.0 / (1.0 - gamma));
}

static G4double SampleThetaCosN(G4double n, G4double thetaMax)
{
    // If intensity per solid angle I(θ) ~ cos^n θ, then p(cosθ) ~ (cosθ)^n.
    // Sample cosθ in [cos(thetaMax), 1].
    const G4double u  = G4UniformRand();
    const G4double x0 = std::cos(thetaMax);

    const G4double x0n1 = std::pow(x0, n + 1.0);
    const G4double x    = std::pow(x0n1 + u * (1.0 - x0n1), 1.0 / (n + 1.0));
    return std::acos(x);
}

static inline double CosThetaStar(double cosTheta)
    {
    cosTheta = std::clamp(cosTheta, 0.0, 1.0);

    // Table 1 (Guan et al.)
    const double P1 = 0.102573;
    const double P2 = -0.068287;
    const double P3 = 0.958633;
    const double P4 = 0.0407253;
    const double P5 = 0.817285;

    const double num = cosTheta*cosTheta
                        + P1*P1
                        + P2*std::pow(cosTheta, P3)
                        + P4*std::pow(cosTheta, P5);

    const double den = 1.0 + P1*P1 + P2 + P4;

    const double cs = std::sqrt(std::max(0.0, num/den));
    return std::clamp(cs, 0.0, 1.0);
}

static inline double ModifiedGaisser(double EGeV, double cosTheta)
{
    // dI/dE in (cm^-2 s^-1 sr^-1 GeV^-1), mas pra accept-reject a normalização não importa.
    const double cs = CosThetaStar(cosTheta);
    if (cs <= 0.0) return 0.0;

    const double a = 3.64;  // GeV
    const double b = 1.29;

    // Eq.(3): 0.14 * [ E/GeV * (1 + a/(E*(cos*)^b)) ]^-2.7 * [ ... ] :contentReference[oaicite:1]{index=1}
    const double termE = EGeV * (1.0 + a / (EGeV * std::pow(cs, b)));
    const double spec  = std::pow(termE, -2.7);

    const double pion = 1.0 / (1.0 + 1.1 * EGeV * cs / 115.0);
    const double kaon = 0.054 / (1.0 + 1.1 * EGeV * cs / 850.0);

    return 0.14 * spec * (pion + kaon);
}

static inline double EstimateMaxRatio(double EminGeV, double EmaxGeV, double cosMin, double nCos)
{
    double M = 0.0;
    const int NE = 120, NC = 120;

    for (int i = 0; i < NE; ++i) {
        const double t  = double(i) / double(NE - 1);
        const double E  = EminGeV * std::pow(EmaxGeV / EminGeV, t); // log-grid

        for (int j = 0; j < NC; ++j) {
            const double u = double(j) / double(NC - 1);
            const double c = cosMin + (1.0 - cosMin) * u;

            const double f = ModifiedGaisser(E, c);
            const double g = std::pow(E, -2.7) * std::pow(c, nCos); // proposta (sem normalização)

            if (g > 0.0) M = std::max(M, f / g);
        }
    }
    return 1.2 * M; // margem de segurança
}

G4ThreeVector calcPosIniticial(G4ThreeVector pos, G4ThreeVector dir, G4double zmax, G4double xmax, G4double xmin,G4double ymax, G4double ymin )
{
    double x = pos.x();
    double y = pos.y();
    double z = pos.z();
    double dirx = dir.x();
    double diry = dir.y();
    double dirz = dir.z();

    double lambda;
    if (dirz!=0.0)
    {
        lambda = (zmax-z)/dirz;
    }
    else
    {
        if(diry!=0.0)
        {
            lambda = (ymax-y)/diry;
        }
        else
        {
            lambda = (xmax-x)/dirx;
        }
    }
    
    double newx = x + lambda*dirx;
    double newy = y + lambda*diry;
    double newz = z + lambda*dirz;

    return G4ThreeVector(newx,newy,newz);
}


// ------------------------------------------------------------------------

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fGPS(0), fParticleGun(0)
{
    fParticleGun = new G4ParticleGun(1);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}




void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    //half_world
    const G4double halfXW  = 0.5 * m/2;       // cobre seu detector
    const G4double halfYW  = 0.5 * m/2;
    const G4double halfZW   = 0.5 * m/2;       // acima do volume (ajuste)

    // volume do detector
    const G4double halfX  = 0.2 * m/2;       // cobre seu detector
    const G4double halfY  = 0.2 * m/2;
    const G4double halfZ   = 0.2 * m/2;       // acima do volume (ajuste)

    // --------------------------------------------------------------------

    // opcional: mistura mu+/mu- (fração mu+ ~ 0.56 para razão ~1.3)
    const G4double muPlusFrac = 0.56;
    G4ParticleDefinition* mu =
        (G4UniformRand() < muPlusFrac)
        ? G4ParticleTable::GetParticleTable()->FindParticle("mu+")
        : G4ParticleTable::GetParticleTable()->FindParticle("mu-");

    const double EminGeV = 0.2;     // você decide (agora dá pra ir <1 GeV com essa parametrização) :contentReference[oaicite:3]{index=3}
    const double EmaxGeV = 500.0;
    const double gamma   = 2.7;

    const double nCos  = 2.0;
    const double thMax = 80.0 * deg;

    const double cosMin = std::cos(thMax);

    // precomputar M uma vez
    static double M = -1.0;
    if (M < 0.0) M = EstimateMaxRatio(EminGeV, EmaxGeV, cosMin, nCos);

    // accept-reject
    double EGeV = 0.0;
    double theta = 0.0;

    while (true)
    {
        EGeV  = SamplePowerLaw(EminGeV, EmaxGeV, gamma);  // agora em GeV (sem unidades)
        theta = SampleThetaCosN(nCos, thMax);
        const double c = std::cos(theta);

        const double f = ModifiedGaisser(EGeV, c);
        const double g = std::pow(EGeV, -gamma) * std::pow(c, nCos);

        const double acc = (g > 0.0) ? (f / (M * g)) : 0.0;
        if (G4UniformRand() < acc) break;
    }

    const G4double E = EGeV * GeV;

    const G4double st = std::sin(theta);
    const G4double ct = std::cos(theta);
    const G4double phi   = 2.0 * CLHEP::pi * G4UniformRand();

    // vindo de cima para baixo -> componente z negativa
    const G4ThreeVector dir(st * std::cos(phi), st * std::sin(phi), -ct);

    // posição no plano superior
    const G4double x = (2.0 * G4UniformRand() - 1.0) * halfX;
    const G4double y = (2.0 * G4UniformRand() - 1.0) * halfY;
    const G4double z = (2.0 * G4UniformRand() - 1.0) * halfZ;
    const G4ThreeVector pos(x, y, z);

    fParticleGun->SetParticleDefinition(mu);
    fParticleGun->SetParticleEnergy(E);
    fParticleGun->SetParticlePosition(calcPosIniticial(pos,dir,halfZW,halfXW,-halfXW,halfYW,-halfYW));
    fParticleGun->SetParticleMomentumDirection(dir);

    fParticleGun->GeneratePrimaryVertex(anEvent);
}
