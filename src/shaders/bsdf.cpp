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

#include <iostream>
#include "bsdf.hpp"

namespace bsdf {
    float Sign(float x){
        if(x < 0.0f) return -1.0f;
        else if(x > 0.0f) return 1.0f;
        return 0.0f;
    }

    glm::vec3 SampleCosineWeightedHemisphere(float r0, float r1){
        float r = sqrt(r0);
        float theta = 2 * M_PI * r1;
        return glm::vec3(r * cos(theta), sqrt(fmax(0.0f, 1 - r0)), r * sin(theta));
    }

    float CosTheta(const glm::vec3& w){
        return w.y;
    }

    float Cos2Theta(const glm::vec3& w){
        return w.y * w.y;
    }

    float Absf(float x){
        return (x < 0) ? -x : x;
    }

    float AbsCosTheta(const glm::vec3& w){
        return Absf(CosTheta(w));
    }

    float Sin2Theta(const glm::vec3& w){
        return fmax(0.0f, 1.0f - Cos2Theta(w));
    }

    float SinTheta(const glm::vec3& w){
        return sqrt(Sin2Theta(w));
    }

    float TanTheta(const glm::vec3& w){
        return SinTheta(w) / CosTheta(w);
    }

    float CosPhi(const glm::vec3& w){
        float sinTheta = SinTheta(w);
        return (sinTheta == 0) ? 1.0f : utils::clamp(w.x / sinTheta, -1.0f, 1.0f);
    }

    float Cos2Phi(const glm::vec3& w){
        float cosPhi = CosPhi(w);
        return cosPhi * cosPhi;
    }

    float SinPhi(const glm::vec3& w){
        float sinTheta = SinTheta(w);
        return (sinTheta == 0) ? 1.0f : utils::clamp(w.z / sinTheta, -1.0f, 1.0f);
    }

    float Sin2Phi(const glm::vec3& w){
        float sinPhi = SinPhi(w);
        return sinPhi * sinPhi;
    }

    float Lerp(float a, float b, float t) {
        return (1.0f - t) * a + t * b;
    }

    glm::vec2 Lerp(glm::vec2 a, glm::vec2 b, float t){
        return (1.0f - t) * a + t * b;
    }

    glm::vec3 Lerp(glm::vec3 a, glm::vec3 b, float t){
        return (1.0f - t) * a + t * b;
    }

    glm::vec4 Lerp(glm::vec4 a, glm::vec4 b, float t){
        return (1.0f - t) * a + t * b;
    }

    float Saturate(float x){
        if(x < 0.0f) return 0.0f;
        else if(x > 1.0f) return 1.0f;
        return x;
    }

    glm::vec3 CalculateTint(glm::vec3 baseColor){
        float luminance = glm::dot(glm::vec3(0.3f, 0.6f, 1.0f), baseColor);
        return (luminance > 0.0f) ? baseColor * (1.0f / luminance) : glm::vec3(1);
    }

    float FresnelSchlickWeight(float u){
        float m = Saturate(1.0f - u);
        float m2 = m * m;
        return m * m2 * m2;
    }

    glm::vec3 EvaluateSheen(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi){
        if(surface.sheen <= 0.0f) return glm::vec3(0);

        float dotHL = glm::dot(wm, wi);
        glm::vec3 tint = CalculateTint(surface.baseColor);
        return surface.sheen * Lerp(glm::vec3(1.0f), tint, surface.sheenTint) * FresnelSchlickWeight(dotHL);
    }

    float GTR1(float absDotHL, float a) {
        if(a >= 1) return M_1_PI;

        float a2 = a * a;
        return (a2 - 1.0f) / (M_PI * log2(a2) * (1.0f + (a2 - 1.0f) * absDotHL * absDotHL));
    }

    float SeparableSmithGGXG1(const glm::vec3& w, float a){
        float a2 = a * a;
        float absDotNV = AbsCosTheta(w);
        return 2.0f / (1.0f + sqrt(a2 + (1 - a2) * absDotNV * absDotNV));
    }

