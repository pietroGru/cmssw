import FWCore.ParameterSet.Config as cms    

process = cms.Process("SiStripClusterizerAlpakaTest")
process.add_(cms.Service("MessageLogger"))

process.load('Configuration.StandardSequences.Accelerators_cff')
process.load("HeterogeneousCore.AlpakaCore.ProcessAcceleratorAlpaka_cfi")
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))

# process.source = cms.Source("EmptySource")

# source module (EDM inputs)
process.source = cms.Source(
  "PoolSource",
  fileNames = cms.untracked.vstring('file:/data/user/pgrutta/work/testPerfSiC/dataset/25361ab0-9b06-4a66-b1f2-765269984153.root'),
  inputCommands = cms.untracked.vstring('keep *')
)

# process.SiStripNoisesRcdSource = cms.ESSource("EmptyESSource", recordName = cms.string("SiStripNoisesRcd"), iovIsRunNotTime = cms.bool(True), firstValid = cms.vuint32(1))
# process.SiStripGainRcdSource = cms.ESSource("EmptyESSource", recordName = cms.string("SiStripGainRcd"), iovIsRunNotTime = cms.bool(True), firstValid = cms.vuint32(1))
# process.SiStripQualityRcdSource = cms.ESSource("EmptyESSource", recordName = cms.string("SiStripQualityRcd"), iovIsRunNotTime = cms.bool(True), firstValid = cms.vuint32(1))

process.HLTConfigVersion = cms.PSet(tableName = cms.string("/dev/CMSSW_14_2_0/GRun/V11"))

process.GlobalParametersRcdSource = cms.ESSource( "EmptyESSource",
    recordName = cms.string( "L1TGlobalParametersRcd" ),
    iovIsRunNotTime = cms.bool( True ),
    firstValid = cms.vuint32( 1 )
)
process.GlobalTag = cms.ESSource( "PoolDBESSource",
    DBParameters = cms.PSet( 
      connectionRetrialTimeOut = cms.untracked.int32( 60 ),
      idleConnectionCleanupPeriod = cms.untracked.int32( 10 ),
      enableReadOnlySessionOnUpdateConnection = cms.untracked.bool( False ),
      enablePoolAutomaticCleanUp = cms.untracked.bool( False ),
      messageLevel = cms.untracked.int32( 0 ),
      authenticationPath = cms.untracked.string( "." ),
      connectionRetrialPeriod = cms.untracked.int32( 10 ),
      connectionTimeOut = cms.untracked.int32( 0 ),
      enableConnectionSharing = cms.untracked.bool( True )
    ),
    connect = cms.string( "frontier://FrontierProd/CMS_CONDITIONS" ),
    globaltag = cms.string( "None" ),
    snapshotTime = cms.string( "" ),
    toGet = cms.VPSet( 
      cms.PSet(  refreshTime = cms.uint64( 2 ),
        record = cms.string( "BeamSpotOnlineLegacyObjectsRcd" )
      ),
      cms.PSet(  refreshTime = cms.uint64( 2 ),
        record = cms.string( "BeamSpotOnlineHLTObjectsRcd" )
      ),
      cms.PSet(  refreshTime = cms.uint64( 40 ),
        record = cms.string( "LHCInfoPerLSRcd" )
      ),
      cms.PSet(  refreshTime = cms.uint64( 40 ),
        record = cms.string( "LHCInfoPerFillRcd" )
      )
    ),
    DumpStat = cms.untracked.bool( False ),
    ReconnectEachRun = cms.untracked.bool( True ),
    RefreshAlways = cms.untracked.bool( False ),
    RefreshEachRun = cms.untracked.bool( True ),
    RefreshOpenIOVs = cms.untracked.bool( False ),
    pfnPostfix = cms.untracked.string( "" ),
    pfnPrefix = cms.untracked.string( "" )
)

# override the GlobalTag, connection string and pfnPrefix
if 'GlobalTag' in process.__dict__:
    from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:phase1_2025_realistic', conditions = 'L1Menu_Collisions2024_v1_3_0_xml,L1TUtmTriggerMenuRcd,,,9999-12-31 23:59:59.000')

