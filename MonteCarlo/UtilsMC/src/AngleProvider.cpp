#include "AngleProvider.h"
#include "TMath.h"
#include "Math/SpecFuncMathMore.h"
#include "TF1.h"
#include "Math/Math.h"


AngleProviderCosIso::AngleProviderCosIso() {
    paramVals["minCos"] = -1;
    paramVals["maxCos"] = 1;
}

double AngleProviderCosIso::GetAngle() {
    auto aCosMin = paramVals["minCos"];
    auto aCosMax = paramVals["maxCos"];
    auto cos = randGen->Uniform(aCosMin, aCosMax);
    return TMath::ACos(cos);
}

void AngleProviderCosIso::ValidateParamValues() {
    auto min = paramVals["minCos"];
    auto max = paramVals["maxCos"];
    if (min < -1)
        throw std::invalid_argument(
                "AngleProviderCosIso::AngleProviderCosIso: min cos is smaller than -1");
    if (max > 1)
        throw std::invalid_argument(
                "AngleProviderCosIso::AngleProviderCosIso: max cos is larger than 1");
    if (min > max)
        throw std::invalid_argument(
                "AngleProviderCosIso::AngleProviderCosIso: min cos is larger than max cos!");
}

AngleProviderE1E2::AngleProviderE1E2() {
    paramVals["sigmaE1"] = 1;
    paramVals["sigmaE2"] = 0;
    paramVals["phaseE1E2"] = ROOT::Math::Pi() / 2.;
    paramVals["phaseCosSign"] = 1;
    std::string fName = "AngleProviderE1E2TF1_" + std::to_string(nInstances++);
    thetaEmissionTF1 = std::make_unique<TF1>(
            fName.c_str(), this, &AngleProviderE1E2::Theta,
            0., ROOT::Math::Pi(), 0);
}

// 1D angular distribution to be used with TF1::GetRandom
// Non-isotropic angular distribution of alpha-particle emission angle wrt gamma beam
// with E1 abd E2 components for Oxygen-16 photo disintegration reaction.
// Ref: M.Assuncao et al., PRC 73 055801 (2006).

double AngleProviderE1E2::Theta(double *x, double *par) {
    auto norm = 1.;
    auto s1 = paramVals["sigmaE1"];
    auto s2 = paramVals["sigmaE2"];
    auto ph12 = paramVals["phaseE1E2"];
    auto sgn = paramVals["phaseCosSign"];
    double c = cos(x[0]);
    double L[5];
    for (auto i = 0; i <= 4; i++) L[i] = ROOT::Math::legendre(i, c);
    auto WE1 = L[0] - L[2];
    auto WE2 = L[0] + (5. / 7.) * L[2] - (12. / 7.) * L[4];
    auto W12 = 6. / sqrt(5.0) * (L[1] - L[3]);
    return norm * (s1 * WE1 + s2 * WE2 + sgn * sqrt(s1 * s2) * cos(ph12) * W12);

}

void AngleProviderE1E2::ValidateParamValues() {
    if (paramVals["sigmaE1"] < 0)
        throw std::invalid_argument("AngleProviderE1E2::ValidateParamValues: sigmaE1 is smaller than 0!");
    if (paramVals["sigmaE2"] < 0)
        throw std::invalid_argument("AngleProviderE1E2::ValidateParamValues: sigmaE2 is smaller than 0!");
}

double AngleProviderE1E2::GetAngle() {
    return thetaEmissionTF1->GetRandom();
}


double AngleProviderIso::GetAngle() {
    return randGen->Uniform(paramVals["minAngle"], paramVals["maxAngle"]);
}

void AngleProviderIso::ValidateParamValues() {
    auto aMin = paramVals["minAngle"];
    auto aMax = paramVals["maxAngle"];
    if (aMin < -ROOT::Math::Pi())
        throw std::invalid_argument(
                "AngleProviderIso::AngleProviderIso: min angle is smaller than -pi!");
    if (aMax > ROOT::Math::Pi())
        throw std::invalid_argument(
                "AngleProviderIso::AngleProviderIso: max angle is larger than pi!");
    if (aMin > aMax)
        throw std::invalid_argument(
                "AngleProviderIso::AngleProviderIso: min angle is larger than max angle!");
}

AngleProviderIso::AngleProviderIso() {
    paramVals["minAngle"] = -ROOT::Math::Pi();
    paramVals["maxAngle"] = ROOT::Math::Pi();
}

AngleProviderPhi::AngleProviderPhi() {
    paramVals["polDegree"] = 0;
    paramVals["polAngle"] = 0;
    std::string fName = "AngleProviderPhiTF1_" + std::to_string(nInstances++);
    phiEmissionTF1 = std::make_unique<TF1>(
            fName.c_str(), this, &AngleProviderPhi::Phi,
            -ROOT::Math::Pi(), ROOT::Math::Pi(), 0);
}

double AngleProviderPhi::Phi(double *x, double *par) {
    return 1 + paramVals["polDegree"] * cos(2 * (x[0] - paramVals["polAngle"]));
}

void AngleProviderPhi::ValidateParamValues() {
    if (paramVals["polDegree"] < 0 || paramVals["polDegree"] > 1)
        throw std::invalid_argument(
                "AngleProviderPhi::ValidateParamValues: polarisation degree out of [0,1] interval!");
    if (paramVals["polAngle"] < 0 || paramVals["polDegree"] > 2 * ROOT::Math::Pi())
        throw std::invalid_argument(
                "AngleProviderPhi::ValidateParamValues: polarisation angle out of [0,2*pi] interval!");

}

double AngleProviderPhi::GetAngle() {
    return phiEmissionTF1->GetRandom();
}