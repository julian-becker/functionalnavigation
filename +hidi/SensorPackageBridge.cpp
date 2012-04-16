#include <map>
#include <vector>
#include "mex.h"
#include "SensorPackage.h"

namespace SensorPackageBridge
{
  enum SensorPackageMember
  {
    undefined,
    SensorPackageIsConnected,
    SensorPackageDescription,
    SensorPackageCreate,
    getAccelerometerArray,
    getGyroscopeArray,
    getMagnetometerArray,
    getAltimeter,
    getGPSReceiver,
    refresh,
    accelerometerArrayRefresh,
    accelerometerArrayHasData,
    accelerometerArrayFirst,
    accelerometerArrayLast,
    accelerometerArrayGetTime,
    getSpecificForce,
    getSpecificForceCalibrated,
    getAccelerometerVelocityRandomWalk,
    getAccelerometerTurnOnBiasSigma,
    getAccelerometerInRunBiasSigma,
    getAccelerometerInRunBiasStability,
    getAccelerometerTurnOnScaleSigma,
    getAccelerometerInRunScaleSigma,
    getAccelerometerInRunScaleStability,
    gyroscopeArrayRefresh,
    gyroscopeArrayHasData,
    gyroscopeArrayFirst,
    gyroscopeArrayLast,
    gyroscopeArrayGetTime,
    getAngularRate,
    getAngularRateCalibrated,
    getGyroscopeAngleRandomWalk,
    getGyroscopeTurnOnBiasSigma,
    getGyroscopeInRunBiasSigma,
    getGyroscopeInRunBiasStability,
    getGyroscopeTurnOnScaleSigma,
    getGyroscopeInRunScaleSigma,
    getGyroscopeInRunScaleStability,
    magnetometerArrayRefresh,
    magnetometerArrayHasData,
    magnetometerArrayFirst,
    magnetometerArrayLast,
    magnetometerArrayGetTime,
    getMagneticField,
    getMagneticFieldCalibrated,
    altimeterRefresh,
    altimeterHasData,
    altimeterFirst,
    altimeterLast,
    altimeterGetTime,
    getAltitude,
    gpsReceiverRefresh,
    gpsReceiverHasData,
    gpsReceiverFirst,
    gpsReceiverLast,
    gpsReceiverGetTime,
    getLongitude,
    getLatitude,
    getHeight,
    hasPrecision,
    getPrecisionHorizontal,
    getPrecisionVertical,
    getPrecisionCircular
  };

  void argcheck(int& narg, int n)
  {
    if(n>narg)
    {
      throw("Too few input arguments.");
    }
    return;
  }

  void uint32check(const mxArray*& array)
  {
    if((mxGetClassID(array)!=mxUINT32_CLASS))
    {
      throw("Array must be uint32.");
    }
    return;
  }
  
  void convert(const mxArray*& array, uint32_t& value)
  {
    uint32check(array);
    value = (*static_cast<uint32_t*>(mxGetData(array)));
    return;
  }

  void convert(const mxArray*& array, std::vector<uint32_t> &value)
  {
    uint32_t* data;
    size_t n;
    size_t N;
    uint32check(array);
    N = mxGetNumberOfElements(array);
    data = (uint32_t*)mxGetData(array);
    value.resize(N, 0);
    for(n = 0; n<N; ++n)
    {
      value[n] = data[n];
    }
    return;
  }

  void convert(const mxArray*& array, std::string& cppString)
  {
    unsigned N = mxGetNumberOfElements(array)+1; // add one for terminating character
    char *cString = new char[N];
    if(mxGetClassID(array)!=mxCHAR_CLASS)
    {
      throw("Array must be char.");
    }
    mxGetString(array, cString, N);
    cppString = cString;
    delete[] cString;
    return;
  }

  void convert(const double& value, mxArray*& array)
  {
    array = mxCreateDoubleScalar(value);
    return;
  }

  void convert(const uint32_t& value, mxArray*& array)
  {
    array = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    (*static_cast<uint32_t*>(mxGetData(array))) = value;
    return;
  }

  void convert(const bool& value, mxArray*& array)
  {
    array = mxCreateLogicalScalar(value);
    return;
  }

  void convert(std::string str, mxArray*& array)
  {
    array = mxCreateString(str.c_str());
    return;
  }
  
