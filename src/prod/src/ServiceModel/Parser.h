// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace ServiceModel
{
    struct Parser :
        Common::TextTraceComponent<Common::TraceTaskCodes::ServiceModel>
    {
    public:
/*
        static Common::ErrorCode ParseServicePackage(
            std::string const & fileName,
            __out ServicePackageDescription & servicePackage);

        static Common::ErrorCode ParseServiceManifest(
            std::string const & fileName,
            __out ServiceManifestDescription & serviceManifest);

        static Common::ErrorCode ParseConfigSettings(
            std::string const & fileName,
            __out Common::ConfigSettings & configSettings);

        static Common::ErrorCode ParseApplicationManifest(
            std::string const & fileName,
            __out ApplicationManifestDescription & applicationManifest);

        static Common::ErrorCode ParseApplicationPackage(
            std::string const & fileName,
            __out ApplicationPackageDescription & applicationPackage);

        static Common::ErrorCode ParseApplicationInstance(
            std::string const & fileName,
            __out ApplicationInstanceDescription & applicationInstance);

        static Common::ErrorCode ParseInfrastructureDescription(
            std::string const & fileName,
            __out InfrastructureDescription & infrastructureDescription);

        static Common::ErrorCode ParseTargetInformationFileDescription(
            std::string const & fileName,
            __out TargetInformationFileDescription & targetInformationFileDescription);

        static void ThrowInvalidContent(
            Common::XmlReaderUPtr const & xmlReader,
            std::string const & expectedContent,
            std::string const & actualContent);

        static void ThrowInvalidContent(
            Common::XmlReaderUPtr const & xmlReader,
            std::string const & expectedContent,
            std::string const & actualContent,
            std::string const & reason);

        static HRESULT ReadFromConfigurationPackage(
            __in Common::ComPointer<IFabricCodePackageActivationContext> codePackageActivationContextCPtr,
            __in Common::ComPointer<IFabricConfigurationPackage> configPackageCPtr,
            __in std::string const & sectionName,
            __in std::string const & hostName);
*/

        static Common::ErrorCode ReadSettingsValue(
            __in Common::ComPointer<IFabricConfigurationPackage> configPackageCPtr,
            __in std::string const & sectionName,
            __in std::string const & paramName,
            __out std::string & value,
            __out bool & hasValue);

        static Common::ErrorCode ReadSettingsValue(
            __in Common::ComPointer<IFabricConfigurationPackage> configPackageCPtr,
            __in std::string const & sectionName,
            __in std::string const & paramName,
            __out Common::TimeSpan & value,
            __out bool & hasValue);

        static Common::ErrorCode ReadSettingsValue(
            __in Common::ComPointer<IFabricConfigurationPackage> configPackageCPtr,
            __in std::string const & sectionName,
            __in std::string const & paramName,
            __out int64 & value,
            __out bool & hasValue);

        static Common::ErrorCode ReadSettingsValue(
            __in Common::ComPointer<IFabricConfigurationPackage> configPackageCPtr,
            __in std::string const & sectionName,
            __in std::string const & paramName,
            __out int & value,
            __out bool & hasValue);

        static Common::ErrorCode ReadSettingsValue(
            __in Common::ComPointer<IFabricConfigurationPackage> configPackageCPtr,
            __in std::string const & sectionName,
            __in std::string const & paramName,
            __out bool & value,
            __out bool & hasValue);

        static Common::ErrorCode ReadSettingsValue(
            __in Common::ComPointer<IFabricConfigurationPackage2> configPackageCPtr,
            __in std::string const & sectionName,
            __in std::string const & paramPrefix,
            __out std::map<string, string> & values,
            __out bool & hasValue);

/*
        static Common::ErrorCode IsServiceManifestFile(
            std::string const & fileName,
            __out bool & result);

        // Continuation tokens are string values passed into Service Fabric subsystems in order to aid with paging.
        // There are situations where continuationTokens are a compilation of different components, but are still passed
        // in as a string. In these cases, we want to take in a string and return a vector of separate parts
        // We use "+" as the designated separator OR a json object.
        // We check first to see if it's a json. If it's not, then we use "+" as a delimiter. Eventually, the "+" will be deprecated.
        //
        // Will return ArgumentNull error if the provided continuation token is empty.
        // Parser assumes that continuation token is URL encoded!
        template <class T>
        static Common::ErrorCode ParseContinuationToken(
            std::string const & continuationToken,
            __out T & continuationTokenObj);

    public:
        struct Utility
        {
            static void ReadPercentageAttribute(
            Common::XmlReaderUPtr const & xmlReader,
            std::string const & attrName,
            byte & value);
        };
*/

    private:
        template <typename ElementType>
        static Common::ErrorCode ParseElement(
            std::string const & fileName,
            std::string const & elementTypeName,
            __out ElementType & element);
    };
}
