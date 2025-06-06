/* Data descriptors shared between the backends. Supports I/O to simple text
   format generated by the python training. */

#ifndef DESC_H
#define DESC_H

#include <istream>
#include <string>
#include <vector>

#include "../game/rules.h"
#include "../neuralnet/activations.h"

struct ConvLayerDesc {
  std::string name;
  int convYSize;
  int convXSize;
  int inChannels;
  int outChannels;
  int dilationY;
  int dilationX;
  // outC x inC x H x W (col-major order - W has least stride, outC greatest)
  std::vector<float> weights;

  ConvLayerDesc();
  ConvLayerDesc(std::istream& in, bool binaryFloats);
  ConvLayerDesc(ConvLayerDesc&& other);

  ConvLayerDesc(const ConvLayerDesc&) = delete;
  ConvLayerDesc& operator=(const ConvLayerDesc&) = delete;

  ConvLayerDesc& operator=(ConvLayerDesc&& other);

  double getSpatialConvDepth() const;

  void scaleOutputChannels(const std::vector<float>& scaling);
};

struct BatchNormLayerDesc {
  std::string name;
  int numChannels;
  float epsilon;
  bool hasScale;
  bool hasBias;
  std::vector<float> mean;
  std::vector<float> variance;
  std::vector<float> scale;
  std::vector<float> bias;

  std::vector<float> mergedScale;
  std::vector<float> mergedBias;

  BatchNormLayerDesc();
  BatchNormLayerDesc(std::istream& in, bool binaryFloats);
  BatchNormLayerDesc(BatchNormLayerDesc&& other);

  BatchNormLayerDesc(const BatchNormLayerDesc&) = delete;
  BatchNormLayerDesc& operator=(const BatchNormLayerDesc&) = delete;

  BatchNormLayerDesc& operator=(BatchNormLayerDesc&& other);

  void scaleInputChannels(const std::vector<float>& scaling);
  void extractChannelFactorsAbsLtOne(std::vector<float>& channelFactors);
  void extractChannelFactorsAbsLtOneWithInverses(std::vector<float>& channelFactors, std::vector<float>& invChannelFactors);
  void applyScale8ToReduceActivations();
};

struct ActivationLayerDesc {
  std::string name;
  int activation;

  ActivationLayerDesc();
  ActivationLayerDesc(std::istream& in, int modelVersion);
  ActivationLayerDesc(ActivationLayerDesc&& other);

  ActivationLayerDesc(const ActivationLayerDesc&) = delete;
  ActivationLayerDesc& operator=(const ActivationLayerDesc&) = delete;

  ActivationLayerDesc& operator=(ActivationLayerDesc&& other);

  void applyScale8ToReduceActivations();
};

struct MatMulLayerDesc {
  std::string name;
  int inChannels;
  int outChannels;
  // inC x outC
  std::vector<float> weights;

  MatMulLayerDesc();
  MatMulLayerDesc(std::istream& in, bool binaryFloats);
  MatMulLayerDesc(MatMulLayerDesc&& other);

  MatMulLayerDesc(const MatMulLayerDesc&) = delete;
  MatMulLayerDesc& operator=(const MatMulLayerDesc&) = delete;

  MatMulLayerDesc& operator=(MatMulLayerDesc&& other);

  void scaleOutputChannels(const std::vector<float>& scaling);
};

struct MatBiasLayerDesc {
  std::string name;
  int numChannels;
  std::vector<float> weights;

  MatBiasLayerDesc();
  MatBiasLayerDesc(std::istream& in, bool binaryFloats);
  MatBiasLayerDesc(MatBiasLayerDesc&& other);

  MatBiasLayerDesc(const MatBiasLayerDesc&) = delete;
  MatBiasLayerDesc& operator=(const MatBiasLayerDesc&) = delete;

  MatBiasLayerDesc& operator=(MatBiasLayerDesc&& other);
  void applyScale8ToReduceActivations();
};

struct ResidualBlockDesc {
  std::string name;
  BatchNormLayerDesc preBN;
  ActivationLayerDesc preActivation;
  ConvLayerDesc regularConv;
  BatchNormLayerDesc midBN;
  ActivationLayerDesc midActivation;
  ConvLayerDesc finalConv;

  ResidualBlockDesc();
  ResidualBlockDesc(std::istream& in, int modelVersion, bool binaryFloats);
  ResidualBlockDesc(ResidualBlockDesc&& other);

  ResidualBlockDesc(const ResidualBlockDesc&) = delete;
  ResidualBlockDesc& operator=(const ResidualBlockDesc&) = delete;

  ResidualBlockDesc& operator=(ResidualBlockDesc&& other);

  void iterConvLayers(std::function<void(const ConvLayerDesc& dest)> f) const;
  double getSpatialConvDepth() const;

  void transformToReduceActivations();
  void applyScale8ToReduceActivations();
};

