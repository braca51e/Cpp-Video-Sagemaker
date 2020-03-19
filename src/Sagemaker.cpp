#include <iostream>
#include <sstream>
#include "Frame.h"
#include "Sagemaker.h"

const char* Sagemaker::_ALLOCATION_TAG = "SagemakerEndpoint";

Sagemaker::Sagemaker(){
    _awsSdkOptions.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    _awsClient_configuration.region = Aws::Region::US_EAST_1;
    _awsClient_configuration.maxConnections = 180;
    _awsClient_configuration.connectTimeoutMs = 2000;
    _awsSdkOptions.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
    _endpoint = "endpoint-name";
    InitAPI(_awsSdkOptions);
    _sagemakerClient = Aws::MakeShared<Aws::SageMakerRuntime::SageMakerRuntimeClient>(_ALLOCATION_TAG,
                                   Aws::MakeShared<Auth::DefaultAWSCredentialsProviderChain>(_ALLOCATION_TAG),
                                        _awsClient_configuration);
}

Sagemaker::~Sagemaker(){
    Aws::ShutdownAPI(_awsSdkOptions);
}

void Sagemaker::infer(std::shared_ptr<Frame> frame){
    InfereceResult response;
    //TODO call sagemaker here
    SageMakerRuntime::Model::InvokeEndpointRequest request;
    request.SetEndpointName(_endpoint);
    //std::shared_ptr<Aws::IOStream> payload = Aws::MakeShared<Aws::IOStream>("FrameInference",
    //                             &frame->getFrameBuffer().at(0), std::ios_base::in | std::ios_base::binary);
    auto payload = Aws::MakeShared<Aws::StringStream>("FrameInference", std::stringstream::in | std::stringstream::out | std::stringstream::binary);
    payload->write(reinterpret_cast<char*>(&frame->getFrameBuffer().at(0)), frame->getFrameBuffer().size());
    request.SetBody(payload);
    request.SetContentType("image/jpeg");
    SageMakerRuntime::Model::InvokeEndpointOutcome result = _sagemakerClient->InvokeEndpoint(request);

    if(result.IsSuccess()){
        SageMakerRuntime::Model::InvokeEndpointResult infeResult = std::move(result.GetResult());
        if(infeResult.GetContentType().compare("text/csv")){
            auto &strResul = infeResult.GetBody();
            char predictions[255] = { 0 };
            while (strResul.getline(predictions, 254)) {
               std::string sPredictions(predictions);
               std::replace(sPredictions.begin(), sPredictions.end(), ' ', '-');
               std::replace(sPredictions.begin(), sPredictions.end(), ',', ' ');
               std::stringstream ssPredictions(sPredictions);
               ssPredictions >> response.label;
               ssPredictions >> response.confidence;
               ssPredictions >> response.bboxX;
               ssPredictions >> response.bboxY;
               ssPredictions >> response.bboxR;
               ssPredictions >> response.bboxB;
               response.frameNumber = frame->getFrameNumber();
               std::cout << "Predictions: " << predictions << std::endl;
            }
        }
    }
    else {
        std::cout << "Frame # " << frame->getFrameNumber() << " NOT processed by Sagemaker" << std::endl;
    }
    frame->setInferenceResult(response);
}
