#pragma once

BEGIN_TEE

class MZLoader
{
public:

	static void load();

protected:

	static bool readMzVersion(std::ifstream& fin, uint dataSize);
private:
};

END_TEE