    glm::vec3 FresnelSchlick(glm::vec3 r0, float radians){
        float exponential = pow(1.0f - radians, 5.0f);
        return r0 + (glm::vec3(1.0f) - r0) * exponential;
    }

    float FresnelSchlick(float r0, float radians){
        return Lerp(1.0f, FresnelSchlickWeight(radians), r0);
    }

    float EvaluateDisneyClearcoat(float clearcoat, float alpha, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi, float& fPdfW, float& rPdfW){
        if(clearcoat <= 0.0f) return 0.0f;

        float absDotNH = AbsCosTheta(wm);
        float absDotNL = AbsCosTheta(wi);
        float absDotNV = AbsCosTheta(wo);
        float dotHL = glm::dot(wm, wi);

        float d = GTR1(absDotNH, Lerp(0.1f, 0.001f, alpha));
        float f = FresnelSchlick(0.04f, dotHL);
        float gl = SeparableSmithGGXG1(wi, 0.25f);
        float gv = SeparableSmithGGXG1(wo, 0.25f);

        fPdfW = d / (4.0f * absDotNL);
        rPdfW = d / (4.0f * absDotNV);

        return 0.25f * clearcoat * d * f * gl * gv;
    }

    float GgxAnisotropicD(const glm::vec3& wm, float ax, float ay){
        float dotHX2 = pow(wm.x, 2);
        float dotHY2 = pow(wm.z, 2);
        float cos2Theta = Cos2Theta(wm);
        float ax2 = pow(ax, 2);
        float ay2 = pow(ay, 2);

        return 1.0f / (M_PI * ax * ay * pow(dotHX2 / ax2 + dotHY2 / ay2 + cos2Theta, 2));
    }

    float SeparableSmithGGXG1(const glm::vec3& w, const glm::vec3& wm, float ax, float ay){
        float dotHW = glm::dot(w, wm);
        if (dotHW <= 0.0f) return 0.0f;

        float absTanTheta = Absf(TanTheta(w));
        if(std::isinf(absTanTheta)) return 0.0f;

        float a = sqrt(Cos2Phi(w) * ax * ax + Sin2Phi(w) * ay * ay);
        float a2Tan2Theta = pow(a * absTanTheta, 2);

        float lambda = 0.5f * (-1.0f + sqrt(1.0f + a2Tan2Theta));
        return 1.0f / (1.0f + lambda);
    }

    void CalculateAnisotropicParams(float roughness, float anisotropic, float& ax, float& ay) {
        float aspect = sqrt(1.0f - 0.9f * anisotropic);
        ax = fmax(0.001f, pow(roughness, 2) / aspect);
        ay = fmax(0.001f, pow(roughness, 2) * aspect);
    }

    glm::vec3 SampleGgxVndfAnisotropic(const glm::vec3& wo, float ax, float ay, float u1, float u2){
        glm::vec3 v = glm::normalize(glm::vec3(wo.x * ax, wo.y, wo.z * ay));

        // -- Build an orthonormal basis with v, t1, and t2
        glm::vec3 t1 = (v.y < 0.9999f) ? glm::normalize(glm::cross(v, glm::vec3(0, 1, 0))) : glm::vec3(1, 0, 0);
        glm::vec3 t2 = glm::cross(t1, v);

        // -- Choose a point on a disk with each half of the disk weighted proportionally to its projection onto direction v
        float a = 1.0f / (1.0f + v.y);
        float r = sqrt(u1);
        float phi = (u2 < a) ? (u2 / a) * M_PI : M_PI + (u2 - a) / (1.0f - a) * M_PI;
        float p1 = r * cos(phi);
        float p2 = r * sin(phi) * ((u2 < a) ? 1.0f : v.y);

        // -- Calculate the normal in this stretched tangent space
        glm::vec3 n = p1 * t1 + p2 * t2 + static_cast<float>(sqrt(fmax(0.0f, 1.0f - p1 * p1 - p2 * p2))) * v;

        // -- unstretch and normalize the normal
        return glm::normalize(glm::vec3(ax * n.x, n.y, ay * n.z));
    }

