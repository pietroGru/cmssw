// Check that ALPAKA_HOST_ONLY is not defined during device compilation:
#ifdef ALPAKA_HOST_ONLY
  #error ALPAKA_HOST_ONLY defined in device compilation
#endif

#include <alpaka/alpaka.hpp>

#include "DataFormats/PortableTestObjects/interface/alpaka/TestDeviceCollection.h"

#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

#include "SiStripRawToClusterAlgo.h"

// #include "EventFilter/SiStripRawToDigi/interface/SiStripFEDBufferComponents.h"


// kernels and related objects
namespace ALPAKA_ACCELERATOR_NAMESPACE {
  using namespace cms::alpakatools;


  class SiStripRawToClusterAlgoKernel_unpacker {
    public:
      template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
      ALPAKA_FN_ACC void operator()(TAcc const& acc,
                                    SiStripMappingConstView mapping,
                                    SiStripClusterizerConditionsDetToFedsConstView DetToFeds,
                                    SiStripClusterizerConditionsData_fedchConstView Data_fedch,
                                    SiStripClusterizerConditionsData_stripConstView Data_strip,
                                    SiStripClusterizerConditionsData_apvConstView Data_apv,
                                    StripDataView stripDataObj) const {
                                      
        // // set this only once in the whole kernel grid
        // if (once_per_grid(acc)) {
        //   output.r() = input.r();
        // }

        // make a strided loop over the kernel grid, covering up to "size" elements
        for (auto chan : uniform_elements(acc, mapping.metadata().size())) {
          const auto fedid = mapping.fedID(chan);
          const auto fedch = mapping.fedCh(chan);
          const auto idx = (fedid - sistrip::FEDCH_PER_FED) * sistrip::FEDCH_PER_FED + fedch;
          const auto ipair = Data_fedch.iPair_(idx);
          const auto ipoff = sistrip::STRIPS_PER_FEDCH * ipair;

          const auto data = mapping.input(chan);
          const auto len = mapping.length(chan);

          if (data != nullptr && len > 0) {
            auto aoff = mapping.offset(chan);
            auto choff = mapping.inoff(chan);
            const auto end = choff + len;

            while (choff < end) {
              auto stripIndex = data[(choff++) ^ 7] + ipoff;
              const auto groupLength = data[(choff++) ^ 7];

              for (auto i = 0; i < 2; ++i) {
                stripDataObj.stripId(aoff) = 0xFFFF;
                stripDataObj.adc(aoff++) = 0;
              }

              for (auto i = 0; i < groupLength; ++i) {
                stripDataObj.stripId(aoff) = stripIndex++;
                stripDataObj.channel(aoff) = chan;
                stripDataObj.adc(aoff++) = data[(choff++) ^ 7];
              }
            }
          }  // choff < end
        }  // data != nullptr && len > 0
      }
  };


  // class SiStripRawToClusterAlgoKernel_setSeedStripsGPU {
  //   public:
  //   template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
  //   ALPAKA_FN_ACC void operator()(TAcc const& acc,
  //                                 SiStripClusterizerConditionsDetToFedsConstView DetToFeds,
  //                                 SiStripClusterizerConditionsData_fedchConstView Data_fedch,
  //                                 SiStripClusterizerConditionsData_stripConstView Data_strip,
  //                                 SiStripClusterizerConditionsData_apvConstView Data_apv,
  //                                 StripDataDevice::View output) const {
  //     // (StripDataView *sst_data_d, const ConditionsDeviceView *conditions)
  //     auto nStrips = sst_data_d->nStrips;
  //     auto chanlocs = sst_data_d->chanlocs;
  //     auto adc = sst_data_d->adc;
  //     auto channels = sst_data_d->channel;
  //     auto stripId = sst_data_d->stripId;
  //     auto seedStripsMask = sst_data_d->seedStripsMask;
  //     auto seedStripsNCMask = sst_data_d->seedStripsNCMask;
  //     auto seedThreshold = sst_data_d->seedThreshold;
        
