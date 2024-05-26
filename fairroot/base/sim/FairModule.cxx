/********************************************************************************
 * Copyright (C) 2014-2024 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                     FairModule source file                    -----
// -----            Created 06/01/04  by M. Al-Turany                  -----
// -------------------------------------------------------------------------
/* Generated by Together */

#include "FairModule.h"

#include "FairGeoBuilder.h"     // for FairGeoBuilder
#include "FairGeoInterface.h"   // for FairGeoInterface
#include "FairGeoLoader.h"      // for FairGeoLoader
#include "FairGeoMedia.h"       // for FairGeoMedia
#include "FairGeoNode.h"        // for FairGeoNode
#include "FairGeoParSet.h"      // for FairBaseParSet
#include "FairMCApplication.h"
#include "FairRun.h"          // for FairRun
#include "FairRuntimeDb.h"    // for FairRuntimeDb
#include "FairVolume.h"       // for FairVolume
#include "FairVolumeList.h"   // for FairVolumeList

#include <TBuffer.h>           // for TBuffer, operator<<, etc
#include <TCollection.h>       // for TIter, TRangeDynCast
#include <TDirectory.h>        // for TDirectory::TContext
#include <TFile.h>             // for TFile
#include <TGeoManager.h>       // for TGeoManager, gGeoManager
#include <TGeoMaterial.h>      // for TGeoMaterial
#include <TGeoMedium.h>        // for TGeoMedium
#include <TGeoVoxelFinder.h>   // for TGeoVoxelFinder
#include <TKey.h>              // for TKey
#include <TObjArray.h>         // for TObjArray
#include <TObject.h>           // for TObject
#include <TSeqCollection.h>    // for TSeqCollection
#include <TSystem.h>           // for TSystem, gSystem
#include <fairlogger/Logger.h>

#ifdef ROOT_HAS_GDML
#include <TGDMLParse.h>
#endif

#include <cstdlib>   // for getenv
#include <cstring>   // for strcmp, strlen
#include <map>
#include <memory>
#include <tuple>   // for std::ignore

thread_local std::vector<FairVolume*> FairModule::fAllSensitiveVolumes;

void FairModule::ConstructGeometry()
{
    LOG(warn)
        << "The method ConstructGeometry has to be implemented in the detector class which inherits from FairModule";
}

void FairModule::ConstructOpGeometry()
{
    LOG(debug2)
        << "The method ConstructOpGeometry has to be implemented in the detector class which inherits from FairModule";
}

FairModule::~FairModule() {}

FairModule::FairModule(const char* Name, const char* title, Bool_t Active)
    : TNamed(Name, title)
    , fActive(Active)
{
    if (!vList) {
        vList = new FairVolumeList();
    }
}

FairModule::FairModule(const FairModule& rhs)
    : TNamed(rhs)
    , fMotherVolumeName(rhs.fMotherVolumeName)
    , fgeoVer(rhs.fgeoVer)
    , fgeoName(rhs.fgeoName)
    , fModId(rhs.fModId)
    , fActive(rhs.fActive)
    , fNbOfSensitiveVol(rhs.fNbOfSensitiveVol)
    , fVerboseLevel(rhs.fVerboseLevel)
    , fGeoSaved(rhs.fGeoSaved)
{
    if (!vList) {
        vList = new FairVolumeList();
    }

    // Initialize cached pointer to MC (on worker)
    fMC = TVirtualMC::GetMC();

    // TO DO - add when we know what type is the elements of flGeoPar
    // flGeoPar=new TObjArray();
    // TIter it = rhs.flGeoPar->MakeIterator();
    // Copy parameters
    // TObject* obj;
    // while((obj=it->Next())) {
    //  flGeoPar->Add(...);
    //}
}

FairModule::FairModule()
    : TNamed()
{}

