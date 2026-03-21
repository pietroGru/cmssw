
from collections.abc import Iterable
def assign(target, val):
    print(f"Customising {target}\n\tbefore", target)
    if isinstance(target, Iterable):
        target[2] = val
    else:
        target = val
    print("\tafter", target)

def customize_for_hp_tuning(process):
    print(f"Before customisation\n\t",end='')
    print(process.hltIter0PFlowTrackCutClassifier.dumpPython())
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.d0err, 0.003968462793686334)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.d0err_par, 0.000118143034192274)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.dr_exp, int(2.0))
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.dr_par1, 1.1563859327766106)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.dr_par2, 0.8431723014309169)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dz_par.dz_exp, int(2.6748437953689503))
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dz_par.dz_par1, 1.2501966670755764)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dz_par.dz_par2, 0.6042645354699189)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.maxChi2, 857.0958554215441)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.maxChi2n, 954.4857184737727)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.maxLostLayers, int(-0.0))
    assign(process.hltIter0PFlowTrackCutClassifier.mva.min3DLayers, int(-0.0))
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minLayers, int(0.0))
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minNVtxTrk, int(1.0))
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minNdof, 0.0)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minPixelHits, int(2.0))
    print(f"After customisation\n\t", end='')
    print(process.hltIter0PFlowTrackCutClassifier.dumpPython())
    return process

def customize_for_hp_tuning_minNVtxTrk(process):
    import FWCore.ParameterSet.Config as cms
    print(f"Before customisation\n\t",end='')
    print(process.hltIter0PFlowTrackCutClassifier.dumpPython())
    
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.d0err, 0.003968462793686334)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.d0err_par, 0.000118143034192274)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.dr_exp, int(2.0))
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.dr_par1, 1.1563859327766106)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.dr_par2, 0.8431723014309169)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dz_par.dz_exp, int(2.6748437953689503))
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dz_par.dz_par1, 1.2501966670755764)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dz_par.dz_par2, 0.6042645354699189)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.maxChi2, 857.0958554215441)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.maxChi2n, 954.4857184737727)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.maxLostLayers, int(-0.0))
    assign(process.hltIter0PFlowTrackCutClassifier.mva.min3DLayers, int(-0.0))
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minLayers, int(0.0))
    # assign(process.hltIter0PFlowTrackCutClassifier.mva.minNVtxTrk, int(3.0))
    process.hltIter0PFlowTrackCutClassifier.mva.minNVtxTrk = cms.int32(1)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minNdof, 0.0)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minPixelHits, int(2.0))
    
    print(f"After customisation\n\t", end='')
    print(process.hltIter0PFlowTrackCutClassifier.dumpPython())
    return process


def customize_for_hp_tuning_SANITIZED(process):
    import FWCore.ParameterSet.Config as cms
    print(f"Before customisation customize_for_hp_tuning_SANITIZED\n\t",end='')
    print(process.hltIter0PFlowTrackCutClassifier.dumpPython())
    
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.d0err, 3.96846e-3)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.d0err_par, 1.18143e-4)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.dr_exp, 2)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.dr_par1, 1.15638)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.dr_par2, 8.43172e-1)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dz_par.dz_exp, 2)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dz_par.dz_par1, 1.25020)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dz_par.dz_par2, 6.04264e-1)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.maxChi2, 999)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.maxChi2n, 999)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.maxLostLayers, 0)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.min3DLayers, 0)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minLayers, 0)
    process.hltIter0PFlowTrackCutClassifier.mva.minNVtxTrk = cms.int32(3)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minNdof, 0)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minPixelHits, 2)
    
    print(f"After customisation customize_for_hp_tuning_SANITIZED\n\t", end='')
    print(process.hltIter0PFlowTrackCutClassifier.dumpPython())
    return process


def customize_for_hp_tuning_SANITIZED_dz_exp(process):
    import FWCore.ParameterSet.Config as cms
    print(f"Before customisation customize_for_hp_tuning_SANITIZED_dz_exp \n\t",end='')
    print(process.hltIter0PFlowTrackCutClassifier.dumpPython())
    
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.d0err, 3.96846e-3)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.d0err_par, 1.18143e-4)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.dr_exp, 2)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.dr_par1, 1.15638)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dr_par.dr_par2, 8.43172e-1)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dz_par.dz_exp, 3)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dz_par.dz_par1, 1.25020)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.dz_par.dz_par2, 6.04264e-1)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.maxChi2, 999)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.maxChi2n, 999)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.maxLostLayers, 0)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.min3DLayers, 0)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minLayers, 0)
    process.hltIter0PFlowTrackCutClassifier.mva.minNVtxTrk = cms.int32(3)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minNdof, 0)
    assign(process.hltIter0PFlowTrackCutClassifier.mva.minPixelHits, 2)
    
    print(f"After customisation customize_for_hp_tuning_SANITIZED_dz_exp \n\t", end='')
    print(process.hltIter0PFlowTrackCutClassifier.dumpPython())
    return process
