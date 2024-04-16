/********************************************************************************
 *   Copyright (C) 2024 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH     *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#include "tests/geobase/checks_FairGeoSet.h"

#include <FairTutorialDet1Geo.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FairTutorialDet1Geo")
{
    FairTutorialDet1Geo geo;

    fairroot::tests::checkGeoSetNamingConventions(geo, "tutdet", 10);
}
