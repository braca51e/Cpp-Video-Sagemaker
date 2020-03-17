#ifndef SAGEMAKER_H
#define SAGEMAKER_H

#include <aws/core/Aws.h>
#include <aws/sagemaker-runtime/model/InvokeEndpointRequest.h>
#include <aws/sagemaker-runtime/model/InvokeEndpointResult.h>
#include <aws/sagemaker-runtime/SageMakerRuntimeClient.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>
#include <fstream>
#include "Frame.h"

using namespace Aws::Auth;
using namespace Aws::SageMakerRuntime;

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
    Aws::Client::ClientConfiguration _awsClient_configuration;
    Aws::SDKOptions _awsOptions;
    std::shared_ptr<SageMakerRuntimeClient> _sagemakerClient;
    Aws::String _endpoint;
};

#endif
