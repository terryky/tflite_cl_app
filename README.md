# tflite_cl_app
several applications using TensorFlow Lite with OpenCL delegate.


## At first, please check your OpenCL Environment

```
$ sudo apt install clinfo
$ clinfo
Number of platforms                               0
```
**if you don't have GPU that supoprts OpenCL, let's try Intel CPU Runtime.**

## How to install Intel CPU Runtime for OpenCL.

### preparation
```
$ sudo apt install lsb-core
```

### download install file from Intel Web Site.

download **"l_opencl_p_18.1.0.015.tgz"** from [HERE](https://software.intel.com/en-us/articles/opencl-drivers).

### install

```
$ tar xvfz l_opencl_p_18.1.0.015.tgz
$ cd l_opencl_p_18.1.0.015
$ sudo ./install.sh
```

### check again

```
$ clinfo
Number of platforms                               1
  Platform Name                                   Intel(R) CPU Runtime for OpenCL(TM) Applications
  Platform Vendor                                 Intel(R) Corporation
  Platform Version                                OpenCL 2.1 LINUX
  Platform Profile                                FULL_PROFILE

  (snipped)

```
