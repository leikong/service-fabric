// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace PyHost
{
	class PyInterpreter
	{
	public:
		//
		// C++ -> Python
		//
		static void Execute(
            std::string const & moduleName, 
            std::string const & funcName, 
            std::vector<std::string> const & args);

	public:
		//
		// Python -> C++
		//
		using SetNodeIdOwnershipCallback = std::function<void(std::string const &, std::string const &)>;

		void Register_SetNodeIdOwnership(SetNodeIdOwnershipCallback const &);
	};
}