  //     // make a strided loop over the kernel grid, covering up to "size" elements
  //     for (auto chan : uniform_elements(acc, mapping.metadata().size())) {
  //       seedStripsMask[chan] = 0;
  //       seedStripsNCMask[chan] = 0;
  //       const uint16_t strip = stripId[chan];
  //       if (strip != 0xFFFF) {
  //         const auto chan_ = channels[chan];
  //         const auto fed = chanlocs->fedID(chan_);
  //         const auto channel = chanlocs->fedCh(chan_);
  //         const float noise_i = conditions->noise(fed, channel, strip);
  //         const auto adc_i = adc[chan];
    
  //         seedStripsMask[chan] = (adc_i >= static_cast<uint8_t>(noise_i * seedThreshold)) ? 1 : 0;
  //         seedStripsNCMask[chan] = seedStripsMask[i];
  //       }
  //     }
  //   }
  // };


  class SiStripRawToClusterAlgoKernel {
    public:
      template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
      ALPAKA_FN_ACC void operator()(TAcc const& acc, portabletest::TestDeviceCollection::View view, double xvalue) const {
        const portabletest::Matrix matrix{{1, 2, 3, 4, 5, 6}, {2, 4, 6, 8, 10, 12}, {3, 6, 9, 12, 15, 18}};
        const portabletest::Array flags = {{6, 4, 2, 0}};
  
        // set this only once in the whole kernel grid
        if (once_per_grid(acc)) {
          view.r() = 1.;
        }
  
        // make a strided loop over the kernel grid, covering up to "size" elements
        for (int32_t i : uniform_elements(acc, view.metadata().size())) {
          view[i] = {xvalue, 0., 0., i, flags, matrix * i};
        }
      }
  };
  
  class SiStripRawToClusterAlgoMultiKernel3 {
  public:
    template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
    ALPAKA_FN_ACC void operator()(TAcc const& acc,
                                  portabletest::TestDeviceMultiCollection3::View<2> view,
                                  double xvalue) const {
      const portabletest::Matrix matrix{{1, 2, 3, 4, 5, 6}, {2, 4, 6, 8, 10, 12}, {3, 6, 9, 12, 15, 18}};

      // set this only once in the whole kernel grid
      if (once_per_grid(acc)) {
        view.r3() = 3.;
      }

      // make a strided loop over the kernel grid, covering up to "size" elements
      for (int32_t i : uniform_elements(acc, view.metadata().size())) {
        view[i] = {xvalue, 0., 0., i, matrix * i};
      }
    }
  };

  class SiStripRawToClusterAlgoKernelUpdate {
    public:
      template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
      ALPAKA_FN_ACC void operator()(TAcc const& acc,
                                    portabletest::TestDeviceCollection::ConstView input,
                                    AlpakaESTestDataEDevice::ConstView esData,
                                    portabletest::TestDeviceCollection::View output) const {
        // set this only once in the whole kernel grid
        if (once_per_grid(acc)) {
          output.r() = input.r();
        }
  
        // make a strided loop over the kernel grid, covering up to "size" elements
        for (int32_t i : uniform_elements(acc, output.metadata().size())) {
          double x = input[i].x();
          if (i < esData.size()) {
            x += esData.val(i) + esData.val2(i);
          }
          output[i] = {x, input[i].y(), input[i].z(), input[i].id(), input[i].flags(), input[i].m()};
        }
      }
  };
  
  class SiStripRawToClusterAlgoKernelUpdateMulti3 {
  public:
    template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
    ALPAKA_FN_ACC void operator()(TAcc const& acc,
                                  portabletest::TestSoA::ConstView input,
                                  portabletest::TestSoA2::ConstView input2,
                                  portabletest::TestSoA3::ConstView input3,
                                  AlpakaESTestDataEDevice::ConstView esData,
                                  portabletest::TestSoA::View output,
                                  portabletest::TestSoA2::View output2,
                                  portabletest::TestSoA3::View output3) const {
      // set this only once in the whole kernel grid
      if (once_per_grid(acc)) {
        output.r() = input.r();
        output2.r2() = input2.r2();
        output3.r3() = input3.r3();
      }

      // make a strided loop over the kernel grid, covering up to "size" elements
      for (int32_t i : uniform_elements(acc, output.metadata().size())) {
        double x = input[i].x();
        if (i < esData.size()) {
          x += esData.val(i) + esData.val2(i);
          if (0 == i)
            printf("Setting x[0] to %f\n", x);
        }
        output[i] = {x, input[i].y(), input[i].z(), input[i].id(), input[i].flags(), input[i].m()};
      }
      for (int32_t i : uniform_elements(acc, output2.metadata().size())) {
        double x2 = input2[i].x2();
        if (i < esData.size()) {
          x2 += esData.val(i) + esData.val2(i);
        }
        output2[i] = {x2, input2[i].y2(), input2[i].z2(), input2[i].id2(), input2[i].m2()};
      }
      for (int32_t i : uniform_elements(acc, output3.metadata().size())) {
        double x3 = input3[i].x3();
        if (i < esData.size()) {
          x3 += esData.val(i) + esData.val2(i);
        }
        output3[i] = {x3, input3[i].y3(), input3[i].z3(), input3[i].id3(), input3[i].m3()};
      }
    }
  };

