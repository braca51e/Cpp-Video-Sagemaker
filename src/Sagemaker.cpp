#include <iostream>
#include "Frame.h"
#include "Sagemaker.h"

Sagemaker::Sagemaker(){
    _ALLOCATION_TAG = "SagemakerEndpoint";
    _awsClient_configuration.region = Aws::Region::US_EAST_1;
    _awsClient_configuration.maxConnections = 180;
    _awsClient_configuration.connectTimeoutMs = 2000;
    _awsOptions.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
    _endpoint = "ott-detect";
    Aws::InitAPI(_awsOptions);
    _sagemakerClient = Aws::MakeShared<Aws::SageMakerRuntime::SageMakerRuntimeClient>(_ALLOCATION_TAG,
                                   Aws::MakeShared<DefaultAWSCredentialsProviderChain>(_ALLOCATION_TAG),
                                        _awsClient_configuration);
}

Sagemaker::~Sagemaker(){
    Aws::ShutdownAPI(_awsOptions);
}

void Sagemaker::infer(std::shared_ptr<Frame> frame){
    InfereceResult response;
    //TODO call sagemaker here
    Aws::SageMakerRuntime::Model::InvokeEndpointRequest request;
    request.SetEndpointName(_endpoint);
    //std::shared_ptr<Aws::IOStream> payload = Aws::MakeShared<Aws::IOStream>("FrameInference",
    //                             &frame->getFrameBuffer().at(0), std::ios_base::in | std::ios_base::binary);
    auto payload = Aws::MakeShared<Aws::StringStream>("FrameInference", std::stringstream::in | std::stringstream::out | std::stringstream::binary);
    payload->write(reinterpret_cast<char*>(&frame->getFrameBuffer().at(0)), frame->getFrameBuffer().size());
    request.SetBody(payload);
    request.SetContentType("image/jpeg");
    auto result = _sagemakerClient->InvokeEndpoint(request);
    std::cout << "Calling sagemaker endpoint success: " << result.IsSuccess() << std::endl;
    response.label = "processed";
    response.bboxH = 0.5;
    response.bboxW = 0.4;
    response.bboxX = 0.3;
    response.bboxY = 0.1;
    response.confidence = 0.99;
    response.frameNumber = frame->getFrameNumber();
    frame->setInferenceResult(response);
}
