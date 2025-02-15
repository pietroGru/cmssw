Test for the Alpaka SiStripClusterizerConditionsSoA is
```bash
scram b -j 16 runtests_SiStripClusterizerConditionsSoA_alpakaSerialSync
scram b -j 16 runtests_SiStripClusterizerConditionsSoA_alpakaCudaAsync
scram b -j 16 runtests_SiStripClusterizerConditionsSoA_alpakaROCmAsync
```