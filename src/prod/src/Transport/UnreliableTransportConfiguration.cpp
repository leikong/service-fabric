// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace Transport;
using namespace std;
using namespace Common;

void UnreliableTransportConfiguration::Initialize()
{
    LoadConfiguration(config_);
}

TimeSpan UnreliableTransportConfiguration::GetDelay(std::string const & source, std::string const & destination, std::string const & action)
{
    AcquireReadLock lock(specsTableLock_);

    int priority = -1;
    TimeSpan delay = TimeSpan::Zero;

    for (auto it = specifications_.begin(); it != specifications_.end(); ++it)
    {
        if ((*it)->GetPriority() > priority && (*it)->Match(source, destination, action))
        {
            priority = (*it)->GetPriority();
            delay = (*it)->GetDelay();
        }
    }

    return delay > MaxAllowedDelayInSeconds ? TimeSpan::MaxValue : delay;
}

TimeSpan ParseTimeSpan(string const & data)
{
    if (StringUtility::AreEqualCaseInsensitive(data, "Max"))
    {
        return TimeSpan::MaxValue;
    }

    return TimeSpan::FromSeconds(Common::Double_Parse(data));
}

bool UnreliableTransportConfiguration::AddSpecification(UnreliableTransportSpecificationUPtr && spec)
{
    RemoveSpecification(spec->GetName());

    AcquireExclusiveLock lock(specsTableLock_);
    specifications_.push_back(move(spec));

    return true;
}

bool UnreliableTransportConfiguration::AddSpecification(std::string const & name, std::string const & data)
{
    StringCollection params;
    StringUtility::Split<string>(data, params, " ");

    string source = (params.size() > 0 ? params[0] : string());
    string destination = (params.size() > 1 ? params[1] : string());
    string action = (params.size() > 2 ? params[2] : string());
    double probability = (params.size() > 3 ? Common::Double_Parse(params[3]) : 1.0);
    TimeSpan delay = (params.size() > 4 ? ParseTimeSpan(params[4]) : TimeSpan::MaxValue);
    TimeSpan delaySpan = (params.size() > 5 ? ParseTimeSpan(params[5]) : TimeSpan::Zero);
    int priority = (params.size() > 6 ? Common::Int32_Parse(params[6]) : 0);
    int applyCount = (params.size() > 7 ? Common::Int32_Parse(params[7]) : -1);

    return AddSpecification(make_unique<UnreliableTransportSpecification>(name, source, destination, action, probability, delay, delaySpan, priority, applyCount));
}

bool UnreliableTransportConfiguration::RemoveSpecification(std::string const & name)
{
    AcquireExclusiveLock lock(specsTableLock_);
    auto it = remove_if(specifications_.begin(), specifications_.end(),
        [&name](UnreliableTransportSpecificationUPtr const & spec) { return spec->GetName() == name; });

    if (it == specifications_.end())
    {
        return false;
    }

    specifications_.erase(it, specifications_.end());
    return true;
}

void UnreliableTransportConfiguration::LoadConfiguration(Config & config)
{
    string section("UnreliableTransport");
    StringCollection specKeys;
    config.GetKeys(section, specKeys);

    for(string const & key : specKeys)
    {
        bool isEncrypted = false;
        string value = config.ReadString(section, key, isEncrypted);

        ASSERT_IF(
            isEncrypted, 
            "value of SectionName={0} and KeyName={1} is encrypted. Encryption is not supported on this parameter", section, key);

        if (!AddSpecification(key, value))
        {
            UnreliableTransport::WriteWarning("Config", "Config for {0} is invalid", key);
        }
    }

    UnreliableTransport::WriteInfo("Config", "Loaded config: {0}", *this);
}

void UnreliableTransportConfiguration::WriteTo(Common::TextWriter & w, Common::FormatOptions const &) const
{
    for (auto it = specifications_.begin(); it != specifications_.end(); ++it)
    {
        w.Write("{0}\n", *it);
    }
}

DEFINE_SINGLETON_COMPONENT_CONFIG(UnreliableTransportConfiguration)