  class SiStripRawToClusterAlgoStructKernel {
    public:
      template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
      ALPAKA_FN_ACC void operator()(TAcc const& acc,
                                    portabletest::TestDeviceObject::Product* data,
                                    double x,
                                    double y,
                                    double z,
                                    int32_t id) const {
        // run on a single thread
        if (once_per_grid(acc)) {
          data->x = x;
          data->y = y;
          data->z = z;
          data->id = id;
        }
      }
  };

  class SiStripRawToClusterZeroCollectionKernel {
    public:
      template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
      ALPAKA_FN_ACC void operator()(TAcc const& acc, portabletest::TestDeviceCollection::ConstView view) const {
        const portabletest::Matrix matrix{{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};
        const portabletest::Array flags = {{0, 0, 0, 0}};
  
        // check this only once in the whole kernel grid
        if (once_per_grid(acc)) {
          ALPAKA_ASSERT(view.r() == 0.);
        }
  
        // make a strided loop over the kernel grid, covering up to "size" elements
        for (int32_t i : uniform_elements(acc, view.metadata().size())) {
          auto element = view[i];
          ALPAKA_ASSERT(element.x() == 0.);
          ALPAKA_ASSERT(element.y() == 0.);
          ALPAKA_ASSERT(element.z() == 0.);
          ALPAKA_ASSERT(element.id() == 0.);
          ALPAKA_ASSERT(element.flags() == flags);
          ALPAKA_ASSERT(element.m() == matrix);
        }
      }
  };
  
  class SiStripRawToClusterZeroCollectionKernel3 {
    public:
      template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
      ALPAKA_FN_ACC void operator()(TAcc const& acc, portabletest::TestDeviceMultiCollection3::ConstView<2> view) const {
        const portabletest::Matrix matrix{{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};
  
        // check this only once in the whole kernel grid
        if (once_per_grid(acc)) {
          ALPAKA_ASSERT(view.r3() == 0.);
        }
  
        // make a strided loop over the kernel grid, covering up to "size" elements
        for (int32_t i : uniform_elements(acc, view.metadata().size())) {
          auto element = view[i];
          ALPAKA_ASSERT(element.x3() == 0.);
          ALPAKA_ASSERT(element.y3() == 0.);
          ALPAKA_ASSERT(element.z3() == 0.);
          ALPAKA_ASSERT(element.id3() == 0.);
          ALPAKA_ASSERT(element.m3() == matrix);
        }
      }
  };
  
  class SiStripRawToClusterZeroStructKernel {
    public:
      template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
      ALPAKA_FN_ACC void operator()(TAcc const& acc, portabletest::TestDeviceObject::Product const* data) const {
        // check this only once in the whole kernel grid
        if (once_per_grid(acc)) {
          ALPAKA_ASSERT(data->x == 0.);
          ALPAKA_ASSERT(data->y == 0.);
          ALPAKA_ASSERT(data->z == 0.);
          ALPAKA_ASSERT(data->id == 0);
        }
      }
  };

} // namespace ALPAKA_ACCELERATOR_NAMESPACE // kernels




// kernels launchers
namespace ALPAKA_ACCELERATOR_NAMESPACE {
  using namespace cms::alpakatools;
  using namespace sistripclusterizer;

