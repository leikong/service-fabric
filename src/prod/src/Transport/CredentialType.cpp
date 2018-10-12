// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------


#include "stdafx.h"

namespace Transport
{
    namespace CredentialType
    {
        void WriteToTextWriter(Common::TextWriter & w, Enum const & e)
        {
            switch (e)
            {
                case None: w << "None"; return;
                case X509: w << "X509"; return;
                case Windows: w << "Windows"; return;
            }

            w << "CredentialType(" << static_cast<int>(e) << ')';
        }

        bool TryParse(std::string const & inputString, __out Enum & result)
        {
            const char * providerPtr = inputString.c_str();
            if (Common::StringUtility::AreEqualCaseInsensitive(providerPtr, "None"))
            {
                result = Enum::None;
                return true;
            }

            if (Common::StringUtility::AreEqualCaseInsensitive(providerPtr, "X509"))
            {
                result = Enum::X509;
                return true;
            }

            if (Common::StringUtility::AreEqualCaseInsensitive(providerPtr, "Windows"))
            {
                result = Enum::Windows;
                return true;
            }

            return false;
        }
    }
}