    float GgxVndfAnisotropicPdf(const glm::vec3& wi, const glm::vec3& wm, const glm::vec3& wo, float ax, float ay){
        float absDotNL = AbsCosTheta(wi);
        float absDotLH = Absf(glm::dot(wm, wi));

        float G1 = SeparableSmithGGXG1(wo, wm, ax, ay);
        float D = GgxAnisotropicD(wm, ax, ay);

        return G1 * absDotLH * D / absDotNL;
    }

    void GgxVndfAnisotropicPdf(const glm::vec3& wi, const glm::vec3& wm, const glm::vec3& wo, float ax, float ay, float& forwardPdfW, float& reversePdfW){
        float D = GgxAnisotropicD(wm, ax, ay);

        float absDotNL = AbsCosTheta(wi);
        float absDotHL = Absf(glm::dot(wm, wi));
        float G1v = SeparableSmithGGXG1(wo, wm, ax, ay);

        forwardPdfW = G1v * absDotHL * D / absDotNL;
        float absDotNV = AbsCosTheta(wo);
        float absDotHV = Absf(glm::dot(wm, wo));
        float G1l = SeparableSmithGGXG1(wi, wm, ax, ay);
        reversePdfW = G1l * absDotHV * D / absDotNV;
    }

    float ThinTransmissionRoughness(float ior, float roughness){
        return Saturate((0.65f * ior - 0.35f) * roughness);
    }

    float FresnelSchlickR0FromRelativeIOR(float eta) {
        // https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
        return pow(eta - 1.0f, 2) / pow(eta + 1.0f, 2);
    }

    float FresnelSchlickDielectic(float cosThetaI, float relativeIor){
        float r0 = FresnelSchlickR0FromRelativeIOR(relativeIor);
        return r0 + (1.0f - r0) * FresnelSchlickWeight(cosThetaI);
    }

    float FresnelDielectric(float cosThetaI, float ni, float nt){
        // Copied from PBRT. This function calculates the full Fresnel term for a dielectric material.
        // See Sebastion Legarde's link above for details.

        cosThetaI = utils::clamp(cosThetaI, -1.0f, 1.0f);

        // Swap index of refraction if this is coming from inside the surface
        if(cosThetaI < 0.0f) {
            float temp = ni;
            ni = nt;
            nt = temp;

            cosThetaI = -cosThetaI;
        }

        float sinThetaI = sqrt(fmax(0.0f, 1.0f - cosThetaI * cosThetaI));
        float sinThetaT = ni / nt * sinThetaI;

        // Check for total internal reflection
        if(sinThetaT >= 1) return 1;

        float cosThetaT = sqrt(fmax(0.0f, 1.0f - sinThetaT * sinThetaT));

        float rParallel     = ((nt * cosThetaI) - (ni * cosThetaT)) / ((nt * cosThetaI) + (ni * cosThetaT));
        float rPerpendicuar = ((ni * cosThetaI) - (nt * cosThetaT)) / ((ni * cosThetaI) + (nt * cosThetaT));
        return (rParallel * rParallel + rPerpendicuar * rPerpendicuar) / 2;
    }

    glm::vec3 DisneyFresnel(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi){
        float dotHV = abs(glm::dot(wm, wo));

        glm::vec3 tint = CalculateTint(surface.baseColor);

        // -- See section 3.1 and 3.2 of the 2015 PBR presentation + the Disney BRDF explorer (which does their
        // -- 2012 remapping rather than the SchlickR0FromRelativeIOR seen here but they mentioned the switch in 3.2).
        glm::vec3 R0 = FresnelSchlickR0FromRelativeIOR(surface.relativeIOR) * Lerp(glm::vec3(1.0f), tint, surface.specularTint);
        R0 = Lerp(R0, surface.baseColor, surface.metallic);

        float dielectricFresnel = FresnelDielectric(dotHV, 1.0f, surface.ior);
        glm::vec3 metallicFresnel = FresnelSchlick(R0, glm::dot(wi, wm));

        return Lerp(glm::vec3(dielectricFresnel), metallicFresnel, surface.metallic);
    }