struct GlobalPoolingResidualBlockDesc {
  std::string name;
  int modelVersion;
  BatchNormLayerDesc preBN;
  ActivationLayerDesc preActivation;
  ConvLayerDesc regularConv;
  ConvLayerDesc gpoolConv;
  BatchNormLayerDesc gpoolBN;
  ActivationLayerDesc gpoolActivation;
  MatMulLayerDesc gpoolToBiasMul;
  BatchNormLayerDesc midBN;
  ActivationLayerDesc midActivation;
  ConvLayerDesc finalConv;

  GlobalPoolingResidualBlockDesc();
  GlobalPoolingResidualBlockDesc(std::istream& in, int modelVersion, bool binaryFloats);
  GlobalPoolingResidualBlockDesc(GlobalPoolingResidualBlockDesc&& other);

  GlobalPoolingResidualBlockDesc(const GlobalPoolingResidualBlockDesc&) = delete;
  GlobalPoolingResidualBlockDesc& operator=(const GlobalPoolingResidualBlockDesc&) = delete;

  GlobalPoolingResidualBlockDesc& operator=(GlobalPoolingResidualBlockDesc&& other);

  void iterConvLayers(std::function<void(const ConvLayerDesc& dest)> f) const;
  double getSpatialConvDepth() const;

  void transformToReduceActivations();
  void applyScale8ToReduceActivations();
};

struct NestedBottleneckResidualBlockDesc {
  std::string name;
  int numBlocks;

  BatchNormLayerDesc preBN;
  ActivationLayerDesc preActivation;
  ConvLayerDesc preConv;

  std::vector<std::pair<int, unique_ptr_void>> blocks;

  BatchNormLayerDesc postBN;
  ActivationLayerDesc postActivation;
  ConvLayerDesc postConv;

  NestedBottleneckResidualBlockDesc();
  NestedBottleneckResidualBlockDesc(std::istream& in, int modelVersion, bool binaryFloats);
  NestedBottleneckResidualBlockDesc(NestedBottleneckResidualBlockDesc&& other);

  NestedBottleneckResidualBlockDesc(const NestedBottleneckResidualBlockDesc&) = delete;
  NestedBottleneckResidualBlockDesc& operator=(const NestedBottleneckResidualBlockDesc&) = delete;

  NestedBottleneckResidualBlockDesc& operator=(NestedBottleneckResidualBlockDesc&& other);

  void iterConvLayers(std::function<void(const ConvLayerDesc& dest)> f) const;
  double getSpatialConvDepth() const;

  void transformToReduceActivations();
  void applyScale8ToReduceActivations();
};

struct SGFMetadataEncoderDesc {
  std::string name;
  int metaEncoderVersion;
  int numInputMetaChannels;
  MatMulLayerDesc mul1;
  MatBiasLayerDesc bias1;
  ActivationLayerDesc act1;
  MatMulLayerDesc mul2;
  MatBiasLayerDesc bias2;
  ActivationLayerDesc act2;
  MatMulLayerDesc mul3;

  SGFMetadataEncoderDesc();
  ~SGFMetadataEncoderDesc();
  SGFMetadataEncoderDesc(std::istream& in, int modelVersion, int metaEncoderVersion, bool binaryFloats);
  SGFMetadataEncoderDesc(SGFMetadataEncoderDesc&& other);

  SGFMetadataEncoderDesc(const SGFMetadataEncoderDesc&) = delete;
  SGFMetadataEncoderDesc& operator=(const SGFMetadataEncoderDesc&) = delete;

  SGFMetadataEncoderDesc& operator=(SGFMetadataEncoderDesc&& other);
};


constexpr int ORDINARY_BLOCK_KIND = 0;
constexpr int GLOBAL_POOLING_BLOCK_KIND = 2;
constexpr int NESTED_BOTTLENECK_BLOCK_KIND = 3;

struct TrunkDesc {
  std::string name;
  int modelVersion;
  int numBlocks;
  int trunkNumChannels;
  int midNumChannels;      // Currently every plain residual block must have the same number of mid conv channels
  int regularNumChannels;  // Currently every gpool residual block must have the same number of regular conv hannels
  int gpoolNumChannels;    // Currently every gpooling residual block must have the same number of gpooling conv channels

  int metaEncoderVersion;

  ConvLayerDesc initialConv;
  MatMulLayerDesc initialMatMul;
  SGFMetadataEncoderDesc sgfMetadataEncoder;
  std::vector<std::pair<int, unique_ptr_void>> blocks;
  BatchNormLayerDesc trunkTipBN;
  ActivationLayerDesc trunkTipActivation;

  TrunkDesc();
  ~TrunkDesc();
  TrunkDesc(std::istream& in, int modelVersion, bool binaryFloats, int metaEncoderVersion);
  TrunkDesc(TrunkDesc&& other);

  TrunkDesc(const TrunkDesc&) = delete;
  TrunkDesc& operator=(const TrunkDesc&) = delete;