FairModule& FairModule::operator=(const FairModule& rhs)
{
    // check assignment to self
    if (this == &rhs)
        return *this;

    // base class assignment
    TNamed::operator=(rhs);

    // assignment operator
    fMotherVolumeName = rhs.fMotherVolumeName;
    fgeoVer = rhs.fgeoVer;
    fgeoName = rhs.fgeoName;
    fModId = rhs.fModId;
    fActive = rhs.fActive;
    fNbOfSensitiveVol = rhs.fNbOfSensitiveVol;
    fVerboseLevel = rhs.fVerboseLevel;
    flGeoPar = nullptr;
    fGeoSaved = rhs.fGeoSaved;
    fMC = TVirtualMC::GetMC();

    // TO DO - add when we know what type is the elements of flGeoPar
    // flGeoPar=new TObjArray();
    // TIter it = rhs.flGeoPar->MakeIterator();
    // copy parameters
    // TObject* obj;
    // while((obj=it->Next())) {
    //  flGeoPar->Add(...);
    //}

    return *this;
}

void FairModule::Streamer(TBuffer& b)
{
    TNamed::Streamer(b);

    if (b.IsReading()) {
        fgeoVer.Streamer(b);
        fgeoName.Streamer(b);
        b >> fActive;
        b >> fModId;
    } else {
        fgeoVer.Streamer(b);
        fgeoName.Streamer(b);
        b << fActive;
        b << fModId;
    }
}

void FairModule::SetGeometryFileName(TString fname, TString)
{
    // If absolute path is given as argument, try to find it there.
    // If the file don't exist break. Don't look anywhere else.
    if (fname.BeginsWith("/")) {
        if (gSystem->AccessPathName(fname.Data())) {
            LOG(fatal) << fName << ": geometry file " << fname << " not found in absolut path!";
            fgeoName = "";
        }   // file not found
        fgeoName = fname;
        LOG(debug) << fName << ": using geometry file " << fgeoName;
        return;
    }

    // If GEOMPATH is set, try to find the file there.
    // If there is no such file go on to look in the default location.
    TString userPath = getenv("GEOMPATH");
    userPath.ReplaceAll("//", "/");
    if (!userPath.IsNull()) {
        if (!userPath.EndsWith("/")) {
            userPath += "/";
        }
        fgeoName = userPath + fname;
        if (!gSystem->AccessPathName(fgeoName.Data())) {
            LOG(debug) << fName << ": using geometry file " << fgeoName;
            return;
        }
        LOG(debug) << fName << ": geometry file " << fname << " not found in GEOMPATH " << userPath;
    }

    // Look in the standard path
    fgeoName = getenv("VMCWORKDIR");
    fgeoName += "/geometry/" + fname;
    if (!gSystem->AccessPathName(fgeoName.Data())) {
        LOG(debug) << fName << ": using geometry file " << fgeoName;
        return;
    }

    // File not found
    LOG(fatal) << fName << ": geometry file " << fname << " not found in standard path ";
    fgeoName = "";
}

void FairModule::RegisterSensitiveVolume(FairVolume& vol)
{
    vol.setModId(fModId);
    vol.SetModule(this);
    fAllSensitiveVolumes.push_back(&vol);
    ++fNbOfSensitiveVol;
}

void FairModule::ProcessNodes(TList* nodes)
{
    if (FairMCApplicationState::kConstructGeometry != FairMCApplication::Instance()->GetState()) {
        LOG(fatal) << "Detected call to FairModule::ProcessNodes() \
      while not in FairMCApplication::ConstructGeometry()\n\
      Call templated function FairModule::ConstructASCIIGeometry()\
      from ConstructGeometry() of your detector class. Aborting...";
    }

    if (!vList) {
        vList = new FairVolumeList();
    }

    auto rtdb = FairRun::Instance()->GetRuntimeDb();
    auto par = static_cast<FairGeoParSet*>(rtdb->getContainer("FairGeoParSet"));
    TSeqCollection* parNodes = par->GetGeoNodes();
    for (auto node : TRangeDynCast<FairGeoNode>(nodes)) {
        if (!node) {
            continue;
        }
        std::ignore = node->calcLabTransform();

        auto nodeTruncName = node->getTruncName();
        auto volume = std::make_unique<FairVolume>(nodeTruncName, fNbOfVolumes++);
        volume->setRealName(node->GetName());

        auto addedVol = vList->addVolume(std::move(volume));
        if (!addedVol) {
            LOG(warn) << "Skipping further processing of FairGeoNode " << nodeTruncName
                      << ". It is already present in the global volume list.";
            continue;
        }
        addedVol->setGeoNode(node);
        addedVol->setCopyNo(node->getCopyNo());

        auto motherNode = node->getMotherNode();
        if (motherNode) {
            addedVol->setMotherId(node->getMCid());
            addedVol->setMotherCopyNo(motherNode->getCopyNo());
        }

        if (node->isSensitive() && fActive) {
            RegisterSensitiveVolume(*addedVol);
            parNodes->AddLast(node);
        }
    }
}

