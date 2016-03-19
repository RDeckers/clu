#include <clu.h>
#include <utilities/logging.h>
int main(int argc, char **argv){
  cl_platform_id *platforms = NULL;
  char *platform_name = NULL;
  cl_uint n_platforms = cluGetPlatforms(&platforms, CLU_DYNAMIC);
  report(INFO, "Found %d platforms", n_platforms);
  for(unsigned p = 0; p < n_platforms; p++){
    cluGetPlatformInfo(platforms[p], CL_PLATFORM_NAME, CLU_DYNAMIC, (void**)&platform_name);
    report(INFO, "Platform %d: %s", p, platform_name);
    cl_device_id *devices = NULL;
    const char* device_name = NULL;
    const char* builtin_kernels = NULL;
    cl_uint n_devices = cluGetDevices(platforms[p], CL_DEVICE_TYPE_ALL, CLU_DYNAMIC, &devices);
    report(INFO, "\tnumber of devices: %u", n_devices);
    for(unsigned d = 0; d < n_devices; d++){
      cluGetDeviceInfo(devices[d], CL_DEVICE_NAME, CLU_DYNAMIC, (void**)&device_name);
      report(INFO, "\tdevice %u: %s", d, device_name);
      cluGetDeviceInfo(devices[d], CL_DEVICE_BUILT_IN_KERNELS, CLU_DYNAMIC, (void**)&builtin_kernels);
      report(INFO, "\t builtin-kernels: %s", builtin_kernels);
    }
  }
}
