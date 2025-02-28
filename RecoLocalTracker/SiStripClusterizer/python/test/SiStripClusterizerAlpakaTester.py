#################################################################################################################################### 
#################################################################################################################################### 
#################################################################################################################################### 
from SiStripClusterizerAlpakaTester_init_hltCfg import *
#################################################################################################################################### 
#################################################################################################################################### 
#################################################################################################################################### 

####### Produce ES for the alpaka clusterizer #######
# Produce the SiStripClusterizerConditionsHost object
process.siStripClusterizerConditionsESProducerAlpaka = cms.ESProducer("SiStripClusterizerConditionsESProducerAlpaka@alpaka",
    QualityLabel = cms.string(""),
    Label = cms.string(""),
)
#################################################################################################################################### 
#################################################################################################################################### 


####### HLTL1UnpackerSequence
process.hltGtStage2Digis = cms.EDProducer( "L1TRawToDigi",
    FedIds = cms.vint32( 1404 ),
    Setup = cms.string( "stage2::GTSetup" ),
    FWId = cms.uint32( 0 ),
    DmxFWId = cms.uint32( 0 ),
    FWOverride = cms.bool( False ),
    TMTCheck = cms.bool( True ),
    CTP7 = cms.untracked.bool( False ),
    MTF7 = cms.untracked.bool( False ),
    InputLabel = cms.InputTag( "rawDataCollector" ),
    lenSlinkHeader = cms.untracked.int32( 8 ),
    lenSlinkTrailer = cms.untracked.int32( 8 ),
    lenAMCHeader = cms.untracked.int32( 8 ),
    lenAMCTrailer = cms.untracked.int32( 0 ),
    lenAMC13Header = cms.untracked.int32( 8 ),
    lenAMC13Trailer = cms.untracked.int32( 8 ),
    debug = cms.untracked.bool( False ),
    MinFeds = cms.uint32( 0 )
)
process.hltGtStage2ObjectMap = cms.EDProducer( "L1TGlobalProducer",
    MuonInputTag = cms.InputTag( 'hltGtStage2Digis','Muon' ),
    MuonShowerInputTag = cms.InputTag( 'hltGtStage2Digis','MuonShower' ),
    EGammaInputTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    TauInputTag = cms.InputTag( 'hltGtStage2Digis','Tau' ),
    JetInputTag = cms.InputTag( 'hltGtStage2Digis','Jet' ),
    EtSumInputTag = cms.InputTag( 'hltGtStage2Digis','EtSum' ),
    EtSumZdcInputTag = cms.InputTag( 'hltGtStage2Digis','EtSumZDC' ),
    CICADAInputTag = cms.InputTag( 'hltGtStage2Digis','CICADAScore' ),
    ExtInputTag = cms.InputTag( "hltGtStage2Digis" ),
    AlgoBlkInputTag = cms.InputTag( "hltGtStage2Digis" ),
    GetPrescaleColumnFromData = cms.bool( False ),
    AlgorithmTriggersUnprescaled = cms.bool( True ),
    RequireMenuToMatchAlgoBlkInput = cms.bool( True ),
    AlgorithmTriggersUnmasked = cms.bool( True ),
    useMuonShowers = cms.bool( True ),
    produceAXOL1TLScore = cms.bool( False ),
    resetPSCountersEachLumiSec = cms.bool( True ),
    semiRandomInitialPSCounters = cms.bool( False ),
    ProduceL1GtDaqRecord = cms.bool( True ),
    ProduceL1GtObjectMapRecord = cms.bool( True ),
    EmulateBxInEvent = cms.int32( 1 ),
    L1DataBxInEvent = cms.int32( 5 ),
    AlternativeNrBxBoardDaq = cms.uint32( 0 ),
    BstLengthBytes = cms.int32( -1 ),
    PrescaleSet = cms.uint32( 1 ),
    Verbosity = cms.untracked.int32( 0 ),
    PrintL1Menu = cms.untracked.bool( False ),
    TriggerMenuLuminosity = cms.string( "startup" )
)
####### /HLTL1UnpackerSequence #######


####### HLTBeamSpot
process.hltOnlineMetaDataDigis = cms.EDProducer( "OnlineMetaDataRawToDigi",
    onlineMetaDataInputLabel = cms.InputTag( "rawDataCollector" )
)
process.hltOnlineBeamSpot = cms.EDProducer( "BeamSpotOnlineProducer",
    changeToCMSCoordinates = cms.bool( False ),
    maxZ = cms.double( 40.0 ),
    setSigmaZ = cms.double( 0.0 ),
    beamMode = cms.untracked.uint32( 11 ),
    src = cms.InputTag( "" ),
    gtEvmLabel = cms.InputTag( "" ),
    maxRadius = cms.double( 2.0 ),
    useTransientRecord = cms.bool( True )
)
####### /HLTBeamSpot #######


