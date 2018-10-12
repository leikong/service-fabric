// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------


#include "stdafx.h"

namespace Common
{
    namespace X509FindType
    {
        Common::ErrorCode FromPublic(::FABRIC_X509_FIND_TYPE publicEnum, Enum & internalEnum)
        {
            switch (publicEnum)
            {
            case ::FABRIC_X509_FIND_TYPE_FINDBYTHUMBPRINT:
                internalEnum = FindByThumbprint;
                return Common::ErrorCode::Success();

            case ::FABRIC_X509_FIND_TYPE_FINDBYSUBJECTNAME:
                internalEnum = FindBySubjectName;
                return Common::ErrorCode::Success();

            case ::FABRIC_X509_FIND_TYPE_FINDBYEXTENSION:
                internalEnum = FindByExtension;
                return ErrorCode::Success();

            default:
                return Common::ErrorCodeValue::InvalidX509FindType;
            }
        }

        ::FABRIC_X509_FIND_TYPE ToPublic(Enum internalEnum)
        {
            switch(internalEnum)
            {
            case FindByThumbprint:
                return ::FABRIC_X509_FIND_TYPE_FINDBYTHUMBPRINT;

            case FindBySubjectName:
            case FindByCommonName:
                return ::FABRIC_X509_FIND_TYPE_FINDBYSUBJECTNAME;

            case FindByExtension:
                return ::FABRIC_X509_FIND_TYPE_FINDBYEXTENSION;

            default:
                Common::Assert::CodingError("Cannot convert X509FindType {0}", internalEnum);
            }
        }

        void WriteToTextWriter(Common::TextWriter & w, Enum const & e)
        {
            switch (e)
            {
            case FindByThumbprint: w << "FindByThumbprint"; return;

            case FindBySubjectName: w << "FindBySubjectName"; return;

            case FindByExtension: w << "FindByExtension"; return;

            case FindByCommonName: w << "FindByCommonName"; return;

            default:
                w << "X509FindType(" << static_cast<int>(e) << ')';
            }
        }

        Common::ErrorCode Parse(std::string const & inputString, __out Enum & result)
        {
            if (Common::StringUtility::AreEqualCaseInsensitive(inputString, "FindByThumbprint"))
            {
                result = Enum::FindByThumbprint;
                return Common::ErrorCode::Success();
            }

            if (Common::StringUtility::AreEqualCaseInsensitive(inputString, "FindBySubjectName"))
            {
                result = Enum::FindBySubjectName;
                return Common::ErrorCode::Success();
            }

            if (StringUtility::AreEqualCaseInsensitive(inputString, "FindByExtension"))
            {
                result = Enum::FindByExtension;
                return ErrorCode::Success();
            }

            return Common::ErrorCodeValue::InvalidX509FindType;
        }
		std::string EnumToString(Enum value)
		{
			switch (value)
			{
			case FindBySubjectName:
				return "FindBySubjectName";
			case FindByCommonName:
				return "FindByCommonName";
			default:
				return "FindByThumbprint";
			}
		}
    }
}
