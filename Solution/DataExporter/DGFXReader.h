#pragma once
#include <FBXFactory.h>
#include "IReader.h"

class DGFXReader : public IReader
{
public:
	DGFXReader();
	~DGFXReader();

	void ReadFile(const std::string& aFilePath) override;

private:
	Prism::FBXFactory myFactory;
	CU::GrowingArray<std::string> myErrors;
	CU::GrowingArray<std::string> myConvertedFiles;
};

