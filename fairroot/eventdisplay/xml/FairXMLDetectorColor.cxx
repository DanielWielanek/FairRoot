/********************************************************************************
 * Copyright (C) 2020-2023 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/*
 * FairXMLDetColor.cxx
 *
 *  Created on: 17 lip 2024
 *      Author: daniel
 */

#include "FairXMLDetectorColor.h"

#include "FairXMLNode.h"

#include <RtypesCore.h>
#include <TAttFill.h>
#include <TGeoNode.h>
#include <TGeoVolume.h>
#include <TNamed.h>
#include <TString.h>
Int_t FairXMLDetectorColor::ApplyToNode(TGeoNode& node, const FairXMLNode& xml) const
{
    if (auto transparency_atrib = xml.GetAttrib("transparency")) {
        Char_t transparency = 0;
        if (transparency_atrib->GetValue().Length()) {
            transparency = transparency_atrib->GetValue().Atoi();
            node.GetVolume()->SetTransparency(transparency);
        }
    }

    if (auto color_atrib = xml.GetAttrib("color")) {
        TString color = xml.GetAttrib("color")->GetValue();
        if (!color.EqualTo("")) {
            node.GetVolume()->SetFillColor(FairXMLEveConf::StringToColor(color));
            node.GetVolume()->SetLineColor(FairXMLEveConf::StringToColor(color));
        }
    }

    if (auto rec_atrib = xml.GetAttrib("recursive")) {
        TString rec = xml.GetAttrib("recursive")->GetValue();
        if (rec.Length() == 0)
            return 0;
        return rec.Atoi();
    }
    return 0;
}

void FairXMLDetectorColor::ColorizeNode(TGeoNode& node, const FairXMLNode& xml, Int_t depth) const
{
    switch (depth) {
        case 0: {   // recursive mode of
            return;
        } break;
        case -1: {   // standard mode
            Int_t newDepth = ApplyToNode(node, xml);
            if (newDepth) {
                ColorizeDaughters(node, xml, newDepth);
            } else {
                ColorizeDaughters(node, xml, -1);
            }
        } break;
        default: {   // recusrive mode
            ApplyToNode(node, xml);
            ColorizeDaughters(node, xml, depth);
        } break;
    }
}

FairXMLDetectorColor::FairXMLDetectorColor(FairXMLNode* node)
{
    if (node)
        fNode = *node;
}

void FairXMLDetectorColor::Colorize(TGeoNode* node)
{
    if (!node)
        return;
    ColorizeNode(*node, fNode, -1);
}

void FairXMLDetectorColor::ColorizeDaughters(TGeoNode& node, const FairXMLNode& xml, Int_t depth) const
{
    if (depth == -1) {
        for (int i = 0; i < node.GetNdaughters(); i++) {
            TString subdetector_name = node.GetDaughter(i)->GetName();
            for (int j = 0; j < xml.GetNChildren(); j++) {
                FairXMLNode* subnode = xml.GetChild(j);
                TString subnode_name = subnode->GetAttrib("name")->GetValue();
                if (subnode_name.EqualTo(subdetector_name)) {
                    ColorizeNode(*node.GetDaughter(i), *subnode, -1);
                }
            }
        }
    } else {
        for (int i = 0; i < node.GetNdaughters(); i++) {
            ColorizeNode(*node.GetDaughter(i), xml, depth - 1);
        }
    }
}