    static glm::vec3 EvaluateDisneyBRDF(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm,const glm::vec3& wi, float& fPdf, float& rPdf){
        fPdf = 0.0f;
        rPdf = 0.0f;

        float dotNL = CosTheta(wi);
        float dotNV = CosTheta(wo);
        if(dotNL <= 0.0f || dotNV <= 0.0f) return glm::vec3(0);

        float ax, ay;
        CalculateAnisotropicParams(surface.roughness, surface.anisotropic, ax, ay);

        float d = GgxAnisotropicD(wm, ax, ay);
        float gl = SeparableSmithGGXG1(wi, wm, ax, ay);
        float gv = SeparableSmithGGXG1(wo, wm, ax, ay);

        glm::vec3 f = DisneyFresnel(surface, wo, wm, wi);

        GgxVndfAnisotropicPdf(wi, wm, wo, ax, ay, fPdf, rPdf);
        fPdf *= (1.0f / (4 * abs(glm::dot(wo, wm))));
        rPdf *= (1.0f / (4 * abs(glm::dot(wi, wm))));

        return d * gl * gv * f / (4.0f * dotNL * dotNV);
    }

    glm::vec3 EvaluateDisneySpecTransmission(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi, float ax, float ay, bool thin){
        float relativeIor = surface.relativeIOR;
        float n2 = relativeIor * relativeIor;

        float absDotNL = AbsCosTheta(wi);
        float absDotNV = AbsCosTheta(wo);
        float dotHL = glm::dot(wm, wi);
        float dotHV = glm::dot(wm, wo);
        float absDotHL = Absf(dotHL);
        float absDotHV = Absf(dotHV);

        float d = GgxAnisotropicD(wm, ax, ay);
        float gl = SeparableSmithGGXG1(wi, wm, ax, ay);
        float gv = SeparableSmithGGXG1(wo, wm, ax, ay);

        float f = FresnelDielectric(dotHV, 1.0f, 1.0f / surface.relativeIOR);

        glm::vec3 color;
        if(thin) color = sqrt(surface.baseColor);
        else color = surface.baseColor;
        
        // Note that we are intentionally leaving out the 1/n2 spreading factor since for VCM we will be evaluating
        // particles with this. That means we'll need to model the air-[other medium] transmission if we ever place
        // the camera inside a non-air medium.
        float c = (absDotHL * absDotHV) / (absDotNL * absDotNV);
        float t = (n2 / pow(dotHL + relativeIor * dotHV, 2));
        return color * c * t * (1.0f - f) * gl * gv * d;
    }

    float EvaluateDisneyRetroDiffuse(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi){
        float dotNL = AbsCosTheta(wi);
        float dotNV = AbsCosTheta(wo);

        float roughness = surface.roughness * surface.roughness;

        float rr = 0.5f + 2.0f * dotNL * dotNL * roughness;
        float fl = FresnelSchlickWeight(dotNL);
        float fv = FresnelSchlickWeight(dotNV);
        
        return rr * (fl + fv + fl * fv * (rr - 1.0f));
    }

    float EvaluateDisneyDiffuse(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi, bool thin){
        float dotNL = AbsCosTheta(wi);
        float dotNV = AbsCosTheta(wo);

        float fl = FresnelSchlickWeight(dotNL);
        float fv = FresnelSchlickWeight(dotNV);

        float hanrahanKrueger = 0.0f;

        if(thin && surface.flatness > 0.0f) {
            float roughness = surface.roughness * surface.roughness;

            float dotHL = glm::dot(wm, wi);
            float fss90 = dotHL * dotHL * roughness;
            float fss = Lerp(1.0f, fss90, fl) * Lerp(1.0f, fss90, fv);

            float ss = 1.25f * (fss * (1.0f / (dotNL + dotNV) - 0.5f) + 0.5f);
            hanrahanKrueger = ss;
        }

        float lambert = 1.0f;
        float retro = EvaluateDisneyRetroDiffuse(surface, wo, wm, wi);
        float subsurfaceApprox = Lerp(lambert, hanrahanKrueger, thin ? surface.flatness : 0.0f);

        return M_1_PI * (retro + subsurfaceApprox * (1.0f - 0.5f * fl) * (1.0f - 0.5f * fv));
    }

