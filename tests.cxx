#include <v4d.h>

namespace v4d::tests {
	int SYSTEMS_SAMPLE_1(v4d::CoreInstance* v4dCore) {

		v4dCore->SetProjectName("Test Project");

		v4d::SystemsLoader systemsLoader(v4dCore);

		auto sampleSystem = systemsLoader.Load("sample");
		if (!sampleSystem) {
			LOG_ERROR("ERROR : Could not load sample system library")
			return 1;
		}

		LOAD_DLL_FUNC(sampleSystem, int, test1, int)
		if (!test1) {
			LOG_ERROR("Error getting symbol pointer for function test1 in sample system : " << LOAD_DLL_ERR)
			return 2;
		}
		
		int res = test1(4);
		if (res != 8) {
			LOG_ERROR("ERROR : test1 from sample system returned " << res << " instead of 8")
			return 3;
		}

		LOAD_DLL_FUNC(sampleSystem, void, test2)
		if (test2) {
			LOG_ERROR("ERROR: function test2 was supposed to cause an error because it does not exist")
			return 4;
		}

		return 0;
	}
}
