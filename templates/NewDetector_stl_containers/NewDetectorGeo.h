/********************************************************************************
 * Copyright (C) 2014-2024 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#ifndef NEWDETECTORGEO_H
#define NEWDETECTORGEO_H

#include "FairGeoSet.h"

class NewDetectorGeo : public FairGeoSet
{
  public:
    NewDetectorGeo()
        : FairGeoSet("NewDetector", 10)
    {}
    ~NewDetectorGeo() override = default;

    ClassDefOverride(NewDetectorGeo, 1);
};

#endif