    void CalculateLobePdfs(const SurfaceParameters& surface, float& pSpecular, float& pDiffuse, float& pClearcoat, float& pSpecTrans){
        float metallicBRDF   = surface.metallic;
        float specularBSDF   = (1.0f - surface.metallic) * surface.specTrans;
        float dielectricBRDF = (1.0f - surface.specTrans) * (1.0f - surface.metallic);

        float specularWeight     = metallicBRDF + dielectricBRDF;
        float transmissionWeight = specularBSDF;
        float diffuseWeight      = dielectricBRDF;
        float clearcoatWeight    = 1.0f * Saturate(surface.clearcoat); 

        float norm = 1.0f / (specularWeight + transmissionWeight + diffuseWeight + clearcoatWeight);

        pSpecular  = specularWeight     * norm;
        pSpecTrans = transmissionWeight * norm;
        pDiffuse   = diffuseWeight      * norm;
        pClearcoat = clearcoatWeight    * norm;
    }

    glm::vec3 EvaluateDisney(const SurfaceParameters& surface, glm::vec3 v, glm::vec3 l, bool thin, float& forwardPdf, float& reversePdf){
        glm::vec3 wo = glm::normalize(glm::transpose(surface.basis.transform) * v);
        glm::vec3 wi = glm::normalize(glm::transpose(surface.basis.transform) * l);

        glm::vec3 wm = glm::normalize(wo + wi);

        float dotNV = CosTheta(wo);
        float dotNL = CosTheta(wi);

        glm::vec3 reflectance = glm::vec3(0);
        forwardPdf = 0.0f;
        reversePdf = 0.0f;

        float pBRDF, pDiffuse, pClearcoat, pSpecTrans;
        CalculateLobePdfs(surface, pBRDF, pDiffuse, pClearcoat, pSpecTrans);

        glm::vec3 baseColor = surface.baseColor;
        float metallic = surface.metallic;
        float specTrans = surface.specTrans;
        float roughness = surface.roughness;

        float ax, ay;
        CalculateAnisotropicParams(surface.roughness, surface.anisotropic, ax, ay);

        float diffuseWeight = (1.0f - metallic) * (1.0f - specTrans);
        float transWeight   = (1.0f - metallic) * specTrans;

        bool upperHemisphere = dotNL > 0.0f && dotNV > 0.0f;
        if(upperHemisphere && surface.clearcoat > 0.0f) {
            
            float forwardClearcoatPdfW;
            float reverseClearcoatPdfW;

            float clearcoat = EvaluateDisneyClearcoat(surface.clearcoat, surface.clearcoatGloss, wo, wm, wi,
                                                    forwardClearcoatPdfW, reverseClearcoatPdfW);
            reflectance += glm::vec3(clearcoat);
            forwardPdf += pClearcoat * forwardClearcoatPdfW;
            reversePdf += pClearcoat * reverseClearcoatPdfW;
        }

        // -- Diffuse
        if(diffuseWeight > 0.0f) {
            float forwardDiffusePdfW = AbsCosTheta(wi);
            float reverseDiffusePdfW = AbsCosTheta(wo);
            float diffuse = EvaluateDisneyDiffuse(surface, wo, wm, wi, thin);

            glm::vec3 sheen = EvaluateSheen(surface, wo, wm, wi);

            reflectance += diffuseWeight * (diffuse * surface.baseColor + sheen);

            forwardPdf += pDiffuse * forwardDiffusePdfW;
            reversePdf += pDiffuse * reverseDiffusePdfW;
        }

        // -- transmission
        if(transWeight > 0.0f) {

            // Scale roughness based on IOR (Burley 2015, Figure 15).
            float rscaled = thin ? ThinTransmissionRoughness(surface.ior, surface.roughness) : surface.roughness;
            float tax, tay;
            CalculateAnisotropicParams(rscaled, surface.anisotropic, tax, tay);

            glm::vec3 transmission = EvaluateDisneySpecTransmission(surface, wo, wm, wi, tax, tay, thin);
            reflectance += transWeight * transmission;

            float forwardTransmissivePdfW;
            float reverseTransmissivePdfW;
            GgxVndfAnisotropicPdf(wi, wm, wo, tax, tay, forwardTransmissivePdfW, reverseTransmissivePdfW);

            float dotLH = glm::dot(wm, wi);
            float dotVH = glm::dot(wm, wo);
            forwardPdf += pSpecTrans * forwardTransmissivePdfW / (pow(dotLH + surface.relativeIOR * dotVH, 2));
            reversePdf += pSpecTrans * reverseTransmissivePdfW / (pow(dotVH + surface.relativeIOR * dotLH, 2));
        }

        // -- specular
        if(upperHemisphere) {
            float forwardMetallicPdfW;
            float reverseMetallicPdfW;
            glm::vec3 specular = EvaluateDisneyBRDF(surface, wo, wm, wi, forwardMetallicPdfW, reverseMetallicPdfW);
            
            reflectance += specular;
            forwardPdf += pBRDF * forwardMetallicPdfW / (4 * abs(glm::dot(wo, wm)));
            reversePdf += pBRDF * reverseMetallicPdfW / (4 * abs(glm::dot(wi, wm)));
        }

        reflectance = reflectance * Absf(dotNL);

        return reflectance;
    }

