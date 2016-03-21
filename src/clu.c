#include <clu.h>
#include <stdlib.h>
#include <utilities/logging.h>
#include <utilities/file.h>

cl_uint cluPlatformCount(){
  cl_uint num_platforms;
  cl_int ret = clGetPlatformIDs(0, NULL, &num_platforms);
  if(CL_SUCCESS != ret){
    report(FAIL, "clGetPlatformIDs returned: %s (%d)", cluErrorString(ret), ret);
  }
  return num_platforms;
}

cl_uint cluGetPlatforms(cl_platform_id **platforms, int max_platform_count){
  cl_uint platform_count = max_platform_count;
  if(CLU_DYNAMIC == max_platform_count){
    platform_count = cluPlatformCount();
    free(*platforms);
    *platforms = platform_count ? malloc(platform_count*sizeof(cl_platform_id)) : NULL;
    if(NULL == *platforms){
      return 0;
    }
  }
  cl_int ret = clGetPlatformIDs(platform_count, *platforms, NULL);
  if(CL_SUCCESS != ret){
    report(FAIL, "clGetPlatformIDs returned: %s (%d)", cluErrorString(ret), ret);
  }
  return platform_count;
}

size_t cluPlatformGetInfoSize(cl_platform_id platform, cl_platform_info param){
  size_t size;
  cl_int ret = clGetPlatformInfo(platform, param, 0, NULL, &size);
  if(CL_SUCCESS != ret){
    report(FAIL, "clGetPlatformInfo returned: %s (%d)", cluErrorString(ret), ret);
  }
  return size;
}

size_t cluGetPlatformInfo(cl_platform_id platform, cl_platform_info param, int max_size, void** output){
  cl_int ret;
  if(CLU_DYNAMIC == max_size){
    size_t size = cluPlatformGetInfoSize(platform, param);
    free(*output);
    *output = size ? malloc(size) : NULL;
    if(NULL == *output){
      return 0;
    }
    ret = clGetPlatformInfo(platform, param, size, *output, NULL);
    if(CL_SUCCESS != ret){
      report(FAIL, "clGetPlatformInfo returned: %s (%d)", cluErrorString(ret), ret);
    }
    return size;
  }
  else{
    size_t size = max_size;
    ret = clGetPlatformInfo(platform, param, size, *output, NULL);
    if(CL_SUCCESS != ret){
      report(FAIL, "clGetPlatformInfo returned: %s (%d)", cluErrorString(ret), ret);
    }
    return size;
  }
}

cl_uint cluDeviceCount(cl_platform_id platform, cl_device_type device_type){
  cl_uint device_count;
  cl_int ret;
  ret = clGetDeviceIDs(platform, device_type, 0, NULL, &device_count);
  if(CL_SUCCESS != ret){
    report(FAIL, "clGetDeviceIDs returned: %s (%d)", cluErrorString(ret), ret);
  }
  return device_count;
}

cl_uint cluGetDevices(cl_platform_id platform, cl_device_type device_type, int max_count, cl_device_id **devices){
  cl_uint device_count = max_count;
  if(CLU_DYNAMIC == max_count){
    device_count = cluDeviceCount(platform, device_type);
    free(*devices);
    *devices = device_count ? malloc(device_count*sizeof(cl_device_id)) : NULL;
    if(NULL == *devices){
      return 0;
    }
  }
  cl_int ret = clGetDeviceIDs(platform, device_type, device_count, *devices, NULL);
  if(CL_SUCCESS != ret){
    report(FAIL, "clGetDeviceIDs returned: %s (%d)", cluErrorString(ret), ret);
  }
  return device_count;
}

size_t cluDeviceGetInfoSize(cl_device_id device, cl_device_info param){
  size_t size;
  cl_int ret = clGetDeviceInfo(device, param, 0, NULL, &size);
  if(CL_SUCCESS != ret){
    report(FAIL, "clGetDeviceInfo returned: %s (%d)", cluErrorString(ret), ret);
  }
  return size;
}

size_t cluGetDeviceInfo(cl_device_id device, cl_device_info param, int max_size, void** output){
  cl_int ret;
  if(CLU_DYNAMIC == max_size){
    size_t size = cluDeviceGetInfoSize(device, param);
    free(*output);
    *output = size? malloc(size) : NULL;
    if(NULL == *output){
      report(WARN, "size = %u", size);
      return 0;
    }
    ret = clGetDeviceInfo(device, param, size, *output, NULL);
    if(CL_SUCCESS != ret){
      report(FAIL, "clGetDeviceInfo returned: %s (%d)", cluErrorString(ret), ret);
    }
    return size;
  }
  else{
    size_t size = max_size;
    ret = clGetDeviceInfo(device, param, size, *output, NULL);
    if(CL_SUCCESS != ret){
      report(FAIL, "clGetDeviceInfo returned: %s (%d)", cluErrorString(ret), ret);
    }
    return size;
  }
}

void CL_CALLBACK cluDefaultContextErrorHandler(const char *errinfo, const void  *private_info, size_t  cb, void  *user_data){
  report(FAIL, "Context generated error: %s", errinfo);
}

cl_context cluCreateContextFromTypes(cl_platform_id platform, cl_device_type device_types){
  cl_int ret;
  cl_context context = clCreateContextFromType(
    (cl_context_properties[]){CL_CONTEXT_PLATFORM,(cl_context_properties)platform, 0},
    device_types,
    cluDefaultContextErrorHandler,
    NULL,
    &ret);
  if(ret != CL_SUCCESS){
    report(FAIL, "clCreateContextFromType returned: %s (%d)", cluErrorString(ret), ret);
  }
  return context;
}

cl_command_queue cluCreateCommandQueue(cl_context context, cl_device_id device){
  cl_int ret;
  cl_command_queue q = clCreateCommandQueue(context, device, 0, &ret);
  if(ret != CL_SUCCESS){
    report(FAIL, "clCreateCommandQueue returned: %s (%d)", cluErrorString(ret), ret);
  }
  return q;
}
int cluCreateCommandQueues(cl_context context, cl_device_id *devices,  int n_devices, cl_command_queue **queues){
  if(NULL == *queues){
    *queues = malloc(sizeof(cl_command_queue)*n_devices);
    if(NULL == *queues){
      return 0;
    }
  }
  for(int i = 0; i < n_devices; i++){
    *queues[i]  = cluCreateCommandQueue(context, devices[i]);
  }
  return n_devices;
}

cl_program cluProgramFromFilename(cl_context context, const char *filename){
  char *program_src = NULL;
  file_to_string(filename, 0, &program_src);
  cl_int ret;
  cl_program program = clCreateProgramWithSource(context, 1, (const char**)&program_src, NULL, &ret);
  if(CL_SUCCESS != ret){
    report(FAIL, "clCreateProgramWithSource returned: %s (%d)", cluErrorString(ret), ret);
  }
  free(program_src);
  return program;
}
