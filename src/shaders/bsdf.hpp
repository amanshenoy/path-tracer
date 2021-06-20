/*///////////////////////////////////////////////////////////// 
Disney BRDF for broad variety of material choices

# Copyright Disney Enterprises, Inc.  All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License
# and the following modification to it: Section 6 Trademarks.
# deleted and replaced with:
#
# 6. Trademarks. This License does not grant permission to use the
# trade names, trademarks, service marks, or product names of the
# Licensor and its affiliates, except as required for reproducing
# the content of the NOTICE file.
#
# You may obtain a copy of the License at
# http://www.apache.org/licenses/LICENSE-2.0 

More then 90% of this code has been taken from https://github.com/schuttejoe/Selas, 
and re-ordered. Very little of the bsdf namespace has been written by myself 

*////////////////////////////////////////////////////////////////

#pragma once 
#include "../dependancies/glm/glm.hpp"
#include "../utils/utils.hpp"

namespace bsdf {
    struct SurfaceParameters {
        glm::vec3 position;
        glm::mat3 worldToTangent;
        utils::LocalBasis basis; 
        float error;
        glm::vec3 view;
        glm::vec3 baseColor;
        glm::vec3 transmittanceColor;
        float sheen;
        float sheenTint;
        float clearcoat;
        float clearcoatGloss;
        float metallic;
        float specTrans;
        float diffTrans;
        float flatness;
        float anisotropic;
        float relativeIOR;
        float specularTint;
        float roughness;
        float scatterDistance;

        float ior;
    };
    
    enum MediumPhaseFunction {
        eVacuum,
        eIsotropic
    };

    struct MediumParameters{
        MediumPhaseFunction phaseFunction = eVacuum;
        glm::vec3 extinction = glm::vec3(0);
    };

    struct BsdfSample{
        glm::uint32 flags;

        MediumParameters medium = MediumParameters();
        glm::vec3 reflectance      = glm::vec3(0);
        glm::vec3 wi               = glm::vec3(0);
        float forwardPdfW       = 0.0f;
        float reversePdfW       = 0.0f;
    };

    enum SurfaceEventFlags{
        eScatterEvent        = 0x01,
        eTransmissionEvent   = 0x02,
        eDiracEvent          = 0x04 
    };


    glm::vec3 CalculateTint(glm::vec3 baseColor);

    float Lerp(float a, float b, float t);

    glm::vec2 Lerp(glm::vec2 a, glm::vec2 b, float t);

    glm::vec3 Lerp(glm::vec3 a, glm::vec3 b, float t);

    glm::vec4 Lerp(glm::vec4 a, glm::vec4 b, float t);

    float Saturate(float x);

    float CosTheta(const glm::vec3& w);

    float Cos2Theta(const glm::vec3& w);

    float Absf(float x);

    float AbsCosTheta(const glm::vec3& w);

    float Sin2Theta(const glm::vec3& w);

    float SinTheta(const glm::vec3& w);

    float TanTheta(const glm::vec3& w);

    float CosPhi(const glm::vec3& w);

    float Cos2Phi(const glm::vec3& w);

    float SinPhi(const glm::vec3& w);

    float Sin2Phi(const glm::vec3& w);

    float FresnelSchlickWeight(float u);

    glm::vec3 EvaluateSheen(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi);

    float GTR1(float absDotHL, float a);

    float SeparableSmithGGXG1(const glm::vec3& w, float a);

    glm::vec3 FresnelSchlick(glm::vec3 r0, float radians);

    float FresnelSchlick(float r0, float radians);

    float EvaluateDisneyClearcoat(float clearcoat, float alpha, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi, float& fPdfW, float& rPdfW);

    float GgxAnisotropicD(const glm::vec3& wm, float ax, float ay);


    float SeparableSmithGGXG1(const glm::vec3& w, const glm::vec3& wm, float ax, float ay);

    void CalculateAnisotropicParams(float roughness, float anisotropic, float& ax, float& ay);

    glm::vec3 SampleGgxVndfAnisotropic(const glm::vec3& wo, float ax, float ay, float u1, float u2);

    float GgxVndfAnisotropicPdf(const glm::vec3& wi, const glm::vec3& wm, const glm::vec3& wo, float ax, float ay);

    void GgxVndfAnisotropicPdf(const glm::vec3& wi, const glm::vec3& wm, const glm::vec3& wo, float ax, float ay, float& forwardPdfW, float& reversePdfW);

    float ThinTransmissionRoughness(float ior, float roughness);

    float FresnelSchlickR0FromRelativeIOR(float eta);

    float FresnelSchlickDielectic(float cosThetaI, float relativeIor);

    float FresnelDielectric(float cosThetaI, float ni, float nt);

    glm::vec3 DisneyFresnel(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi);

    static glm::vec3 EvaluateDisneyBRDF(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi, float& fPdf, float& rPdf);

    glm::vec3 EvaluateDisneySpecTransmission(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi, float ax, float ay, bool thin);

    float EvaluateDisneyRetroDiffuse(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi);

    static float EvaluateDisneyDiffuse(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi, bool thin);

    void CalculateLobePdfs(const SurfaceParameters& surface, float& pSpecular, float& pDiffuse, float& pClearcoat, float& pSpecTrans);
    
    glm::vec3 EvaluateDisney(const SurfaceParameters& surface, glm::vec3 v, glm::vec3 l, bool thin, float& forwardPdf, float& reversePdf);

    glm::vec3 Reflect(glm::vec3 n, glm::vec3 l);
    bool Transmit(glm::vec3 wm, glm::vec3 wi, float n, glm::vec3& wo);
    static glm::vec3 CalculateExtinction(glm::vec3 apparantColor, float scatterDistance);

    bool SampleDisneySpecTransmission(const SurfaceParameters& surface, glm::vec3 v, bool thin, BsdfSample& sample);
    bool SampleDisneyDiffuse(const SurfaceParameters& surface, glm::vec3 v, bool thin, BsdfSample& sample);
    bool SampleDisneyClearcoat(const SurfaceParameters& surface, const glm::vec3& v, BsdfSample& sample);
}