  void SiStripRawToClusterAlgo::unpackStrips(Queue& queue, SiStripMappingDevice const& mapping, SiStripClusterizerConditionsDevice const& conditions, StripDataDevice& output) const {
    // use 64 items per group (this value is arbitrary, but it's a reasonable starting point)
    uint32_t items = 64;
    // use as many groups as needed to cover the whole problem
    uint32_t groups = divide_up_by(mapping->metadata().size(), items);

    // map items to
    //   - threads with a single element per thread on a GPU backend
    //   - elements within a single thread on a CPU backend
    auto workDiv = make_workdiv<Acc1D>(groups, items);
    
    alpaka::exec<Acc1D>(queue, workDiv, SiStripRawToClusterAlgoKernel_unpacker{},
      mapping.const_view(),
      conditions.const_view<SiStripClusterizerConditionsDetToFedsSoA>(),
      conditions.const_view<SiStripClusterizerConditionsData_fedchSoA>(),
      conditions.const_view<SiStripClusterizerConditionsData_stripSoA>(),
      conditions.const_view<SiStripClusterizerConditionsData_apvSoA>(),
      output.view()
    );
  }



  portabletest::TestDeviceCollection SiStripRawToClusterAlgo::update(Queue& queue, portabletest::TestDeviceCollection const& input,AlpakaESTestDataEDevice const& esData) const {
    portabletest::TestDeviceCollection collection{input->metadata().size(), queue};

    // use 64 items per group (this value is arbitrary, but it's a reasonable starting point)
    uint32_t items = 64;

    // use as many groups as needed to cover the whole problem
    uint32_t groups = divide_up_by(collection->metadata().size(), items);

    // map items to
    //   - threads with a single element per thread on a GPU backend
    //   - elements within a single thread on a CPU backend
    auto workDiv = make_workdiv<Acc1D>(groups, items);

    alpaka::exec<Acc1D>(queue, workDiv, SiStripRawToClusterAlgoKernelUpdate{}, input.view(), esData.view(), collection.view());

    return collection;
  }

  portabletest::TestDeviceMultiCollection3 SiStripRawToClusterAlgo::updateMulti3(Queue& queue, portabletest::TestDeviceMultiCollection3 const& input, AlpakaESTestDataEDevice const& esData) const {
    portabletest::TestDeviceMultiCollection3 collection{input.sizes(), queue};

    // use 64 items per group (this value is arbitrary, but it's a reasonable starting point)
    uint32_t items = 64;

    // use as many groups as needed to cover the whole problem
    auto sizes = collection.sizes();
    uint32_t groups = divide_up_by(*std::max_element(sizes.begin(), sizes.end()), items);

    // map items to
    //   - threads with a single element per thread on a GPU backend
    //   - elements within a single thread on a CPU backend
    auto workDiv = make_workdiv<Acc1D>(groups, items);

    alpaka::exec<Acc1D>(queue,
    workDiv,
    SiStripRawToClusterAlgoKernelUpdateMulti3{},
    input.view<portabletest::TestSoA>(),
    input.view<portabletest::TestSoA2>(),
    input.view<portabletest::TestSoA3>(),
    esData.view(),
    collection.view<portabletest::TestSoA>(),
    collection.view<portabletest::TestSoA2>(),
    collection.view<portabletest::TestSoA3>());

    return collection;
  }



  void SiStripRawToClusterAlgo::fill(Queue& queue, portabletest::TestDeviceCollection& collection, double xvalue) const {
    // use 64 items per group (this value is arbitrary, but it's a reasonable starting point)
    uint32_t items = 64;

    // use as many groups as needed to cover the whole problem
    uint32_t groups = divide_up_by(collection->metadata().size(), items);

    // map items to
    //   - threads with a single element per thread on a GPU backend
    //   - elements within a single thread on a CPU backend
    auto workDiv = make_workdiv<Acc1D>(groups, items);

    alpaka::exec<Acc1D>(queue, workDiv, SiStripRawToClusterAlgoKernel{}, collection.view(), xvalue);
  }


