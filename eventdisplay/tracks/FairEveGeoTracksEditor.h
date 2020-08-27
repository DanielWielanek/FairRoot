/*
 * FairEveGeoTracksEditor.h
 *
 *  Created on: 23 kwi 2020
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef FAIREVEGEOTRACKSEDITOR_H_
#define FAIREVEGEOTRACKSEDITOR_H_

#include "FairEveCut.h"
#include "FairEveGeoTracks.h"

#include <GuiTypes.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TGButton.h>
#include <TGDoubleSlider.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGedFrame.h>   // for TGedFrame

/**
 * editor of TGeoTracks in event display
 */

class FairEveGeoTracksEditor : public TGedFrame
{
    std::unique_ptr<FairEveMinMaxCut> fPtCut;
    std::unique_ptr<FairEveMinMaxCut> fEtaCut;
    std::unique_ptr<FairEveMinMaxCut> fEnergyCut;
    std::unique_ptr<FairEveBoolCut> fPrimary;
    std::unique_ptr<FairEveBoolCut> fSecondary;
    std::unique_ptr<FairEveIntCut> fPdgCut;
    FairEveGeoTracks* fGeoTracks;

  public:
    FairEveGeoTracksEditor(const TGWindow* p = 0,
                           Int_t width = 170,
                           Int_t height = 30,
                           UInt_t options = kChildFrame,
                           Pixel_t back = GetDefaultFrameBackground());
    virtual void SetModel(TObject* obj);
    void SwapTracks();
    void Repaint();
    virtual ~FairEveGeoTracksEditor();
    ClassDef(FairEveGeoTracksEditor, 1)
};

#endif /* FAIREVEGEOTRACKSEDITOR_H_ */