    glm::vec3 Reflect(glm::vec3 n, glm::vec3 l){
        return 2.0f * glm::dot(n, l) * n - l;
    }

    bool Transmit(glm::vec3 wm, glm::vec3 wi, float n, glm::vec3& wo){
        float c = glm::dot(wi, wm);
        if(c < 0.0f) {
            c = -c;
            wm = -wm;
        }

        float root = 1.0f - n * n * (1.0f - c * c);
        if(root <= 0) return false;

        wo = static_cast<float>(n * c - sqrt(root)) * wm - n * wi;
        return true;
    }

    static glm::vec3 CalculateExtinction(glm::vec3 apparantColor, float scatterDistance){
        glm::vec3 a = apparantColor;
        glm::vec3 a2 = a * a;
        glm::vec3 a3 = a2 * a;

        glm::vec3 alpha = glm::vec3(1.0f) - exp(-5.09406f * a + 2.61188f * a2 - 4.31805f * a3);
        glm::vec3 s = glm::vec3(1.9f) - a + 3.5f * (a - glm::vec3(0.8f)) * (a - glm::vec3(0.8f));

        return 1.0f / (s * scatterDistance);
    }

    bool SampleDisneySpecTransmission(const SurfaceParameters& surface, glm::vec3 v, bool thin, BsdfSample& sample){
        glm::vec3 wo = (glm::transpose(surface.basis.transform) * v);
        if(CosTheta(wo) == 0.0) {
            sample.forwardPdfW = 0.0f;
            sample.reversePdfW = 0.0f;
            sample.reflectance = glm::vec3(0);
            sample.wi = glm::vec3(0);
            return false;
        }

        // -- Scale roughness based on IOR
        float rscaled = thin ? ThinTransmissionRoughness(surface.ior, surface.roughness) : surface.roughness;
        
        float tax, tay;
        CalculateAnisotropicParams(rscaled, surface.anisotropic, tax, tay);
        
        // -- Sample visible distribution of normals
        float r0 = utils::sampler::random_double();
        float r1 = utils::sampler::random_double();
        glm::vec3 wm = SampleGgxVndfAnisotropic(wo, tax, tay, r0, r1);

        float dotVH = glm::dot(wo, wm);
        if(wm.y < 0.0f) {
            dotVH = -dotVH;
        }

        float ni = wo.y > 0.0f ? 1.0f : surface.ior;
        float nt = wo.y > 0.0f ? surface.ior : 1.0f;
        float relativeIOR = ni / nt;

        // -- Disney uses the full dielectric Fresnel equation for transmission. We also importance sample F
        // -- to switch between refraction and reflection at glancing angles.
        float F = FresnelDielectric(dotVH, 1.0f, surface.ior);
        
        // -- Since we're sampling the distribution of visible normals the pdf cancels out with a number of other terms.
        // -- We are left with the weight G2(wi, wo, wm) / G1(wi, wm) and since Disney uses a separable masking function
        // -- we get G1(wi, wm) * G1(wo, wm) / G1(wi, wm) = G1(wo, wm) as our weight.
        float G1v = SeparableSmithGGXG1(wo, wm, tax, tay);

        float pdf;

        glm::vec3 wi;
        if(utils::sampler::random_double() <= F) {
            wi = glm::normalize(Reflect(wm, wo));

            sample.flags = SurfaceEventFlags::eScatterEvent;
            sample.reflectance = G1v * surface.baseColor;

            float jacobian = (4 * abs(glm::dot(wo, wm)));
            pdf = F / jacobian;
        }
        else {
            if(thin) {
                // -- When the surface is thin so it refracts into and then out of the surface during this shading event.
                // -- So the ray is just reflected then flipped and we use the sqrt of the surface color.
                wi = Reflect(wm, wo);
                wi.y = -wi.y;
                sample.reflectance = G1v * sqrt(surface.baseColor);

                // -- Since this is a thin surface we are not ending up inside of a volume so we treat this as a scatter event.
                sample.flags = SurfaceEventFlags::eScatterEvent;
            }
            else {
                if(Transmit(wm, wo, relativeIOR, wi)) {
                    sample.flags = SurfaceEventFlags::eTransmissionEvent;
                    sample.medium.phaseFunction = dotVH > 0.0f ? MediumPhaseFunction::eIsotropic : MediumPhaseFunction::eVacuum;
                    sample.medium.extinction = CalculateExtinction(surface.transmittanceColor, surface.scatterDistance);
                }
                else {
                    sample.flags = SurfaceEventFlags::eScatterEvent;
                    wi = Reflect(wm, wo);
                }

                sample.reflectance = G1v * surface.baseColor;
            }

            wi = glm::normalize(wi);
            
            float dotLH = abs(glm::dot(wi, wm));
            float jacobian = dotLH / (pow(dotLH + surface.relativeIOR * dotVH, 2));
            pdf = (1.0f - F) / jacobian;
        }

        if(CosTheta(wi) == 0.0f) {
            sample.forwardPdfW = 0.0f;
            sample.reversePdfW = 0.0f;
            sample.reflectance = glm::vec3(0);
            sample.wi = glm::vec3(0);
            return false;
        }

        if(surface.roughness < 0.01f) {
            // -- This is a hack to allow us to sample the correct IBL texture when a path bounced off a smooth surface.
            sample.flags |= SurfaceEventFlags::eDiracEvent;
        }

        // -- calculate VNDF pdf terms and apply Jacobian and Fresnel sampling adjustments
        GgxVndfAnisotropicPdf(wi, wm, wo, tax, tay, sample.forwardPdfW, sample.reversePdfW);
        sample.forwardPdfW *= pdf;
        sample.reversePdfW *= pdf;

        // -- convert wi back to world space
        // sample.wi = glm::normalize(wi * glm::transpose(surface.worldToTangent));
        sample.wi = glm::normalize((surface.basis.transform * wi));

        return true;
    }