process.siStripBackPlaneCorrectionDepESProducer = cms.ESProducer( "SiStripBackPlaneCorrectionDepESProducer",
  LatencyRecord = cms.PSet( 
    label = cms.untracked.string( "" ),
    record = cms.string( "SiStripLatencyRcd" )
  ),
  BackPlaneCorrectionPeakMode = cms.PSet( 
    label = cms.untracked.string( "peak" ),
    record = cms.string( "SiStripBackPlaneCorrectionRcd" )
  ),
  BackPlaneCorrectionDeconvMode = cms.PSet( 
    label = cms.untracked.string( "deconvolution" ),
    record = cms.string( "SiStripBackPlaneCorrectionRcd" )
  )
)
process.siStripLorentzAngleDepESProducer = cms.ESProducer( "SiStripLorentzAngleDepESProducer",
  LatencyRecord = cms.PSet( 
    label = cms.untracked.string( "" ),
    record = cms.string( "SiStripLatencyRcd" )
  ),
  LorentzAnglePeakMode = cms.PSet( 
    label = cms.untracked.string( "peak" ),
    record = cms.string( "SiStripLorentzAngleRcd" )
  ),
  LorentzAngleDeconvMode = cms.PSet( 
    label = cms.untracked.string( "deconvolution" ),
    record = cms.string( "SiStripLorentzAngleRcd" )
  )
)
process.sistripconn = cms.ESProducer( "SiStripConnectivity" )
process.trackerTopology = cms.ESProducer( "TrackerTopologyEP",
  appendToDataLabel = cms.string( "" )
)
process.SiStripRegionConnectivity = cms.ESProducer( "SiStripRegionConnectivity",
  EtaDivisions = cms.untracked.uint32( 20 ),
  PhiDivisions = cms.untracked.uint32( 20 ),
  EtaMax = cms.untracked.double( 2.5 )
)

process.SiStripRecHitMatcherESProducer = cms.ESProducer( "SiStripRecHitMatcherESProducer",
  ComponentName = cms.string( "StandardMatcher" ),
  NSigmaInside = cms.double( 3.0 ),
  PreFilter = cms.bool( False )
)

process.SiStripQualityESProducer = cms.ESProducer( "SiStripQualityESProducer",
  appendToDataLabel = cms.string( "" ),
  ListOfRecordToMerge = cms.VPSet( 
    cms.PSet(  record = cms.string( "SiStripDetVOffRcd" ),
      tag = cms.string( "" )
    ),
    cms.PSet(  record = cms.string( "SiStripDetCablingRcd" ),
      tag = cms.string( "" )
    ),
    cms.PSet(  record = cms.string( "SiStripBadChannelRcd" ),
      tag = cms.string( "" )
    ),
    cms.PSet(  record = cms.string( "SiStripBadFiberRcd" ),
      tag = cms.string( "" )
    ),
    cms.PSet(  record = cms.string( "SiStripBadModuleRcd" ),
      tag = cms.string( "" )
    )
  ),
  ReduceGranularity = cms.bool( False ),
  ThresholdForReducedGranularity = cms.double( 0.3 ),
  PrintDebugOutput = cms.bool( False ),
  UseEmptyRunInfo = cms.bool( False )
)

process.SiStripGainESProducer = cms.ESProducer( "SiStripGainESProducer",
  appendToDataLabel = cms.string( "" ),
  printDebug = cms.untracked.bool( False ),
  AutomaticNormalization = cms.bool( False ),
  APVGain = cms.VPSet( 
    cms.PSet(  NormalizationFactor = cms.untracked.double( 1.0 ),
      Label = cms.untracked.string( "" ),
      Record = cms.string( "SiStripApvGainRcd" )
    ),
    cms.PSet(  NormalizationFactor = cms.untracked.double( 1.0 ),
      Label = cms.untracked.string( "" ),
      Record = cms.string( "SiStripApvGain2Rcd" )
    )
  )
)



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
    ProductLabel = cms.InputTag("rawDataCollector"),
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
    # process.SiStripNoisesRcdSource,
    # process.SiStripGainRcdSource,
    # process.SiStripQualityRcdSource,
    process.standardConditions,
    process.siStripClusterizerConditionsESProducerAlpaka,
)

process.p1 = cms.Path(
    process.siStripClusterizererAlpaka,
    process.t
)

process.ep = cms.EndPath(process.output)