  void SiStripRawToClusterAlgo::fillObject(Queue& queue, portabletest::TestDeviceObject& object, double x, double y, double z, int32_t id) const {
    // run on a single thread
    auto workDiv = make_workdiv<Acc1D>(1, 1);

    alpaka::exec<Acc1D>(queue, workDiv, SiStripRawToClusterAlgoStructKernel{}, object.data(), x, y, z, id);
  }

  void SiStripRawToClusterAlgo::fillMulti3(Queue& queue, portabletest::TestDeviceMultiCollection3& collection, double xvalue) const {
    // use 64 items per group (this value is arbitrary, but it's a reasonable starting point)
    uint32_t items = 64;

    // use as many groups as needed to cover the whole problem
    uint32_t groups = divide_up_by(collection.view<portabletest::TestSoA>().metadata().size(), items);
    uint32_t groups3 = divide_up_by(collection.view<portabletest::TestSoA3>().metadata().size(), items);

    // map items to
    //   - threads with a single element per thread on a GPU backend
    //   - elements within a single thread on a CPU backend
    auto workDiv = make_workdiv<Acc1D>(groups, items);
    auto workDiv3 = make_workdiv<Acc1D>(groups3, items);

    alpaka::exec<Acc1D>(queue, workDiv, SiStripRawToClusterAlgoKernel{}, collection.view<portabletest::TestSoA>(), xvalue);
    alpaka::exec<Acc1D>(queue, workDiv3, SiStripRawToClusterAlgoMultiKernel3{}, collection.view<portabletest::TestSoA3>(), xvalue);
  }

  // Check that the collection has been filled with zeroes.
  void SiStripRawToClusterAlgo::checkZero(Queue& queue, portabletest::TestDeviceCollection const& collection) const {
    // create a work division with a single block and
    //   - 32 threads with a single element per thread on a GPU backend
    //   - 32 elements within a single thread on a CPU backend
    auto workDiv = make_workdiv<Acc1D>(1, 32);

    // the kernel will make a strided loop over the launch grid to cover all elements in the collection
    alpaka::exec<Acc1D>(queue, workDiv, SiStripRawToClusterZeroCollectionKernel{}, collection.const_view());
  }

  // Check that the collection has been filled with zeroes.
  void SiStripRawToClusterAlgo::checkZero(Queue& queue, portabletest::TestDeviceMultiCollection3 const& collection) const {
    // create a work division with a single block and
    //   - 32 threads with a single element per thread on a GPU backend
    //   - 32 elements within a single thread on a CPU backend
    auto workDiv = make_workdiv<Acc1D>(1, 32);

    // the kernels will make a strided loop over the launch grid to cover all elements in the collection
    alpaka::exec<Acc1D>(queue, workDiv, SiStripRawToClusterZeroCollectionKernel{}, collection.const_view<portabletest::TestSoA>());
    alpaka::exec<Acc1D>(queue, workDiv, SiStripRawToClusterZeroCollectionKernel3{}, collection.const_view<portabletest::TestSoA3>());
  }

  // Check that the object has been filled with zeroes.
  void SiStripRawToClusterAlgo::checkZero(Queue& queue, portabletest::TestDeviceObject const& object) const {
    // create a work division with a single block and
    //   - 32 threads with a single element per thread on a GPU backend
    //   - 32 elements within a single thread on a CPU backend
    auto workDiv = make_workdiv<Acc1D>(1, 32);

    // the kernel will actually use a single thread
    alpaka::exec<Acc1D>(queue, workDiv, SiStripRawToClusterZeroStructKernel{}, object.data());
  }

} // namespace ALPAKA_ACCELERATOR_NAMESPACE // kernels launchers



namespace ALPAKA_ACCELERATOR_NAMESPACE {
  // Get the conditions cablingMapData and create the data for the unpacking
  void SiStripRawToClusterAlgo::filterConditions(
    Queue& queue,
    const SiStripClusterizerConditionsDetToFedsConstView& detToFedsMap,
    sistrip::FEDReadoutMode mode,
    DataFedAppender& fedBufferBlocksRaw_,
    const std::vector<std::unique_ptr<sistrip::FEDBuffer>>& buffers_,
    const DataFedAppender& dataFed,
    const SiStripMappingConstView& chanlocs
  ) const {
    //
  }
} // namespace ALPAKA_ACCELERATOR_NAMESPACE