    bool SampleDisneyDiffuse(const SurfaceParameters& surface, glm::vec3 v, bool thin, BsdfSample& sample) {
        glm::vec3 wo = (glm::transpose(surface.basis.transform) * v);
        // glm::vec3 wo = glm::normalize(surface.basis.local(v));

        float sign = Sign(CosTheta(wo));
        // std::cout << sign;

        // -- Sample cosine lobe
        float r0 = utils::sampler::random_double();
        float r1 = utils::sampler::random_double();
        glm::vec3 wi = - sign * SampleCosineWeightedHemisphere(r0, r1);
        // glm::vec3 wi = random_cosine_direction(r0, r1);

        glm::vec3 wm = glm::normalize(wi + wo);

        float dotNL = CosTheta(wi);
        if(dotNL == 0.0f) {
            sample.forwardPdfW = 0.0f;
            sample.reversePdfW = 0.0f;
            sample.reflectance = glm::vec3(0);
            sample.wi = glm::vec3(0);
            return false;
        }

        float dotNV = CosTheta(wo);

        float pdf;

        SurfaceEventFlags eventType = SurfaceEventFlags::eScatterEvent;

        glm::vec3 color = surface.baseColor;

        float p = utils::sampler::random_double();
        if(p <= surface.diffTrans) {
            wi = -wi;
            pdf = surface.diffTrans;

            if(thin) color = sqrt(color);
            else {
                eventType = SurfaceEventFlags::eTransmissionEvent;
                sample.medium.phaseFunction = MediumPhaseFunction::eIsotropic;
                sample.medium.extinction = CalculateExtinction(surface.transmittanceColor, surface.scatterDistance);
            }
        }
        else pdf = (1.0f - surface.diffTrans);

        glm::vec3 sheen = EvaluateSheen(surface, wo, wm, wi);

        float diffuse = EvaluateDisneyDiffuse(surface, wo, wm, wi, thin);

        assert (pdf > 0.0f);
        sample.reflectance = sheen + color * (diffuse / pdf);
        sample.wi = glm::normalize((surface.basis.transform * wi));
        sample.forwardPdfW = Absf(dotNL) * pdf;
        sample.reversePdfW = Absf(dotNV) * pdf;
        sample.flags = eventType;
        return true;
    }