void FairModule::AddSensitiveVolume(TGeoVolume* vol)
{
    auto volName = vol->GetName();
    LOG(debug2) << "AddSensitiveVolume " << volName;

    auto addedVol = vList->addVolume(std::make_unique<FairVolume>(volName, fNbOfVolumes));
    if (!addedVol) {
        return;
    }
    ++fNbOfVolumes;
    RegisterSensitiveVolume(*addedVol);
}

FairVolume* FairModule::getFairVolume(FairGeoNode* fN)
{
    FairVolume* fv;
    FairVolume* fvol = 0;
    for (Int_t i = 0; i < vList->getEntries(); i++) {
        fv = vList->At(i);
        if ((fv->getGeoNode()) == fN) {
            fvol = fv;
            return fvol;
        }
    }
    return fvol;
}

void FairModule::ConstructRootGeometry(TGeoMatrix* shiftM)
{
    /** Construct the detector geometry from ROOT files, possible inputs are:
     * 1. A TGeoVolume as a mother (master) volume containing the detector geometry
     * 2. A TGeoManager with the detector geometry
     * 3. A TGeoVolume as a mother or Master volume which is the output of the CAD2ROOT geometry, in this case
     *    the materials are not proprely defined and had to be reset
     *  In all cases we have to check that the material properties are the same or is the materials defined in
     *  the current simulation session
     */
    TGeoManager* OldGeo = gGeoManager;
    TGeoManager* NewGeo = 0;

    std::unique_ptr<TFile> f{TFile::Open(GetGeometryFileName().Data())};
    TList* l = f->GetListOfKeys();
    TKey* key;
    TIter next(l);
    TGeoNode* n = 0;
    TGeoVolume* v1 = 0;
    while ((key = static_cast<TKey*>(next()))) {
        /**loop inside the delivered root file and try to fine a TGeoManager object
         * the first TGeoManager found will be read
         */
        NewGeo = key->ReadObject<TGeoManager>();
        if (!NewGeo) {
            continue;
        }
        gGeoManager = 0;
        break;
    }
    if (NewGeo != 0) {
        /** in case a TGeoManager was found get the top most volume and the node
         */

        NewGeo->cd();
        TGeoVolume* volume = NewGeo->GetNode(0)->GetDaughter(0)->GetVolume();
        v1 = volume->MakeCopyVolume(volume->GetShape());
        // n=NewGeo->GetTopNode();
        n = v1->GetNode(0);
        //  NewGeo=0;
        // delete NewGeo; //move it to the end of the method

    } else {
        /** The file does not contain any TGeoManager, so we assume to have a file with a TGeoVolume
         * try to look for a TGeoVolume inside the file
         */

        key = static_cast<TKey*>(l->At(0));   // Get the first key in the list
        auto volume = key->ReadObject<TGeoVolume>();
        if (volume) {
            n = volume->GetNode(0);
        }
        if (n != 0) {
            v1 = n->GetVolume();
        }
    }

    if (v1 == 0) {
        LOG(fatal) << "Could not find any geometry in file " << GetGeometryFileName().Data();
    } else {
        gGeoManager = OldGeo;
        gGeoManager->cd();
        // If AddToVolume is empty add the volume to the top volume Cave
        // If it is defined check i´f the volume exists and if it exists add the volume from the root file
        // to the already existing volume
        TGeoVolume* Cave = nullptr;
        if (0 == fMotherVolumeName.Length()) {
            Cave = gGeoManager->GetTopVolume();
        } else {
            Cave = gGeoManager->GetVolume(fMotherVolumeName);
        }
        if (Cave != nullptr) {
            /** Everything is OK, we have a TGeoVolume and now we add it to the simulation TGeoManager */
            gGeoManager->AddVolume(v1);
            /** Force rebuilding of voxels */
            TGeoVoxelFinder* voxels = v1->GetVoxels();
            if (voxels) {
                voxels->SetNeedRebuild();
            }

            /**To avoid having different names of the default matrices
             * because we could have get the volume from another
             * TGeoManager, we reset the default matrix name
             */
            TGeoMatrix* M = n->GetMatrix();

            // very nasty!
            TGeoHMatrix* M2 = new TGeoHMatrix(*M);

            // For debugging
            // M2->Dump();
            // const Double_t* transl1 = M2->GetTranslation();
            // std::cout << transl1[0] << "\t" << transl1[1] << "\t" << transl1[2] << std::endl;
            // if (shiftM) {
            //  shiftM->Dump();
            //  const Double_t* transl2 = shiftM->GetTranslation();
            //  std::cout << transl2[0] << "\t" << transl2[1] << "\t" << transl2[2] << std::endl;
            //}

            if (shiftM) {
                M2->Multiply(shiftM);   // HACK!
            }

            SetDefaultMatrixName(M2);

            // TODO
            // I don't really understand this juggling with the matices,
            // so, please, take care of how it works with my changes.
            // Egor.

            /** Now we can remove the matrix so that the new geomanager will rebuild it properly*/
            gGeoManager->GetListOfMatrices()->Remove(M2);
            TGeoHMatrix* global = gGeoManager->GetHMatrix();
            gGeoManager->GetListOfMatrices()->Remove(global);   // Remove the Identity matrix
            /** Now we can add the node to the existing cave */

            Cave->AddNode(v1, 0, M2);
            /** correction from O. Merle: in case of a TGeoVolume (v1) set the material properly */

            AssignMediumAtImport(v1);
            /** now go through the herachy and set the materials properly, this is important becase the CAD converter
             *  produce TGeoVolumes with materials that have only names and no properties
             */
            ExpandNode(n);
            delete NewGeo;
        } else {
            LOG(fatal) << "Could not find the given mother volume " << fMotherVolumeName.Data()
                       << " where the geomanger should be added.";
        }
    }
}