  template<class T>
  void getSensor(std::vector<T> sensor, mxArray*& array)
  {
    uint32_t* data;
    uint32_t index;
    array = mxCreateNumericMatrix(sensor.size(), 1, mxUINT32_CLASS, mxREAL);
    data = static_cast<uint32_t*>(mxGetData(array));
    for(index = 0; index<sensor.size(); ++index)
    {
      data[index] = index;
    }
    return;
  }

  void getTime(hidi::Sensor* sensor, const mxArray*& n, mxArray*& array)
  {
    uint32check(n);
    mxArray* time = mxCreateDoubleMatrix(mxGetM(n), mxGetN(n), mxREAL);
    double* data = mxGetPr(time);
    uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
    size_t K = mxGetNumberOfElements(n);
    size_t k = 0;
    while(k<K)
    {
      data[k] = sensor->getTime(node[k]);
      ++k;
    }
    mexCallMATLAB(1, &array, 1, &time, "hidi.WorldTime");
    mxDestroyArray(time);
    return;
  }
  
  static hidi::SensorPackage* package = NULL;

  void deleteSensorPackage(void)
  {
    if(package)
    {
      delete package;
      package = NULL;
    }
  }
  
  void safeMexFunction(int& nlhs, mxArray**& plhs, int& nrhs, const mxArray**& prhs)
  {
    static uint32_t index;
    static std::string memberName;
    static std::map<std::string, SensorPackageMember> memberMap;
    static bool initialized = false;

    if(!initialized)
    {
      mexAtExit(deleteSensorPackage);
      memberMap["SensorPackageIsConnected"] = SensorPackageIsConnected;
      memberMap["SensorPackageDescription"] = SensorPackageDescription;
      memberMap["SensorPackageCreate"] = SensorPackageCreate;
      memberMap["getAccelerometerArray"] = getAccelerometerArray;
      memberMap["getGyroscopeArray"] = getGyroscopeArray;
      memberMap["getMagnetometerArray"] = getMagnetometerArray;
      memberMap["getAltimeter"] = getAltimeter;
      memberMap["getGPSReceiver"] = getGPSReceiver;
      memberMap["refresh"] = refresh;
      memberMap["accelerometerArrayRefresh"] = accelerometerArrayRefresh;
      memberMap["accelerometerArrayHasData"] = accelerometerArrayHasData;
      memberMap["accelerometerArrayFirst"] = accelerometerArrayFirst;
      memberMap["accelerometerArrayLast"] = accelerometerArrayLast;
      memberMap["accelerometerArrayGetTime"] = accelerometerArrayGetTime;
      memberMap["getSpecificForce"] = getSpecificForce;
      memberMap["getSpecificForceCalibrated"] = getSpecificForceCalibrated;
      memberMap["getAccelerometerVelocityRandomWalk"] = getAccelerometerVelocityRandomWalk;
      memberMap["getAccelerometerTurnOnBiasSigma"] = getAccelerometerTurnOnBiasSigma;
      memberMap["getAccelerometerInRunBiasSigma"] = getAccelerometerInRunBiasSigma;
      memberMap["getAccelerometerInRunBiasStability"] = getAccelerometerInRunBiasStability;
      memberMap["getAccelerometerTurnOnScaleSigma"] = getAccelerometerTurnOnScaleSigma;
      memberMap["getAccelerometerInRunScaleSigma"] = getAccelerometerInRunScaleSigma;
      memberMap["getAccelerometerInRunScaleStability"] = getAccelerometerInRunScaleStability;
      memberMap["gyroscopeArrayRefresh"] = gyroscopeArrayRefresh;
      memberMap["gyroscopeArrayHasData"] = gyroscopeArrayHasData;
      memberMap["gyroscopeArrayFirst"] = gyroscopeArrayFirst;
      memberMap["gyroscopeArrayLast"] = gyroscopeArrayLast;
      memberMap["gyroscopeArrayGetTime"] = gyroscopeArrayGetTime;
      memberMap["getAngularRate"] = getAngularRate;
      memberMap["getAngularRateCalibrated"] = getAngularRateCalibrated;
      memberMap["getGyroscopeAngleRandomWalk"] = getGyroscopeAngleRandomWalk;
      memberMap["getGyroscopeTurnOnBiasSigma"] = getGyroscopeTurnOnBiasSigma;
      memberMap["getGyroscopeInRunBiasSigma"] = getGyroscopeInRunBiasSigma;
      memberMap["getGyroscopeInRunBiasStability"] = getGyroscopeInRunBiasStability;
      memberMap["getGyroscopeTurnOnScaleSigma"] = getGyroscopeTurnOnScaleSigma;
      memberMap["getGyroscopeInRunScaleSigma"] = getGyroscopeInRunScaleSigma;
      memberMap["getGyroscopeInRunScaleStability"] = getGyroscopeInRunScaleStability;
      memberMap["magnetometerArrayRefresh"] = magnetometerArrayRefresh;
      memberMap["magnetometerArrayHasData"] = magnetometerArrayHasData;
      memberMap["magnetometerArrayFirst"] = magnetometerArrayFirst;
      memberMap["magnetometerArrayLast"] = magnetometerArrayLast;
      memberMap["magnetometerArrayGetTime"] = magnetometerArrayGetTime;
      memberMap["getMagneticField"] = getMagneticField;
      memberMap["getMagneticFieldCalibrated"] = getMagneticFieldCalibrated;
      memberMap["altimeterRefresh"] = altimeterRefresh;
      memberMap["altimeterHasData"] = altimeterHasData;
      memberMap["altimeterFirst"] = altimeterFirst;
      memberMap["altimeterLast"] = altimeterLast;
      memberMap["altimeterGetTime"] = altimeterGetTime;
      memberMap["getAltitude"] = getAltitude;
      memberMap["gpsReceiverRefresh"] = gpsReceiverRefresh;
      memberMap["gpsReceiverHasData"] = gpsReceiverHasData;
      memberMap["gpsReceiverFirst"] = gpsReceiverFirst;
      memberMap["gpsReceiverLast"] = gpsReceiverLast;
      memberMap["gpsReceiverGetTime"] = gpsReceiverGetTime;
      memberMap["getLongitude"] = getLongitude;
      memberMap["getLatitude"] = getLatitude;
      memberMap["getHeight"] = getHeight;
      memberMap["hasPrecision"] = hasPrecision;
      memberMap["getPrecisionHorizontal"] = getPrecisionHorizontal;
      memberMap["getPrecisionVertical"] = getPrecisionVertical;
      memberMap["getPrecisionCircular"] = getPrecisionCircular;
      initialized = true;
    }

    argcheck(nrhs, 2);
    convert(prhs[0], index);
    convert(prhs[1], memberName);
    switch(memberMap[memberName])
    {
      case undefined:
      {
        throw("Undefined function call.");
        break;
      }
      
      case SensorPackageIsConnected:
      {
        argcheck(nrhs, 3);
        static std::string name;
        convert(prhs[2], name);
        convert(hidi::SensorPackage::isConnected(name), plhs[0]);
        break;
      }
      
      case SensorPackageDescription:
      {
        argcheck(nrhs, 3);
        static std::string name;
        convert(prhs[2], name);
        convert(hidi::SensorPackage::description(name), plhs[0]);
        break;
      }
      
      case SensorPackageCreate:
      {
        argcheck(nrhs, 4);
        static std::string name;
        static std::string uri;
        convert(prhs[2], name);
        convert(prhs[3], uri);
        deleteSensorPackage();
        package = hidi::SensorPackage::create(name, uri);
        break;
      }

      case getAccelerometerArray:
      {
        getSensor(package->getAccelerometerArray(), plhs[0]); 
        break;
      }

      case getGyroscopeArray:
      {
        getSensor(package->getGyroscopeArray(), plhs[0]); 
        break;
      }

      case getMagnetometerArray:
      {
        getSensor(package->getMagnetometerArray(), plhs[0]); 
        break;
      }

      case getAltimeter:
      {
        getSensor(package->getAltimeter(), plhs[0]); 
        break;
      }

      case getGPSReceiver:
      {
        getSensor(package->getGPSReceiver(), plhs[0]); 
        break;
      }

      case refresh:
      {
        package->refresh();
        break;
      }

      case accelerometerArrayRefresh:
      {
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        sensor->refresh();
        break;
      }
      
      case accelerometerArrayHasData:
      {
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        convert(sensor->hasData(), plhs[0]);
        break;
      }

      case accelerometerArrayFirst:
      {
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        convert(sensor->first(), plhs[0]);
        break;
      }

      case accelerometerArrayLast:
      {
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        convert(sensor->last(), plhs[0]);
        break;
      }

      case accelerometerArrayGetTime:
      {
        argcheck(nrhs, 3);
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        getTime(static_cast<hidi::Sensor*>(sensor), prhs[2], plhs[0]);
        break;
      }

      case getSpecificForce:
      {
        argcheck(nrhs, 4);
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        const mxArray* n = prhs[2];
        const mxArray* ax = prhs[3];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        uint32_t* axis = static_cast<uint32_t*>(mxGetData(ax));
        size_t N = mxGetNumberOfElements(n);
        size_t A = mxGetNumberOfElements(ax);
        double* data;
        size_t i;
        size_t j;
        size_t k;
        uint32check(n);
        uint32check(ax);
        plhs[0] = mxCreateDoubleMatrix(N, A, mxREAL);
        data = mxGetPr(plhs[0]);
        k = 0;
        for(j = 0; j<A; ++j)
        {
          for(i = 0; i<N; ++i)
          {
            data[k] = sensor->getSpecificForce(node[i], axis[j]);
            ++k;
          }
        }
        break;
      }

      case getSpecificForceCalibrated:
      {
        argcheck(nrhs, 4);
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        const mxArray* n = prhs[2];
        const mxArray* ax = prhs[3];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        uint32_t* axis = static_cast<uint32_t*>(mxGetData(ax));
        size_t N = mxGetNumberOfElements(n);
        size_t A = mxGetNumberOfElements(ax);
        double* data;
        size_t i;
        size_t j;
        size_t k;
        uint32check(n);
        uint32check(ax);
        plhs[0] = mxCreateDoubleMatrix(N, A, mxREAL);
        data = mxGetPr(plhs[0]);
        k = 0;
        for(j = 0; j<A; ++j)
        {
          for(i = 0; i<N; ++i)
          {
            data[k] = sensor->getSpecificForceCalibrated(node[i], axis[j]);
            ++k;
          }
        }
        break;
      }

      case getAccelerometerVelocityRandomWalk:
      {
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        convert(sensor->getAccelerometerVelocityRandomWalk(), plhs[0]);
        break;
      }

      case getAccelerometerTurnOnBiasSigma:
      {
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        convert(sensor->getAccelerometerTurnOnBiasSigma(), plhs[0]);
        break;
      }

      case getAccelerometerInRunBiasSigma:
      {
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        convert(sensor->getAccelerometerInRunBiasSigma(), plhs[0]);
        break;
      }

      case getAccelerometerInRunBiasStability:
      {
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        convert(sensor->getAccelerometerInRunBiasStability(), plhs[0]);
        break;
      }

      case getAccelerometerTurnOnScaleSigma:
      {
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        convert(sensor->getAccelerometerTurnOnScaleSigma(), plhs[0]);
        break;
      }

      case getAccelerometerInRunScaleSigma:
      {
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        convert(sensor->getAccelerometerInRunScaleSigma(), plhs[0]);
        break;
      }

      case getAccelerometerInRunScaleStability:
      {
        hidi::AccelerometerArray* sensor = package->getAccelerometerArray()[index];
        convert(sensor->getAccelerometerInRunScaleStability(), plhs[0]);
        break;
      }

      case gyroscopeArrayRefresh:
      {
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        sensor->refresh();
        break;
      }
      
      case gyroscopeArrayHasData:
      {
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        convert(sensor->hasData(), plhs[0]);
        break;
      }

      case gyroscopeArrayFirst:
      {
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        convert(sensor->first(), plhs[0]);
        break;
      }

      case gyroscopeArrayLast:
      {
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        convert(sensor->last(), plhs[0]);
        break;
      }

      case gyroscopeArrayGetTime:
      {
        argcheck(nrhs, 3);
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        getTime(static_cast<hidi::Sensor*>(sensor), prhs[2], plhs[0]);
        break;
      }
      
      case getAngularRate:
      {
        argcheck(nrhs, 4);
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        const mxArray* n = prhs[2];
        const mxArray* ax = prhs[3];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        uint32_t* axis = static_cast<uint32_t*>(mxGetData(ax));
        size_t N = mxGetNumberOfElements(n);
        size_t A = mxGetNumberOfElements(ax);
        double* data;
        size_t i;
        size_t j;
        size_t k;
        uint32check(n);
        uint32check(ax);
        plhs[0] = mxCreateDoubleMatrix(N, A, mxREAL);
        data = mxGetPr(plhs[0]);
        k = 0;
        for(j = 0; j<A; ++j)
        {
          for(i = 0; i<N; ++i)
          {
            data[k] = sensor->getAngularRate(node[i], axis[j]);
            ++k;
          }
        }
        break;
      }

      case getAngularRateCalibrated:
      {
        argcheck(nrhs, 4);
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        const mxArray* n = prhs[2];
        const mxArray* ax = prhs[3];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        uint32_t* axis = static_cast<uint32_t*>(mxGetData(ax));
        size_t N = mxGetNumberOfElements(n);
        size_t A = mxGetNumberOfElements(ax);
        double* data;
        size_t i;
        size_t j;
        size_t k;
        uint32check(n);
        uint32check(ax);
        plhs[0] = mxCreateDoubleMatrix(N, A, mxREAL);
        data = mxGetPr(plhs[0]);
        k = 0;
        for(j = 0; j<A; ++j)
        {
          for(i = 0; i<N; ++i)
          {
            data[k] = sensor->getAngularRateCalibrated(node[i], axis[j]);
            ++k;
          }
        }
        break;
      }

      case getGyroscopeAngleRandomWalk:
      {
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        convert(sensor->getGyroscopeAngleRandomWalk(), plhs[0]);
        break;
      }

      case getGyroscopeTurnOnBiasSigma:
      {
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        convert(sensor->getGyroscopeTurnOnBiasSigma(), plhs[0]);
        break;
      }

      case getGyroscopeInRunBiasSigma:
      {
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        convert(sensor->getGyroscopeInRunBiasSigma(), plhs[0]);
        break;
      }

      case getGyroscopeInRunBiasStability:
      {
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        convert(sensor->getGyroscopeInRunBiasStability(), plhs[0]);
        break;
      }

      case getGyroscopeTurnOnScaleSigma:
      {
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        convert(sensor->getGyroscopeTurnOnScaleSigma(), plhs[0]);
        break;
      }

      case getGyroscopeInRunScaleSigma:
      {
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        convert(sensor->getGyroscopeInRunScaleSigma(), plhs[0]);
        break;
      }

      case getGyroscopeInRunScaleStability:
      {
        hidi::GyroscopeArray* sensor = package->getGyroscopeArray()[index];
        convert(sensor->getGyroscopeInRunScaleStability(), plhs[0]);
        break;
      }

      case magnetometerArrayRefresh:
      {
        hidi::MagnetometerArray* sensor = package->getMagnetometerArray()[index];
        sensor->refresh();
        break;
      }
      
      case magnetometerArrayHasData:
      {
        hidi::MagnetometerArray* sensor = package->getMagnetometerArray()[index];
        convert(sensor->hasData(), plhs[0]);
        break;
      }

      case magnetometerArrayFirst:
      {
        hidi::MagnetometerArray* sensor = package->getMagnetometerArray()[index];
        convert(sensor->first(), plhs[0]);
        break;
      }

      case magnetometerArrayLast:
      {
        hidi::MagnetometerArray* sensor = package->getMagnetometerArray()[index];
        convert(sensor->last(), plhs[0]);
        break;
      }

      case magnetometerArrayGetTime:
      {
        argcheck(nrhs, 3);
        hidi::MagnetometerArray* sensor = package->getMagnetometerArray()[index];
        getTime(static_cast<hidi::Sensor*>(sensor), prhs[2], plhs[0]);
        break;
      }
      
      case getMagneticField:
      {
        argcheck(nrhs, 4);
        hidi::MagnetometerArray* sensor = package->getMagnetometerArray()[index];
        const mxArray* n = prhs[2];
        const mxArray* ax = prhs[3];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        uint32_t* axis = static_cast<uint32_t*>(mxGetData(ax));
        size_t N = mxGetNumberOfElements(n);
        size_t A = mxGetNumberOfElements(ax);
        double* data;
        size_t i;
        size_t j;
        size_t k;
        uint32check(n);
        uint32check(ax);
        plhs[0] = mxCreateDoubleMatrix(N, A, mxREAL);
        data = mxGetPr(plhs[0]);
        k = 0;
        for(j = 0; j<A; ++j)
        {
          for(i = 0; i<N; ++i)
          {
            data[k] = sensor->getMagneticField(node[i], axis[j]);
            ++k;
          }
        }
        break;
      }

      case getMagneticFieldCalibrated:
      {
        argcheck(nrhs, 4);
        hidi::MagnetometerArray* sensor = package->getMagnetometerArray()[index];
        const mxArray* n = prhs[2];
        const mxArray* ax = prhs[3];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        uint32_t* axis = static_cast<uint32_t*>(mxGetData(ax));
        size_t N = mxGetNumberOfElements(n);
        size_t A = mxGetNumberOfElements(ax);
        double* data;
        size_t i;
        size_t j;
        size_t k;
        uint32check(n);
        uint32check(ax);
        plhs[0] = mxCreateDoubleMatrix(N, A, mxREAL);
        data = mxGetPr(plhs[0]);
        k = 0;
        for(j = 0; j<A; ++j)
        {
          for(i = 0; i<N; ++i)
          {
            data[k] = sensor->getMagneticFieldCalibrated(node[i], axis[j]);
            ++k;
          }
        }
        break;
      }
      
      case altimeterRefresh:
      {
        hidi::Altimeter* sensor = package->getAltimeter()[index];
        sensor->refresh();
        break;
      }
      
      case altimeterHasData:
      {
        hidi::Altimeter* sensor = package->getAltimeter()[index];
        convert(sensor->hasData(), plhs[0]);
        break;
      }

      case altimeterFirst:
      {
        hidi::Altimeter* sensor = package->getAltimeter()[index];
        convert(sensor->first(), plhs[0]);
        break;
      }

      case altimeterLast:
      {
        hidi::Altimeter* sensor = package->getAltimeter()[index];
        convert(sensor->last(), plhs[0]);
        break;
      }

      case altimeterGetTime:
      {
        argcheck(nrhs, 3);
        hidi::Altimeter* sensor = package->getAltimeter()[index];
        getTime(static_cast<hidi::Sensor*>(sensor), prhs[2], plhs[0]);
        break;
      }

      case getAltitude:
      {
        argcheck(nrhs, 3);
        hidi::Altimeter* sensor = package->getAltimeter()[index];
        const mxArray* n = prhs[2];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        size_t N = mxGetNumberOfElements(n);
        double* data;
        size_t k;
        uint32check(n);
        plhs[0] = mxCreateDoubleMatrix(mxGetM(n), mxGetN(n), mxREAL);
        data = mxGetPr(plhs[0]);
        for(k = 0; k<N; ++k)
        {
          data[k] = sensor->getAltitude(node[k]);
        }
        break;
      }

      case gpsReceiverRefresh:
      {
        hidi::GPSReceiver* sensor = package->getGPSReceiver()[index];
        sensor->refresh();
        break;
      }
      
      case gpsReceiverHasData:
      {
        hidi::GPSReceiver* sensor = package->getGPSReceiver()[index];
        convert(sensor->hasData(), plhs[0]);
        break;
      }

      case gpsReceiverFirst:
      {
        hidi::GPSReceiver* sensor = package->getGPSReceiver()[index];
        convert(sensor->first(), plhs[0]);
        break;
      }

      case gpsReceiverLast:
      {
        hidi::GPSReceiver* sensor = package->getGPSReceiver()[index];
        convert(sensor->last(), plhs[0]);
        break;
      }

      case gpsReceiverGetTime:
      {
        argcheck(nrhs, 3);
        hidi::GPSReceiver* sensor = package->getGPSReceiver()[index];
        getTime(static_cast<hidi::Sensor*>(sensor), prhs[2], plhs[0]);
        break;
      }
      
      case getLongitude:
      {
        argcheck(nrhs, 3);
        hidi::GPSReceiver* sensor = package->getGPSReceiver()[index];
        const mxArray* n = prhs[2];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        size_t N = mxGetNumberOfElements(n);
        double* data;
        size_t k;
        uint32check(n);
        plhs[0] = mxCreateDoubleMatrix(mxGetM(n), mxGetN(n), mxREAL);
        data = mxGetPr(plhs[0]);
        for(k = 0; k<N; ++k)
        {
          data[k] = sensor->getLongitude(node[k]);
        }
        break;
      }

      case getLatitude:
      {
        argcheck(nrhs, 3);
        hidi::GPSReceiver* sensor = package->getGPSReceiver()[index];
        const mxArray* n = prhs[2];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        size_t N = mxGetNumberOfElements(n);
        double* data;
        size_t k;
        uint32check(n);
        plhs[0] = mxCreateDoubleMatrix(mxGetM(n), mxGetN(n), mxREAL);
        data = mxGetPr(plhs[0]);
        for(k = 0; k<N; ++k)
        {
          data[k] = sensor->getLatitude(node[k]);
        }
        break;
      }

      case getHeight:
      {
        argcheck(nrhs, 3);
        hidi::GPSReceiver* sensor = package->getGPSReceiver()[index];
        const mxArray* n = prhs[2];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        size_t N = mxGetNumberOfElements(n);
        double* data;
        size_t k;
        uint32check(n);
        plhs[0] = mxCreateDoubleMatrix(mxGetM(n), mxGetN(n), mxREAL);
        data = mxGetPr(plhs[0]);
        for(k = 0; k<N; ++k)
        {
          data[k] = sensor->getHeight(node[k]);
        }
        break;
      }

      case hasPrecision:
      {
        hidi::GPSReceiver* sensor = package->getGPSReceiver()[index];
        convert(sensor->hasPrecision(), plhs[0]);
        break;
      }

      case getPrecisionHorizontal:
      {
        argcheck(nrhs, 3);
        hidi::GPSReceiver* sensor = package->getGPSReceiver()[index];
        const mxArray* n = prhs[2];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        size_t N = mxGetNumberOfElements(n);
        double* data;
        size_t k;
        uint32check(n);
        plhs[0] = mxCreateDoubleMatrix(mxGetM(n), mxGetN(n), mxREAL);
        data = mxGetPr(plhs[0]);
        for(k = 0; k<N; ++k)
        {
          data[k] = sensor->getPrecisionHorizontal(node[k]);
        }
        break;
      }

      case getPrecisionVertical:
      {
        argcheck(nrhs, 3);
        hidi::GPSReceiver* sensor = package->getGPSReceiver()[index];
        const mxArray* n = prhs[2];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        size_t N = mxGetNumberOfElements(n);
        double* data;
        size_t k;
        uint32check(n);
        plhs[0] = mxCreateDoubleMatrix(mxGetM(n), mxGetN(n), mxREAL);
        data = mxGetPr(plhs[0]);
        for(k = 0; k<N; ++k)
        {
          data[k] = sensor->getPrecisionVertical(node[k]);
        }
        break;
      }

      case getPrecisionCircular:
      {
        argcheck(nrhs, 3);
        hidi::GPSReceiver* sensor = package->getGPSReceiver()[index];
        const mxArray* n = prhs[2];
        uint32_t* node = static_cast<uint32_t*>(mxGetData(n));
        size_t N = mxGetNumberOfElements(n);
        double* data;
        size_t k;
        uint32check(n);
        plhs[0] = mxCreateDoubleMatrix(mxGetM(n), mxGetN(n), mxREAL);
        data = mxGetPr(plhs[0]);
        for(k = 0; k<N; ++k)
        {
          data[k] = sensor->getPrecisionCircular(node[k]);
        }
        break;
      }

      default:
      {
        throw("Invalid member function call.");
      }
    }
    return;
  }
}
  
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
  std::string prefix("SensorPackageBridge: ");
  try
  {
    SensorPackageBridge::safeMexFunction(nlhs, plhs, nrhs, prhs);
  }
  catch(std::exception& e)
  {
    mexErrMsgTxt((prefix+e.what()).c_str());
  }
  catch(const char* str)
  {
    mexErrMsgTxt((prefix+str).c_str());
  }
  catch(...)
  {
    mexErrMsgTxt((prefix+"Unhandled exception.").c_str());
  }
  return;
}