####### HLTBeginSequence
process.hltTriggerType = cms.EDFilter( "HLTTriggerTypeFilter",
    SelectedTriggerType = cms.int32( 1 )
)


####### HLTDoLocalStripSequence
process.hltSiStripExcludedFEDListProducer = cms.EDProducer( "SiStripExcludedFEDListProducer",
    ProductLabel = cms.InputTag( "rawDataCollector" )
)
process.hltSiStripRawToClustersFacility = cms.EDProducer( "SiStripClusterizerFromRaw",
    ProductLabel = cms.InputTag( "rawDataCollector" ),
    ConditionsLabel = cms.string( "" ),
    onDemand = cms.bool( True ),
    DoAPVEmulatorCheck = cms.bool( False ),
    LegacyUnpacker = cms.bool( False ),
    HybridZeroSuppressed = cms.bool( False ),
    Clusterizer = cms.PSet( 
      ConditionsLabel = cms.string( "" ),
      ClusterThreshold = cms.double( 5.0 ),
      SeedThreshold = cms.double( 3.0 ),
      Algorithm = cms.string( "ThreeThresholdAlgorithm" ),
      ChannelThreshold = cms.double( 2.0 ),
      MaxAdjacentBad = cms.uint32( 0 ),
      setDetId = cms.bool( True ),
      MaxSequentialHoles = cms.uint32( 0 ),
      RemoveApvShots = cms.bool( True ),
      clusterChargeCut = cms.PSet(  refToPSet_ = cms.string( "HLTSiStripClusterChargeCutNone" ) ),
      MaxSequentialBad = cms.uint32( 1 )
    ),
    Algorithms = cms.PSet( 
      Use10bitsTruncation = cms.bool( False ),
      CommonModeNoiseSubtractionMode = cms.string( "Median" ),
      useCMMeanMap = cms.bool( False ),
      TruncateInSuppressor = cms.bool( True ),
      doAPVRestore = cms.bool( False ),
      SiStripFedZeroSuppressionMode = cms.uint32( 4 ),
      PedestalSubtractionFedMode = cms.bool( True )
    )
)
## alpaka-dependent module
process.hltSiStripRawToClustersFacilityAlpaka = cms.EDProducer("SiStripRawToCluster@alpaka",
    ProductLabel = cms.InputTag("rawDataCollector"),
    ConditionsLabel = cms.string(""),
    CablingConditionsLabel = cms.string("")
)
##
process.hltMeasurementTrackerEvent = cms.EDProducer( "MeasurementTrackerEventProducer",
    measurementTracker = cms.string( "hltESPMeasurementTracker" ),
    skipClusters = cms.InputTag( "" ),
    pixelClusterProducer = cms.string( "hltSiPixelClusters" ),
    stripClusterProducer = cms.string( "hltSiStripRawToClustersFacility" ),
    Phase2TrackerCluster1DProducer = cms.string( "" ),
    vectorHits = cms.InputTag( "" ),
    vectorHitsRej = cms.InputTag( "" ),
    inactivePixelDetectorLabels = cms.VInputTag( 'hltSiPixelDigiErrors' ),
    badPixelFEDChannelCollectionLabels = cms.VInputTag( 'hltSiPixelDigiErrors' ),
    pixelCablingMapLabel = cms.string( "" ),
    inactiveStripDetectorLabels = cms.VInputTag( 'hltSiStripExcludedFEDListProducer' ),
    switchOffPixelsIfEmpty = cms.bool( True )
)
####### /HLTDoLocalStripSequence #######



####### HLTEndSequence
process.hltBoolEnd = cms.EDFilter( "HLTBool",
    result = cms.bool( True )
)
####### /HLTEndSequence #######




#################################################################################################################################### 
#################################################################################################################################### 
#################################################################################################################################### 




process.HLTL1UnpackerSequence = cms.Sequence( process.hltGtStage2Digis + process.hltGtStage2ObjectMap )
process.HLTBeamSpot = cms.Sequence( process.hltOnlineMetaDataDigis + process.hltOnlineBeamSpot )
process.HLTBeginSequence = cms.Sequence( process.hltTriggerType + process.HLTL1UnpackerSequence + process.HLTBeamSpot )
process.HLTDoLocalStripSequence = cms.Sequence( process.hltSiStripExcludedFEDListProducer + process.hltSiStripRawToClustersFacility
                                               + process.hltSiStripRawToClustersFacilityAlpaka
                                               + process.hltMeasurementTrackerEvent )
process.HLTEndSequence = cms.Sequence( process.hltBoolEnd )