#ifdef ROOT_HAS_GDML

void FairModule::ConstructGDMLGeometry(__attribute__((unused)) TGeoMatrix* posrot)
{
    // Parse the GDML file
    TDirectory::TContext restorecwd{};
    TGDMLParse parser;
    TGeoVolume* gdmlTop;
    gdmlTop = parser.GDMLReadFile(GetGeometryFileName());

    // Change ID of media. TGDMLParse starts allways from 0. Need to shift.
    ReAssignMediaId();

    // Add volume to the cave and go through it recursively
    gGeoManager->GetTopVolume()->AddNode(gdmlTop, 1, posrot);
    ExpandNodeForGDML(gGeoManager->GetTopVolume()->GetNode(gGeoManager->GetTopVolume()->GetNdaughters() - 1));
}

void FairModule::ExpandNodeForGDML(TGeoNode* curNode)
{
    // Get pointer to volume and assign medium
    TGeoVolume* curVol = curNode->GetVolume();
    AssignMediumAtImport(curVol);

    // Check if the volume is sensitive
    if ((this->InheritsFrom("FairDetector")) && IsSensitive(curVol->GetName())) {
        LOG(debug2) << "Sensitive Volume " << curVol->GetName();
        AddSensitiveVolume(curVol);
    }

    //! Recursevly go down the tree of nodes
    if (curVol->GetNdaughters() != 0) {
        TObjArray* NodeChildList = curVol->GetNodes();
        TGeoNode* curNodeChild;
        for (Int_t j = 0; j < NodeChildList->GetEntriesFast(); j++) {
            curNodeChild = static_cast<TGeoNode*>(NodeChildList->At(j));
            ExpandNodeForGDML(curNodeChild);
        }
    }
}

#else

