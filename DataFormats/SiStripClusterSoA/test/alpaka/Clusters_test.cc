#include <cstdlib>

#include <alpaka/alpaka.hpp>

#include "DataFormats/SiStripClusterSoA/interface/SiStripClustersSoA.h"
#include "DataFormats/SiStripClusterSoA/interface/SiStripClustersHostCollection.h"
#include "DataFormats/SiStripClusterSoA/interface/alpaka/SiStripClustersDeviceCollection.h"

#include "FWCore/Utilities/interface/stringize.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/devices.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"
#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

#include "Clusters_test.h"

using namespace ALPAKA_ACCELERATOR_NAMESPACE;

int main() {
  // Get the list of devices on the current platform
  auto const& devices = cms::alpakatools::devices<Platform>();
  if (devices.empty()) {
    std::cerr << "No devices available for the " EDM_STRINGIZE(ALPAKA_ACCELERATOR_NAMESPACE) " backend, the test will be skipped.\n";
    exit(EXIT_FAILURE);
  }

  // Run the test on each device
  for (const auto& device : devices) {
    Queue queue(device);

    // Inner scope to deallocate memory before destroying the stream
    {
      // Instantiate tracks on device. PortableDeviceCollection allocates
      // SoA on device automatically.
      ALPAKA_ACCELERATOR_NAMESPACE::sistrip::SiStripClustersDeviceCollection clusters_d(100, queue);    // (the namespace specification is to avoid confusion with the non-alpaka sistrip namespace)
      testClusterSoA::runKernels(clusters_d.view(), queue);
      
      // Instantate tracks on host. This is where the data will be
      // copied to from device.
      ALPAKA_ACCELERATOR_NAMESPACE::sistrip::SiStripClustersHostCollection clusters_h(clusters_d.view().metadata().size(), queue);    // (the namespace specification is to avoid confusion with the non-alpaka sistrip namespace)
      std::cout << "clusters_h.view().metadata().size() = " << clusters_h.view().metadata().size() << std::endl;
      alpaka::memcpy(queue, clusters_h.buffer(), clusters_d.const_buffer());
      alpaka::wait(queue);
    }
  }

  return EXIT_SUCCESS;
}
