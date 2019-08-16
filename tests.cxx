namespace v4d::tests {
	int SYSTEMS_SAMPLE_1() {

		v4d::SystemsLoader systemsLoader;

		auto sampleSystem = systemsLoader.LoadSystem("sample");
		if (!sampleSystem) {
			LOG_ERROR("ERROR : Could not load sample system library")
			return 1;
		}

		LOAD_DLL_FUNC(sampleSystem, int, test1, int)
		
		int res = test1(4);
		if (res != 8) {
			LOG_ERROR("ERROR : test1 from sample system returned " << res << " instead of 8")
			return 2;
		}

		return 0;
	}
}
