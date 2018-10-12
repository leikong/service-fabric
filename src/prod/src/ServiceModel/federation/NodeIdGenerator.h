// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Federation
{
    class NodeIdGenerator
    {
    public:
        static Common::ErrorCode GenerateFromString(std::string const & string, NodeId & nodeId, std::string const & rolesForWhichToUseV1generator, bool useV2NodeIdGenerator, std::string const & nodeIdGeneratorVersion);
        static Common::ErrorCode GenerateFromString(std::string const & string, NodeId & nodeId);
        static std::string GenerateNodeName(NodeId nodeId);
        static Common::GlobalString NodeIdNamePrefix;
        static Common::GlobalString PrefixPearson;
        static Common::GlobalString SuffixPearson;

    private:
        static Common::ErrorCode GenerateHashedNodeId(std::string const & string, NodeId & nodeId, std::string const & nodeIdGeneratorVersion);

        static Common::ErrorCode GenerateHash(std::string const & string, BYTE * hash, DWORD dWHashLen, std::string const & nodeIdGeneratorVersion);
        static Common::ErrorCode GenerateMD5Hash(std::string const & string, BYTE * hash, DWORD dWHashLen);
        static Common::ErrorCode GeneratePearsonHash(std::string const & string, BYTE * hash, DWORD dWHashLen);
        static bool UseV1NodeIdGenerator(std::string const & roleName, std::string const & rolesForWhichToUseV1generator);

        static Common::ErrorCode GenerateV4NodeId(std::string const & string, NodeId & nodeId, size_t index);
    };
}
