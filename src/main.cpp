#include "google/cloud/speech/v1/speech_client.h"
#include "google/cloud/project.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) try {
  auto constexpr kDefaultUri = "gs://cloud-samples-data/speech/hello.wav";
  if (argc > 2) {
    std::cerr << "Usage: " << argv[0] << " [gcs-uri]\n"
              << "  The gcs-uri must be in gs://... format. It defaults to "
              << kDefaultUri << "\n";
    return 1;
  }
  auto uri = std::string{argc == 2 ? argv[1] : kDefaultUri};

  cout << uri << endl;

  namespace speech = ::google::cloud::speech_v1;
  auto client = speech::SpeechClient(speech::MakeSpeechConnection());

   google::cloud::speech::v1::RecognitionConfig config;
  config.set_language_code("en-US");
  google::cloud::speech::v1::RecognitionAudio audio;
  audio.set_uri(uri);
  auto response = client.Recognize(config, audio);
  if (!response) throw std::move(response).status();
  std::cout << response->DebugString() << "\n";
 
  return 0;
} catch (google::cloud::Status const& status) {
  std::cerr << "google::cloud::Status thrown: " << status << "\n";
  return 1;
}