  TrunkDesc& operator=(TrunkDesc&& other);

  void iterConvLayers(std::function<void(const ConvLayerDesc& dest)> f) const;
  double getSpatialConvDepth() const;

  void transformToReduceActivations();
  void applyScale8ToReduceActivations();
};

struct PolicyHeadDesc {
  std::string name;
  int modelVersion;
  int policyOutChannels;
  ConvLayerDesc p1Conv;
  ConvLayerDesc g1Conv;
  BatchNormLayerDesc g1BN;
  ActivationLayerDesc g1Activation;
  MatMulLayerDesc gpoolToBiasMul;
  BatchNormLayerDesc p1BN;
  ActivationLayerDesc p1Activation;
  ConvLayerDesc p2Conv;
  MatMulLayerDesc gpoolToPassMul;
  MatBiasLayerDesc gpoolToPassBias;
  ActivationLayerDesc passActivation;
  MatMulLayerDesc gpoolToPassMul2;

  PolicyHeadDesc();
  ~PolicyHeadDesc();
  PolicyHeadDesc(std::istream& in, int modelVersion, bool binaryFloats);
  PolicyHeadDesc(PolicyHeadDesc&& other);

  PolicyHeadDesc(const PolicyHeadDesc&) = delete;
  PolicyHeadDesc& operator=(const PolicyHeadDesc&) = delete;

  PolicyHeadDesc& operator=(PolicyHeadDesc&& other);

  void iterConvLayers(std::function<void(const ConvLayerDesc& dest)> f) const;

  void transformToReduceActivations();
  void applyScale8ToReduceActivations();
};

struct ValueHeadDesc {
  std::string name;
  int modelVersion;
  ConvLayerDesc v1Conv;
  BatchNormLayerDesc v1BN;
  ActivationLayerDesc v1Activation;
  MatMulLayerDesc v2Mul;
  MatBiasLayerDesc v2Bias;
  ActivationLayerDesc v2Activation;
  MatMulLayerDesc v3Mul;
  MatBiasLayerDesc v3Bias;
  MatMulLayerDesc sv3Mul;
  MatBiasLayerDesc sv3Bias;
  ConvLayerDesc vOwnershipConv;

  ValueHeadDesc();
  ~ValueHeadDesc();
  ValueHeadDesc(std::istream& in, int modelVersion, bool binaryFloats);
  ValueHeadDesc(ValueHeadDesc&& other);

  ValueHeadDesc(const ValueHeadDesc&) = delete;
  ValueHeadDesc& operator=(const ValueHeadDesc&) = delete;

  ValueHeadDesc& operator=(ValueHeadDesc&& other);

  void iterConvLayers(std::function<void(const ConvLayerDesc& dest)> f) const;

  void transformToReduceActivations();
  void applyScale8ToReduceActivations();
};

struct ModelPostProcessParams {
  double tdScoreMultiplier;
  double scoreMeanMultiplier;
  double scoreStdevMultiplier;
  double leadMultiplier;
  double varianceTimeMultiplier;
  double shorttermValueErrorMultiplier;
  double shorttermScoreErrorMultiplier;

  float outputScaleMultiplier;

  ModelPostProcessParams();
  ~ModelPostProcessParams();
};

struct ModelDesc {
  std::string name;
  std::string sha256;
  int modelVersion;
  int numInputChannels;
  int numInputGlobalChannels;
  int numInputMetaChannels;
  int numPolicyChannels;
  int numValueChannels;
  int numScoreValueChannels;
  int numOwnershipChannels;

  int metaEncoderVersion;

  ModelPostProcessParams postProcessParams;

  TrunkDesc trunk;
  PolicyHeadDesc policyHead;
  ValueHeadDesc valueHead;

  ModelDesc();
  ~ModelDesc();
  ModelDesc(std::istream& in, const std::string& sha256, bool binaryFloats);
  ModelDesc(ModelDesc&& other);

  ModelDesc(const ModelDesc&) = delete;
  ModelDesc& operator=(const ModelDesc&) = delete;

  ModelDesc& operator=(ModelDesc&& other);

  void iterConvLayers(std::function<void(const ConvLayerDesc& dest)> f) const;
  int maxConvChannels(int convXSize, int convYSize) const;
  double getTrunkSpatialConvDepth() const;

  void transformToReduceActivations();
  void applyScale8ToReduceActivations();

  //Loads a model from a file that may or may not be gzipped, storing it in descBuf
  //If expectedSha256 is nonempty, will also verify sha256 of the loaded data.
  static void loadFromFileMaybeGZipped(const std::string& fileName, ModelDesc& descBuf, const std::string& expectedSha256);

  //Return the "nearest" supported ruleset to desiredRules by this model.
  //Fills supported with true if desiredRules itself was exactly supported, false if some modifications had to be made.
  Rules getSupportedRules(const Rules& desiredRules, bool& supported) const;

};

#endif  // #ifndef DESC_H
