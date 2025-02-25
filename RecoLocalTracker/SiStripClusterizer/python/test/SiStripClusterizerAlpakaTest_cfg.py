import FWCore.ParameterSet.Config as cms    

process = cms.Process("SiStripClusterizerAlpakaTest")
process.add_(cms.Service("MessageLogger"))

process.load('Configuration.StandardSequences.Accelerators_cff')
process.load("HeterogeneousCore.AlpakaCore.ProcessAcceleratorAlpaka_cfi")

process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.SiStripNoisesRcdSource = cms.ESSource("EmptyESSource", recordName = cms.string("SiStripNoisesRcd"), iovIsRunNotTime = cms.bool(True), firstValid = cms.vuint32(1))
process.SiStripGainRcdSource = cms.ESSource("EmptyESSource", recordName = cms.string("SiStripGainRcd"), iovIsRunNotTime = cms.bool(True), firstValid = cms.vuint32(1))
process.SiStripQualityRcdSource = cms.ESSource("EmptyESSource", recordName = cms.string("SiStripQualityRcd"), iovIsRunNotTime = cms.bool(True), firstValid = cms.vuint32(1))

process.SiStripNoisesRcdSource_clone = process.SiStripNoisesRcdSource.clone()
# Produce the SiStripClusterizerConditions object (with the FED connection lookup table, the strip-quality conditions, etc.)
process.standardConditions = cms.ESProducer("SiStripClusterizerConditionsESProducer",
    QualityLabel = cms.string(""),
    Label = cms.string("")
    )

# Produce the SiStripClusterizerConditionsHost object
process.siStripClusterizerConditionsESProducerAlpaka = cms.ESProducer("SiStripClusterizerConditionsESProducerAlpaka@alpaka",
    QualityLabel = cms.string(""),
    Label = cms.string(""),
)


process.siStripClusterizererAlpaka = cms.EDProducer("SiStripRawToCluster@alpaka",
    ProductLabel = cms.InputTag("", "rawFED"),
    ConditionsLabel = cms.string(""),
    CablingConditionsLabel = cms.string("")
)

# process.alpakaGlobalProducerE = cms.EDProducer("TestAlpakaGlobalProducerE@alpaka",
#     source = cms.InputTag("alpakaGlobalProducer")
# )

process.output = cms.OutputModule('PoolOutputModule',
    fileName = cms.untracked.string('testAlpaka.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        # 'keep *_alpakaGlobalProducer_*_*',
        # 'keep *_alpakaStreamProducer_*_*',
        # 'keep *_alpakaStreamSynchronizingProducer_*_*',
  )
)

process.t = cms.Task(
    process.standardConditions,
    process.siStripClusterizerConditionsESProducerAlpaka
    )

process.p1 = cms.Path(
    process.siStripClusterizererAlpaka,
    process.t
)

process.ep = cms.EndPath(process.output)