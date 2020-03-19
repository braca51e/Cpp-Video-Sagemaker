#ifndef SAGEMAKER_H
#define SAGEMAKER_H

#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/sagemaker-runtime/SageMakerRuntimeClient.h>
#include <aws/sagemaker-runtime/model/InvokeEndpointRequest.h>
#include <aws/sagemaker-runtime/model/InvokeEndpointResult.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>
#include <fstream>
#include "Frame.h"

using namespace Aws;

// forward declaration to avoid include cycle
class Frame;

class Sagemaker
{
public:
    // constructor / desctructor
    Sagemaker();
    ~Sagemaker();

    // getters / setters

    // typical behaviour methods
    void infer(std::shared_ptr<Frame> frame);

    // miscellaneous

private:
    static const char* _ALLOCATION_TAG;
    Client::ClientConfiguration _awsClient_configuration;
    SDKOptions _awsSdkOptions;
    std::shared_ptr<SageMakerRuntime::SageMakerRuntimeClient> _sagemakerClient;
    String _endpoint;
};

#endif
