# Changelog

## [1.4.0](https://github.com/AmbiqAI/neuralSPOT/compare/v1.3.0...v1.4.0) (2026-03-24)


### Features

* AutoDeploy fixes for AP3/AP4 ([#296](https://github.com/AmbiqAI/neuralSPOT/issues/296)) ([026f967](https://github.com/AmbiqAI/neuralSPOT/commit/026f96792fb380f0f4b95e0fe0141d8975cb8dc9))
* Pull in latest TFLM and HeliaRT. ([4681d47](https://github.com/AmbiqAI/neuralSPOT/commit/4681d47025dbe27f602005691866961504094c36))
* Update TFLM v1.7 ([a5de31f](https://github.com/AmbiqAI/neuralSPOT/commit/a5de31f2b80244fcc8596cea95e0ddbe8ad32b91))
* Use compatible LiteRT for AOT. ([57b959c](https://github.com/AmbiqAI/neuralSPOT/commit/57b959c65f34c5183de43d3c34f7e0cbdfe32cc2))


### Bug Fixes

* stabilize AutoDeploy PMU capture and chunking state ([#299](https://github.com/AmbiqAI/neuralSPOT/issues/299)) ([308c4e6](https://github.com/AmbiqAI/neuralSPOT/commit/308c4e631b52511ac9affb4a9261ec51d35b7b52))


### Documentation

* update HeliaAOT invocation guidance for uv --with-editable ([3222d17](https://github.com/AmbiqAI/neuralSPOT/commit/3222d17f8085fdf950b4bdad1be2843152861995))
* update tensorflow runtime workflow and fix AOT config path ([df988cc](https://github.com/AmbiqAI/neuralSPOT/commit/df988ccb4acf83f0182dfb11f6faa3618306a6b4))

## [1.3.0](https://github.com/AmbiqAI/neuralSPOT/compare/v1.2.0...v1.3.0) (2026-02-03)


### Features

* Add GHA workflow for GitHub Pages  ([#292](https://github.com/AmbiqAI/neuralSPOT/issues/292)) ([d585bcb](https://github.com/AmbiqAI/neuralSPOT/commit/d585bcb5e117217d3e4081e9b5b7bc2575cf0e69))
* Make LiteRT optional for AutoDeploy. ([dff50ba](https://github.com/AmbiqAI/neuralSPOT/commit/dff50bafe06f295ccc910e7bf11be366babca1e2))
* Make LiteRT optional for AutoDeploy. ([a027a63](https://github.com/AmbiqAI/neuralSPOT/commit/a027a63b9117228ef1946d324802a33b5847ef1e))


### Bug Fixes

* Add missing include. ([da16606](https://github.com/AmbiqAI/neuralSPOT/commit/da1660682a6d5c7ec8e643372e558c92c170d334))
* Add missing include. ([dfd4cd6](https://github.com/AmbiqAI/neuralSPOT/commit/dfd4cd62a950398475e0008034f52c1a67043837))
* Correct flattening input/output arrays. ([b1dcb2e](https://github.com/AmbiqAI/neuralSPOT/commit/b1dcb2e62104962b5eb22c78799059c46abdc3ba))
* Correct flattening input/output arrays. ([4e1d6ea](https://github.com/AmbiqAI/neuralSPOT/commit/4e1d6ea4813de0214dbe10678762286050fc7b3f))
* Enforce docs/index.md is used at root ([#293](https://github.com/AmbiqAI/neuralSPOT/issues/293)) ([247edba](https://github.com/AmbiqAI/neuralSPOT/commit/247edba0a6b8e56f10995b6400824f17aeeb7bc7))
* Include padding when computing max arena size. ([09a1652](https://github.com/AmbiqAI/neuralSPOT/commit/09a165218a4b902c73652ea1750f7c1af44e42b5))
* Rename SVD to svd to work under Linux. ([8e22a00](https://github.com/AmbiqAI/neuralSPOT/commit/8e22a0002f37df892d41aa4aa9e2ef133b72d58a))
* Rename SVD to svd to work under Linux. ([4a9f315](https://github.com/AmbiqAI/neuralSPOT/commit/4a9f3153e3ad5c2f2a1062476798d2d7fab7753a))
* Support python 3.10+ ([45f6032](https://github.com/AmbiqAI/neuralSPOT/commit/45f6032e81bf7ae6245b260628a25d796a004643))
* Use __package__ for locating resources. ([50309cd](https://github.com/AmbiqAI/neuralSPOT/commit/50309cdfbf8d6951c5978492fb29617715fbd466))