    bool SampleDisneyClearcoat(const SurfaceParameters& surface, const glm::vec3& v, BsdfSample& sample){
        glm::vec3 wo = glm::normalize(glm::transpose(surface.basis.transform) * v);

        float a = 0.25f;
        float a2 = a * a;

        float r0 = utils::sampler::random_double();
        float r1 = utils::sampler::random_double();
        float cosTheta = sqrt(fmax(0, (1.0f - pow(a2, 1.0f - r0)) / (1.0f - a2)));
        float sinTheta = sqrt(fmax(0, 1.0f - cosTheta * cosTheta));
        float phi = 2 * M_PI * r1;

        glm::vec3 wm = glm::vec3(sinTheta * cos(phi), cosTheta, sinTheta * cos(phi));
        if(glm::dot(wm, wo) < 0.0f) wm = -wm;

        glm::vec3 wi = Reflect(wm, wo);
        if(glm::dot(wi, wo) < 0.0f) return false;

        float clearcoatWeight = surface.clearcoat;
        float clearcoatGloss = surface.clearcoatGloss;

        float dotNH = CosTheta(wm);
        float dotLH = glm::dot(wm, wi);
        float absDotNL = Absf(CosTheta(wi));
        float absDotNV = Absf(CosTheta(wo));

        float d = GTR1(Absf(dotNH), Lerp(0.1f, 0.001f, clearcoatGloss));
        float f = FresnelSchlick(0.04f, dotLH);
        float g = SeparableSmithGGXG1(wi, 0.25f) * SeparableSmithGGXG1(wo, 0.25f);

        float fPdf = d / (4.0f * glm::dot(wo, wm));

        sample.reflectance = glm::vec3(0.25f * clearcoatWeight * g * f * d) / fPdf;
        sample.wi = glm::normalize((surface.basis.transform * wi));
        sample.forwardPdfW = fPdf;
        sample.reversePdfW = d / (4.0f * glm::dot(wi, wm));

        return true;
    }
}