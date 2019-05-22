// **********************************************************************
//
// Copyright (c) 2003-2018 ZeroC, Inc. All rights reserved.
//
// **********************************************************************

#pragma once

module record
{
    sequence<long> recordIds;

    interface idGenerator
    {
        idempotent long getNextId();
		idempotent recordIds getNextIds(int idNum);
    };
};