void FairModule::ConstructGDMLGeometry(__attribute__((unused)) TGeoMatrix* posrot)
{
    LOG(error) << "Could not construct magnet geometry from gdml file.";
    LOG(error) << "The used ROOT version does not support gdml.";
    LOG(error) << "Please recompile ROOT with gdml support.";
    LOG(fatal) << "Stop execution at this point.";
}

void FairModule::ExpandNodeForGDML(__attribute__((unused)) TGeoNode* curNode) {}

#endif

void FairModule::ReAssignMediaId()
{
    // Initialise pointer to GeoBuilder
    FairGeoBuilder* geoBuilder = FairGeoLoader::Instance()->getGeoBuilder();
    // Get list of TGeo media
    TList* media = gGeoManager->GetListOfMedia();
    // Loop over new media which are not in GeoBase and shift the ID
    TGeoMedium* med;
    //    TGeoMedium* med2;
    for (Int_t i = geoBuilder->GetNMedia(); i < media->GetEntries(); i++) {
        med = static_cast<TGeoMedium*>(media->At(i));
        med->SetId(i + 1);
    }
    // Change GeoBase medium index
    geoBuilder->SetNMedia(media->GetEntries());

    // Revove dublicated materials
    TList* materials = gGeoManager->GetListOfMaterials();
    TIter next1(materials);
    // map for existing materials
    std::map<TString, Bool_t> mapMatName;
    TGeoMaterial* mat;
    while ((mat = static_cast<TGeoMaterial*>(next1()))) {
        // If material exist - delete dublicated. If not - set the flag
        if (mapMatName[mat->GetName()]) {
            materials->Remove(mat);
        } else {
            mapMatName[mat->GetName()] = kTRUE;
        }
    }
}

void FairModule::ConstructASCIIGeometry()
{
    LOG(warn) << "The method ConstructASCIIGeometry has to be implemented in the detector class which inherits from "
                 "FairModule";
}

//__________________________________________________________________________
Bool_t FairModule::IsSensitive(const std::string& name)
{
    //    LOG(warn) << "Implement IsSensitive in the detector class which inherits from FairModule";
    //    LOG(warn) << "For now calling the obsolete function CheckIfSensitive";
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    return CheckIfSensitive(name);
#pragma GCC diagnostic pop
}

void FairModule::ExpandNode(TGeoNode* fN)
{
    // FairGeoLoader* geoLoad = FairGeoLoader::Instance();
    // FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    // FairGeoMedia* Media =  geoFace->getMedia();
    // FairGeoBuilder* geobuild=geoLoad->getGeoBuilder();
    TGeoMatrix* Matrix = fN->GetMatrix();
    if (gGeoManager->GetListOfMatrices()->FindObject(Matrix)) {
        gGeoManager->GetListOfMatrices()->Remove(Matrix);
    }
    TGeoVolume* v1 = fN->GetVolume();
    TObjArray* NodeList = v1->GetNodes();
    for (Int_t Nod = 0; Nod < NodeList->GetEntriesFast(); Nod++) {
        TGeoNode* fNode = static_cast<TGeoNode*>(NodeList->At(Nod));
        TGeoMatrix* M = fNode->GetMatrix();
        // M->SetDefaultName();
        SetDefaultMatrixName(M);
        if (fNode->GetNdaughters() > 0) {
            ExpandNode(fNode);
        }
        TGeoVolume* v = fNode->GetVolume();
        AssignMediumAtImport(v);
        if (!gGeoManager->FindVolumeFast(v->GetName())) {
            LOG(debug2) << "Register Volume " << v->GetName();
            v->RegisterYourself();
        }
        if ((this->InheritsFrom("FairDetector")) && IsSensitive(v->GetName())) {
            LOG(debug2) << "Sensitive Volume " << v->GetName();
            AddSensitiveVolume(v);
        }
        v->GetShape()->AfterStreamer();
    }
}