process.MC_ReducedIterativeTracking_v22 = cms.Path( process.HLTBeginSequence + process.HLTDoLocalStripSequence + process.HLTEndSequence )

process.schedule = cms.Schedule( *(process.MC_ReducedIterativeTracking_v22, ))

#################################################################################################################################### 
#################################################################################################################################### 


# source module (EDM inputs)
process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/data/user/pgrutta/work/testPerfSiC/dataset/25361ab0-9b06-4a66-b1f2-765269984153.root',
    ),
    inputCommands = cms.untracked.vstring(
        'keep *'
    )
)

# run the Full L1T emulator, then repack the data into a new RAW collection, to be used by the HLT
from HLTrigger.Configuration.CustomConfigs import L1REPACK
process = L1REPACK(process,"uGT")

# add a single "keep *" output
process.hltOutputMinimal = cms.OutputModule( "PoolOutputModule",
    fileName = cms.untracked.string( "hltOutputMinimal.root" ),
    fastCloning = cms.untracked.bool( False ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string( 'AOD' ),
        filterName = cms.untracked.string( '' )
    ),
    outputCommands = cms.untracked.vstring( 'drop *',
        # 'keep edmTriggerResults_*_*_*',
        # 'keep triggerTriggerEvent_*_*_*',
        # 'keep GlobalAlgBlkBXVector_*_*_*',
        # 'keep GlobalExtBlkBXVector_*_*_*',
        # 'keep l1tEGammaBXVector_*_EGamma_*',
        # 'keep l1tEtSumBXVector_*_EtSum_*',
        # 'keep l1tJetBXVector_*_Jet_*',
        # 'keep l1tMuonBXVector_*_Muon_*',
        # 'keep l1tTauBXVector_*_Tau_*',
    )
)
process.MinimalOutput = cms.FinalPath( process.hltOutputMinimal )
process.schedule.append( process.MinimalOutput )

# limit the number of events to be processed
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 1 )
)

# enable TrigReport, TimeReport and MultiThreading
process.options.wantSummary = True
process.options.numberOfThreads = 32
process.options.numberOfStreams = 1


# override the GlobalTag, connection string and pfnPrefix
if 'GlobalTag' in process.__dict__:
    from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:phase1_2025_realistic', conditions = 'L1Menu_Collisions2024_v1_3_0_xml,L1TUtmTriggerMenuRcd,,,9999-12-31 23:59:59.000')

# show summaries from trigger analysers used at HLT
if 'MessageLogger' in process.__dict__:
    # process.MessageLogger.TriggerSummaryProducerAOD = cms.untracked.PSet()
    # process.MessageLogger.L1GtTrigReport = cms.untracked.PSet()
    # process.MessageLogger.L1TGlobalSummary = cms.untracked.PSet()
    # process.MessageLogger.HLTrigReport = cms.untracked.PSet()
    # process.MessageLogger.FastReport = cms.untracked.PSet()
    # process.MessageLogger.ThroughputService = cms.untracked.PSet()
    pass


# # load the DQMStore and DQMRootOutputModule
# process.load( "DQMServices.Core.DQMStore_cfi" )

# process.dqmOutput = cms.OutputModule("DQMRootOutputModule",
#     fileName = cms.untracked.string("DQMIO.root")
# )

# process.DQMOutput = cms.FinalPath( process.dqmOutput )
# process.schedule.append( process.DQMOutput )

# add specific customizations
_customInfo = {}
_customInfo['menuType'  ]= "GRun"
_customInfo['globalTags']= {}
_customInfo['globalTags'][True ] = "auto:run3_hlt_GRun"
_customInfo['globalTags'][False] = "auto:run3_mc_GRun"
_customInfo['inputFiles']={}
_customInfo['inputFiles'][True]  = "file:RelVal_Raw_GRun_DATA.root"
_customInfo['inputFiles'][False] = "file:RelVal_Raw_GRun_MC.root"
_customInfo['maxEvents' ]=  1
_customInfo['globalTag' ]= "auto:phase1_2025_realistic"
_customInfo['inputFile' ]=  ['file:/data/user/pgrutta/work/testPerfSiC/dataset/25361ab0-9b06-4a66-b1f2-765269984153.root']
_customInfo['realData'  ]=  False

from HLTrigger.Configuration.customizeHLTforALL import customizeHLTforAll
process = customizeHLTforAll(process,"GRun",_customInfo)

from HLTrigger.Configuration.customizeHLTforCMSSW import customizeHLTforCMSSW
process = customizeHLTforCMSSW(process,"GRun")

# Eras-based customisations
from HLTrigger.Configuration.Eras import modifyHLTforEras
modifyHLTforEras(process)