void FairModule::SetDefaultMatrixName(TGeoMatrix* matrix)
{
    // Copied from root TGeoMatrix::SetDefaultName() and modified (memory leak)
    // If no name was supplied in the ctor, the type of transformation is checked.
    // A letter will be prepended to the name :
    //   t - translation
    //   r - rotation
    //   s - scale
    //   c - combi (translation + rotation)
    //   g - general (tr+rot+scale)
    // The index of the transformation in gGeoManager list of transformations will
    // be appended.
    if (!gGeoManager) {
        return;
    }
    if (strlen(matrix->GetName())) {
        return;
    }
    char type = 'n';
    if (matrix->IsTranslation()) {
        type = 't';
    }
    if (matrix->IsRotation()) {
        type = 'r';
    }
    if (matrix->IsScale()) {
        type = 's';
    }
    if (matrix->IsCombi()) {
        type = 'c';
    }
    if (matrix->IsGeneral()) {
        type = 'g';
    }
    TObjArray* matrices = gGeoManager->GetListOfMatrices();
    Int_t index = 0;
    if (matrices) {
        index = matrices->GetEntriesFast() - 1;
    }
    matrix->SetName(Form("%c%i", type, index));
}

void FairModule::AssignMediumAtImport(TGeoVolume* v)
{

    /**
     * Assign medium to the the volume v, this has to be done in all cases:
     * case 1: For CAD converted volumes they have no mediums (only names)
     * case 2: TGeoVolumes, we need to be sure that the material is defined in this session
     */
    FairGeoMedia* Media = FairGeoLoader::Instance()->getGeoInterface()->getMedia();
    FairGeoBuilder* geobuild = FairGeoLoader::Instance()->getGeoBuilder();

    TGeoMedium* med1 = v->GetMedium();

    if (med1) {
        // In newer ROOT version also a TGeoVolumeAssembly has a material and medium.
        // This medium is called dummy and is automatically set when the geometry is constructed.
        // Since this material and medium is neither in the TGeoManager (at this point) nor in our
        // ASCII file we have to create it the same way it is done in TGeoVolume::CreateDummyMedium()
        // In the end the new medium and material has to be added to the TGeomanager, because this is
        // not done automatically when using the default constructor. For all other constructors the
        // newly created medium or material is added to the TGeomanger.
        // Create the medium and material only the first time.
        TString medName = static_cast<TString>(med1->GetName());
        if (medName.EqualTo("dummy") && nullptr == gGeoManager->GetMedium(medName)) {

            TGeoMaterial* dummyMaterial = new TGeoMaterial();
            dummyMaterial->SetName("dummy");

            TGeoMedium* dummyMedium = new TGeoMedium();
            dummyMedium->SetName("dummy");
            dummyMedium->SetMaterial(dummyMaterial);

            gGeoManager->GetListOfMedia()->Add(dummyMedium);
            gGeoManager->AddMaterial(dummyMaterial);
        }

        TGeoMaterial* mat1 = v->GetMaterial();
        TGeoMaterial* newMat = gGeoManager->GetMaterial(mat1->GetName());
        if (newMat == 0) {
            /**The Material is not defined in the TGeoManager, we try to create one if we have enough information about
             * it*/
            FairGeoMedium* FairMedium = Media->getMedium(mat1->GetName());
            if (!FairMedium) {
                LOG(fatal) << "Material " << mat1->GetName() << "is not defined in ASCII file nor in Root file.";
                //     FairMedium=new FairGeoMedium(mat1->GetName());
                //      Media->addMedium(FairMedium);
            } else {

                Int_t nmed = geobuild->createMedium(FairMedium);
                v->SetMedium(gGeoManager->GetMedium(nmed));
                gGeoManager->SetAllIndex();
            }
        } else {
            /**Material is already available in the TGeoManager and we can set it */
            TGeoMedium* med2 = gGeoManager->GetMedium(mat1->GetName());
            v->SetMedium(med2);
        }
    } else {
        if (strcmp(v->ClassName(), "TGeoVolumeAssembly") != 0) {
            //[R.K.-3.3.08]  // When there is NO material defined, set it to avoid conflicts in Geant
            LOG(fatal) << "The volume " << v->GetName()
                       << "has no medium information and not an Assembly so we have to quit";
        }
    }
}

FairModule* FairModule::CloneModule() const
{
    Fatal("CloneModule", "Has to be overriden in multi-threading applications.");
    return 0;
}
